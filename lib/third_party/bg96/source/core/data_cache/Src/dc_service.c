/**
******************************************************************************
* File Name          : dc_service.c
  * @author  MCD Application Team
* Description        : This file contains data cache services
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
#include "dc_service.h"
#include <stdio.h>

static uint16_t month_day[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

static uint16_t get_yday(RTC_DateTypeDef* Date)
{
  uint16_t i;
  uint16_t yday = 0;
  
  for(i=1; i<Date->Month; i++)
  {
    yday += month_day[i];
  }
  
  yday += Date->Date;
  
  if(((Date->Year % 4) == 0) && (Date->Month>1))
  {
    yday++;
  }
  
  return yday;
}


dc_srv_ret_t dc_srv_set_time_date(const dc_time_date_rt_info_t* dc_time_date_rt_info, dc_time_data_type_t dc_time_data_type_t)
{
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

  if(dc_time_data_type_t != DC_DATE)
  {

    sTime.Hours   = dc_time_date_rt_info->hour;
    sTime.Minutes = dc_time_date_rt_info->min;
    sTime.Seconds = dc_time_date_rt_info->sec;
    
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
      return DC_SRV_RET_ERROR;
    }
  }
  
  if(dc_time_data_type_t != DC_TIME)
  {
    if(dc_time_date_rt_info->wday > 7 || dc_time_date_rt_info->wday == 0)
    {
      return DC_SRV_RET_ERROR;
    }
    
    sDate.WeekDay = dc_time_date_rt_info->wday;
    
    sDate.Date = dc_time_date_rt_info->mday;
    sDate.Month = dc_time_date_rt_info->month;
    sDate.Year = (uint8_t)(dc_time_date_rt_info->year - 2000);
    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    {
      return DC_SRV_RET_ERROR;
    }
  }
  
  return (dc_srv_ret_t)dc_com_write(&dc_com_db, DC_COM_TIME_DATE, (void *)dc_time_date_rt_info, sizeof(dc_time_date_info_t));
}

dc_srv_ret_t dc_srv_get_time_date(dc_time_date_rt_info_t* dc_time_date_rt_info, dc_time_data_type_t dc_time_data_type)
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  
  
  /* WARNING : if HAL_RTC_GetDate is called it must be called before HAL_RTC_GetDate */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  if(dc_time_data_type != DC_DATE)
  {
    /* Get the RTC current Time */
    dc_time_date_rt_info->hour  = stimestructureget.Hours;
    dc_time_date_rt_info->min   = stimestructureget.Minutes;
    dc_time_date_rt_info->sec   = stimestructureget.Seconds;
    dc_time_date_rt_info->yday  = get_yday(&sdatestructureget);  /* not managed */
    dc_time_date_rt_info->isdst = 0;  /* not managed */
  }
  
  /* WARNING : HAL_RTC_GetDate must be called after HAL_RTC_GetTime even if date get is not necessary */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
  if(dc_time_data_type != DC_TIME)
  {
    /* Get the RTC current Date */
    dc_time_date_rt_info->wday  = sdatestructureget.WeekDay;
    dc_time_date_rt_info->mday  = sdatestructureget.Date;
    dc_time_date_rt_info->month = sdatestructureget.Month;
    dc_time_date_rt_info->year  = sdatestructureget.Year+2000;
  }
  
  return  DC_SRV_RET_OK;
}

dc_srv_ret_t dc_srv_set_service_state(dc_com_res_id_t service_id, dc_srv_target_service_state_t state, void *params)
{
  PrintDataCache(" Not implemented !\n\r");
  return DC_SRV_RET_OK;	
}


