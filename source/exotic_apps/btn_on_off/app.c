/* Copyright 2017 Wirepas Ltd. All Rights Reserved.
 *
 * See file LICENSE.txt for full license details.
 *
 */

/*
 * \file    app.c
 * \brief   A simple LED blink application
 */

#include <stdlib.h> // For NULL

#include "api.h"
#include "node_configuration.h"
#include "shared_data.h"
#include "led.h"
#include "button.h"
#include "app_scheduler.h"



/** Duration between timer callbacks */
#define CALLBACK_PERIOD_S   1
#define CALLBACK_PERIOD_MS  (CALLBACK_PERIOD_S * 1000)

/** Time spent in timer callback */
#define EXECUTION_TIME_US   100

/** Period to send data */
#define DEFAULT_PERIOD_S    10
#define DEFAULT_PERIOD_MS   (DEFAULT_PERIOD_S*1000)

#define DATA_EP 1

static uint8_t g_au8BtnStates[4u] = {0};

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

    // Create a data packet to send
    app_lib_data_to_send_t data_to_send;
    data_to_send.bytes = (const uint8_t *) g_au8BtnStates;
    data_to_send.num_bytes = sizeof(g_au8BtnStates);
    data_to_send.dest_address = APP_ADDR_ANYSINK;
    data_to_send.src_endpoint = DATA_EP;
    data_to_send.dest_endpoint = DATA_EP;
    data_to_send.qos = APP_LIB_DATA_QOS_HIGH;
    data_to_send.delay = 0;
    data_to_send.flags = APP_LIB_DATA_SEND_FLAG_NONE;
    data_to_send.tracking_id = APP_LIB_DATA_NO_TRACKING_ID;

    // Send the data packet
    Shared_Data_sendData(&data_to_send, NULL);

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
    /*LOG(LVL_INFO, "dataReceivedCb");*/
    if ((data->num_bytes < 1) /*|| (data->num_bytes!= sizeof(g_au8BtnStates))*/)
    {
        // Data was not for this application
        /*LOG(LVL_INFO, "dataReceiveCb - data was not for this application");*/
        return APP_LIB_DATA_RECEIVE_RES_NOT_FOR_APP;
    }

    /*LOG(LVL_INFO, "dataReceivedCb");	*/
	
	for (uint8_t i = 0; i < 4; ++i)
	{
		Led_set(i, data->bytes[i]);
	}

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
        .src_endpoint = DATA_EP,
        /* Filtering by destination endpoint. */
        .dest_endpoint = DATA_EP,
        .multicast_cb = NULL
    }
};

/**
 * \brief   Callback for button press
 * \param   button_id
 *          Number of button that was pressed
 * \param   event
 *          Always BUTTON_PRESSED here
 *
 * This function is called when a button is pressed down.
 */
static void button_press_func(uint8_t button_id, button_event_e event)
{
    (void) event;

    // Turn LED on
    /*Led_set(button_id, true);*/
	
	g_au8BtnStates[button_id] = 1u;
}

/**
 * \brief   Callback for button release
 * \param   button_id
 *          Number of button that was released
 * \param   event
 *          Always BUTTON_RELEASED here
 *
 * This function is called when a button is released.
 */
static void button_release_func(uint8_t button_id, button_event_e event)
{
    (void) event;

    // Turn LED off
    /*Led_set(button_id, false);*/
	
	g_au8BtnStates[button_id] = 0u;
}

/**
 * \brief   Initialization callback for application
 *
 * This function is called after hardware has been initialized but the
 * stack is not yet running.
 *
 */
void App_init(const app_global_functions_t * functions)
{
    // Basic configuration of the node with a unique node address
    if (configureNodeFromBuildParameters() != APP_RES_OK)
    {
        // Could not configure the node
        // It should not happen except if one of the config value is invalid
        return;
    }

    // Set up LEDs
    Led_init();

    // Set up buttons
    Button_init();
    uint_fast8_t num_buttons = Button_get_number();
    for (uint_fast8_t button_id = 0; button_id < num_buttons; button_id++)
    {
        Button_register_for_event(button_id,
                                  BUTTON_PRESSED,
                                  button_press_func);
        Button_register_for_event(button_id,
                                  BUTTON_RELEASED,
                                  button_release_func);
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
