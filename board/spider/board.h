/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * @file
 *
 * Template to be used for nRF52 board definitions
 */

#ifndef _BOARD_NRF52_TEMPLATE_BOARD_H_
#define _BOARD_NRF52_TEMPLATE_BOARD_H_

// Define the SPI instance to use
#define USE_SPI0
// SPI Port pins
#define BOARD_SPI_SCK_PIN   29
#define BOARD_SPI_MOSI_PIN  25
#define BOARD_SPI_MISO_PIN  28

// SPI Chip Select for various sensor
#define BOARD_SPI_LIS2DH12_CS_PIN 8
#define BOARD_SPI_BME280_CS_PIN   3

// INT pins
#define BOARD_LIS2DH12_INT1_PIN 2
//#define BOARD_LIS2DH12_INT2_PIN 6



// List of GPIO pins for the LEDs on the board: LED1, LED2
#define BOARD_LED_PIN_LIST              {17, 19}

// Active low polarity for LEDs
#define BOARD_LED_ACTIVE_LOW            true

// List of GPIO pins for buttons on the board: Button1
#define BOARD_BUTTON_PIN_LIST           {13}

// Active low polarity for buttons
#define BOARD_BUTTON_ACTIVE_LOW         true

// Active internal pull-up for buttons
#define BOARD_BUTTON_INTERNAL_PULL      true

// The board supports DCDC (#define BOARD_SUPPORT_DCDC)
// Since SDK v1.2 (bootloader > v7) this option has been move to
// board/<board_name>/config.mk. Set board_hw_dcdc to yes to enable DCDC.
#ifdef BOARD_SUPPORT_DCDC
#error This option has been moved to board/<board_name>/config.mk
#endif

/*#define BOARD_BUTTON_INTERNAL_PULL true*/


#define BOARD_POWER_VSEL       18u

#define BOARD_CSD               22u
#define BOARD_CTX               29u
#define BOARD_CHL               27u
#define BOARD_CPS               25u


#define I2C_SDA            19u
#define I2C_SCL            20u



#define ST25DV_GPO_INT     3u
#define ST25DV_LPD         2u

#define MAX44009_INT       31u

#define LSM6_INT1          13u
#define LSM6_INT2          12u

#define LIS2_INT           18u


#endif /* _BOARD_NRF52_TEMPLATE_BOARD_H_ */
