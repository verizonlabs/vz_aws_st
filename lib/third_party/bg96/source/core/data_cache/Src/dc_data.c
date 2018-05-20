/**
******************************************************************************
* File Name          : dc_data.c
  * @author  MCD Application Team
* Description        : This file contains all the functions for dc_data
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


#include "plf_config.h"
#include "rtc.h"
#include "dc_common.h"
#include "dc_data.h"
#include <stdio.h>
#include <string.h>

dc_com_status_t dc_get_mmi_state_info(dc_mmi_state_t* state)
{
    *state = DC_MMI_HP;
    return DC_COM_OK;    
}


dc_com_status_t dc_get_cellular_info(void *dc, dc_cellular_rt_info_t* dc_cellular_rt_info)
{
  return  dc_com_read (&dc_com_db, DC_COM_CELLULAR, (void*)dc_cellular_rt_info, sizeof(dc_cellular_rt_info_t));
}

dc_com_status_t dc_get_hrm_info(void *dc, dc_hrm_rt_info_t* dc_hrm_rt_info)
{
  return  dc_com_read (&dc_com_db, DC_COM_HRM, (void*)dc_hrm_rt_info, sizeof(dc_hrm_rt_info_t));
}

dc_com_status_t dc_get_activity_info(void *dc, dc_activity_rt_info_t* dc_activity_rt_info)
{
  return  dc_com_read (&dc_com_db, DC_COM_ACTIVITY, (void*)dc_activity_rt_info, sizeof(dc_activity_rt_info_t));
}

dc_com_status_t dc_get_battery_info(void *dc, dc_battery_rt_info_t* dc_battery_rt_info)
{
  return  dc_com_read (&dc_com_db, DC_COM_BATTERY, (void*)dc_battery_rt_info, sizeof(dc_battery_rt_info_t));
}
