/*
 * em_device.h
 *
 *  Chip variant include file selection.
 */

#ifndef EM_DEVICE_H_
#define EM_DEVICE_H_

#if defined(EFR32FG12)
#include "efr32fg12/Include/em_device.h"
#define GPIO_PORT_MAX 10
#elif defined(EFR32MG21)
#include "efr32mg21/Include/em_device.h"
#define GPIO_PORT_MAX 3
#elif defined(EFR32MG22)
#include "efr32mg22/Include/em_device.h"
#define GPIO_PORT_MAX 3
#elif defined(EFR32FG23)
#include "efr32fg23/Include/em_device.h"
#define GPIO_PORT_MAX 3
#define __SYSTEM_CLOCK                 (39000000UL)
#elif defined(EFR32MG24)
#include "efr32mg24/Include/em_device.h"
#define GPIO_PORT_MAX 3
#define __SYSTEM_CLOCK                 (39000000UL)
#else
#error "em_device.h: Unknown EFR32 PART"
#endif

#ifndef __SYSTEM_CLOCK
#define __SYSTEM_CLOCK                 (38400000UL)
#endif

#endif /* EM_DEVICE_H_ */