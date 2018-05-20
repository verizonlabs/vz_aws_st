/**
  ******************************************************************************
  * @file    radio_mngt.h
  * @author  MCD Application Team
  * @brief   This file contains radio management protoype
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
#ifndef __RADIO_MNGT_H
#define __RADIO_MNGT_H

#ifdef __cplusplus
 extern "C" {
#endif

#if 0
#include "error_handler.h"
#endif

typedef enum
{
    RADIO_MNGT_OK = 0x00,
    RADIO_MNGT_KO
}radio_mngt_status_t;


extern radio_mngt_status_t radio_mngt_radio_on(void);

#ifdef __cplusplus
}
#endif

#endif /* __RADIO_MNGT_H */

/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/

