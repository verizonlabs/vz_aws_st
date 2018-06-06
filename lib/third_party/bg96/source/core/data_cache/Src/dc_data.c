/**
  ******************************************************************************
  * @file    Application\core\data_cache\Src\dc_data.c
  * @author  MCD Application Team
  * @brief   This file contains all the functions for dc_data
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */


#include "dc_common.h"
#include "dc_data.h"
#include "plf_config.h"
#include "rtc.h"

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
