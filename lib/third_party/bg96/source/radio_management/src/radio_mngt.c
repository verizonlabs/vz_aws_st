/**
  ******************************************************************************
  * @file    radio_mngt.c
  * @author  MCD Application Team
  * @brief   This file contains radio management
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

#include "error_handler.h"
#include "radio_mngt.h"
#include "cellular_service_task.h"


radio_mngt_status_t radio_mngt_radio_on()
{
  return  (radio_mngt_status_t)CST_radio_on();
}


