/* 
 *    ____  _  _   __   ____   __    ___    ____  _  _  ____  ____  ____  _  _  ____
 *   (  __)( \/ ) /  \ (_  _) (  )  / __)  / ___)( \/ )/ ___)(_  _)(  __)( \/ )/ ___)
 *    ) _)  )  ( (  O )  )(    )(  ( (__   \___ \ )  / \___ \  )(   ) _) / \/ \\___ \     
 *   (____)(_/\_) \__/  (__)  (__)  \___)  (____/(__/  (____/ (__) (____)\_)(_/(____/
 *
 * Copyright (c) 2019 EXOTIC SYSTEMS. All Rights Reserved.
 *
 * Licensees are granted free, non-transferable use of the information. NO WARRANTY 
 * of ANY KIND is provided. This heading must NOT be removed from the file.
 *
 * Date:          12/03/2019 (dd MM YYYY)
 * Author:        Yoann Rebischung
 * Description:   Driver of Time Of Flight Sensor VL53L1X
 *
 */
 
/****************************************************************************************
 * Include Files
 ****************************************************************************************/
#include <stdint.h>
#include <string.h>
/*#include "GlobalDefs.h"*/

/* API include */
#include "API/platform/vl53l1_types.h"
#include "API/platform/vl53l1_platform.h"
#include "API/core/VL53L1X_api.h"
#include "API/core/VL53L1X_calibration.h"

/* Self include */
#include "VL53L1X_Ext.h"

/****************************************************************************************
 * Defines
 ****************************************************************************************/
#define VL53L1X_I2C_ADDR   (uint8_t)0x29  // 0x52>>1

#define REBOOT_DELAY_MS    (uint32_t)2u

#define EXIT_ERROR_CHECK(error)  do {     \
      if((error != VL53L1X_ERROR_NONE))   \
      {                                   \
         return error;                    \
      }                                   \
   }while(0);


#define STATE_CTX_CHECK()        do {        \
      if(   (g_u8VL53L1XContext != 1u)       \
         || (g_u8VL53L1XInitialized != 1u))  \
      {                                      \
         return VL53L1X_ERROR_STATE;         \
      }                                      \
   }while(0);
   
/****************************************************************************************
 * Private type declarations
 ****************************************************************************************/
 
/****************************************************************************************
 * Private function declarations
 ****************************************************************************************/

/****************************************************************************************
 * Variable declarations
 ****************************************************************************************/
static s_VL53L1X_Context_t g_sVL53L1XContext;
static uint8_t g_u8VL53L1XContext = 0u;
static uint8_t g_u8VL53L1XInitialized = 0u;
//static uint8_t g_u8VL53L1XCommFailure = 0u;

/****************************************************************************************
 * Public functions
 ****************************************************************************************/ 
/**@brief This function set the context of the sensor.
 * @return Error Code.
 */
e_VL53L1X_Error_t eVL53L1X_ContextSet(s_VL53L1X_Context_t p_sContext)
{   
   e_VL53L1X_Error_t l_eErrCode = VL53L1X_ERROR_PARAM;

   if(   (p_sContext.fp_vPinSet != NULL)
      && (p_sContext.fp_vPinClear != NULL)
      && (p_sContext.fp_vDelay_ms != NULL) )
   {
      /* Set new context */
      g_sVL53L1XContext.fp_vPinSet = p_sContext.fp_vPinSet;
      g_sVL53L1XContext.fp_vPinClear = p_sContext.fp_vPinClear;
      g_sVL53L1XContext.fp_vDelay_ms = p_sContext.fp_vDelay_ms;
      g_sVL53L1XContext.u32ShutdownPin = p_sContext.u32ShutdownPin;

      l_eErrCode = VL53L1X_ERROR_NONE;
      g_u8VL53L1XContext = 1u;
   }

   return l_eErrCode;
}

/**
 * @brief This function loads the 135 bytes default values to initialize the sensor.
 * @return 0:success, != 0:failed
 */
e_VL53L1X_Error_t eVL53L1X_SensorInit(void)
{
   e_VL53L1X_Error_t l_eErrCode = VL53L1X_ERROR_NONE;
   uint8_t l_u8BootState = 0u;
   if(g_u8VL53L1XContext == 1u)
   {
      g_sVL53L1XContext.fp_vPinSet(g_sVL53L1XContext.u32ShutdownPin);
      g_sVL53L1XContext.fp_vDelay_ms(REBOOT_DELAY_MS);
      
      while (!l_u8BootState && !l_eErrCode)
      {
         l_eErrCode = eVL53L1X_BootState(&l_u8BootState);
         g_sVL53L1XContext.fp_vDelay_ms(REBOOT_DELAY_MS);
      }
      EXIT_ERROR_CHECK(l_eErrCode);
      
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_SensorInit(VL53L1X_I2C_ADDR);
      
      EXIT_ERROR_CHECK(l_eErrCode);
      
      g_u8VL53L1XInitialized = 1u;
   }
   
   return l_eErrCode;
}
/**
 * @brief This function check the state of the sensor.
 * @return 0:Init, != 0:not init
 */
e_VL53L1X_Error_t eVL53L1X_IsSensorInit(void)
{
   e_VL53L1X_Error_t l_eErrCode = VL53L1X_ERROR_STATE;

   if(g_u8VL53L1XContext == 1u)
   {
      l_eErrCode  = VL53L1X_ERROR_NONE;
   }
   
   return l_eErrCode;
}
/**
 * @brief This function shutdown the sensor.
 * @return 0:success, != 0:failed
 */
e_VL53L1X_Error_t eVL53L1X_SensorShutdown(void)
{
   e_VL53L1X_Error_t l_eErrCode = VL53L1X_ERROR_STATE;
   
   if(g_u8VL53L1XContext == 1u)
   {
      g_sVL53L1XContext.fp_vPinClear(g_sVL53L1XContext.u32ShutdownPin);
      l_eErrCode = VL53L1X_ERROR_NONE;
   }
   
   g_u8VL53L1XInitialized = 0u;
   
   return l_eErrCode;
}

/**
 * @brief This function clears the interrupt, to be called after a ranging data reading
 * to arm the interrupt for the next data ready event.
 */
e_VL53L1X_Error_t eVL53L1X_ClearInterrupt(void)
{
   STATE_CTX_CHECK();
   
   return (e_VL53L1X_Error_t) VL53L1X_ClearInterrupt(VL53L1X_I2C_ADDR);
}

/**
 * @brief This function programs the interrupt polarity\n
 * 1=active high (default), 0=active low
 */
e_VL53L1X_Error_t eVL53L1X_SetInterruptPolarity(uint8_t p_u8IntPol)
{
   STATE_CTX_CHECK();
   
   return (e_VL53L1X_Error_t) VL53L1X_SetInterruptPolarity(VL53L1X_I2C_ADDR, p_u8IntPol);
}

/**
 * @brief This function returns the current interrupt polarity\n
 * 1=active high (default), 0=active low
 */
e_VL53L1X_Error_t eVL53L1X_GetInterruptPolarity(uint8_t * p_pu8IntPol)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu8IntPol != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetInterruptPolarity(VL53L1X_I2C_ADDR, p_pu8IntPol);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function starts the ranging distance operation\n
 * The ranging operation is continuous. The clear interrupt has to be done after each get data to allow the interrupt to raise when the next data is ready\n
 * 1=active high (default), 0=active low, use SetInterruptPolarity() to change the interrupt polarity if required.
 */
e_VL53L1X_Error_t eVL53L1X_StartRanging(void)
{
   STATE_CTX_CHECK();
   
   return (e_VL53L1X_Error_t) VL53L1X_StartRanging(VL53L1X_I2C_ADDR);
}

/**
 * @brief This function stops the ranging.
 */
e_VL53L1X_Error_t eVL53L1X_StopRanging(void)
{
   STATE_CTX_CHECK();
   
   return (e_VL53L1X_Error_t) VL53L1X_StopRanging(VL53L1X_I2C_ADDR);
}

/**
 * @brief This function checks if the new ranging data is available by polling the dedicated register.
 * @param : isDataReady==0 -> not ready; isDataReady==1 -> ready
 */
e_VL53L1X_Error_t eVL53L1X_CheckForDataReady(uint8_t * p_pu8isDataReady)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu8isDataReady != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_CheckForDataReady(VL53L1X_I2C_ADDR, p_pu8isDataReady);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function programs the timing budget in ms.
 * Predefined values = 15, 20, 33, 50, 100(default), 200, 500.
 */
e_VL53L1X_Error_t eVL53L1X_SetTimingBudgetInMs(uint16_t p_u16TimingBudgetInMs)
{
   STATE_CTX_CHECK();
   
   return (e_VL53L1X_Error_t) VL53L1X_SetTimingBudgetInMs(VL53L1X_I2C_ADDR, p_u16TimingBudgetInMs);
}

/**
 * @brief This function returns the current timing budget in ms.
 */
e_VL53L1X_Error_t eVL53L1X_GetTimingBudgetInMs(uint16_t * p_pu16TimingBudgetInMs)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16TimingBudgetInMs != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetTimingBudgetInMs(VL53L1X_I2C_ADDR, p_pu16TimingBudgetInMs);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function programs the distance mode (1=short, 2=long(default)).
 * Short mode max distance is limited to 1.3 m but better ambient immunity.\n
 * Long mode can range up to 4 m in the dark with 200 ms timing budget.
 */
e_VL53L1X_Error_t eVL53L1X_SetDistanceMode(uint16_t p_u16DistanceMode)
{
   STATE_CTX_CHECK();
   
   return (e_VL53L1X_Error_t) VL53L1X_SetDistanceMode(VL53L1X_I2C_ADDR, p_u16DistanceMode);
}

/**
 * @brief This function returns the current distance mode (1=short, 2=long).
 */
e_VL53L1X_Error_t eVL53L1X_GetDistanceMode(uint16_t * p_pu16DistanceMode)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16DistanceMode != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetDistanceMode(VL53L1X_I2C_ADDR, p_pu16DistanceMode);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function programs the Intermeasurement period in ms\n
 * Intermeasurement period must be > timing budget - 4ms. This condition is not checked by the API,
 * the customer has the duty to check the condition. Default = 100 ms
 */
e_VL53L1X_Error_t eVL53L1X_SetInterMeasurementInMs(uint32_t p_u32InterMeasurementInMs)
{
   STATE_CTX_CHECK();
   
   return (e_VL53L1X_Error_t) VL53L1X_SetInterMeasurementInMs(VL53L1X_I2C_ADDR, p_u32InterMeasurementInMs);
}

/**
 * @brief This function returns the Intermeasurement period in ms.
 */
e_VL53L1X_Error_t eVL53L1X_GetInterMeasurementInMs(uint16_t * p_pu16IM)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16IM != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetInterMeasurementInMs(VL53L1X_I2C_ADDR, p_pu16IM);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function returns the boot state of the device (1:booted, 0:not booted)
 */
e_VL53L1X_Error_t eVL53L1X_BootState(uint8_t * p_pu8state)
{   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(   (g_u8VL53L1XContext == 1u)
      && (p_pu8state != NULL) )
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_BootState(VL53L1X_I2C_ADDR, p_pu8state);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function returns the sensor id, sensor Id must be 0xEEAC
 */
e_VL53L1X_Error_t eVL53L1X_GetSensorId(uint16_t * p_pu16id)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16id != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetSensorId(VL53L1X_I2C_ADDR, p_pu16id);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function returns the distance measured by the sensor in mm
 */
e_VL53L1X_Error_t eVL53L1X_GetDistance(uint16_t * p_pu16distance)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16distance != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetDistance(VL53L1X_I2C_ADDR, p_pu16distance);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function returns the returned signal per SPAD in kcps/SPAD.
 * With kcps stands for Kilo Count Per Second
 */
e_VL53L1X_Error_t eVL53L1X_GetSignalPerSpad(uint16_t * p_pu16signalPerSp)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16signalPerSp != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetSignalPerSpad(VL53L1X_I2C_ADDR, p_pu16signalPerSp);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function returns the ambient per SPAD in kcps/SPAD
 */
e_VL53L1X_Error_t eVL53L1X_GetAmbientPerSpad(uint16_t * p_pu16amb)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16amb != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetAmbientPerSpad(VL53L1X_I2C_ADDR, p_pu16amb);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function returns the returned signal in kcps.
 */
e_VL53L1X_Error_t eVL53L1X_GetSignalRate(uint16_t * p_pu16signalRate)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16signalRate != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetSignalRate(VL53L1X_I2C_ADDR, p_pu16signalRate);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function returns the current number of enabled SPADs
 */
e_VL53L1X_Error_t eVL53L1X_GetSpadNb(uint16_t * p_pu16spNb)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16spNb != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetSpadNb(VL53L1X_I2C_ADDR, p_pu16spNb);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function returns the ambient rate in kcps
 */
e_VL53L1X_Error_t eVL53L1X_GetAmbientRate(uint16_t * p_pu16ambRate)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16ambRate != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetAmbientRate(VL53L1X_I2C_ADDR, p_pu16ambRate);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function returns the ranging status error
 * (0:no error, 1:sigma failed, 2:signal failed, 4:out-of-bounds, 7:wrap-around)
 */
e_VL53L1X_Error_t eVL53L1X_GetRangeStatus(uint8_t * p_pu8rangeStatus)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu8rangeStatus != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetRangeStatus(VL53L1X_I2C_ADDR, p_pu8rangeStatus);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function returns measurements and the range status in a single read access
 */
e_VL53L1X_Error_t eVL53L1X_GetResult(s_VL53L1X_Result_t * p_psResult)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_psResult != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetResult(VL53L1X_I2C_ADDR, (VL53L1X_Result_t*)p_psResult);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function programs the offset correction in mm
 * @param OffsetValue:the offset correction value to program in mm
 */
e_VL53L1X_Error_t eVL53L1X_SetOffset(int16_t p_s16OffsetValue)
{
   STATE_CTX_CHECK();
   int16_t l_s16Offset = 0;
   e_VL53L1X_Error_t l_eError = VL53L1X_ERROR_STATE;
   
   if(VL53L1X_SetOffset(VL53L1X_I2C_ADDR, p_s16OffsetValue) == VL53L1X_ERROR_NONE)
   {
      if(eVL53L1X_GetOffset(&l_s16Offset) == VL53L1X_ERROR_NONE)
      {
         if(l_s16Offset == p_s16OffsetValue)
         {
            l_eError = VL53L1X_ERROR_NONE;
         }
      }
   }
   
   return l_eError;
}

/**
 * @brief This function returns the programmed offset correction value in mm
 */
e_VL53L1X_Error_t eVL53L1X_GetOffset(int16_t * p_ps16Offset)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_ps16Offset != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetOffset(VL53L1X_I2C_ADDR, p_ps16Offset);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function programs the xtalk correction value in cps (Count Per Second).\n
 * This is the number of photons reflected back from the cover glass in cps.
 */
e_VL53L1X_Error_t eVL53L1X_SetXtalk(uint16_t p_u16XtalkValue)
{
   STATE_CTX_CHECK();
   
   return (e_VL53L1X_Error_t)VL53L1X_SetXtalk(VL53L1X_I2C_ADDR, p_u16XtalkValue);
}

/**
 * @brief This function returns the current programmed xtalk correction value in cps
 */
e_VL53L1X_Error_t eVL53L1X_GetXtalk(uint16_t * p_pu16Xtalk)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16Xtalk != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetXtalk(VL53L1X_I2C_ADDR, p_pu16Xtalk);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function programs the threshold detection mode\n
 * Example:\n
 * VL53L1X_SetDistanceThreshold(dev,100,300,0,1): Below 100 \n
 * VL53L1X_SetDistanceThreshold(dev,100,300,1,1): Above 300 \n
 * VL53L1X_SetDistanceThreshold(dev,100,300,2,1): Out of window \n
 * VL53L1X_SetDistanceThreshold(dev,100,300,3,1): In window \n
 * @param  	ThreshLow(in mm) : the threshold under which one the device raises an interrupt if Window = 0
 * @param 	ThreshHigh(in mm) :  the threshold above which one the device raises an interrupt if Window = 1
 * @param   Window detection mode : 0=below, 1=above, 2=out, 3=in
 */
e_VL53L1X_Error_t eVL53L1X_SetDistanceThreshold(uint16_t p_u16ThreshLow,
			      uint16_t p_u16ThreshHigh, uint8_t p_u8Window)
{
   STATE_CTX_CHECK();
   
   return (e_VL53L1X_Error_t) VL53L1X_SetDistanceThreshold(VL53L1X_I2C_ADDR, p_u16ThreshLow, p_u16ThreshHigh, p_u8Window, 1u);
}

/**
 * @brief This function returns the window detection mode (0=below; 1=above; 2=out; 3=in)
 */
e_VL53L1X_Error_t eVL53L1X_GetDistanceThresholdWindow(uint16_t * p_pu16window)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16window != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetDistanceThresholdWindow(VL53L1X_I2C_ADDR, p_pu16window);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function returns the low threshold in mm
 */
e_VL53L1X_Error_t eVL53L1X_GetDistanceThresholdLow(uint16_t * p_pu16low)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16low != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetDistanceThresholdLow(VL53L1X_I2C_ADDR, p_pu16low);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function returns the high threshold in mm
 */
e_VL53L1X_Error_t eVL53L1X_GetDistanceThresholdHigh(uint16_t * p_pu16high)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16high != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetDistanceThresholdHigh(VL53L1X_I2C_ADDR, p_pu16high);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function programs the ROI (Region of Interest)\n
 * The ROI position is centered, only the ROI size can be reprogrammed.\n
 * The smallest acceptable ROI size = 4\n
 * The highest acceptable ROI size = 16\n
 * @param X:ROI Width; Y=ROI Height
 */
e_VL53L1X_Error_t eVL53L1X_SetROI(uint16_t p_u16X, uint16_t p_u16Y)
{
   STATE_CTX_CHECK();
   
   return (e_VL53L1X_Error_t)VL53L1X_SetROI(VL53L1X_I2C_ADDR, p_u16X, p_u16Y);
}

/**
 *@brief This function returns width X and height Y
 */
e_VL53L1X_Error_t eVL53L1X_GetROI_XY(uint16_t * p_pu16ROI_X, uint16_t * p_pu16ROI_Y)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if( (p_pu16ROI_X != NULL) && (p_pu16ROI_Y != NULL) )
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_GetROI_XY(VL53L1X_I2C_ADDR, p_pu16ROI_X, p_pu16ROI_Y);
   }
   
   return l_eErrCode;
}

/**
 *@brief This function programs the new user ROI center, please to be aware that there is no check in this function.
 *if the ROI center vs ROI size is out of border the ranging function return error #13
 */
e_VL53L1X_Error_t eVL53L1X_SetROICenter(uint8_t p_u8ROICenter)
{
   STATE_CTX_CHECK();
   
   return (e_VL53L1X_Error_t)VL53L1X_SetROICenter(VL53L1X_I2C_ADDR, p_u8ROICenter);
}

/**
 *@brief This function returns the current user ROI center
 */
e_VL53L1X_Error_t eVL53L1X_GetROICenter(uint8_t * p_pu8ROICenter)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu8ROICenter != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t)VL53L1X_GetROICenter(VL53L1X_I2C_ADDR, p_pu8ROICenter);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function programs a new signal threshold in kcps (default=1024 kcps\n
 */
e_VL53L1X_Error_t eVL53L1X_SetSignalThreshold(uint16_t p_u16signal)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = 0;
   
   l_eErrCode = (e_VL53L1X_Error_t)VL53L1X_SetSignalThreshold(VL53L1X_I2C_ADDR, p_u16signal);
   
   return l_eErrCode;
}

/**
 * @brief This function returns the current signal threshold in kcps
 */
e_VL53L1X_Error_t eVL53L1X_GetSignalThreshold(uint16_t * p_pu16signal)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16signal != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t)VL53L1X_GetSignalThreshold(VL53L1X_I2C_ADDR, p_pu16signal);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function programs a new sigma threshold in mm (default=15 mm)
 */
e_VL53L1X_Error_t eVL53L1X_SetSigmaThreshold(uint16_t p_u16sigma)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = 0;
   
   l_eErrCode = (e_VL53L1X_Error_t)VL53L1X_SetSigmaThreshold(VL53L1X_I2C_ADDR, p_u16sigma);
   
   return l_eErrCode;
}

/**
 * @brief This function returns the current sigma threshold in mm
 */
e_VL53L1X_Error_t eVL53L1X_GetSigmaThreshold(uint16_t * p_pu16signal)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_pu16signal != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t)VL53L1X_GetSigmaThreshold(VL53L1X_I2C_ADDR, p_pu16signal);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function performs the temperature calibration.
 * It is recommended to call this function any time the temperature might have changed by more than 8 deg C
 * without sensor ranging activity for an extended period.
 */
e_VL53L1X_Error_t eVL53L1X_StartTemperatureUpdate(void)
{
   STATE_CTX_CHECK();
   
   return (e_VL53L1X_Error_t)VL53L1X_StartTemperatureUpdate(VL53L1X_I2C_ADDR);
}

/**
 * @brief This function performs the offset calibration.\n
 * The function returns the offset value found and programs the offset compensation into the device.
 * @param TargetDistInMm target distance in mm, ST recommended 100 mm
 * Target reflectance = grey17%
 * @return 0:success, !=0: failed
 * @return offset pointer contains the offset found in mm
 */
e_VL53L1X_Error_t eVL53L1X_CalibrateOffset(uint16_t p_u16TargetDistInMm, int16_t * p_ps16offset)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
   
   if(p_ps16offset != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_CalibrateOffset(VL53L1X_I2C_ADDR, p_u16TargetDistInMm, p_ps16offset);
   }
   
   return l_eErrCode;
}

/**
 * @brief This function performs the xtalk calibration.\n
 * The function returns the xtalk value found and programs the xtalk compensation to the device
 * @param TargetDistInMm target distance in mm\n
 * The target distance : the distance where the sensor start to "under range"\n
 * due to the influence of the photons reflected back from the cover glass becoming strong\n
 * It's also called inflection point\n
 * Target reflectance = grey 17%
 * @return 0: success, !=0: failed
 * @return xtalk pointer contains the xtalk value found in cps (number of photons in count per second)
 */
e_VL53L1X_Error_t eVL53L1X_CalibrateXtalk(uint16_t p_u16TargetDistInMm, uint16_t * p_pu16xtalk)
{
   STATE_CTX_CHECK();
   
   e_VL53L1X_Error_t l_eErrCode = -1;
      
   if(p_pu16xtalk != NULL)
   {
      l_eErrCode = (e_VL53L1X_Error_t) VL53L1X_CalibrateXtalk(VL53L1X_I2C_ADDR, p_u16TargetDistInMm, p_pu16xtalk);
   }
   
   return l_eErrCode;
}

uint8_t u8VL53L1X_IsAvailable(void)
{
   return ((g_u8VL53L1XInitialized == 1u) && (g_u8VL53L1XContext == 1u))?1u:0u;
}

/****************************************************************************************
 * Private functions
 ****************************************************************************************/

/****************************************************************************************
 * End Of File
 ****************************************************************************************/

