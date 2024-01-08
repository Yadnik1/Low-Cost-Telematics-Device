#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/spi.h"
#include "lwgps.h"
#include "sd_card.h"
#include "ff.h"

// SHT30A sensor code
#define SHT30A_ADDRESS 0x45
#define SHT30A_COMMAND {0x2C, 0x06}
#define SHT30A_SDA_PIN 16
#define SHT30A_SCL_PIN 17

// GPS code
#define GPS_UART_ID uart1
#define GPS_TX_PIN 4
#define GPS_RX_PIN 5
lwgps_t hgps;

// RTC code
#define RTC_I2C_PORT i2c0
#define RTC_I2C_SCL 20
#define RTC_I2C_SDA 21
#define RTC_ADDR 0x68

// IMU code
#define IMU_SPI_PORT spi0
#define IMU_PIN_MISO 4
#define IMU_PIN_CS 5
#define IMU_PIN_SCK 6
#define IMU_PIN_MOSI 7
#define READ_BIT 0x80

// SD Card code
#define FILENAME "data_log.csv"

// Function prototypes
void initSHT30A();
void readSHT30A(float *temp, float *hum);
void initGPS();
void readGPS(char *gpsData);
void initRTC();
void readRTC(char *timeStr);
void initIMU();
void readIMU(float *accel, float *gyro);
void initSD();
void logData(FIL *file, float temp, float hum, char *gpsData, char *timeStr, float *accel, float *gyro);

int main() {
    stdio_init_all();
    sleep_ms(2000); // Give time for stdio initialization

    // Sensor initialization
    initSHT30A();
    initGPS();
    initRTC();
    initIMU();
    initSD();

    // SD Card Initialization
    FATFS fs;
    FIL fil;
    f_mount(&fs, "", 0);
    f_open(&fil, FILENAME, FA_WRITE | FA_CREATE_ALWAYS);

    while (true) {
        float temp, hum;
        char gpsData[128];
        char timeStr[20];
        float accel[3], gyro[3];

        // Read from sensors
        readSHT30A(&temp, &hum);
        readGPS(gpsData);
        readRTC(timeStr);
        readIMU(accel, gyro);

        // Log data to SD card
        logData(&fil, temp, hum, gpsData, timeStr, accel, gyro);

        sleep_ms(5000);
    }

    // Cleanup
    f_close(&fil);
    f_unmount("");

    return 0;
}


// SHT30A Initialization
void initSHT30A() {
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(SHT30A_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SHT30A_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SHT30A_SDA_PIN);
    gpio_pull_up(SHT30A_SCL_PIN);
}

// SHT30A Data Reading
void readSHT30A(float *temp, float *hum) {
    uint8_t command[2] = {0x2C, 0x06};
    uint8_t val[6];

    i2c_write_blocking(i2c0, SHT30A_ADDRESS, command, 2, false);
    i2c_read_blocking(i2c0, SHT30A_ADDRESS, val, 6, false);

    *temp = -45 + (175 * (val[0] << 8 | val[1]) / 65535.0);
    *hum = 100 * (val[3] << 8 | val[4]) / 65535.0;
}

void initGPS() {
    // Initialize GPS
    uart_init(GPS_UART_ID, 9600);
    gpio_set_function(GPS_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(GPS_RX_PIN, GPIO_FUNC_UART);
    lwgps_init(&hgps);
}

// GPS Data Reading
void readGPS(char *gpsData) {
    char gps_rx_data = uart_getc(GPS_UART_ID);

    // Assuming gpsData is a sufficiently large buffer
    sprintf(gpsData, "Data: %c", gps_rx_data);

    lwgps_process(&hgps, &gps_rx_data, 1);

    if (hgps.is_valid) {
        sprintf(gpsData, "Latitude: %f, Longitude: %f, Altitude: %f", hgps.latitude, hgps.longitude, hgps.altitude);
    }
}

// RTC Initialization
void initRTC() {
    i2c_init(RTC_I2C_PORT, 100 * 1000);
    gpio_set_function(RTC_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(RTC_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(RTC_I2C_SDA);
    gpio_pull_up(RTC_I2C_SCL);
}

// RTC Data Reading
void readRTC(char *timeStr) {
    uint8_t val = 0x00;
    char buf[7];
    i2c_write_blocking(RTC_I2C_PORT, RTC_ADDR, &val, 1, true);
    i2c_read_blocking(RTC_I2C_PORT, RTC_ADDR, buf, 7, false);

    sprintf(timeStr, "20%02x/%02x/%02x %02x:%02x:%02x", buf[6], buf[5], buf[4], buf[2], buf[1], buf[0]);
}

// IMU Initialization
void initIMU() {
    spi_init(IMU_SPI_PORT, 500 * 1000);
    gpio_set_function(IMU_PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(IMU_PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(IMU_PIN_MOSI, GPIO_FUNC_SPI);

    gpio_init(IMU_PIN_CS);
    gpio_set_dir(IMU_PIN_CS, GPIO_OUT);
    gpio_put(IMU_PIN_CS, 1);

    mpu9250_init();
}

// IMU Data Reading
void readIMU(float *accel, float *gyro) {
    int16_t acceleration[3], gyroscope[3];
    mpu9250_read_raw(acceleration, gyroscope);

    for (int i = 0; i < 3; i++) {
        accel[i] = acceleration[i] / 16384.0;
        gyro[i] = gyroscope[i] / 131.0;
    }
}

// SD Card Initialization
void initSD() {
    if (!sd_init_driver()) {
        printf("ERROR: Could not initialize SD card\r\n");
        while (true);
    }
}

// Logging Data to SD Card
void logData(FIL *file, float temp, float hum, char *gpsData, char *timeStr, float *accel, float *gyro) {
    char logLine[256];
    sprintf(logLine, "%s, Temp: %f, Hum: %f, Accel: %f,%f,%f, Gyro: %f,%f,%f\n", timeStr, temp, hum, accel[0], accel[1], accel[2], gyro[0], gyro[1], gyro[2]);
    f_puts(logLine, file);
}

