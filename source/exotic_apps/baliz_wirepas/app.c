/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 */

/*
 * \file    app.c
 * \brief   This file is a template for writing a custom application
 */

#include <stdlib.h>

#include "api.h"
#include "node_configuration.h"
#include "shared_data.h"
#include "app_scheduler.h"


#define DEBUG_LOG_MODULE_NAME "CUSTOM_APP"
/** To activate logs, configure the following line with "LVL_INFO". */
#define DEBUG_LOG_MAX_LEVEL LVL_NOLOG

#include "debug_log.h"
#include "../../../mcu/hal_api/hal_api.h"
#include "../../../board/baliz_V3E/board.h"
#include "../../../mcu/nrf/common/vendor/hal/nrf_gpio.h"
#include "../../../util/debug_log.h"
#include "../../../util/node_configuration.h"
#include "../../../api/wms_app.h"
#include "../../../libraries/scheduler/app_scheduler.h"
#include "../../../libraries/shared_data/shared_data.h"

/** Period to send data */
#define DEFAULT_PERIOD_S    10
#define DEFAULT_PERIOD_MS   (DEFAULT_PERIOD_S*1000)

/** Time needed to execute the periodic work, in us */
#define EXECUTION_TIME_US 500

/** Endpoint to change the sending period value */
#define SET_PERIOD_EP  10

#define DATA_EP 1

/** Period to send measurements, in ms */
static uint32_t period_ms;

typedef struct __attribute__((packed))
{
    /** periodic message interval in milliseconds */
    uint32_t period_ms;
} payload_periodic_t;

/**
 * @brief   Task to send periodic message.
 * @return  next period
 */

static uint32_t send_data_task(void)
{
    static uint32_t id = 0; // Value to send

    // Create a data packet to send
    app_lib_data_to_send_t data_to_send;
    data_to_send.bytes = (const uint8_t *) &id;
    data_to_send.num_bytes = sizeof(id);
    data_to_send.dest_address = APP_ADDR_ANYSINK;
    data_to_send.src_endpoint = DATA_EP;
    data_to_send.dest_endpoint = DATA_EP;
    data_to_send.qos = APP_LIB_DATA_QOS_HIGH;
    data_to_send.delay = 0;
    data_to_send.flags = APP_LIB_DATA_SEND_FLAG_NONE;
    data_to_send.tracking_id = APP_LIB_DATA_NO_TRACKING_ID;

    // Send the data packet
    Shared_Data_sendData(&data_to_send, NULL);

    // Increment value to send
    id++;

    // Inform the stack that this function should be called again in
    // period_ms microseconds. By returning APP_SCHEDULER_STOP_TASK,
    // the stack won't call this function again.
    return period_ms;
}

/**
 * \brief   Data reception callback
 * \param   data
 *          Received data, \ref app_lib_data_received_t
 * \return  Result code, \ref app_lib_data_receive_res_e
 */
static app_lib_data_receive_res_e dataReceivedCb(
    const shared_data_item_t * item,
    const app_lib_data_received_t * data)
{
    LOG(LVL_INFO, "dataReceivedCb");
    if ((data->num_bytes < 1) || (data->num_bytes!= sizeof(payload_periodic_t)))
    {
        // Data was not for this application
        LOG(LVL_INFO, "dataReceiveCb - data was not for this application");
        return APP_LIB_DATA_RECEIVE_RES_NOT_FOR_APP;
    }

    LOG(LVL_INFO, "dataReceivedCb");
    payload_periodic_t payload = *((payload_periodic_t *)data->bytes);

    /* Change the period for a new one */
    period_ms = payload.period_ms;

    // Data handled successfully
    return APP_LIB_DATA_RECEIVE_RES_HANDLED;
}

/*Unicast messages filter*/
static shared_data_item_t alltype_packets_filter =
{
    .cb = dataReceivedCb,
    .filter = {
        .mode = SHARED_DATA_NET_MODE_ALL,
        /* Filtering by source endpoint. */
        .src_endpoint = SET_PERIOD_EP,
        /* Filtering by destination endpoint. */
        .dest_endpoint = SET_PERIOD_EP,
        .multicast_cb = NULL
    }
};


/**
 * \brief   Initialization callback for application
 *
 * This function is called after hardware has been initialized but the
 * stack is not yet running.
 *
 */
void App_init(const app_global_functions_t * functions)
{
    LOG_INIT();
    LOG(LVL_INFO, "App_init");

    /* Power switch off GPS */
    nrf_gpio_cfg(GPS_POWER_EN, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_clear(GPS_POWER_EN);

    /* Power switch off ADXL */
    nrf_gpio_cfg(ADXL_POWER_EN,NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_clear(ADXL_POWER_EN);

    /* LEDs */
    nrf_gpio_cfg(LEDR_PIN, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_set(LEDR_PIN);
    nrf_gpio_cfg(LEDG_PIN, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_set(LEDG_PIN);
    nrf_gpio_cfg(LEDB_PIN, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_set(LEDB_PIN);

    /* GPS */
    nrf_gpio_cfg(GPS_ON, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_set(GPS_ON);
    nrf_gpio_cfg(GPS_BACKUP, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
   nrf_gpio_pin_set(GPS_BACKUP);

    /* UART GPS */
	nrf_gpio_cfg_input(GPS_UART_RX, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg(GPS_UART_TX, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);

    /* SigFox */
    nrf_gpio_cfg(SIGFOX_RST, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_cfg(SIGFOX_DP_WU, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
    nrf_gpio_pin_set(SIGFOX_RST);	   /* Must be connected to VDD */
    nrf_gpio_pin_set(SIGFOX_DP_WU);

    /* UART SigFox */
	nrf_gpio_cfg_input(SIGFOX_UART_RX, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_cfg(SIGFOX_UART_TX, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);

    /* LSM6DSL Interrupt */
    nrf_gpio_cfg_input(LSM6_INT1, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(LSM6_INT2, NRF_GPIO_PIN_NOPULL);

    /* ADXL Interrupt + IO */
    nrf_gpio_cfg_input(ADXL_INT2, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(ADXL_INT1, NRF_GPIO_PIN_NOPULL);
//	nrf_gpio_cfg(ADXL_CS, NRF_GPIO_PIN_NOPULL);
//   nrf_gpio_pin_set(ADXL_CS);

    /* LIS2MDL Interrupt */
//	nrf_gpio_cfg_input(LIS2_INT, NRF_GPIO_PIN_NOPULL);

    /* ST25DV Interrupt */
    nrf_gpio_cfg_input(ST25DV_GPO_INT, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg(ST25DV_LPD, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
   nrf_gpio_pin_set(ST25DV_LPD);

    /* Gauge */
    nrf_gpio_cfg_input(GAUGE_ALERT, NRF_GPIO_PIN_PULLUP);

    nrf_gpio_cfg(SDCARD_CS, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT,
                 (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
   nrf_gpio_pin_set(SDCARD_CS);

    // I2C
    nrf_gpio_cfg_input(I2C_SDA, (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(I2C_SCL, (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL);
    // SPI
    nrf_gpio_cfg_input(SPI_MOSI, (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_PULLDOWN);
    nrf_gpio_cfg_input(SPI_MISO, (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_PULLDOWN);
    nrf_gpio_cfg_input(SPI_SCLK, (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_PULLDOWN);

    // Basic configuration of the node with a unique node address
    if (configureNodeFromBuildParameters() != APP_RES_OK)
    {
        // Could not configure the node
        // It should not happen except if one of the config value is invalid
        return;
    }

    // Set a periodic callback to be called after DEFAULT_PERIOD_MS
    period_ms = DEFAULT_PERIOD_MS;
    App_Scheduler_addTask_execTime(send_data_task,
                                   APP_SCHEDULER_SCHEDULE_ASAP,
                                   EXECUTION_TIME_US);

    // Set callback for received unicast messages
    Shared_Data_addDataReceivedCb(&alltype_packets_filter);

    // Start the stack
    lib_state->startStack();
}

