/**
  ******************************************************************************
  * @file    dc_data.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for dc_data
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

#ifndef __DC_DATA_H
#define __DC_DATA_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "dc_common.h"
#include "dc_data.h"
#include "com_sockets.h"



#define MAX_SIZE_MNO_NAME 32
#define DC_MAX_SIZE_MNO_NAME MAX_SIZE_MNO_NAME
#define DC_HRM_HISTORY_SAMPLE_MAX 15
#define DC_BAT_HISTORY_SAMPLE_MAX 24
#define DC_NFMC_TEMPO_NB           7



typedef com_ip_addr_t dc_network_addr_t;

/* Exported types ------------------------------------------------------------*/


typedef enum
{
    DC_SERVICE_UNAVAIL = 0x00, /* Service is unavailable. HW and/or SW driver are not present. */
    DC_SERVICE_RESET,          /* Service is resetting. When reset is complete, the Service enters in ON or READY state */
    DC_SERVICE_CALIB,          /* Service is under calibration procedure */
    DC_SERVICE_OFF,            /* Service is OFF */
    DC_SERVICE_SHUTTING_DOWN,  /* Service is being shutdown */
    DC_SERVICE_STARTING,       /* Service is starting but not fully operational */
    DC_SERVICE_RUN,            /* Service is ON and fully operational and calibrated */
    DC_SERVICE_ON,             /* Service is ON (functional) but not Calibrated or not Initialized */
    DC_SERVICE_FAIL            /* Service is Failed */
} dc_service_rt_state_t;

typedef enum
{
    DC_MMI_OFF = 0x00, /* MMI is OFF, but Device may be in ultra low power or in deep standby mode */
    DC_MMI_BOOT,       /* BOOT (Low Power) Definition MMI/GUI is ready */
    DC_MMI_ULP,        /* Ultra Low Power mode where Display is switched OFF */
    DC_MMI_LP,         /* Low Power Definition MMI/GUI is ready */
    DC_MMI_HP          /* Full Power High definition MMI/GUI is ready */
}dc_mmi_state_t;

typedef enum
{
    DC_DISP_OFF = 0x00, /* DISPLAY is POWERED OFF. */
    DC_DISP_ON,         /* DISPLAY is Powered ON and ready to use, screen image updated is allowed. */
    DC_DISP_PAUSE       /* DISPLAY is Powered ON but suspended, update of screen image is not allowed. */
}dc_disp_rt_state_t;

typedef enum
{
    DC_NO_ATTACHED = 0,
    DC_LEVEL_1     = 1,
    DC_LEVEL_2     = 2,
    DC_LEVEL_3     = 3,
    DC_LEVEL_4     = 4,
    DC_LEVEL_5     = 5
} dc_cs_signal_level_t;

typedef enum
{
    DC_NO_NETWORK         = 0,
    DC_CELLULAR_NETWORK   = 1,
    DC_WIFI_NETWORK       = 2
} dc_nifman_network_t;

typedef struct
{
    dc_disp_rt_state_t rt_state;
} dc_disp_state_rt_info_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_disp_state_rt_info_t rt_info;
}dc_disp_state_info_t;

typedef struct
{
    dc_service_rt_state_t rt_state;
} dc_state_rt_info_t;

typedef struct
{
    dc_com_res_id_t res_id;
    uint32_t size;
} dc_service_rt_header_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_state_rt_info_t rt_info;
}dc_state_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    char mno_name[MAX_SIZE_MNO_NAME];
    /* dc_cs_signal_level_t cs_signal_level; */
    uint32_t    cs_signal_level;
    int32_t     cs_signal_level_db;
    uint32_t dc_rx_data_volume;
    uint32_t dc_tx_data_volume;
} dc_cellular_rt_info_t;


typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_cellular_rt_info_t rt_info;
}dc_cellular_info_t;

typedef enum
{
    DC_AR_WALKING,
    DC_AR_FAST_WALKING,
    DC_AR_JOGGING,
    DC_AR_STATIONARY,
    DC_AR_BIKING
} dc_activity_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    uint8_t data;
} dc_base_rt_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    uint32_t walking_duration;
    uint32_t stationary_duration;
    uint32_t jogging_duration;
    uint32_t fast_walking_duration;
    uint32_t biking_duration;
    dc_activity_t current_activity; /*type to align with osx_AR */
    uint32_t current_duration; /* in seconds */
} dc_activity_rt_info_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_activity_rt_info_t rt_info;
} dc_activity_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
} dc_gesture_rt_info_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_gesture_rt_info_t rt_info;
} dc_gesture_info_t;


typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    uint32_t step_counter;
} dc_pedometer_rt_info_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_pedometer_rt_info_t rt_info;
} dc_pedometer_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    float pressure;
} dc_pressure_rt_info_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_pressure_rt_info_t rt_info;
} dc_pressure_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    float humidity;
} dc_humidity_rt_info_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_humidity_rt_info_t rt_info;
} dc_humidity_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    float temperature;
} dc_temperature_rt_info_t;


typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_temperature_rt_info_t rt_info;
} dc_temperature_info_t;

typedef struct
{
  int32_t AXIS_X;
  int32_t AXIS_Y;
  int32_t AXIS_Z;
} dc_sensor_axe_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    dc_sensor_axe_t accelerometer;
} dc_accelerometer_rt_info_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_accelerometer_rt_info_t rt_info;
} dc_accelerometer_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    dc_sensor_axe_t gyroscope;
} dc_gyroscope_rt_info_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_gyroscope_rt_info_t rt_info;
} dc_gyroscope_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    dc_sensor_axe_t magnetometer;
} dc_magnetometer_rt_info_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_magnetometer_rt_info_t rt_info;
} dc_megnetometer_info_t;


typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    uint32_t hr_monitor_duration;
    uint8_t current_heart_rate;
    uint8_t min_heart_rate;
    uint8_t max_heart_rate;
    uint8_t avg_heart_rate;
    uint8_t skin_contact; /* 0 if no skin contact detected, 1 otherwise */
    uint8_t heart_rate_history_count;
    uint8_t heart_rate_history [DC_HRM_HISTORY_SAMPLE_MAX]; /* last XX samples, one sample per defined period */
} dc_hrm_rt_info_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_hrm_rt_info_t rt_info;
} dc_hrm_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    dc_com_res_id_t button_id;
} dc_button_rt_info_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
} dc_button_info_t;


typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    uint8_t current_battery_percentage;
    uint8_t battery_history [DC_BAT_HISTORY_SAMPLE_MAX]; /* last 24 samples, one sample per period */
} dc_battery_rt_info_t;


typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
} dc_remote_ppp_server_rt_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t  rt_state;
    dc_network_addr_t ip_addr;
    dc_network_addr_t netmask;
    dc_network_addr_t gw;
} dc_local_ppp_server_rt_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t  rt_state;
    dc_nifman_network_t    network;
    dc_network_addr_t ip_addr;
    dc_network_addr_t netmask;
    dc_network_addr_t gw;
} dc_nifman_rt_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
} dc_radio_lte_rt_info_t;

typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    dc_network_addr_t ip_addr;
    dc_network_addr_t netmask;
    dc_network_addr_t gw;
} dc_ppp_client_rt_info_t;


typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_battery_rt_info_t rt_info;
} dc_battery_info_t;


typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    uint32_t sec;    /* seconds 0-61 */
    uint32_t min;    /* minutes 0-59 */
    uint32_t hour;   /* hours 0-23 */
    uint32_t mday;   /* day of the month  1-31   (based on RTC HAL values)*/
    uint32_t month;  /* month since january 1-12 (based on RTC HAL values) */
    uint32_t year;   /* year since 1970 */
    uint32_t wday;   /* day since monday 1-7 (based on RTC HAL values) */
    uint32_t yday;   /* days since January 1 */
    uint32_t isdst;  /* daylight saving time 0-365 */
}   dc_time_date_rt_info_t;

typedef struct
{
    dc_com_reg_notif_t user_reg;
    dc_time_date_rt_info_t rt_info;
} dc_time_date_info_t;


typedef struct
{
    dc_service_rt_header_t header;
    dc_service_rt_state_t rt_state;
    uint32_t activate;
    uint32_t tempo[DC_NFMC_TEMPO_NB];
}   dc_nfmc_rt_info_t;



typedef enum
{
    DC_TIME,
    DC_DATE,
    DC_DATE_AND_TIME
}dc_time_data_type_t;


dc_com_status_t dc_get_mmi_state_info(dc_mmi_state_t* state);

dc_com_status_t dc_get_cellular_info(void *dc, dc_cellular_rt_info_t* info);

dc_com_status_t dc_get_hrm_info(void *dc, dc_hrm_rt_info_t* info);

dc_com_status_t dc_get_activity_info(void *dc, dc_activity_rt_info_t* info);

dc_com_status_t dc_get_battery_info(void *dc, dc_battery_rt_info_t* info);


#ifdef __cplusplus
}
#endif

#endif /* __DC_DATA_H */

/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/
