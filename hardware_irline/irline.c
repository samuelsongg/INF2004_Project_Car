#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/irline.h"

// Global variables for left and right IR sensor results and pulse width
volatile uint32_t l_ir_result;
volatile uint32_t r_ir_result;
volatile uint64_t ir_pulse_width;
volatile absolute_time_t start_time_ir;
volatile absolute_time_t end_time_ir;

uint8_t barcodeFirstChar=0;
uint8_t barcodeSecondChar=0;
uint8_t barcodeThirdChar=0;

enum bartype{
    THICK_BLACK, // 0
    THIN_BLACK, // 1
    THICK_WHITE, // 2
    THIN_WHITE // 3
};

//code 39 format of letter F using enum bartype
static char* A_ARRAY_MAP = "031312130";
static char* B_ARRAY_MAP = "130312130";
static char* C_ARRAY_MAP = "030312131";
static char* D_ARRAY_MAP = "131302130";
static char* E_ARRAY_MAP = "031302131";
static char* F_ARRAY_MAP = "130302131";
static char* G_ARRAY_MAP = "131312030";
static char* H_ARRAY_MAP = "031312031";
static char* I_ARRAY_MAP = "130312031";
static char* J_ARRAY_MAP = "131302031";
static char* K_ARRAY_MAP = "031313120";
static char* L_ARRAY_MAP = "130313120";
static char* M_ARRAY_MAP = "030313121";
static char* N_ARRAY_MAP = "131303120";
static char* O_ARRAY_MAP = "031303121";
static char* P_ARRAY_MAP = "130303121";
static char* Q_ARRAY_MAP = "131313020";
static char* R_ARRAY_MAP = "031313021";
static char* S_ARRAY_MAP = "130313021";
static char* T_ARRAY_MAP = "131303021";
static char* U_ARRAY_MAP = "021313130";
static char* V_ARRAY_MAP = "120313130";
static char* W_ARRAY_MAP = "020313131";
static char* X_ARRAY_MAP = "121303130";
static char* Y_ARRAY_MAP = "021303131";
static char* Z_ARRAY_MAP = "120303131";

static int waitingForStart = 1;


//code 39 format of asterisk using enum bartype
static char* ASTERISK_ARRAY_MAP = "121303031";

static int barcodeArr[BARCODE_ARR_SIZE];

static uint32_t res = 0;
static uint16_t prevAvg = 0;
static uint16_t max = 0;
static uint16_t min = 3000;

static int i = 0 ;
static int barcode_arr_index = 1;

char* outputBuffer;

static absolute_time_t blockStart;
static absolute_time_t blockEnd;

volatile char read_char;



// Function to read IR sensor values and calculate pulse width
void read_ir(void *params) {
    // Read ADC values from left and right IR sensors
    adc_select_input(0);
    l_ir_result = adc_read();
    adc_select_input(1);
    r_ir_result = adc_read();
}

struct voltageClassification {
    uint16_t voltage;
    // 0 - white
    // 1 - black
    int blackWhite;
    absolute_time_t blockStart;
    int64_t  blockLength;
    enum bartype type;
} voltageClassification;

// queue for voltageclassifications of length 9
static struct voltageClassification voltageClassifications[BARCODE_BUF_SIZE];

// queue for barcode read of length 3
static char barcodeRead[3];

//function to append to barcodeRead Queue
static void appendToBarcodeRead(char barcodeChar){
    barcodeRead[0] = barcodeRead[1];
    barcodeRead[1] = barcodeRead[2];
    barcodeRead[2] = barcodeChar;
}

static int isValidBarcode(){
    if(barcodeRead[0] == '*' && barcodeRead[2] == '*'){
        if(barcodeRead[1] != 0)
            return 1;
    }

    return 0;
}

static int isBarcodeFull(){
    if(barcodeRead[0] != 0 && barcodeRead[1] != 0 && barcodeRead[2] != 0){
        return 1;
    }

    return 0;
}

static void clearBarcodeRead(){
    barcodeRead[0] = 0;
    barcodeRead[1] = 0;
    barcodeRead[2] = 0;
}

//function to convert array of integer to string
static char *intArrayToString(int *arr, int size){
    char *str = malloc(size + 1);
    for(int i = 0; i < size; i++){
        str[i] = arr[i] + '0';
    }
    str[size] = '\0';
    return str;
}

// function to flush queue 
static void flushVoltageClassification(){
    barcode_arr_index = 1;
    blockStart = get_absolute_time();

    struct voltageClassification lastReading = voltageClassifications[BARCODE_BUF_SIZE - 1];

    for(int i = 0; i < BARCODE_BUF_SIZE; i++){
        voltageClassifications[i].voltage = 0;
        voltageClassifications[i].blackWhite = -1;
        voltageClassifications[i].blockLength = 0;
        voltageClassifications[i].type = 0;
    }

    voltageClassifications[0] = lastReading;
}   

// Setup function for IR sensors
void ir_setup(void *params) {
    // Initialize GPIO pins for IR sensors' GND
    gpio_init(LEFT_IR_SENSOR_GND);
    gpio_init(RIGHT_IR_SENSOR_GND);

    // Set GPIO directions for IR sensors' GND pins
    gpio_set_dir(LEFT_IR_SENSOR_GND, GPIO_OUT);
    gpio_set_dir(RIGHT_IR_SENSOR_GND, GPIO_OUT);

    // Set LEFT and RIGHT IR sensor GND pins to low
    gpio_put(LEFT_IR_SENSOR_GND, 0); // Set to low for LEFT_IR_Sensor's GND
    gpio_put(RIGHT_IR_SENSOR_GND, 0); // Set to low for RIGHT_IR_Sensor's GND

    // Initialize GPIO pins for IR sensors
    gpio_set_dir(LEFT_IR_SENSOR_A0, GPIO_IN); // IR Sensor
    gpio_set_function(LEFT_IR_SENSOR_A0, GPIO_FUNC_SIO);

    gpio_init(RIGHT_IR_SENSOR_A0);
    gpio_set_dir(RIGHT_IR_SENSOR_A0, GPIO_IN);
    
    adc_init();

    // init the queue
    flushVoltageClassification();

    adc_gpio_init(ADC_PIN);

    adc_select_input(0);

    adc_fifo_setup(true, false, 1, false, false);
    adc_set_clkdiv(0);
    adc_irq_set_enabled(true);

    irq_clear(ADC_IRQ_FIFO);
    irq_set_exclusive_handler(ADC_IRQ_FIFO, ADC_IRQ_FIFO_HANDLER);
    irq_set_enabled(ADC_IRQ_FIFO, true);
    
    adc_run(true);

    // sleep_ms(2000);

    blockStart = get_absolute_time();
}


static int* thickThinClassification(){
    //calculate average block length
    int64_t totalBarLength = 0;
    for(int i = 0; i< BARCODE_ARR_SIZE; i++){
        totalBarLength += voltageClassifications[i].blockLength;
    }

    int *barsRead = malloc(BARCODE_ARR_SIZE * sizeof(int));

    int64_t avgBarLength = (totalBarLength/BARCODE_ARR_SIZE);
    //assign thick thin classification
    for(int i = 0; i< BARCODE_ARR_SIZE; i++){
        //printf("%"PRId64", ",voltageClassifications[i].blockLength);

        if(voltageClassifications[i].blackWhite){
            if(voltageClassifications[i].blockLength < avgBarLength){
                voltageClassifications[i].type = THIN_BLACK;
            }else{
                voltageClassifications[i].type = THICK_BLACK;
            }
        }else{
            if(voltageClassifications[i].blockLength < avgBarLength){
                voltageClassifications[i].type = THIN_WHITE;
            }else{
                voltageClassifications[i].type = THICK_WHITE;
            }
        }
        barsRead[i] = voltageClassifications[i].type;
    }
    //printf("\n\r");
    return barsRead;
}
// function to check if queue is full
static int isVoltageClassificationFull(){
    for(int i = 0; i < BARCODE_BUF_SIZE; i++){
        if(voltageClassifications[i].blackWhite == -1){
            return 0;
        }
    }
    return 1;
}

//function to compare buffer and the barcodes
static char compareTwoArray () {
    int* barsRead = thickThinClassification();

    if(voltageClassifications[0].blackWhite == 0){
        //printf("Start with white\n\r");
        return 0;
    }

    char* string = intArrayToString(barsRead, BARCODE_ARR_SIZE);
    free(barsRead);

    char* barcodes[] = {
        A_ARRAY_MAP,
        B_ARRAY_MAP,
        C_ARRAY_MAP,
        D_ARRAY_MAP,
        E_ARRAY_MAP,
        F_ARRAY_MAP,
        G_ARRAY_MAP,
        H_ARRAY_MAP,
        I_ARRAY_MAP,
        J_ARRAY_MAP,
        K_ARRAY_MAP,
        L_ARRAY_MAP,
        M_ARRAY_MAP,
        N_ARRAY_MAP,
        O_ARRAY_MAP,
        P_ARRAY_MAP,
        Q_ARRAY_MAP,
        R_ARRAY_MAP,
        S_ARRAY_MAP,
        T_ARRAY_MAP,
        U_ARRAY_MAP,
        V_ARRAY_MAP,
        W_ARRAY_MAP,
        X_ARRAY_MAP,
        Y_ARRAY_MAP,
        Z_ARRAY_MAP,
        ASTERISK_ARRAY_MAP   
    };

    
    char characters[] = {
        'A',
        'B',
        'C',
        'D',
        'E',
        'F',
        'G',
        'H',
        'I',
        'J',
        'K',
        'L',
        'M',
        'N',
        'O',
        'P',
        'Q',
        'R',
        'S',
        'T',
        'U',
        'V',
        'W',
        'X',
        'Y',
        'Z',
        '*'
    };
     

    
    for(int i = 0; i < 27; i++){
        if(strncmp(barcodes[i], string, BARCODE_ARR_SIZE) == 0){
            free(string);
            flushVoltageClassification();
            return characters[i];
        }
    }

    return 0;
    //return 1;
}


// function to append queue
static void appendVoltageClassification(struct voltageClassification voltageClassification){
    voltageClassifications[0] = voltageClassifications[1];
    voltageClassifications[1] = voltageClassifications[2];
    voltageClassifications[2] = voltageClassifications[3];
    voltageClassifications[3] = voltageClassifications[4];
    voltageClassifications[4] = voltageClassifications[5];
    voltageClassifications[5] = voltageClassifications[6];
    voltageClassifications[6] = voltageClassifications[7];
    voltageClassifications[7] = voltageClassifications[8];
    voltageClassifications[8] = voltageClassifications[9];
    voltageClassifications[9] = voltageClassification;
    if(barcode_arr_index == BARCODE_BUF_SIZE){
        char read = compareTwoArray();
        if(read != 0){
            read_char = read;
            printf("%c\0", read);

            
            appendToBarcodeRead(read);
        }
    }
}

static void ADC_IRQ_FIFO_HANDLER() {
    // read data from ADC FIFO
    if(!adc_fifo_is_empty()){
        uint16_t data = adc_fifo_get();
        res += data;
        if(i < 100){
            i++;
        }else{
            //uint16_t avg = res/(i);
            uint16_t avg = res/ (i);

            //printf("%d\0", gpio_get(DIGITAL_PIN));
            //printf("%"PRIu16"", data);
            //printf(",%"PRIu16"", avg);
            if(prevAvg == 0){
                prevAvg = avg;
            }else{
                if(abs(prevAvg - avg) > ADC_DIFFERENCE_THRESHHOLD){
                    prevAvg = avg;
                }else{
                    avg = prevAvg;
                }
            }
            //printf(",%"PRIu16"\n\r", avg);
            i = 0;
            res = 0;

            struct voltageClassification voltageClassification;
            voltageClassification.voltage = avg;
            
            if(avg > BLACK_THRESHOLD || gpio_get(DIGITAL_PIN) == 1){
                voltageClassification.blackWhite = 1;
            }else{
                voltageClassification.blackWhite = 0;
            }

            if(barcode_arr_index == BARCODE_BUF_SIZE){
            
                // if((abs(voltageClassifications[BARCODE_ARR_SIZE - 1].voltage - curr)/ADC_DIFFERENCE_THRESHHOLD * 100 < 10)){
                //     return;
                // }
                if(voltageClassifications[BARCODE_BUF_SIZE - 1].blackWhite != voltageClassification.blackWhite){
                    blockEnd = get_absolute_time();
                    voltageClassification.blockStart = blockEnd;
                    //printf("white block length: %"PRIu64", %"PRIu64"\n\r", blockEnd,blockStart);
                    int64_t blockLength = absolute_time_diff_us(voltageClassifications[BARCODE_BUF_SIZE - 1].blockStart, blockEnd);
                    
                    voltageClassifications[BARCODE_BUF_SIZE - 1].blockLength =  blockLength / 10000;
                    appendVoltageClassification(voltageClassification);
                }
            }else{
                if(voltageClassifications[barcode_arr_index-1].blackWhite != voltageClassification.blackWhite){
                    blockEnd = get_absolute_time();
                    voltageClassification.blockStart = blockEnd;
                    //printf("white block length: %"PRIu64", %"PRIu64"\n\r", blockEnd,blockStart);
                    if(barcode_arr_index == 0){
                        int64_t blockLength = absolute_time_diff_us(blockStart,blockEnd);
                        voltageClassification.blockLength =  blockLength / 10000;
                    }else{
                        int64_t blockLength = absolute_time_diff_us(voltageClassifications[barcode_arr_index-1].blockStart, blockEnd);
                        voltageClassifications[barcode_arr_index - 1].blockLength =  blockLength / 10000;
                    }
                    
                    voltageClassifications[barcode_arr_index] = voltageClassification;
                    barcode_arr_index++;
                }
            }
        }
    }
    irq_clear(ADC_IRQ_FIFO);
}

void ir_main_loop() {
    //i2c_write_byte('I');
    if(isValidBarcode()){
        printf("Valid Barcode\n\r");
        barcodeFirstChar = barcodeRead[0];
        barcodeSecondChar = barcodeRead[1];
        barcodeThirdChar = barcodeRead[2];
        printf("Barcode: %c%c%c\n\r", barcodeFirstChar, barcodeSecondChar, barcodeThirdChar);
        //sendBarcodeVal(); To send barcode values to comms
        clearBarcodeRead();
        barcodeFirstChar = 0;
        barcodeSecondChar = 0;
        barcodeThirdChar = 0;
    }
}

/* End of file */
