#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

//#include <iostream>

//using byte = uint8_t;
static const uint8_t address = 0x45;
static const uint8_t command[2] = {0X2C,0X06};


int main()
{
	#define sda_pin 16
	#define scl_pin 17
	i2c_inst_t *i2c = i2c0;		
	stdio_init_all();
// Pins
	// baud rate 100kHz
	
	// default SDA and SCL pins (GP4, GP5 on a Pico)
 	i2c_init(i2c, 100 * 1000);

    // Initialize I2C pins
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);

    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(sda_pin,scl_pin,GPIO_FUNC_I2C));

	while(true)
	{
		//std::cout << "Sensor test" << "\n";
		
		// write 2 byte command
		int ret1 = i2c_write_blocking(i2c, address, command, 2, false);
		//std::cout << "Write returned " << ret1 << "\n";
		//printf("%d/n",ret1);
		// reply is two (word plus crc)
		uint8_t val[6];		
		int ret2 = i2c_read_blocking(i2c, address, val, 6, false);		
		//std::cout << "Read returned " << ret2 << "\n";
        //printf("%d/n",ret2);

		// if all is good get values
		if((ret1 == 2) && (ret2 = 6))
		{
			float temp_c = -45 + (175 * (val[0] << 8 | val[1])/ 65535.0);
			float hum = 100 * (val[3] << 8 | val[4]) / 65535.0;

			//std::cout << "Temperature " << temp_c << " Humidity " << hum << "\n";
            printf("Temperature=%f\t",temp_c);
            printf("Humidity=%f\n",hum );
		}
		
		sleep_ms(3000);
	}
}