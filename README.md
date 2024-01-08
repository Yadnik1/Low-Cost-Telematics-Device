# Low-Cost Telematics Device for Motor Vehicle Usage-Based Applications

## Overview
This repository details the development of a low-cost telematics device aimed at revolutionizing the motor vehicle insurance sector in India. The device captures critical vehicle kinematics and dynamics data, enabling "pay as you drive" (PAYD) and "pay how you drive" (PHYD) insurance policies. This project addresses the gap in the Indian market for an affordable, after-sales telematics device to assist insurance companies in offering more personalized services.

## Problem Statement
In India, the lack of differentiated underwriting for vehicle insurance fails to consider individual driving patterns and vehicle usage. With over 210 million registered vehicles, the opportunity to tailor insurance policies to driving behavior represents a significant untapped market. This device aims to incentivize safer driving and provide accurate data to support insurance claims and reduce fraud.

## Power Management
The telematics device is powered via a car charger and carefully manages power to support the Raspberry Pi Pico and various sensors. A specialized IC ensures stable voltage for optimal component performance.

## Interfacing Components
Components such as an OLED display, 6DOF IMU, GPS module, and environmental sensors interface with the microcontroller, providing real-time data on vehicle behavior and environmental conditions.

## Data Storage
Data is logged onto a micro SD card in CSV format, allowing for extensive data analysis and application in developing insurance models.

# Development

### Documentation and Project Planning
All the project planning was done from ground up and all the documentaion of the project can be seen in the documentation folder:

### Component Selection
The component selection for the project was done by benchmarking the STEVAL-STWINKT1B industrial node with the VBOX IMU in real world conditions.

Additional indoor tests were also done by making a setup by laser cutting an acryclic sheet and mounting 5 industrial nodes on it to understand the sensor Systematic Errors and Random Errors in the sensors.


### Designing Breakout boards and Printed Circuit Boards

1) The project involved the use of AECQ100 grade IMU and Temperature and Humidity sensors (SHT30A)  sensors for which custom breakout boards had to be created in KiCAD and hold soldered:

2) The PCB of the device was a 2 layered PCB developed using KiCAD consisting of the sensor subsytem on the top layer and power circuitry at the bottom:

### Developing firmware:

The firmware for the device was written by using the Raspberry Pi Pico as the Microcontroller in C.
The individual C code of the components as well as the entire systems firmware along with the CMAKE files can be seen in the Firmware folder

### Post-Processing sensor data:

After retreiving the data from the device errors and noise have to be addressed, for which there is a seperate repository where I have mention the different methodologies and filers used in Python for the post-processing process. 

## Usage
To operate the device:
1. Connect to the vehicle's power source.
2. Interact with the OLED display for instructions and status.
3. Retrieve and analyze data from the SD card as needed.
