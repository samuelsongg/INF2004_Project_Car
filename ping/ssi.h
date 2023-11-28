#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "hardware/barcode.h"

// Define SSI tags - tag length limited to 8 bytes by default
static const char * const ssi_tags[] = {"code"};
static u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen)
{
    size_t printed; // Variable to store the number of characters printed

    switch (iIndex) {
    case 0:
        // Handle the first SSI tag - output a character from the barcode
        printed = snprintf(pcInsert, iInsertLen, "%c", getBarcodeChar());
        break;

    default:
        // For unrecognized tags, no characters are printed
        printed = 0;
        break;
    }

    return (u16_t)printed;
}

/**
 * Initialize the SSI handler.
 * This function sets up the SSI handler for processing SSI tags in HTML.
 */
void ssi_init(void)
{
    // Initialize ADC (internal pin) - optional, currently commented out
    // adc_init();
    // adc_set_temp_sensor_enabled(true);
    // adc_select_input(4);

    // Set the SSI handler with the defined tags
    http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
}
