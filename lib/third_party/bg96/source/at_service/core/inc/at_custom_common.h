/**
  ******************************************************************************
  * @file    Middlewares\ST\at_service\core\inc\at_custom_common.h
  * @author  MCD Application Team
  * @brief   This file defines all the common custom types
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

#ifndef AT_CUSTOM_COMMON_H_
#define AT_CUSTOM_COMMON_H_

#include "at_core.h"
#include "at_parser.h"
#include "sysctrl.h"
#include "ipc_common.h"

#define _AT_INVALID ((uint32_t) 0xFFFFFFFFU)

typedef enum
{
    INTERMEDIATE_CMD = 0,
    FINAL_CMD        = 1,
}atcustom_FinalCmd_t;

/* functions prototypes  ----------------------------------------------------------*/
typedef void        (*ATC_initTypeDef)(atparser_context_t *p_atp_ctxt);
typedef uint8_t     (*ATC_checkEndOfMsgCallbackTypeDef)(uint8_t rxChar);
typedef at_status_t (*ATC_getCmdTypeDef)(atparser_context_t *p_atp_ctxt,
                                         uint32_t *p_ATcmdTimeout);
typedef at_endmsg_t (*ATC_extractElementTypeDef)(atparser_context_t *p_atp_ctxt,
                                                 IPC_RxMessage_t *p_msg_in,
                                                 uint16_t *p_parseIndex,
                                                 at_element_info_t *element_infos);
typedef at_action_rsp_t (*ATC_analyzeCmdTypeDef)(atparser_context_t *p_atp_ctxt,
                                                 IPC_RxMessage_t *p_msg_in,
                                                 at_element_info_t *element_infos);
typedef at_action_rsp_t (*ATC_analyzeParamTypeDef)(atparser_context_t *p_atp_ctxt,
                                                   IPC_RxMessage_t *p_msg_in,
                                                   at_element_info_t *element_infos);
typedef at_action_rsp_t (*ATC_terminateCmdTypedef)(atparser_context_t *p_atp_ctxt, at_element_info_t *element_infos);
typedef at_status_t (*ATC_get_rsp)(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf);
typedef at_status_t (*ATC_get_urc)(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf);

/* structures prototypes  --------------------------------------------------*/
typedef struct
{
    uint8_t                            initialized;
    ATC_initTypeDef                    f_init;
    ATC_checkEndOfMsgCallbackTypeDef   f_checkEndOfMsgCallback;
    ATC_getCmdTypeDef                  f_getCmd;
    ATC_extractElementTypeDef          f_extractElement;
    ATC_analyzeCmdTypeDef              f_analyzeCmd;
    ATC_analyzeParamTypeDef            f_analyzeParam;
    ATC_terminateCmdTypedef            f_terminateCmd;
    ATC_get_rsp                        f_get_rsp;
    ATC_get_urc                        f_get_urc;
} atcustom_funcPtrs_t;


/* functions prototypes  ----------------------------------------------------------*/
at_status_t atcc_initParsers(sysctrl_device_type_t device_type);

void atcc_init(at_context_t *p_at_ctxt);
ATC_checkEndOfMsgCallbackTypeDef atcc_checkEndOfMsgCallback(at_context_t *p_at_ctxt);
at_status_t atcc_getCmd(at_context_t *p_at_ctxt, uint32_t *p_ATcmdTimeout);
at_endmsg_t atcc_extractElement(at_context_t *p_at_ctxt,
                                IPC_RxMessage_t *p_msg_in,
                                uint16_t *p_parseIndex,
                                at_element_info_t *element_infos);
at_action_rsp_t atcc_analyzeCmd(at_context_t *p_at_ctxt,
                                IPC_RxMessage_t *p_msg_in,
                                at_element_info_t *element_infos);
at_action_rsp_t atcc_analyzeParam(at_context_t *p_at_ctxt,
                                  IPC_RxMessage_t *p_msg_in,
                                  at_element_info_t *element_infos);
at_action_rsp_t atcc_terminateCmd(at_context_t *p_at_ctxt, at_element_info_t *element_infos);
at_status_t atcc_get_rsp(at_context_t *p_at_ctxt, at_buf_t *p_rsp_buf);
at_status_t atcc_get_urc(at_context_t *p_at_ctxt, at_buf_t *p_rsp_buf);

#endif /* AT_CUSTOM_COMMON_H_ */
