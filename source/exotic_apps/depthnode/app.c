/*
 *  ____  _  _   __   ____   __    ___    ____  _  _  ____  ____  ____  _  _  ____
 * (  __)( \/ ) /  \ (_  _) (  )  / __)  / ___)( \/ )/ ___)(_  _)(  __)( \/ )/ ___)
 *  ) _)  )  ( (  O )  )(    )(  ( (__   \___ \ )  / \___ \  )(   ) _) / \/ \\___ \
 * (____)(_/\_) \__/  (__)  (__)  \___)  (____/(__/  (____/ (__) (____)\_)(_/(____/
 *
 * Copyright (c) 2022 EXOTIC SYSTEMS. All Rights Reserved.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * Date:          24 01 2022 (dd MM YYYY)
 * Author:        Martin Cornu
 * Description:   Main program file app.c
 *
 */

#include <stdlib.h>

/****************************************************************************************
 * Include Files
 ****************************************************************************************/
#include "api.h"
#include "node_configuration.h"
#include "shared_data.h"
#include "app_scheduler.h"
#include "led.h"
#include "board.h"
#include "usart.h"
#include "drivers/VL53L1X/VL53L1X_Ext.h"
#include "SensorManager.h"
#include "util.h"

/************************************************************************
 * Defines
 ************************************************************************/
#define DEBUG_LOG_MODULE_NAME "DBX_WIIO_APP"
/** To activate logs, configure the following line with "LVL_INFO". */
#define DEBUG_LOG_MAX_LEVEL LVL_NOLOG

#include "debug_log.h"
#include "util.h"

/** Period to send data */
#define DEFAULT_PERIOD_S    5
#define DEFAULT_PERIOD_MS   (DEFAULT_PERIOD_S*1000)

/** Time needed to execute the periodic work, in us */
#define EXECUTION_TIME_US 5000

/** Endpoint to change the sending period value */
#define SET_PERIOD_EP  10

#define DATA_EP 12

#define SSP_PAYLOAD_SIZE_MAX       (uint8_t)160u

/************************************************************************
 * Private type declarations
 ************************************************************************/
typedef struct __attribute__((packed))
{
    /** periodic message interval in milliseconds */
    uint32_t period_ms;
} payload_periodic_t;

/************************************************************************
 * Private function declarations
 ************************************************************************/
static uint32_t send_data_task(void);
static app_lib_data_receive_res_e dataReceivedCb(
        const shared_data_item_t * item,
        const app_lib_data_received_t * data);

static e_VL53L1X_RangeStatus_t eReadTofData(uint16_t * p_pu16Distance);

/************************************************************************
 * Variable declarations
 ************************************************************************/
/** Period to send measurements, in ms */
static uint32_t period_ms;

/** TOF variables */
static s_VL53L1X_Context_t g_sVL53L1XContext = {
        .fp_vPinSet = &nrf_gpio_pin_set,                      /* Pointer function to pin set */
        .fp_vPinClear = &nrf_gpio_pin_clear,                   /* Pointer function to pin clear */  //DBG : no & sur fn..
        .fp_vDelay_ms = &VL53L1_WaitMs,               /* Function pointer to a timer in ms */
        .u32ShutdownPin = VL53_XSHUT
};

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

/****************************************************************************************
 * Public functions
 ****************************************************************************************/

/**
 * \brief   Initialization callback for application
 *
 * This function is called after hardware has been initialized but the
 * stack is not yet running.
 *
 */
void App_init(const app_global_functions_t * functions) {
    LOG_INIT();
    LOG(LVL_INFO, "App_init");

    // 2V8
    nrf_gpio_cfg_output(EN_2V8);
    nrf_gpio_cfg_output(LTC3336_IPK0);
    nrf_gpio_cfg_output(LTC3336_IPK1);
    nrf_gpio_pin_set(EN_2V8);
    nrf_gpio_pin_set(LTC3336_IPK0);
    nrf_gpio_pin_clear(LTC3336_IPK1);
    // gpio config

    // turn adxl off
    nrf_gpio_cfg_output(ADXL_POWER_EN);
    nrf_gpio_pin_set(ADXL_POWER_EN);
    nrf_gpio_cfg_output(ADXL_CS);
    nrf_gpio_pin_set(ADXL_CS);
    
    // leds
    nrf_gpio_cfg_output(LEDR_PIN);
    nrf_gpio_cfg_output(LEDG_PIN);
    nrf_gpio_cfg_output(LEDB_PIN);

    // VL53
    nrf_gpio_cfg_output(VL53_XSHUT);    
    nrf_gpio_pin_clear(VL53_XSHUT);

    //da16200
    nrf_gpio_cfg_output(DA16200_RST);
    nrf_gpio_cfg_output(DA16200_WPS);
    nrf_gpio_cfg_output(DA16200_PWR_KEY); 
    nrf_gpio_cfg_output(DA16200_WAKE); 

    nrf_gpio_pin_clear(DA16200_RST);
    nrf_gpio_pin_clear(DA16200_WPS);
    nrf_gpio_pin_clear(DA16200_PWR_KEY);
    nrf_gpio_pin_clear(DA16200_WAKE);

    //nfc
    nrf_gpio_cfg_output(NFC_LPD);
    nrf_gpio_pin_clear(NFC_LPD);
    nrf_gpio_cfg_input(NFC_INT, (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL);


    // I2C
    nrf_gpio_cfg_input(BOARD_I2C_SDA_PIN, (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(BOARD_I2C_SCL_PIN, (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_NOPULL);

    // SPI
    nrf_gpio_cfg_input(SPI_MOSI, (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_PULLDOWN);
    nrf_gpio_cfg_input(SPI_MISO, (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_PULLDOWN);
    nrf_gpio_cfg_input(SPI_SCLK, (nrf_gpio_pin_pull_t) NRF_GPIO_PIN_PULLDOWN);
    // UART
#if (DEBUG_LOG_MAX_LEVEL == LVL_NOLOG)
    Usart_setEnabled(false);
#endif

    // Init TOF
    if (eVL53L1X_ContextSet(g_sVL53L1XContext) == VL53L1X_ERROR_NONE) {
        if (eVL53L1X_SensorInit() != VL53L1X_ERROR_NONE) {
            LOG(LVL_WARNING, "Fail to configure VL53L1X !\r\n");
                nrf_gpio_pin_set(LEDR_PIN);

        }else{
            LOG(LVL_WARNING, "TOF INIT SUCCESS\r\n");
               
        }
    }

    //eVL53L1X_SensorShutdown();
     


    // Basic configuration of the node with a unique node address
    if (configureNodeFromBuildParameters() != APP_RES_OK) {
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

/****************************************************************************************
 * Private functions
 ****************************************************************************************/

/**
 * @brief   Task to send periodic message.
 * @return  next period
 */

static uint32_t send_data_task(void)
{ 
    
    uint8_t l_u8Data[2u] = {0};
    uint16_t l_u16Distance = 0;
  

    // Send distance only if measure is valid 
    e_VL53L1X_RangeStatus_t l_eRangeStatus = eReadTofData(&l_u16Distance);
     //   eVL53L1X_SensorShutdown();

    if (l_eRangeStatus == VL53L1_RANGESTATUS_RANGE_VALID)
    {
        l_u8Data[0u] = (l_u16Distance & 0xFF00) >> 8u;
        l_u8Data[1u] = (l_u16Distance & 0x00FF);

       // Create a data packet to send 
        app_lib_data_to_send_t data_to_send;
        data_to_send.bytes = (const uint8_t *) l_u8Data;
        data_to_send.num_bytes = sizeof(l_u8Data);
        data_to_send.dest_address = APP_ADDR_ANYSINK;
        data_to_send.src_endpoint = DATA_EP;
        data_to_send.dest_endpoint = DATA_EP;
        data_to_send.qos = APP_LIB_DATA_QOS_HIGH;
        data_to_send.delay = 0;
        data_to_send.flags = APP_LIB_DATA_SEND_FLAG_NONE;
        data_to_send.tracking_id = APP_LIB_DATA_NO_TRACKING_ID;

        //Send the data packet 
        Shared_Data_sendData(&data_to_send, NULL);

    }

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


static e_VL53L1X_RangeStatus_t eReadTofData(uint16_t * p_pu16Distance)
{
    LOG(LVL_INFO, "Read tof data\r\n");
    uint8_t l_au8Data[SSP_PAYLOAD_SIZE_MAX] = { 0u };
    uint8_t l_u8Size = 0u;
    uint8_t l_u8SensorE = 0u;
    e_VL53L1X_RangeStatus_t l_eRangingStatus = 0u;

    if (p_pu16Distance != NULL)
    {
        l_u8Size = 0u;
        memset(l_au8Data, '\0', SSP_PAYLOAD_SIZE_MAX);

        l_u8SensorE = u8SensorMngr_ProximityAvailable();
        if(l_u8SensorE == 0u) {
            vSensorMngr_ProximityWakeUp();
        }

        vSensorMngr_ProximityRead();
        vSensorMngr_ProximityGet(l_au8Data, &l_u8Size);
       *p_pu16Distance = U8_TO_U16(l_au8Data[0u], l_au8Data[1u]);
        l_eRangingStatus = l_au8Data[2u];
        LOG(LVL_INFO, "Distance = %hu ; status = %d\r\n", *p_pu16Distance, (uint8_t) l_eRangingStatus);
        vSensorMngr_ProximityShutdown();
    }

    // Inform the stack that this function should be called again in
    // period_ms microseconds. By returning APP_SCHEDULER_STOP_TASK,
    // the stack won't call this function again.
    return l_eRangingStatus;
}
