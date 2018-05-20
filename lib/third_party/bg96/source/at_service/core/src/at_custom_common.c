/**
  ******************************************************************************
  * @file    Middlewares\ST\at_service\core\src\at_custom_common.c
  * @author  MCD Application Team
  * @brief   This file provides common code for devices
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

/*
AT+<X>=?     : TEST COMMAND
AT+<X>?      : READ COMMAND
AT+<X>=...   : WRITE COMMAND
AT+<X>       : EXECUTION COMMAND
*/

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"
#include "at_custom_common.h"
#include "at_datapack.h"
#include "at_util.h"
#include "at_custom_modem_api.h"
#include "string.h"

#if (USE_TRACE_INTERFACE_ATCUSTOM_COMMON == 1)
#include "trace_interface.h"
#define PrintINFO(format, args...) TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P0, "ATCustom:" format "\r\n", ## args)
#define PrintDBG(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P1, "ATCustom:" format "\r\n", ## args)
#define PrintAPI(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P2, "ATCustom API:" format "\r\n", ## args)
#define PrintErr(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_ERR, "ATCustom ERROR:" format "\r\n", ## args)
#elif (USE_PRINTF == 1)
#define PrintINFO(format, args...)  printf("ATCustom:" format "\r\n", ## args)
#define PrintDBG(format, args...)   printf("ATCustom:" format "\r\n", ## args)
#define PrintAPI(format, args...)   printf("ATCustom API:" format "\r\n", ## args)
#define PrintErr(format, args...)   printf("ATCustom ERROR:" format "\r\n", ## args)
#elif (USE_CONFIGPRINTF_ATCUSTOM_COMMON == 1)
#define PrintINFO(format, args...)  vLoggingPrintf("ATCustom:" format "\r\n", ## args)
#define PrintDBG(format, args...)   vLoggingPrintf("ATCustom:" format "\r\n", ## args)
#define PrintAPI(format, args...)   vLoggingPrintf("ATCustom API:" format "\r\n", ## args)
#define PrintErr(format, args...)   vLoggingPrintf("ATCustom ERROR:" format "\r\n", ## args)
#else
#define PrintINFO(format, args...)  do {} while(0)
#define PrintDBG(format, args...)   do {} while(0)
#define PrintAPI(format, args...)   do {} while(0)
#define PrintErr(format, args...)   do {} while(0)
#endif


/* AT custom functions ptrs table */
static atcustom_funcPtrs_t custom_func[DEVTYPE_INVALID] = { 0 };

/* functions  ----------------------------------------------------------*/

at_status_t atcc_initParsers(sysctrl_device_type_t device_type)
{
    at_status_t retval = ATSTATUS_ERROR;

    /* check if device is already initialized */
    if (custom_func[device_type].initialized == 0)
    {
        /* Init  AT functions pointers */
        retval = atcma_init_at_func_ptrs(&custom_func[device_type]);
        /* device is initialized now */
        if (retval == ATSTATUS_OK)
        {
            custom_func[device_type].initialized = 1;
        }
    }
    else
    {
        PrintErr("Device type %d AT functions already initialized", device_type);
    }

    return(retval);
}

void atcc_init(at_context_t *p_at_ctxt)
{
    (* custom_func[p_at_ctxt->device_type].f_init)(&p_at_ctxt->parser);
}

ATC_checkEndOfMsgCallbackTypeDef atcc_checkEndOfMsgCallback(at_context_t *p_at_ctxt)
{
    /* called under interrupion, do not put trace here */
    return(custom_func[p_at_ctxt->device_type].f_checkEndOfMsgCallback);
}

at_status_t atcc_getCmd(at_context_t *p_at_ctxt, uint32_t *p_ATcmdTimeout)
{
    at_status_t retval;

    retval = (*custom_func[p_at_ctxt->device_type].f_getCmd)(&p_at_ctxt->parser, p_ATcmdTimeout);

    PrintDBG("atcc_getCmd returned status = %d", retval);
    return(retval);
}

at_endmsg_t atcc_extractElement(at_context_t *p_at_ctxt,
                                IPC_RxMessage_t *p_msg_in,
                                uint16_t *p_parseIndex,
                                at_element_info_t *element_infos)
{
    at_endmsg_t retval;

    retval = (*custom_func[p_at_ctxt->device_type].f_extractElement)(&p_at_ctxt->parser, p_msg_in, p_parseIndex, element_infos);

    PrintDBG("start idx=%d  end idx=%d  size=%d rank=%d",
             element_infos->str_start_idx, element_infos->str_end_idx,
             element_infos->str_size, element_infos->param_rank);
    PrintDBG("atcc_extractElement returned endmsg = %d", retval);
    return(retval);
}

at_action_rsp_t atcc_analyzeCmd(at_context_t *p_at_ctxt,
                            IPC_RxMessage_t *p_msg_in,
                            at_element_info_t *element_infos)
{
    at_action_rsp_t retval;

    retval = (*custom_func[p_at_ctxt->device_type].f_analyzeCmd)(&p_at_ctxt->parser, p_msg_in, element_infos);

    PrintDBG("atcc_analyzeCmd returned action = 0x%x", retval);
    return(retval);
}

at_action_rsp_t atcc_analyzeParam(at_context_t *p_at_ctxt,
                              IPC_RxMessage_t *p_msg_in,
                              at_element_info_t *element_infos)
{
    at_action_rsp_t retval;

    /* check if element size is not null */
    if (element_infos->str_size == 0)
    {
        PrintDBG("element size = 0");
        /* return(ATACTION_RSP_ERROR); */
        return(ATACTION_RSP_NO_ACTION);
    }

    retval = (*custom_func[p_at_ctxt->device_type].f_analyzeParam)(&p_at_ctxt->parser, p_msg_in, element_infos);

    PrintDBG("atcc_analyzeParam returned action = 0x%x", retval);
    return(retval);
}

at_action_rsp_t atcc_terminateCmd(at_context_t *p_at_ctxt, at_element_info_t *element_infos)
{
    at_action_rsp_t retval;

    retval = (*custom_func[p_at_ctxt->device_type].f_terminateCmd)(&p_at_ctxt->parser, element_infos);

    PrintDBG("atcc_terminateCmd returned action = 0x%x", retval);
    return(retval);
}

at_status_t atcc_get_rsp(at_context_t *p_at_ctxt, at_buf_t *p_rsp_buf)
{
    at_status_t retval;

    retval = (*custom_func[p_at_ctxt->device_type].f_get_rsp)(&p_at_ctxt->parser, p_rsp_buf);

    PrintDBG("atcc_get_rsp returned status = %d", retval);
    return(retval);
}

at_status_t atcc_get_urc(at_context_t *p_at_ctxt, at_buf_t *p_rsp_buf)
{
    at_status_t retval;

    retval = (*custom_func[p_at_ctxt->device_type].f_get_urc)(&p_at_ctxt->parser, p_rsp_buf);

    PrintDBG("atcc_get_urc returned status = %d", retval);
    return(retval);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
