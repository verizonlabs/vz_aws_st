/**
  ******************************************************************************
  * @file    dc_control.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for dc_control
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

#ifndef __DC_CONTROL_H
#define __DC_CONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "dc_config.h"
#include "cmsis_os.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    DC_CTRL_OK = 0x00,
    DC_CTRL_ERROR
} dc_ctrl_status_t;


extern osThreadId dc_CtrlEventTaskId;

/* Exported functions ------------------------------------------------------------*/
dc_ctrl_status_t dc_ctrl_event_init(void);
dc_ctrl_status_t dc_ctrl_event_start(void);
void dc_ctrl_post_event_normal(dc_com_event_id_t event_id);
void dc_ctrl_post_event_debounce(dc_com_event_id_t event_id);

#ifdef __cplusplus
}
#endif

#endif /* __DC_CONTROL_H */

/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/
