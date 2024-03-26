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
 */
#ifndef VL53L1X_EXT_H
#define VL53L1X_EXT_H

/****************************************************************************************
 * Include Files
 ****************************************************************************************/
#include <stdint.h>
#include "API/core/VL53L1X_api.h"
#include "API/core/VL53L1X_calibration.h"

/****************************************************************************************
 * Defines
 ****************************************************************************************/
#define VL53L1X_ERROR_NONE    (int8_t)0
#define VL53L1X_ERROR_PARAM   (int8_t)-1
#define VL53L1X_ERROR_STATE   (int8_t)-2

/****************************************************************************************
 * Type definitions
 ****************************************************************************************/
typedef VL53L1X_ERROR e_VL53L1X_Error_t;
typedef VL53L1X_Result_t s_VL53L1X_Result_t;

typedef enum _VL53L1_RANGESTATUS_ {
    VL53L1_RANGESTATUS_RANGE_VALID = 0,
    VL53L1_RANGESTATUS_SIGMA_FAIL = 2,
    VL53L1_RANGESTATUS_OUTOFBOUNDS = 4,
    VL53L1_RANGESTATUS_HARDWARE_FAIL = 5,
    VL53L1_RANGESTATUS_WRAP_TARGET = 7,
    VL53L1_RANGESTATUS_PROCESSING = 8,
    VL53L1_RANGESTATUS_RANGE_INVALID = 14
} e_VL53L1X_RangeStatus_t;

typedef struct _VL53L1X_CONTEXT_ {
   /* Pointer function to pin set */
   void (*fp_vPinSet)(uint32_t p_u32PinNumber);
   /* Pointer function to pin clear */
   void (*fp_vPinClear)(uint32_t p_u32PinNumber);
   /* Function pointer to a timer in ms */
   void (*fp_vDelay_ms)(uint32_t p_u32Timeout); 
   /* Pin number of XShutdown */  
   uint32_t u32ShutdownPin;
}s_VL53L1X_Context_t;

typedef struct _VL53L1X_CALIB_DATA_ {
   int16_t s16Offset;
   uint16_t u16XTalk;
}s_VL53L1X_CalibrationData_t;

typedef struct _VL53L1X_PARAM_DATA_ {
   uint32_t u32InterMeasurement;
   uint16_t u16TimingBudget;
   uint16_t u16SignalThr;
   uint16_t u16SigmaThr;
   uint8_t u8DistanceMode;
   uint8_t u8ROIX;
   uint8_t u8ROIY;
   uint8_t u8ROICenter;
}s_VL53L1X_Parameters_t;

/****************************************************************************************
 * Public function declarations
 ****************************************************************************************/ 
/**@brief This function set the context of the sensor.
 * @return Error Code.
 */
e_VL53L1X_Error_t eVL53L1X_ContextSet(s_VL53L1X_Context_t p_sContext);
/**
 * @brief This function loads the 135 bytes default values to initialize the sensor.
 * @return 0:success, != 0:failed
 */
e_VL53L1X_Error_t eVL53L1X_SensorInit(void);

/**
 * @brief This function check the state of the sensor.
 * @return 0:Init, != 0:not init
 */
e_VL53L1X_Error_t eVL53L1X_IsSensorInit(void);
   
/**
 * @brief This function shutdown the sensor.
 * @return 0:success, != 0:failed
 */
e_VL53L1X_Error_t eVL53L1X_SensorShutdown(void);

/**
 * @brief This function clears the interrupt, to be called after a ranging data reading
 * to arm the interrupt for the next data ready event.
 */
e_VL53L1X_Error_t eVL53L1X_ClearInterrupt(void);

/**
 * @brief This function programs the interrupt polarity\n
 * 1=active high (default), 0=active low
 */
e_VL53L1X_Error_t eVL53L1X_SetInterruptPolarity(uint8_t p_u8IntPol);

/**
 * @brief This function returns the current interrupt polarity\n
 * 1=active high (default), 0=active low
 */
e_VL53L1X_Error_t eVL53L1X_GetInterruptPolarity(uint8_t * p_pu8IntPol);

/**
 * @brief This function starts the ranging distance operation\n
 * The ranging operation is continuous. The clear interrupt has to be done after each get data to allow the interrupt to raise when the next data is ready\n
 * 1=active high (default), 0=active low, use SetInterruptPolarity() to change the interrupt polarity if required.
 */
e_VL53L1X_Error_t eVL53L1X_StartRanging(void);

/**
 * @brief This function stops the ranging.
 */
e_VL53L1X_Error_t eVL53L1X_StopRanging(void);

/**
 * @brief This function checks if the new ranging data is available by polling the dedicated register.
 * @param : isDataReady==0 -> not ready; isDataReady==1 -> ready
 */
e_VL53L1X_Error_t eVL53L1X_CheckForDataReady(uint8_t * p_pu8isDataReady);

/**
 * @brief This function programs the timing budget in ms.
 * Predefined values = 15, 20, 33, 50, 100(default), 200, 500.
 */
e_VL53L1X_Error_t eVL53L1X_SetTimingBudgetInMs(uint16_t p_u16TimingBudgetInMs);

/**
 * @brief This function returns the current timing budget in ms.
 */
e_VL53L1X_Error_t eVL53L1X_GetTimingBudgetInMs(uint16_t * p_pu16TimingBudgetInMs);

/**
 * @brief This function programs the distance mode (1=short, 2=long(default)).
 * Short mode max distance is limited to 1.3 m but better ambient immunity.\n
 * Long mode can range up to 4 m in the dark with 200 ms timing budget.
 */
e_VL53L1X_Error_t eVL53L1X_SetDistanceMode(uint16_t p_u16DistanceMode);

/**
 * @brief This function returns the current distance mode (1=short, 2=long).
 */
e_VL53L1X_Error_t eVL53L1X_GetDistanceMode(uint16_t * p_pu16DistanceMode);

/**
 * @brief This function programs the Intermeasurement period in ms\n
 * Intermeasurement period must be > timing budget - 4ms. This condition is not checked by the API,
 * the customer has the duty to check the condition. Default = 100 ms
 */
e_VL53L1X_Error_t eVL53L1X_SetInterMeasurementInMs(uint32_t p_u32InterMeasurementInMs);

/**
 * @brief This function returns the Intermeasurement period in ms.
 */
e_VL53L1X_Error_t eVL53L1X_GetInterMeasurementInMs(uint16_t * p_pu16IM);

/**
 * @brief This function returns the boot state of the device (1:booted, 0:not booted)
 */
e_VL53L1X_Error_t eVL53L1X_BootState(uint8_t * p_pu8state);

/**
 * @brief This function returns the sensor id, sensor Id must be 0xEEAC
 */
e_VL53L1X_Error_t eVL53L1X_GetSensorId(uint16_t * p_pu16id);

/**
 * @brief This function returns the distance measured by the sensor in mm
 */
e_VL53L1X_Error_t eVL53L1X_GetDistance(uint16_t * p_pu16distance);

/**
 * @brief This function returns the returned signal per SPAD in kcps/SPAD.
 * With kcps stands for Kilo Count Per Second
 */
e_VL53L1X_Error_t eVL53L1X_GetSignalPerSpad(uint16_t * p_pu16signalPerSp);

/**
 * @brief This function returns the ambient per SPAD in kcps/SPAD
 */
e_VL53L1X_Error_t eVL53L1X_GetAmbientPerSpad(uint16_t * p_pu16amb);

/**
 * @brief This function returns the returned signal in kcps.
 */
e_VL53L1X_Error_t eVL53L1X_GetSignalRate(uint16_t * p_pu16signalRate);

/**
 * @brief This function returns the current number of enabled SPADs
 */
e_VL53L1X_Error_t eVL53L1X_GetSpadNb(uint16_t * p_pu16spNb);

/**
 * @brief This function returns the ambient rate in kcps
 */
e_VL53L1X_Error_t eVL53L1X_GetAmbientRate(uint16_t * p_pu16ambRate);

/**
 * @brief This function returns the ranging status error 
 * (0:no error, 1:sigma failed, 2:signal failed, 4:out-of-bounds, 7:wrap-around)
 */
e_VL53L1X_Error_t eVL53L1X_GetRangeStatus(uint8_t * p_pu8rangeStatus);

/**
 * @brief This function returns measurements and the range status in a single read access
 */
e_VL53L1X_Error_t eVL53L1X_GetResult(s_VL53L1X_Result_t * p_psResult);

/**
 * @brief This function programs the offset correction in mm
 * @param OffsetValue:the offset correction value to program in mm
 */
e_VL53L1X_Error_t eVL53L1X_SetOffset(int16_t p_s16OffsetValue);

/**
 * @brief This function returns the programmed offset correction value in mm
 */
e_VL53L1X_Error_t eVL53L1X_GetOffset(int16_t * p_ps16Offset);

/**
 * @brief This function programs the xtalk correction value in cps (Count Per Second).\n
 * This is the number of photons reflected back from the cover glass in cps.
 */
e_VL53L1X_Error_t eVL53L1X_SetXtalk(uint16_t p_u16XtalkValue);

/**
 * @brief This function returns the current programmed xtalk correction value in cps
 */
e_VL53L1X_Error_t eVL53L1X_GetXtalk(uint16_t * p_pu16Xtalk);

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
			      uint16_t p_u16ThreshHigh, uint8_t p_u8Window);

/**
 * @brief This function returns the window detection mode (0=below; 1=above; 2=out; 3=in)
 */
e_VL53L1X_Error_t eVL53L1X_GetDistanceThresholdWindow(uint16_t * p_pu16window);

/**
 * @brief This function returns the low threshold in mm
 */
e_VL53L1X_Error_t eVL53L1X_GetDistanceThresholdLow(uint16_t * p_pu16low);

/**
 * @brief This function returns the high threshold in mm
 */
e_VL53L1X_Error_t eVL53L1X_GetDistanceThresholdHigh(uint16_t * p_pu16high);

/**
 * @brief This function programs the ROI (Region of Interest)\n
 * The ROI position is centered, only the ROI size can be reprogrammed.\n
 * The smallest acceptable ROI size = 4\n
 * The highest acceptable ROI size = 16\n
 * @param X:ROI Width; Y=ROI Height
 */
e_VL53L1X_Error_t eVL53L1X_SetROI(uint16_t p_u16X, uint16_t p_u16Y);

/**
 *@brief This function returns width X and height Y
 */
e_VL53L1X_Error_t eVL53L1X_GetROI_XY(uint16_t * p_pu16ROI_X, uint16_t * p_pu16ROI_Y);

/**
 *@brief This function programs the new user ROI center, please to be aware that there is no check in this function.
 *if the ROI center vs ROI size is out of border the ranging function return error #13
 */
e_VL53L1X_Error_t eVL53L1X_SetROICenter(uint8_t p_u8ROICenter);

/**
 *@brief This function returns the current user ROI center
 */
e_VL53L1X_Error_t eVL53L1X_GetROICenter(uint8_t * p_pu8ROICenter);

/**
 * @brief This function programs a new signal threshold in kcps (default=1024 kcps\n
 */
e_VL53L1X_Error_t eVL53L1X_SetSignalThreshold(uint16_t p_u16signal);

/**
 * @brief This function returns the current signal threshold in kcps
 */
e_VL53L1X_Error_t eVL53L1X_GetSignalThreshold(uint16_t * p_pu16signal);

/**
 * @brief This function programs a new sigma threshold in mm (default=15 mm)
 */
e_VL53L1X_Error_t eVL53L1X_SetSigmaThreshold(uint16_t p_u16sigma);

/**
 * @brief This function returns the current sigma threshold in mm
 */
e_VL53L1X_Error_t eVL53L1X_GetSigmaThreshold(uint16_t * p_pu16signal);

/**
 * @brief This function performs the temperature calibration.
 * It is recommended to call this function any time the temperature might have changed by more than 8 deg C
 * without sensor ranging activity for an extended period.
 */
e_VL53L1X_Error_t eVL53L1X_StartTemperatureUpdate(void);

/**
 * @brief This function performs the offset calibration.\n
 * The function returns the offset value found and programs the offset compensation into the device.
 * @param TargetDistInMm target distance in mm, ST recommended 100 mm
 * Target reflectance = grey17%
 * @return 0:success, !=0: failed
 * @return offset pointer contains the offset found in mm
 */
e_VL53L1X_Error_t eVL53L1X_CalibrateOffset(uint16_t p_u16TargetDistInMm, int16_t * p_ps16offset);

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
e_VL53L1X_Error_t eVL53L1X_CalibrateXtalk(uint16_t p_u16TargetDistInMm, uint16_t * p_pu16xtalk);

uint8_t u8VL53L1X_IsAvailable(void);

#endif /* VL53L1X_EXT_H */

