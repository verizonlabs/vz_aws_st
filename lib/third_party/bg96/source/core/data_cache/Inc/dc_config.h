/**
  ******************************************************************************
  * @file    Application\core\data_cache\Inc\dc_config.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for dc_config
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#ifndef __DC_CONFIG_H
#define __DC_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "plf_config.h"

#if (USE_TRACE_INTERFACE_DCLIB == 1)
#include "trace_interface.h"
#define PrintDataCache(format, args...)     TracePrint(DBG_CHAN_DATA_CACHE, DBL_LVL_P0, format "\r\n", ## args)
#define PrintDataCacheErr(format, args...)  TracePrint(DBG_CHAN_DATA_CACHE, DBL_LVL_ERR, "ERROR " format "\r\n", ## args)
#elif (USE_PRINTF == 1)
#define PrintDataCache(format, args...)    	printf(format "\r\n", ## args)
#define PrintPPPoSIfErr(format, args...)   	printf(format "\r\n", ## args)
#elif (USE_CONFIGPRINTF_DCLIB == 1)
#define PrintDataCache(format, args...)   	vLoggingPrintf(format "\r\n", ## args)
#define PrintPPPoSIfErr(format, args...)   	vLoggingPrintf(format "\r\n", ## args)
#else
#define PrintDataCache(format, args...)
#define PrintPPPoSIfErr(format, args...)
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

typedef enum
{
    DC_COM_TIME_DATE  = 0,
    DC_COM_CELLULAR,
    DC_COM_HRM,
    DC_COM_BATTERY,
    DC_COM_PEDOMETER,
    DC_COM_ACTIVITY,
    DC_COM_GESTURE,
    DC_COM_DISPLAY,
    DC_COM_REMOTE_PPP_SERVER,
    DC_COM_LOCAL_PPP_SERVER,
    DC_COM_PPP_CLIENT,
    DC_COM_RADIO_LTE,
    DC_COM_NIFMAN,
    DC_COM_PRESSURE,
    DC_COM_HUMIDITY,
    DC_COM_TEMPERATURE,
    DC_COM_ACCELEROMETER,
    DC_COM_GYROSCOPE,
    DC_COM_MAGNETOMETER,
    DC_COM_NFMC_TEMPO,
    DC_COM_BUTTON_UP,
    DC_COM_BUTTON_DN,
    DC_COM_BUTTON_RIGHT,
    DC_COM_BUTTON_LEFT,
    DC_COM_BUTTON_SEL,
    DC_COM_BUTTON_BAK,
    DC_COM_BUTTON_BAKL,
    DC_COM_BACKLIGHT_UP,
    DC_COM_DISPLAY_ON,
    DC_COM_DISPLAY_PAUSE,
    DC_COM_DISPLAY_OFF,
    DC_COM_MMI_STATE,
    DC_COM_SERV_MAX
} dc_com_res_id_t;

typedef enum
{
    DC_COM_TIME_DATE_INFO         = DC_COM_TIME_DATE,
    DC_COM_CELLULAR_INFO          = DC_COM_CELLULAR,
    DC_COM_HRM_INFO               = DC_COM_HRM,
    DC_COM_BATTERY_INFO           = DC_COM_BATTERY,
    DC_COM_PEDOMETER_INFO         = DC_COM_PEDOMETER,
    DC_COM_ACTIVITY_INFO          = DC_COM_ACTIVITY,
    DC_COM_GESTURE_WAKEUP         = DC_COM_GESTURE,
    DC_COM_DISPLAY_INFO           = DC_COM_DISPLAY,
    DC_COM_REMOTE_PPP_SERVER_INFO = DC_COM_REMOTE_PPP_SERVER,
    DC_COM_LOCAL_PPP_SERVER_INFO  = DC_COM_LOCAL_PPP_SERVER,
    DC_COM_PPP_CLIENT_INFO        = DC_COM_PPP_CLIENT,
    DC_COM_RADIO_LTE_INFO         = DC_COM_RADIO_LTE,
    DC_COM_NIFMAN_INFO            = DC_COM_NIFMAN,
    DC_COM_PRESSURE_INFO          = DC_COM_PRESSURE,
    DC_COM_HUMIDITY_INFO          = DC_COM_HUMIDITY,
    DC_COM_TEMPERATURE_INFO       = DC_COM_TEMPERATURE,
    DC_COM_ACCELEROMETER_INFO     = DC_COM_ACCELEROMETER,
    DC_COM_GYROSCOPE_INFO         = DC_COM_GYROSCOPE,
    DC_COM_MAGNETOMETER_INFO      = DC_COM_MAGNETOMETER,
    DC_COM_NFMC_TEMPO_INFO        = DC_COM_NFMC_TEMPO,
    DC_COM_BUTTON_UP_EVENT        = DC_COM_BUTTON_UP,
    DC_COM_BUTTON_DN_EVENT        = DC_COM_BUTTON_DN,
    DC_COM_BUTTON_RIGHT_EVENT     = DC_COM_BUTTON_RIGHT,
    DC_COM_BUTTON_LEFT_EVENT      = DC_COM_BUTTON_LEFT,
    DC_COM_BUTTON_SEL_EVENT       = DC_COM_BUTTON_SEL,
    DC_COM_BUTTON_BACK_EVENT      = DC_COM_BUTTON_BAK,
    DC_COM_BUTTON_BACK_LONG_EVENT = DC_COM_BUTTON_BAKL,
    DC_COM_BACKLIGHT_UP_EVENT     = DC_COM_BACKLIGHT_UP,
    DC_COM_DISPLAY_ON_EVENT       = DC_COM_DISPLAY_ON,
    DC_COM_DISPLAY_PAUSE_EVENT    = DC_COM_DISPLAY_PAUSE,
    DC_COM_DISPLAY_OFF_EVENT      = DC_COM_DISPLAY_OFF,
    DC_COM_MMI_STATE_INFO         = DC_COM_MMI_STATE,
    DC_COM_EVENT_MAX              = DC_COM_SERV_MAX,
} dc_com_event_id_t;


#define DC_COM_MAX_NB_USERS 10U

#ifdef __cplusplus
}
#endif

#endif /* __DC_CONFIG_H */

/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/
