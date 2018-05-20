/**
  ******************************************************************************
  * @file    Middlewares\ST\at_service\custom\modem_bg96\src\at_custom_modem_api.c
  * @author  MCD Application Team
  * @brief   This file provides all the specific code to the
  *          BG96 Quectel modem: LTE-cat-M1 or LTE-cat.NB1(=NB-IOT) or GSM
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
#include "at_custom_modem_api.h"
#include "at_custom_modem_specific.h"
#include "sysctrl_specific.h"
#include "string.h"

/* BG96 COMPILATION FLAGS to define in project option if needed:
*
*  - CHECK_SIM_PIN : if defined, check if PIN code is required in SID_CS_INIT_MODEM
*
*
*/

#if (USE_TRACE_INTERFACE_ATCUSTOM_SPECIFIC == 1)
#include "trace_interface.h"
#define PrintINFO(format, args...) TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P0, "BG96:" format "\r\n", ## args)
#define PrintDBG(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P1, "BG96:" format "\r\n", ## args)
#define PrintAPI(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P2, "BG96 API:" format "\r\n", ## args)
#define PrintErr(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_ERR, "BG96 ERROR:" format "\r\n", ## args)
#define PrintBuf(pbuf, size)       TracePrintBufChar(DBG_CHAN_ATCMD, DBL_LVL_P1, (char *)pbuf, size);
#elif (USE_PRINTF == 1)
#define PrintINFO(format, args...)  printf("BG96:" format "\r\n", ## args)
#define PrintDBG(format, args...)   printf("BG96:" format "\r\n", ## args)
#define PrintAPI(format, args...)   printf("BG96 API:" format "\r\n", ## args)
#define PrintErr(format, args...)   printf("BG96 ERROR:" format "\r\n", ## args)
#define PrintBuf(format, args...)   do {} while(0)
#elif (USE_CONFIGPRINTF_ATCUSTOM_SPECIFIC == 1)
#define PrintINFO(format, args...)  vLoggingPrintf("BG96:" format "\r\n", ## args)
#define PrintDBG(format, args...)   vLoggingPrintf("BG96:" format "\r\n", ## args)
#define PrintAPI(format, args...)   vLoggingPrintf("BG96 API:" format "\r\n", ## args)
#define PrintErr(format, args...)   vLoggingPrintf("BG96 ERROR:" format "\r\n", ## args)
#define PrintBuf(format, args...)   do {} while(0) /* Cannot print a buffer using vLoggingPrintf */
#else
#define PrintINFO(format, args...)  do {} while(0)
#define PrintDBG(format, args...)   do {} while(0)
#define PrintAPI(format, args...)   do {} while(0)
#define PrintErr(format, args...)   do {} while(0)
#define PrintBuf(format, args...)   do {} while(0)
#endif

at_status_t atcma_init_at_func_ptrs(atcustom_funcPtrs_t *funcPtrs)
{
    at_status_t retval = ATSTATUS_ERROR;

#if defined(USE_MODEM_BG96)
    PrintDBG("Init AT func ptrs for device = MODEM QUECTEL BG96");

    /* init function pointers with BG96 functions */
    funcPtrs->f_init = ATCustom_BG96_init;
    funcPtrs->f_checkEndOfMsgCallback = ATCustom_BG96_checkEndOfMsgCallback;
    funcPtrs->f_getCmd = ATCustom_BG96_getCmd;
    funcPtrs->f_extractElement = ATCustom_BG96_extractElement;
    funcPtrs->f_analyzeCmd = ATCustom_BG96_analyzeCmd;
    funcPtrs->f_analyzeParam = ATCustom_BG96_analyzeParam;
    funcPtrs->f_terminateCmd = ATCustom_BG96_terminateCmd;
    funcPtrs->f_get_rsp = ATCustom_BG96_get_rsp;
    funcPtrs->f_get_urc = ATCustom_BG96_get_urc;

    retval = ATSTATUS_OK;
#else
#error AT custom does not match with selected modem
#endif /* USE_MODEM_BG96 */

    return(retval);
}

sysctrl_status_t atcma_init_sysctrl_func_ptrs(sysctrl_funcPtrs_t *funcPtrs)
{
    sysctrl_status_t retval = SCSTATUS_ERROR;

#if defined(USE_MODEM_BG96)
    PrintDBG("Init SysCtrl func ptrs for device = MODEM QUECTEL BG96");

    /* init function pointers with BG96 functions */
    funcPtrs->f_getDeviceDescriptor = SysCtrl_BG96_getDeviceDescriptor;
    funcPtrs->f_power_on =  SysCtrl_BG96_power_on;
    funcPtrs->f_power_off = SysCtrl_BG96_power_off;
    funcPtrs->f_reset_device = SysCtrl_BG96_reset;

    retval = SCSTATUS_OK;
#else
#error SysCtrl does not match with selected modem
#endif /* USE_MODEM_BG96 */

    return(retval);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

