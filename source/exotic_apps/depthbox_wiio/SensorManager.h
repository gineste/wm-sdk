//
// Created by Martin on 24/01/2022.
//

#ifndef WM_SDK_SENSORMANAGER_H
#define WM_SDK_SENSORMANAGER_H

#include <stdlib.h>
#include <stdint-gcc.h>
#include <string.h>

uint8_t u8SensorMngr_ProximityAvailable(void);
uint8_t u8SensorMngr_ProximityParamOffsetGet(int16_t * p_ps16Offset);
uint8_t u8SensorMngr_ProximityParamOffsetSet(uint16_t p_u16Offset);
uint8_t u8SensorMngr_ProximityParamXtalkGet(int16_t * p_ps16Xtalk);
uint8_t u8SensorMngr_ProximityParamXtalkSet(int16_t p_s16Xtalk);
uint8_t u8SensorMngr_ProximityParamTimingBudgetGet(uint16_t * p_pu16TimingBudget);
uint8_t u8SensorMngr_ProximityParamTimingBudgetSet(uint16_t p_u16TimingBudget);
uint8_t u8SensorMngr_ProximityParamSignalGet(uint16_t * p_pu16Signal);
uint8_t u8SensorMngr_ProximityParamSignalSet(uint16_t p_u16Signal);
uint8_t u8SensorMngr_ProximityParamSigmaGet(uint16_t * p_pu16Sigma);
uint8_t u8SensorMngr_ProximityParamSigmaSet(uint16_t p_u16Sigma);
uint8_t u8SensorMngr_ProximityParamROIGet(uint8_t * p_pu8ROIX, uint8_t * p_pu8ROIY);
uint8_t u8SensorMngr_ProximityParamROISet(uint8_t p_u8ROIX, uint8_t p_u8ROIY);
uint8_t u8SensorMngr_ProximityParamRangeGet(uint8_t * p_pu8Range);
uint8_t u8SensorMngr_ProximityParamRangeSet(uint8_t p_u8Range);
uint8_t u8SensorMngr_ProximityParamAverageGet(uint8_t * p_pu8SampleNumber);
uint8_t u8SensorMngr_ProximityParamAverageSet(uint8_t p_u8SampleNumber);

void vSensorMngr_ProximityShutdown(void);
void vSensorMngr_ProximityGet(uint8_t * p_pau8Data, uint8_t * p_pu8Size);
void vSensorMngr_ProximityRead(void);
void vSensorMngr_ProximityWakeUp(void);
void vSensorMngr_ProximityMovingAverageCompute(uint16_t p_u16Sample, uint16_t * p_pu16Average);

#endif //WM_SDK_SENSORMANAGER_H
