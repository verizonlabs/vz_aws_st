/**
  ******************************************************************************
  * @file    Middlewares\ST\at_service\custom\modem_bg96\inc\at_custom_modem_api.h
  * @author  MCD Application Team
  * @brief   This file defines all the parameters specific to the
  *          BG96 Quectel modem (3G)
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

#ifndef AT_CUSTOM_MODEM_API_H
#define AT_CUSTOM_MODEM_API_H

#include "at_core.h"
#include "at_custom_common.h"
#include "sysctrl.h"

/* Exported functions ------------------------------------------------------- */

at_status_t      atcma_init_at_func_ptrs(atcustom_funcPtrs_t *funcPtrs);
sysctrl_status_t atcma_init_sysctrl_func_ptrs(sysctrl_funcPtrs_t *funcPtrs);

#endif /* AT_CUSTOM_MODEM_API_H */
