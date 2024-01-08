#include "ds3231.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "pico/binary_info.h"

#define addr 0x68

#define I2C_PORT	i2c0
#define I2C_SCL		20	
#define I2C_SDA		21


//regaddr,seconds,minutes,hours,weekdays,days,months,years
char  buf[]={0x00,0x50,0x06,0x13,0x04,0x010,0x11,0x22};
char  *week[]={"SUN","Mon","Tues","Wed","Thur","Fri","Sat"};
// int ds3231SetTime()
// {
// 	char buf[2];
// 	//set second
// 	buf[0]=0x00;
// 	buf[1]=0x50;
// 	i2c_write_blocking(I2C_PORT,addr,buf,2,false);
// 	//set minute
// 	buf[0]=0x01;
// 	buf[1]=0x07;
// 	i2c_write_blocking(I2C_PORT,addr,buf,2,false);
// 	//set hour
// 	buf[0]=0x02;
// 	buf[1]=0x14;
// 	i2c_write_blocking(I2C_PORT,addr,buf,2,false);
// 	//set weekday
// 	buf[0]=0x03;
// 	buf[1]=0x05;
// 	i2c_write_blocking(I2C_PORT,addr,buf,2,false);
// 	//set day
// 	buf[0]=0x04;
// 	buf[1]=0x10;
// 	i2c_write_blocking(I2C_PORT,addr,buf,2,false);
// 	//set month
// 	buf[0]=0x05;
// 	buf[1]=0x11;
// 	i2c_write_blocking(I2C_PORT,addr,buf,2,false);
// 	//set year
// 	buf[0]=0x06;
// 	buf[1]=0x22;
// 	i2c_write_blocking(I2C_PORT,addr,buf,2,false);
// 	return 0;
// }

void ds3231ReadTime() 
{   
	uint8_t val = 0x00;  
	i2c_write_blocking(I2C_PORT,addr,&val,1,true);
	i2c_read_blocking(I2C_PORT,addr,buf,7,false);
} 

int main()  
{
	int i = 0;
	stdio_init_all();
	i2c_init(I2C_PORT,100*1000);
	gpio_set_function(I2C_SDA,GPIO_FUNC_I2C);
	gpio_set_function(I2C_SCL,GPIO_FUNC_I2C);
	gpio_pull_up(I2C_SDA);
	gpio_pull_up(I2C_SCL);
    printf("DS3231 Test Program ...\n\n"); 
   	
	//ds3231SetTime(); //enable when setting time manually 
    while(1)  
    {  
       	ds3231ReadTime();
		buf[0] = buf[0]&0x7F; //sec
		buf[1] = buf[1]&0x7F; //min
		buf[2] = buf[2]&0x3F; //hour
		buf[3] = buf[3]&0x07; //week
		buf[4] = buf[4]&0x3F; //day
		buf[5] = buf[5]&0x1F; //mouth
		//year/month/day
		printf("20%02x/%02x/%02x  ",buf[6],buf[5],buf[4]);
		//hour:minute/second
		printf("%02x:%02x:%02x  ",buf[2],buf[1],buf[0]);
		//weekday
		printf("%s\n",week[(unsigned char)buf[3]-1]);
		sleep_ms(1000); 

		//if(buf[6]==0x00 && buf[5]==0x01 && buf[4]==0x01)
		//{
		//	ds3231SetTime(); 
		//}
	}  
	return 0;
}
