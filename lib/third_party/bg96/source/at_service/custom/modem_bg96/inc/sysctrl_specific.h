/**
  ******************************************************************************
  * @file    Application\at_service\custom\modem_bg96\inc\sysctrl_specific.h
  * @author  MCD Application Team
  * @brief   This file defines all the parameters for System control of
  *          BG96 Quectel modem
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
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
