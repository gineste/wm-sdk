/* Copyright 2021 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/**
 * @file
 *
 * Board definition for the Wuerth Electronic eiSos radio module 261101102xxxx
 */
#ifndef BOARD_WUERTH_261101102XXXX_BOARD_H_
#define BOARD_WUERTH_261101102XXXX_BOARD_H_

// NRF_GPIO is mapped to NRF_P0 , for pins P0.00 ... P0.31
// Use NRF_P1 for pins P1.00 ... P1.15
// With nrf_gpio.h, use SW_pin (logical pins, port-aware)

/**
NRF_P   SW_pin  Module pad   function
---------------------------------------
P0.00   0       11           [XTAL 32k]
P0.01   1       12           [XTAL 32k]
P0.02   2        7           gpio, DATA_IND
P0.03   3       17           gpio
P0.07   7       B6           gpio
P0.09   9       B1           gpio
P0.10   10      B2           gpio
P0.11   11      15           UART_RTS
P0.12   12      16           UART_CTS
P0.18   18       6           nRESET
P0.19   19       9           gpio, LED1
P0.21   21      B5           gpio
P0.22   22      10           gpio, LED2
P0.23   23      B3           gpio
P1.00   32      B4           gpio
P1.08   40      13           UART_TX
P1.09   41      14           UART_RX
*/

// Serial port pins
#define BOARD_USART_TX_PIN              40
#define BOARD_USART_RX_PIN              41
#define BOARD_USART_CTS_PIN             12  /* For USE_USART_HW_FLOW_CONTROL */
#define BOARD_USART_RTS_PIN             11  /* For USE_USART_HW_FLOW_CONTROL */

// I2C pins
#define BOARD_I2C_SDA_PIN   BOARD_USART_CTS_PIN
#define BOARD_I2C_SCL_PIN   BOARD_USART_RTS_PIN

// List of GPIO pins
#define BOARD_GPIO_PIN_LIST            {19, /* P0.19 */\
                                        22, /* P0.22 */\
                                        41, /* P1.09. required by the dual_mcu app. usart wakeup pin (= BOARD_USART_RX) */\
                                        2}  /* P0.02. required by the dual_mcu app (indication signal) */

// User friendly name for GPIOs (IDs mapped to the BOARD_GPIO_PIN_LIST table)
#define BOARD_GPIO_ID_LED1              0 // mapped to pin P0.19
#define BOARD_GPIO_ID_LED2              1 // mapped to pin P0.22
#define BOARD_GPIO_ID_USART_WAKEUP      2 // mapped to pin P1.09
#define BOARD_GPIO_ID_UART_IRQ          3 // mapped to pin P0.02

// List of LED IDs
#define BOARD_LED_ID_LIST               {BOARD_GPIO_ID_LED1, BOARD_GPIO_ID_LED2}

// Active low polarity for LEDs
#define BOARD_LED_ACTIVE_LOW            false

#endif /* BOARD_WUERTH_261101102XXXX_BOARD_H_ */
