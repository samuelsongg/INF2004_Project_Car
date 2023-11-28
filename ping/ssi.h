/**
 * @file ssi.h
 * @brief Server-Side Include (SSI) handler for an HTTP server using lwIP.
 *
 * This file contains the implementation of an SSI handler for dynamic content
 * generation in an HTTP server. It includes functions to initialize the handler
 * and to process specific SSI tags defined for the server.
 */

#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "hardware/barcode.h"

/**
 * @brief List of Server-Side Include (SSI) tags.
 *
 * These tags are used in HTML files and are processed by the SSI handler.
 * The tag length is limited to 8 bytes by default.
 */
static const char * const ssi_tags[] = {"code"};

/**
 * @brief SSI handler function.
 *
 * This function is called by the HTTP server to process SSI tags.
 *
 * @param iIndex Index of the SSI tag in the ssi_tags array.
 * @param pcInsert Buffer to store the replacement text for the SSI tag.
 * @param iInsertLen Length of the pcInsert buffer.
 * @return The number of characters written to the pcInsert buffer.
 */
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
 * @brief Initialize the SSI handler.
 *
 * Sets up the SSI handler for processing SSI tags in HTML served by the HTTP server.
 * It also initializes any necessary hardware components, such as ADC or barcode reader.
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
