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
 * Description:   Sensor Manager
 *
 */
#include "drivers/VL53L1X/VL53L1X_Ext.h"
#include "../../../util/util.h"

#include "SensorManager.h"

/************************************************************************
 * Defines
 ************************************************************************/
#define EN_VL53L1X 1
#define MAX_AVG_SAMPLE_TOF         ((uint8_t)100u)

/************************************************************************
 * Private type declarations
 ************************************************************************/
typedef struct _VL53L1X_DATA_  {
    uint16_t u16Proximity;
    e_VL53L1X_RangeStatus_t eStatusProximity;  /** 0 VL53L1_RANGESTATUS_RANGE_VALID Ranging measurement is valid
                                                    1 VL53L1_RANGESTATUS_SIGMA_FAIL Raised if sigma estimator check is above the
                                                    internal defined threshold
                                                    2 VL53L1_RANGESTATUS_SIGNAL_FAIL Raised if signal value is below the internal
                                                     defined threshold
                                                    4 VL53L1_RANGESTATUS_OUTOFBOUNDS_
                                                            FAIL Raised when phase is out of bounds
                                                    5 VL53L1_RANGESTATUS_HARDWARE_FAIL Raised in case of HW or VCSEL failure
                                                    7 VL53L1_RANGESTATUS_WRAP_TARGET_
                                                            FAIL Wrapped target, not matching phases
                                                    8 VL53L1_RANGESTATUS_PROCESSING_
                                                            FAIL Internal algorithm underflow or overflow
                                                    14 VL53L1_RANGESTATUS_RANGE_INVALID The reported range is invalid */
}s_VL53L1X_Data_t;

/************************************************************************
 * Private function declarations
 ************************************************************************/

/************************************************************************
 * Variable declarations
 ************************************************************************/
#ifndef DEBUG
static
#endif
s_VL53L1X_CalibrationData_t g_sVL53L1XCalib = {
        .s16Offset = 0,
        .u16XTalk = 0u,
};
static s_VL53L1X_Parameters_t g_sVL53L1XParam = {
        .u32InterMeasurement = 100u,
        .u16TimingBudget = 20u,
        .u16SignalThr = 1024u,
        .u16SigmaThr = 15u,
        .u8DistanceMode = 1u,
        .u8ROIX = 4u,
        .u8ROIY = 4u,
        .u8ROICenter = 199u, // DO NOT modify
};
static uint8_t g_u8VL53L1XSampleNumber = 1u;

#ifndef DEBUG
static
#endif
s_VL53L1X_Data_t g_sVL53L1XData = { 0 };


/****************************************************************************************
 * Public functions
 ****************************************************************************************/
uint8_t u8SensorMngr_ProximityAvailable(void)
{
#if (EN_VL53L1X == 1)
    return u8VL53L1X_IsAvailable();
#else
    return 0u;
#endif
}

uint8_t u8SensorMngr_ProximityParamOffsetGet(int16_t * p_ps16Offset)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    if(p_ps16Offset != NULL)
   {
      (*p_ps16Offset) = g_sVL53L1XCalib.s16Offset;
      l_u8Result = 0u;
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamOffsetSet(uint16_t p_u16Offset)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    // TODO Launch command
   if(eVL53L1X_CalibrateOffset(p_u16Offset, &g_sVL53L1XCalib.s16Offset) == VL53L1X_ERROR_NONE)
   {
      //if(eVL53L1X_SetOffset(g_sVL53L1XCalib.s16Offset) == VL53L1X_ERROR_NONE)
      {
         l_u8Result = 0u;
      }
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamXtalkGet(int16_t * p_ps16Xtalk)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    if(p_ps16Xtalk != NULL)
   {
      (*p_ps16Xtalk) = (int16_t)g_sVL53L1XCalib.u16XTalk;
      l_u8Result = 0u;
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamXtalkSet(int16_t p_s16Xtalk)
{
    uint8_t l_u8Result = 0u;
#if (EN_VL53L1X == 1)
    // TODO Launch command
   if(eVL53L1X_CalibrateXtalk((uint16_t)p_s16Xtalk, &g_sVL53L1XCalib.u16XTalk) == VL53L1X_ERROR_NONE)
   {
      if(eVL53L1X_SetXtalk(g_sVL53L1XCalib.u16XTalk) == VL53L1X_ERROR_NONE)
      {
         l_u8Result = 0u;
      }
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamTimingBudgetGet(uint16_t * p_pu16TimingBudget)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    if(p_pu16TimingBudget != NULL)
   {
      (*p_pu16TimingBudget) = g_sVL53L1XParam.u16TimingBudget;
      l_u8Result = 0u;
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamTimingBudgetSet(uint16_t p_u16TimingBudget)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    // TODO Launch command

   if(eVL53L1X_SetTimingBudgetInMs(p_u16TimingBudget) == VL53L1X_ERROR_NONE)
   {
      (void)eVL53L1X_SetInterMeasurementInMs(g_sVL53L1XParam.u16TimingBudget + 4u);
      g_sVL53L1XParam.u16TimingBudget = p_u16TimingBudget;
      l_u8Result = 0u;
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamSignalGet(uint16_t * p_pu16Signal)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    if(p_pu16Signal != NULL)
   {
      (*p_pu16Signal) = g_sVL53L1XParam.u16SignalThr;
      l_u8Result = 0u;
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamSignalSet(uint16_t p_u16Signal)
{
    uint8_t l_u8Result = 0u;
#if (EN_VL53L1X == 1)
    // TODO Launch command
   g_sVL53L1XParam.u16SignalThr = p_u16Signal;
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamSigmaGet(uint16_t * p_pu16Sigma)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    if(p_pu16Sigma != NULL)
   {
      (*p_pu16Sigma) = g_sVL53L1XParam.u16SigmaThr;
      l_u8Result = 0u;
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamSigmaSet(uint16_t p_u16Sigma)
{
    uint8_t l_u8Result = 0u;
#if (EN_VL53L1X == 1)
    // TODO Launch command
   g_sVL53L1XParam.u16SigmaThr = p_u16Sigma;
#endif

    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamROIGet(uint8_t * p_pu8ROIX, uint8_t * p_pu8ROIY)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    if( (p_pu8ROIX != NULL) && (p_pu8ROIY != NULL) )
   {
      (*p_pu8ROIX) = g_sVL53L1XParam.u8ROIX;
      (*p_pu8ROIY) = g_sVL53L1XParam.u8ROIY;
      l_u8Result = 0u;
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamROISet(uint8_t p_u8ROIX, uint8_t p_u8ROIY)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    // TODO Launch command
   if(eVL53L1X_SetROI(p_u8ROIX, p_u8ROIY) == VL53L1X_ERROR_NONE)
   {
      g_sVL53L1XParam.u8ROIX = p_u8ROIX;
      g_sVL53L1XParam.u8ROIY = p_u8ROIY;
      l_u8Result = 0u;
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamRangeGet(uint8_t * p_pu8Range)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    if(p_pu8Range != NULL)
   {
      (*p_pu8Range) = g_sVL53L1XParam.u8DistanceMode;
      l_u8Result = 0u;
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamRangeSet(uint8_t p_u8Range)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    // TODO Launch command
   if(eVL53L1X_SetDistanceMode(p_u8Range) == VL53L1X_ERROR_NONE)
   {
      g_sVL53L1XParam.u8DistanceMode = p_u8Range;
      l_u8Result = 0u;
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamAverageGet(uint8_t * p_pu8SampleNumber)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    if(p_pu8SampleNumber != NULL)
   {
      (*p_pu8SampleNumber) = g_u8VL53L1XSampleNumber;
      l_u8Result = 0u;
   }
#endif
    return l_u8Result;
}
uint8_t u8SensorMngr_ProximityParamAverageSet(uint8_t p_u8SampleNumber)
{
    uint8_t l_u8Result = 1u;
#if (EN_VL53L1X == 1)
    // TODO Launch command
   if(   (p_u8SampleNumber >= 1u)
      && (p_u8SampleNumber <= MAX_AVG_SAMPLE_TOF) )
   {
      g_u8VL53L1XSampleNumber = p_u8SampleNumber;
      l_u8Result = 0u;
   }
#endif
    return l_u8Result;
}

void vSensorMngr_ProximityShutdown(void)
{
    (void)eVL53L1X_SensorShutdown();
}

void vSensorMngr_ProximityGet(uint8_t * p_pau8Data, uint8_t * p_pu8Size)
{
    if( (p_pau8Data != NULL) && (p_pu8Size != NULL) )
    {
        (*p_pu8Size) = 0u;
        p_pau8Data[(*p_pu8Size)++] = MSB_16B_TO_8B(g_sVL53L1XData.u16Proximity);
        p_pau8Data[(*p_pu8Size)++] = LSB_16B_TO_8B(g_sVL53L1XData.u16Proximity);
        p_pau8Data[(*p_pu8Size)++] = g_sVL53L1XData.eStatusProximity;
    }
}

void vSensorMngr_ProximityRead(void)
{
    uint8_t l_u8Status = 0u;
    s_VL53L1X_Result_t l_sResult = { 0 };
    e_VL53L1X_Error_t l_eError = VL53L1X_ERROR_NONE;
    uint16_t l_u8RetryNb = 1000u;
    uint8_t l_u8AverageSample = 1u;
    // Read n time
    if(u8SensorMngr_ProximityParamAverageGet(&l_u8AverageSample) == 0u)
    {
        do {
            if(eVL53L1X_StartRanging() == VL53L1X_ERROR_NONE)
            {
                l_u8RetryNb = 1000u;

                while(   (l_u8Status == 0u)
                         && (l_eError == VL53L1X_ERROR_NONE)
                         && (l_u8RetryNb != 0u) )
                {
                    l_eError = eVL53L1X_CheckForDataReady(&l_u8Status);
                    l_u8RetryNb--;
                }

                if( (l_eError == VL53L1X_ERROR_NONE) && (l_u8RetryNb != 0u) )
                {
                    l_eError = eVL53L1X_ClearInterrupt();
                    if(eVL53L1X_GetRangeStatus(&l_u8Status) == VL53L1X_ERROR_NONE)
                    {
                        g_sVL53L1XData.eStatusProximity = l_u8Status;

                        if(eVL53L1X_GetResult(&l_sResult) == VL53L1X_ERROR_NONE)
                        {
                            vSensorMngr_ProximityMovingAverageCompute(l_sResult.Distance, &g_sVL53L1XData.u16Proximity);
                        }
#ifdef DEBUG
                        else
                  {
                     __nop();
                  }
#endif
                    }
                }
                else
                {
                    g_sVL53L1XData.eStatusProximity = 8u;
                }
            }
            l_u8AverageSample--;
        }while(l_u8AverageSample != 0u);

    }
}
void vSensorMngr_ProximityWakeUp(void)
{
    // TODO Remove all (void)
    // Init Sensor
    (void)eVL53L1X_SensorInit();
    (void)eVL53L1X_SetDistanceMode(g_sVL53L1XParam.u8DistanceMode);
    (void)eVL53L1X_SetTimingBudgetInMs(g_sVL53L1XParam.u16TimingBudget);
    (void)eVL53L1X_SetInterMeasurementInMs(g_sVL53L1XParam.u16TimingBudget + 4u);
    (void)eVL53L1X_SetROI(g_sVL53L1XParam.u8ROIX,g_sVL53L1XParam.u8ROIY);
    (void)eVL53L1X_SetSigmaThreshold(g_sVL53L1XParam.u16SigmaThr);
    (void)eVL53L1X_SetSignalThreshold(g_sVL53L1XParam.u16SignalThr);
    // Store Offset
    (void)eVL53L1X_SetOffset(g_sVL53L1XCalib.s16Offset);
    // Store Xtalk
    (void)eVL53L1X_SetXtalk(g_sVL53L1XCalib.u16XTalk);
    // Calib with Temperature
    (void)eVL53L1X_StartTemperatureUpdate();

//   (void)eVL53L1X_StartRanging();
}

void vSensorMngr_ProximityMovingAverageCompute(uint16_t p_u16Sample, uint16_t * p_pu16Average)
{
    static uint8_t l_u8CurrentIdx = 0u;
    static int32_t l_s32TotalDistance = 0u;
    static uint16_t l_au16Distance[MAX_AVG_SAMPLE_TOF] = { 0u };
    static uint8_t l_u8OldSampleNumber = 1u;

    if(p_pu16Average != NULL)
    {
        if(l_u8OldSampleNumber != g_u8VL53L1XSampleNumber)
        {  // Update it
            l_u8OldSampleNumber = g_u8VL53L1XSampleNumber;
            // Clear Total Distance with current sample
            l_s32TotalDistance = 0;
            memset(l_au16Distance, 0u, sizeof(l_au16Distance));
            l_u8CurrentIdx = 0u;
        }

        /* Remove latest value from total */
        l_s32TotalDistance -= l_au16Distance[l_u8CurrentIdx];
        /* Replace by the new one */
        l_au16Distance[l_u8CurrentIdx] = p_u16Sample;
        /* Add it to total */
        l_s32TotalDistance += l_au16Distance[l_u8CurrentIdx];
        /* Compute average */
        (*p_pu16Average) = (l_s32TotalDistance/(uint16_t)l_u8OldSampleNumber);

        l_u8CurrentIdx++;
        l_u8CurrentIdx %= l_u8OldSampleNumber;
    }
}

/****************************************************************************************
 * Private functions
 ****************************************************************************************/
