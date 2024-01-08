#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include <string.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "lwgps.h"

lwgps_t hgps;

int main() {
  stdio_init_all(); // init usb stdio
  
  uart_init(uart1, 9600);
  gpio_set_function(4, GPIO_FUNC_UART);
  gpio_set_function(5, GPIO_FUNC_UART);
  
  while (true) {
    char gps_rx_data[]= {uart_getc(uart1)};

    scanf("%[^\n]", &gps_rx_data); 
    printf("%s", gps_rx_data);

    lwgps_init(&hgps);

    /* Process all input data */
    lwgps_process(&hgps, gps_rx_data, strlen(gps_rx_data));

    /* Print messages */
    printf("Valid status: %d\r\n", hgps.is_valid);
    printf("Latitude: %f degrees\r\n", hgps.latitude);
    printf("Longitude: %f degrees\r\n", hgps.longitude);
    printf("Altitude: %f meters\r\n", hgps.altitude);
   // putc(gps_rx_data[],stdout);
             }
  
  return 0;
}