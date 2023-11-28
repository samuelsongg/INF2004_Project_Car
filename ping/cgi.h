/**
 * @file cgi.h
 * @brief CGI handlers for LED control and text data processing in a HTTP server environment.
 */

#ifndef MY_CGI_HANDLERS_H
#define MY_CGI_HANDLERS_H

#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/** @defgroup LEDControlMacros Macros for LED control states */
/**@{*/
#define LED_ON "1"   /**< LED on state */
#define LED_OFF "0"  /**< LED off state */
/**@}*/

// Forward declarations of CGI handler functions
static const char* cgiLedHandler(int index, int numParams, char* params[], char* values[]);
static const char* cgiTextHandler(int index, int numParams, char* params[], char* values[]);

// CGI handlers array
static const tCGI cgiHandlers[] = {
    { "/led.cgi", cgiLedHandler },
    { "/text.cgi", cgiTextHandler }
};

/**
 * @brief Initializes CGI handlers.
 *
 * Sets up the CGI handlers to be used by the HTTP server.
 */
void cgiInit(void) {
    http_set_cgi_handlers(cgiHandlers, sizeof(cgiHandlers) / sizeof(tCGI));
}

/**
 * @brief Handler for LED control via CGI.
 *
 * @param index Index of the CGI request.
 * @param numParams Number of parameters passed.
 * @param params Array of parameter names.
 * @param values Array of parameter values.
 * @return Path to the redirect page.
 */
static const char* cgiLedHandler(int index, int numParams, char* params[], char* values[]) {
    if (strcmp(params[0], "led") == 0) {
        if (strcmp(values[0], LED_OFF) == 0) {
            printf("Stopped car\n");
            // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        } else if (strcmp(values[0], LED_ON) == 0) {
            printf("Started car\n");
            // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        }
    }
    return "/index.shtml";
}

/**
 * @brief Handler for processing text data via CGI.
 *
 * @param index Index of the CGI request.
 * @param numParams Number of parameters passed.
 * @param params Array of parameter names.
 * @param values Array of parameter values.
 * @return Path to the redirect page.
 */
static const char* cgiTextHandler(int index, int numParams, char* params[], char* values[]) {
    printf("Received values!\n");
    for (int i = 0; i < numParams; ++i) {
        printf("%s: %s\n", params[i], values[i]);
    }
    return "/index.shtml";
}

#endif // MY_CGI_HANDLERS_H
