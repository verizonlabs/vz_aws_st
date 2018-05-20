/**
  ******************************************************************************
  * @file    Middlewares\ST\at_service\core\src\sysctrl.c
  * @author  MCD Application Team
  * @brief   This file provides code for generic System Control
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

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"
#include "sysctrl.h"
#include "at_custom_modem_api.h"

#if (USE_TRACE_INTERFACE_SYSCTRL == 1)
#include "trace_interface.h"

#define PrintDBG(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P1, "SysCtrl:" format "\r\n", ## args)
#define PrintAPI(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P2, "SysCtrl API:" format "\r\n", ## args)
#define PrintErr(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_ERR, "SysCtrl ERROR:" format "\r\n", ## args)
#elif (USE_PRINTF == 1)
#define PrintDBG(format, args...)   printf("SysCtrl:" format "\r\n", ## args)
#define PrintAPI(format, args...)   printf("SysCtrl API:" format "\r\n", ## args)
#define PrintErr(format, args...)   printf("SysCtrl ERROR:" format "\r\n", ## args)
#elif (USE_CONFIGPRINTF_SYSCTRL == 1)
#define PrintDBG(format, args...)   vLoggingPrintf("SysCtrl:" format "\r\n", ## args)
#define PrintAPI(format, args...)   vLoggingPrintf("SysCtrl API:" format "\r\n", ## args)
#define PrintErr(format, args...)   vLoggingPrintf("SysCtrl ERROR:" format "\r\n", ## args)
#else
#define PrintDBG(format, args...)   do {} while(0)
#define PrintAPI(format, args...)   do {} while(0)
#define PrintErr(format, args...)   do {} while(0)
#endif


/* AT custom functions ptrs table */
static sysctrl_funcPtrs_t custom_func[DEVTYPE_INVALID] = {{ 0 }};

sysctrl_status_t SysCtrl_getDeviceDescriptor(sysctrl_device_type_t device_type, sysctrl_info_t *p_devices_list)
{
    sysctrl_status_t retval = SCSTATUS_ERROR;

    /* check input parameters validity */
    if (p_devices_list == NULL)
    {
        return(SCSTATUS_ERROR);
    }

    /* check if device is already initialized */
    if (custom_func[device_type].initialized == 0)
    {
        /* Init SysCtrl functions pointers */
        retval = atcma_init_sysctrl_func_ptrs(&custom_func[device_type]);
        if (retval == SCSTATUS_OK)
        {
            /* device is initialized now */
            custom_func[device_type].initialized = 1;
        }
    }

    if (retval == SCSTATUS_OK)
    {
        retval = (*custom_func[device_type].f_getDeviceDescriptor)(device_type, p_devices_list);
    }

    return(retval);
}

sysctrl_status_t SysCtrl_power_on(sysctrl_device_type_t device_type)
{
    sysctrl_status_t retval = SCSTATUS_ERROR;

    /* check if device is initialized */
    if (custom_func[device_type].initialized == 1)
    {
        retval = (*custom_func[device_type].f_power_on)(device_type);
    }
    else
    {
        PrintErr("Device type %d is not initialized", device_type);
    }

    return(retval);
}

sysctrl_status_t SysCtrl_power_off(sysctrl_device_type_t device_type)
{
    sysctrl_status_t retval = SCSTATUS_ERROR;

    /* check if device is initialized */
    if (custom_func[device_type].initialized == 1)
    {
        retval = (*custom_func[device_type].f_power_off)(device_type);
    }
    else
    {
        PrintErr("Device type %d is not initialized", device_type);
    }

    return(retval);
}

sysctrl_status_t SysCtrl_reset_device(sysctrl_device_type_t device_type)
{
    sysctrl_status_t retval = SCSTATUS_ERROR;

    /* check if device is initialized */
    if (custom_func[device_type].initialized == 1)
    {
        retval = (*custom_func[device_type].f_reset_device)(device_type);
    }
    else
    {
        PrintErr("Device type %d is not initialized", device_type);
    }

    return(retval);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
