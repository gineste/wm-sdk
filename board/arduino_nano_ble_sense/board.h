/* Copyright 2018 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * @file
 *
 * Board definition for the
 * <a href="https://www.nordicsemi.com/Software-and-tools/Development-Kits/nRF52840-Dongle">Nordic semiconductor PCA10059 Dongle</a>
 */
#ifndef BOARD_ARDUINO_NANO_BLE_SENSE_BOARD_H_
#define BOARD_ARDUINO_NANO_BLE_SENSE_BOARD_H_

// PCA10059 nRF52840 USB Dongle
// Note: This board is powered by USB +5V directly to MCU VDDH.
// Do not try to use the new VDDH-related DCDC0 converter without understanding
// the chip errata 197 and 202 (i.e. prefer to use REG0 in default LDO mode).
// The default I/O voltage (VDD) is 1.8V in this configuration. To change that,
// use first_boot() to set the correct I/O voltage (UICR->REGOUT0).
// The old nrf52832-style DCDC can still be used.

// NRF_GPIO is mapped to NRF_P0 , for pins P0.00 ... P0.31
// Use NRF_P1 for pins P1.00 ... P1.15
// With nrf_gpio.h, use SW_pin (logical pins, port-aware)

/**
NRF_P0  SW_pin  PCA10056       PCA10059       Notes (recommended usage)
------------------------------------------------------------------------
P0.00   0       [XTAL 32k]     [XTAL 32k]
P0.01   1       [XTAL 32k]     [XTAL 32k]
P0.02   2       AIN0_A5/SCL           (low freq)
P0.03   3       AIN1_A7               (low freq)
P0.04   4       AIN2_A1     
P0.05   5       NP      
P0.06   6       LB       
P0.07   7       NP       
P0.08   8       NP       
P0.09   9       D7/NFC1              (low freq)
P0.10   10      D8/NFC2               (low freq)
P0.11   11      NP 
P0.12   12      NP  
P0.13   13      SCK_LEDORANGE 
P0.14   14      SDA1  
P0.15   15      SCL1  
P0.16   16      LG    
P0.17   17      NP  
P0.18   18      nRESET         
P0.19   19      INT_APDS    
P0.20   20      APDS_PWR     
P0.21   21      D8            
P0.22   22      VDD_ENV               
P0.23   23      D7     
P0.24   24      LR 
P0.25   25      PDMDIN 
P0.26   26      PDMCLK         
P0.27   27      D9         
P0.28   28      AIN4_A6            (low freq)
P0.29   29      AIN5_A3            (low freq)
P0.30   30      AIN6_A2            (low freq)
P0.31   31      AIN7_A4/SDA        (low freq)

NRF_P1:
P1.00   32      R_PULLUP  
P1.01   33      MOSI    (low freq)
P1.02   34      D10     (low freq)
P1.03   35      TX      (low freq)
P1.04   36      NP      (low freq)
P1.05   37      NP      (low freq)
P1.06   38      NP           nSW1           (low freq)
P1.07   39      NP           1.07           (low freq)
P1.08   40      MISO           -
P1.09   41      LED_PWR           
P1.10   42      RX           1.10           (low freq)
P1.11   43      D2           1.11           (low freq)
P1.12   44      D3           -              (low freq)
P1.13   45      D5           1.13           (low freq)
P1.14   46      D6           -              (low freq)
P1.15   47      D4           1.15           (low freq)
*/
#define BOARD_UART_IRQ_PIN              2
// Serial port pins
#define BOARD_USART_TX_PIN              35
#define BOARD_USART_RX_PIN              42

// List of GPIO pins for the LEDs on the board: LD1, LD2 R, G, B
#define BOARD_LED_PIN_LIST              {6, 13, 24, 41 } //6,

// Active low polarity for LEDs
#define BOARD_LED_ACTIVE_LOW            true

// List of GPIO pins for buttons on the board: SW1
//#define BOARD_BUTTON_PIN_LIST           {38, 42}

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




#endif /* BOARD_ARDUINO_NANO_BLE_SENSE_BOARD_H_ */
