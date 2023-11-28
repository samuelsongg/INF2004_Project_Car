#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"

const char * cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    // Check if a request for LED has been made (/led.cgi?led=x)
    if (strcmp(pcParam[0], "led") == 0) {
        // Check the argument to determine if LED is to be turned on (x=1) or off (x=0)
        if (strcmp(pcValue[0], "0") == 0) {
            // Turn off the LED and print message
            // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
            printf("Stopped car \n");
        } else {
            // Turn on the LED and print message
            // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
            printf("Started car \n");
        }
    }

    // Redirect to the index page
    return "/index.shtml";
}


const char * cgi_text_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    // To output received values from the pico_w
    printf("Received values!\n");
    for (int i = 0; i < iNumParams; i++) {
        printf("%s: %s\n", pcParam[i], pcValue[i]);
    }

    // Send the index page back to the user
    return "/index.shtml";
}

// Array of CGI handlers
static const tCGI cgi_handlers[] = {
    {
        "/led.cgi", // Request path
        cgi_led_handler // Associated handler function
    },
    {
        "/text.cgi", // Request path
        cgi_text_handler // Associated handler function
    }
};

/**
 * Initializes CGI handlers.
 * This function sets up the CGI handlers to be used by the HTTP server.
 */
void cgi_init(void)
{
    // Set the CGI handlers
    http_set_cgi_handlers(cgi_handlers, 2);
}
