/**
  ******************************************************************************
  * @file    Application\at_service\core\src\sysctrl.c
  * @author  MCD Application Team
  * @brief   This file provides code for generic System Control
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sysctrl.h"
#include "at_custom_modem_api.h"
#include "plf_config.h"

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
