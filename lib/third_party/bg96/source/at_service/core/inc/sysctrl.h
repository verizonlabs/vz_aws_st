/**
  ******************************************************************************
  * @file    Middlewares\ST\at_service\core\inc\sysctrl.h
  * @author  MCD Application Team
  * @brief   This file defines all the functions prototypes for generic System
  *          Control
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

#ifndef SYSCTRL_H_
#define SYSCTRL_H_

#include "ipc_common.h"
/*
>> Host to Modem:
Modem_Enable : power on modem
Modem_Reset :
Modem_Service : select modem boot mode (from "Flash" or "Host over USB")
App_Wake0 :
App_Wake1 :

>> Modem to Host
Boot_Ind : indicates end of modem boot
Modem_Wake: modem data shall be read, request host to wake up
*/

#define MAX_CONNECTED_DEVICES (2) /* maximum number of hardwares devices (modem, wifi,...) connected */

typedef uint16_t sysctrl_handle_t;

typedef enum
{
  SCSTATUS_OK = 0,
  SCSTATUS_ERROR,
} sysctrl_status_t;

typedef enum
{
    DEVTYPE_MODEM_CELLULAR = 0,
 /*  DEVTYPE_WIFI, */
 /*  DEVTYPE_GPS, */
 /* etc... all modules using AT commands */

    /* --- */
    DEVTYPE_INVALID,      /* keep it last */
} sysctrl_device_type_t;

typedef struct
{
    sysctrl_device_type_t       type;
    IPC_Device_t                ipc_device;
    IPC_Interface_t             ipc_interface;
} sysctrl_info_t;

/* functions prototypes  ----------------------------------------------------------*/
typedef sysctrl_status_t (*SC_getDeviceDescriptor) (sysctrl_device_type_t device_type, sysctrl_info_t *p_devices_list);
typedef sysctrl_status_t (*SC_power_on) (sysctrl_device_type_t device_type);
typedef sysctrl_status_t (*SC_power_off) (sysctrl_device_type_t device_type);
typedef sysctrl_status_t (*SC_power_reset_device) (sysctrl_device_type_t device_type);

/* structures prototypes  --------------------------------------------------*/
typedef struct
{
    uint8_t                     initialized;
    SC_getDeviceDescriptor      f_getDeviceDescriptor;
    SC_power_on                 f_power_on;
    SC_power_off                f_power_off;
    SC_power_reset_device       f_reset_device;
} sysctrl_funcPtrs_t;

/* API */
sysctrl_status_t SysCtrl_getDeviceDescriptor (sysctrl_device_type_t device_type, sysctrl_info_t *p_devices_list);
sysctrl_status_t SysCtrl_power_on (sysctrl_device_type_t device_type);
sysctrl_status_t SysCtrl_power_off (sysctrl_device_type_t device_type);
sysctrl_status_t SysCtrl_reset_device (sysctrl_device_type_t device_type);


#endif /* SYSCTRL_H_ */
