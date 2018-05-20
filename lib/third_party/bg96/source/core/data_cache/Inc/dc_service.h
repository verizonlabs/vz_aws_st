/**
  ******************************************************************************
  * @file    dc_service.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for data cache service
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

#ifndef __DC_SERVICE_H
#define __DC_SERVICE_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "dc_data.h"
#include "dc_common.h"
#include <stdint.h>

#if 0
typedef enum
{
     DC_SERVICE_RESET,         /* Restart the service. It will restart the HW and SW (drivers) but does not reset stored data */
     DC_SERVICE_RESET_ALL,     /* Reset the Service but all related data */
     DC_SERVICE_RESET_INFO,    /* Reset data only */
}dc_srv_service_reset_param_t; /* this is used as parameter of the DC_SERVICE_RESET */
#endif 

/* dc_srv_target_service_state_t:  used to set the target service state */
typedef enum 
{
    DC_SERVICE_TARGET_RESET,   /* see dc_srv_service_reset_param_t for detail */
    DC_SERVICE_TARGET_RESTORE, /* restore default setting. For example, if the Service is by default OFF then it will power OFF the device. Default value are stored in FW. */
    DC_SERVICE_TARGET_CALIB,   /* calibrate */
    DC_SERVICE_TARGET_OFF,     /* service is powered down but related data remains in data cache. They can be retrieved by GUI when needed. */
    DC_SERVICE_TARGET_ON       /* start the service. The actual service could be ON or READY see DC.  */
}dc_srv_target_service_state_t; 


typedef struct
{
    uint32_t time_update_period; /* in milliseconds, 0 means default value defined in TIME Service or from NVRAM */
    uint8_t time_setting_mode;   /* 0 means manual setting. This is the default setting.
                                    1 full automatic, based on UTC TIme server and location (GPS based or MNO)
                                    2 semi automatic updated from UTC Network Time Server (example via Network Time Server)
                                    3 automatic update via BLE */
    uint8_t utc_offset; /* used only if time is set with time_setting_mode = 2 */
} dc_srv_time_params_t;

typedef struct
{
    uint32_t volume_update_period; /* in milliseconds, 0 means default value defined in CS Service */
    uint32_t signal_strength_update_period; /* in milliseconds, 0 means default value defined in the CS Service */
} dc_srv_cellular_params_t;

typedef struct
{
    uint8_t mode; /* 0 means RTC not preserved in shutdown state */
                  /* 1 means RTC is preserved in shutdown state */
} dc_srv_device_off_params_t;




typedef enum
{
    DC_SRV_RET_OK,
    DC_SRV_RET_ERROR
}dc_srv_ret_t;

dc_srv_ret_t dc_srv_set_service_state(dc_com_res_id_t service_id, dc_srv_target_service_state_t state, void *params);
/* DC_SRV_TIME_DATE, DC_TARGET_SERVICE_ON, (uint32_t) params represents the period of time update in milliseconds,
   0 or NULL pointer means default setting, updated every second */
/* DC_SRV_CELLULAR, DC_TARGET_SERVICE_ON, dc_srv_cellular_params_t. 
	NULL pointer means, default setting or one frome NVRAM*/
/* DC_SRV_HRM, DC_TARGET_SERVICE_ON, (uint32_t) params represents the period of sample in milliseconds,
   0 or NULL pointer means default setting, updated every minute */

/* DC_SRV_BATTERY, DC_TARGET_SERVICE_ON, (uint32_t) params represents the period of sample in milliseconds,
   0 or NULL pointer means default setting, updated every hour */
   
/* DC_SRV_DEVICE, DC_SERVICE_OFF, dc_srv_device_off_params_t. 
   NULL pointer means, default setting : switch off shipping mode. RTC not preserved */

dc_srv_ret_t dc_srv_get_time_date(dc_time_date_rt_info_t* dc_time_date_rt_info, dc_time_data_type_t data_type);

	
dc_srv_ret_t dc_srv_set_time_date(const dc_time_date_rt_info_t* time, dc_time_data_type_t data_type);


/* dc_srv_save_service_state. Store service setting to the permanent memory. The setting is retrieved and used at next boot */
/* dc_srv_ret_t dc_srv_save_service_state(dc_service_id_t service, dc_srv_target_service_state_t state, void *params); */

#ifdef __cplusplus
}
#endif



#endif /* __DC_SERVICE_H */

/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/
