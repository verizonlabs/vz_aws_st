/**
  ******************************************************************************
  * @file    mmi_dcib_main.h
  * @author  MCD Application Team
  * @brief   This file contains data cache definitions
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

#ifndef __DC_MAIN_H
#define __DC_MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum
{
    DC_MAIN_OK = 0x00,
    DC_MAIN_ERROR
} dc_main_status_t;


/* Exported functions ------------------------------------------------------------*/
dc_main_status_t dc_init(void);
void dc_start (void);

#ifdef __cplusplus
}
#endif

#endif /* __DC_MAIN_H */

/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/
