#ifndef __PINS_H__
#define __PINS_H__

// Import the Arduino library
#include "Arduino.h"

// --------------  Pins  --------------
/*
// * = F103C8-CB    | DIGITAL | ANALOG | USART      | TWI       | SPI        | SPECIAL   |
//                  |---------|--------|------------|-----------|------------|-----------|
#define PA0  A0  // | 0       | A0     |            |           |            |           |
#define PA1  A1  // | 1       | A1     |            |           |            |           |
#define PA2  A2  // | 2       | A2     | USART2_TX  |           |            |           |
#define PA3  A3  // | 2       | A2     | USART2_RX  |           |            |           |
#define PA4  A4  // | 4       | A4     |            |           | SPI1_SS    |           |
#define PA5  A5  // | 5       | A5     |            |           | SPI1_SCK   |           |
#define PA6  A6  // | 6       | A6     |            |           | SPI1_MISO  |           |
#define PA7  A7  // | 7       | A7     |            |           | SPI1_MOSI  |           |
#define PA8  8   // | 8       |        |            |           |            |           |
#define PA9  9   // | 9       |        | USART1_TX  |           |            |           |
#define PA10 10  // | 10      |        | USART1_RX  |           |            |           |
#define PA11 11  // | 11      |        |            |           |            | USB_DN    | CAN_IN
#define PA12 12  // | 12      |        |            |           |            | USB_DP    | CAN_OUT
#define PA13 13  // | 13      |        |            |           |            | SWD_SWDIO |
#define PA14 14  // | 14      |        |            |           |            | SWD_SWCLK |
#define PA15 15  // | 15      |        |            |           | SPI1_SS    |           |
//                  |---------|--------|------------|-----------|------------|-----------|
#define PB0  A8  // | 16      | A8     |            |           |            |           |
#define PB1  A9  // | 17      | A9     |            |           |            |           |
#define PB2  18  // | 18      |        |            |           |            | BOOT1     |
#define PB3  19  // | 19      |        |            |           | SPI1_SCK   |           |
#define PB4  20  // | 20      |        |            |           | SPI1_MISO  |           |
#define PB5  21  // | 21      |        |            |           | SPI1_MOSI  |           |
#define PB6  22  // | 22      |        | USART1_TX  | TWI1_SCL  |            |           |
#define PB7  23  // | 23      |        | USART1_RX  | TWI1_SDA  |            |           |
#define PB8  24  // | 24      |        |            | TWI1_SCL  |            |           |
#define PB9  25  // | 25      |        |            | TWI1_SDA  |            |           |
#define PB10 26  // | 26      |        | USART3_TX* | TWI2_SCL* |            |           |
#define PB11 27  // | 27      |        | USART3_RX* | TWI2_SDA* |            |           |
#define PB12 28  // | 28      |        |            |           | SPI2_SS*   |           |
#define PB13 29  // | 29      |        |            |           | SPI2_SCK*  |           |
#define PB14 30  // | 30      |        |            |           | SPI2_MISO* |           |
#define PB15 31  // | 31      |        |            |           | SPI2_MOSI* |           |
//                  |---------|--------|------------|-----------|------------|-----------|
#define PC13 32  // | 32      |        |            |           |            |           |
#define PC14 33  // | 33      |        |            |           |            | OSC32_IN  |
#define PC15 34  // | 34      |        |            |           |            | OSC32_OUT |
//                  |---------|--------|------------|-----------|------------|-----------|
#define PD0  35  // | 35      |        |            |           |            | OSC_IN    |
#define PD1  36  // | 36      |        |            |           |            | OSC_OUT   |
//                  |---------|--------|------------|-----------|------------|-----------|
*/

// OLED Mappings
#define OLED_CS_PIN   PB_12
#define OLED_RST_PIN  PA_8
#define OLED_RS_PIN   PB_13
#define OLED_SCLK_PIN PB_15	//(D0)
#define OLED_SDIN_PIN PB_14	//(D1)

// Button mappings
#define DOWN_BUTTON_PIN         PA_3
#define BACK_BUTTON_PIN         PB_0
#define SELECT_BUTTON_PIN       PB_1

// Dip switch mappings
// PIN     |    Normal orientation      |  Reversed orientation
// DIP_1   |    Microstep 1             |  Calibration mode
// DIP_2   |    Microstep 2             |  Closed/Open loop
// DIP_3   |    Closed/Open loop        |  Microstep 2
// DIP_4   |    Calibration mode        |  Microstep 1
#define DIP_1_PIN  PA_15
#define DIP_2_PIN  PB_3
#define DIP_3_PIN  PB_11
#define DIP_4_PIN  PB_10

// LED pin
#define LED_PIN PC_13

// Motor mappings
#define COIL_A_POWER_OUTPUT_PIN  PB_5
#define COIL_B_POWER_OUTPUT_PIN  PB_4
#define COIL_A_DIR_1_PIN  PB_6
#define COIL_A_DIR_2_PIN  PB_7
#define COIL_B_DIR_1_PIN  PB_8
#define COIL_B_DIR_2_PIN  PB_9

// Encoder SPI interface
#define ENCODER_CS_PIN    PA_4 // SPI1_SS
#define ENCODER_SCK_PIN   PA_5 // SPI1_SCK
#define ENCODER_MISO_PIN  PA_6 // SPI1_MISO
#define ENCODER_MOSI_PIN  PA_7 // SPI1_MOSI

// Stepping interface
#define STEP_PIN       PA_0
#define ENABLE_PIN     PA_2
#define DIRECTION_PIN  PA_1

// CAN bus pins
#define CAN_IN_PIN   PA_11
#define CAN_OUT_PIN  PA_12

// UART pins
#define USART1_TX PA9
#define USART1_RX PA10


#endif // ! __PINS_H__