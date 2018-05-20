/**
  ******************************************************************************
  * @brief   nifman.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for NIFMAN
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

#ifndef __NIFMAN_H
#define __NIFMAN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

typedef enum
{
    NIFMAN_OK = 0x00,
    NIFMAN_ERROR
}nifman_status_t;

/* Exported vars ------------------------------------------------------------*/
extern osThreadId  nifman_ThreadId;


/* Exported functions ------------------------------------------------------- */
nifman_status_t nifman_init  (void);
nifman_status_t nifman_start  (void);

#ifdef __cplusplus
}
#endif

#endif /* __NIFMANLIB_H */
/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/