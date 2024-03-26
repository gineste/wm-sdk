# You can add more sources here and libraries if needed
SRCS += $(APP_SRCS_PATH)drivers/VL53L1X/VL53L1X_Ext.c
SRCS += $(APP_SRCS_PATH)drivers/VL53L1X/API/core/VL53L1X_api.c
SRCS += $(APP_SRCS_PATH)drivers/VL53L1X/API/core/VL53L1X_calibration.c
SRCS += $(APP_SRCS_PATH)drivers/VL53L1X/API/platform/vl53l1_platform.c
SRCS += $(APP_SRCS_PATH)SensorManager.c
INCLUDES += -I$(APP_SRCS_PATH)drivers/VL53L1X/
INCLUDES += -I$(APP_SRCS_PATH)drivers/VL53L1X/API/core/
INCLUDES += -I$(APP_SRCS_PATH)drivers/VL53L1X/API/platform/
LIBS +=
LDFLAGS +=

# Example:
# SRCS += $(APP_SRCS_PATH)my_driver.c
# LIBS += my_lib.a
# include math.h lib
# LDFLAGS += -lm

# Use Shared Data
SHARED_DATA=yes

APP_PRINTING=no

# Use App Scheduler and configure it
APP_SCHEDULER=yes
APP_SCHEDULER_TASKS=1

# HAL flags
HAL_LED=yes
HAL_UART=yes
HAL_I2C=yes