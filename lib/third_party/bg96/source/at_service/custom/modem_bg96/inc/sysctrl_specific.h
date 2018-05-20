/**
  ******************************************************************************
  * @file    Middlewares\ST\at_service\custom\modem_bg96\inc\sysctrl_specific.h
  * @author  MCD Application Team
  * @brief   This file defines all the parameters for System control of
  *          BG96 Quectel modem
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

#ifndef SYSCTRL_BG96_H_
#define SYSCTRL_BG96_H_

#include "sysctrl.h"

sysctrl_status_t SysCtrl_BG96_getDeviceDescriptor(sysctrl_device_type_t type, sysctrl_info_t *p_devices_list);
sysctrl_status_t SysCtrl_BG96_power_on(sysctrl_device_type_t type);
sysctrl_status_t SysCtrl_BG96_power_off(sysctrl_device_type_t type);
sysctrl_status_t SysCtrl_BG96_reset(sysctrl_device_type_t type);

#endif /* SYSCTRL_BG96_H_ */
