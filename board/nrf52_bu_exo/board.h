/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */
/**
NRF_P0  SW_pin  PCA10056       PCA10059       Notes (recommended usage)
------------------------------------------------------------------------
P0.00   0       [XTAL 32k]     [XTAL 32k]
P0.01   1       [XTAL 32k]     [XTAL 32k]
P0.02   2       gpio/AIN0      0.02           (low freq)
P0.03   3       gpio/AIN1      -              (low freq)
P0.04   4       gpio/AIN2      0.04
P0.05   5       UART_RTS       -
P0.06   6       UART_TX        nLED1
P0.07   7       UART_CTS       -
P0.08   8       UART_RX        nLED2red
P0.09   9       gpio/NFC1      0.09           (low freq)
P0.10   10      gpio/NFC2      0.10           (low freq)
P0.11   11      gpio/nBUTTON1  0.11
P0.12   12      gpio/nBUTTON2  nLED2blue
P0.13   13      gpio/nLED1     0.13
P0.14   14      gpio/nLED2     0.14
P0.15   15      gpio/nLED3     0.15
P0.16   16      gpio/nLED4     -
P0.17   17      gpio/FLASH     0.17
P0.18   18      nRESET         nRESET
P0.19   19      gpio/FLASH     nRESET         (QSPI/SCK)
P0.20   20      gpio/FLASH     0.20
P0.21   21      gpio/FLASH     nRESET         (QSPI)
P0.22   22      gpio/FLASH     0.22           (QSPI)
P0.23   23      gpio/FLASH     nRESET         (QSPI)
P0.24   24      gpio/nBUTTON3  0.24
P0.25   25      gpio/nBUTTON4  nRESET
P0.26   26      gpio           0.26
P0.27   27      gpio           -
P0.28   28      gpio/AIN4      -              (low freq)
P0.29   29      gpio/AIN5      0.29           (low freq)
P0.30   30      gpio/AIN6      -              (low freq)
P0.31   31      gpio/AIN7      0.31           (low freq)

NRF_P1:
P1.00   32      gpio/SWO       1.00           (QSPI)
P1.01   33      gpio           1.01           (low freq)
P1.02   34      gpio           1.02           (low freq)
P1.03   35      gpio           -              (low freq)
P1.04   36      gpio           1.04           (low freq)
P1.05   37      gpio           -              (low freq)
P1.06   38      gpio           nSW1           (low freq)
P1.07   39      gpio           1.07           (low freq)
P1.08   40      gpio           -
P1.09   41      gpio           nLED2green
P1.10   42      gpio           1.10           (low freq)
P1.11   43      gpio           1.11           (low freq)
P1.12   44      gpio           -              (low freq)
P1.13   45      gpio           1.13           (low freq)
P1.14   46      gpio           -              (low freq)
P1.15   47      gpio           1.15           (low freq)
*/
/**
 * @file
 *
 * Template to be used for nRF52 board definitions
 */

#ifndef _BOARD_NRF52_TEMPLATE_BOARD_H_
#define _BOARD_NRF52_TEMPLATE_BOARD_H_

// Serial port pins
/**
 * @brief   Transmission pin number
 *
 * @note in order for application to use USART, see @ref
 * source_makefile_hal_uart "here".
 */
#define BOARD_USART_TX_PIN              6
/**
 * @brief Reception pin number
 */
#define BOARD_USART_RX_PIN              8
/**
 * @brief CTS pin number, for @ref USE_USART_HW_FLOW_CONTROL
 */
#define BOARD_USART_CTS_PIN             7
/**
 * @brief RTS pin number, for @ref USE_USART_HW_FLOW_CONTROL
 */
#define BOARD_USART_RTS_PIN             5

/**
 * @brief Interrupt pin for dual mcu app, unread indication
 *
 * This only used in @ref source/dualmcu_app.c "dualmcu_app" application to
 * announce with GPIO pin that there is incoming indication to be read from
 * device.
 *
 * It is optional definition. If not present, no irq pin is present
 */
#define BOARD_UART_IRQ_PIN              11

/**
 * @brief   LED definitions
 *
 * If board contains LEDs, they are defined here. If not defined, a dummy
 * LED driver is compiled, so that applications can use the LED driver
 * unconditionally.
 *
 * For Nordic nRF52 family, LEDs are defined as list of GPIO pin numbers
 *
 * @note in order for application to use LEDs, see @ref source_makefile_hal_led
 * "here".
 */
#define BOARD_LED_PIN_LIST {6, 7, 8}

/**
 * @brief   LED GPIO polarity
 *
 * If LEDs turn on when the GPIO pin is driven low, this setting is true. This
 * is the case for many nRF52 boards, such as the PCA10040 and PCA10056.
 * Otherwise, if a LED is lit when the the GPIO pin is driven high, this
 * setting should be set to false.
 */
#define BOARD_LED_ACTIVE_LOW true

/**
 * @brief Button definitions
 *
 * Any buttons present on the board are defined here. If not defined, a dummy
 * button driver is compiled, so that applications can use the button driver
 * unconditionally.
*
 * For Nordic nRF52 family, buttons are defined simply by the GPIO numbers.
 *
 * @note in order for application to use buttons, see @ref
 * source_makefile_hal_button "here".
 */
#define BOARD_BUTTON_PIN_LIST {13, 14, 15, 16}

/**
 * @brief   Button GPIO polarity
 *
 * If a button press pulls the GPIO pin low, this setting is true. This is the
 * case for many nRF52 boards, such as the PCA10040 and PCA10056. Otherwise, if
 * a button press pulls the GPIO pin high, this setting should be set to false.
 */
#define BOARD_BUTTON_ACTIVE_LOW true

/**
 * @brief   Button GPIO internal pull up/down
 *
 * Some buttons don't have any pull-up or pull-down resistor installed on the
 * board. They need it to be setup in software. Set
 * \ref BOARD_BUTTON_INTERNAL_PULL to true to enable internal pull-up(down).
 * Pull-up(down) is enabled when \ref BOARD_BUTTON_ACTIVE_LOW is true(false).
 */
#define BOARD_BUTTON_INTERNAL_PULL true

#endif /* _BOARD_NRF52_TEMPLATE_BOARD_H_ */
