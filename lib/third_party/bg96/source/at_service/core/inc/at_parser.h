/**
  ******************************************************************************
  * @file    Middlewares\ST\at_service\core\inc\at_parser.h
  * @author  MCD Application Team
  * @brief   This file defines all the functions prototypes for AT Parser
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

#ifndef AT_PARSER_H_
#define AT_PARSER_H_

#include "plf_config.h"
#include "at_core.h"
#include "sysctrl.h"
#include "ipc_common.h"

#if (RTOS_USED == 1)
#include "cmsis_os.h"
#endif /* RTOS_USED */

#define AT_CMD_DEFAULT_TIMEOUT    ((uint32_t)3000)

/* Exported types ------------------------------------------------------------*/
typedef enum
{
    CMD_MANDATORY_ANSWER_EXPECTED   = 0,
    CMD_OPTIONAL_ANSWER_EXPECTED    = 1,
}atparser_AnswerExpect_t;

typedef struct
{
    /* parameters set in AT Parser */
    at_msg_t             current_SID;     /* Current Service ID */

    /* parameters set in AT Custom */
    uint8_t                  step;            /* indicates which step in current SID treatment */
    atparser_AnswerExpect_t  answer_expected; /* expected answer type for this command */
    uint8_t                  is_final_cmd;    /* is it last command in current SID treatment ? */
    atcmd_desc_t             current_atcmd;   /* current AT command to send parameters */
    uint8_t                  endstr[3];       /* termination string for AT cmd */
    uint32_t                 cmd_timeout;     /* command timeout value */

    /* save ptr on input buffer */
    at_buf_t              *p_cmd_input;

}atparser_context_t;

typedef struct
{
    /* Core context */
    sysctrl_device_type_t       device_type;

    IPC_Device_t                ipc_device;
    IPC_Mode_t                  ipc_mode;

    at_bool_t             in_data_mode;   /* current mode is DATA mode or COMMAND mode */
    uint8_t               processing_cmd; /* indicate if a command is currently processed or if idle mode */
    at_bool_t             dataSent;       /* receive the confirmation that data has been sent by the IPC */

    /* Parser context */
    atparser_context_t   parser;

#if (RTOS_USED == 1)
    /* RTOS parameters */
    at_action_rsp_t     action_flags;
    at_buf_t            *p_rsp_buf;
    osSemaphoreId       s_SendConfirm_SemaphoreId;
#endif

}at_context_t;

typedef struct
{
    uint32_t    cmd_id_received;  /* cmd id received */
    uint16_t    param_rank;       /* current param */
    uint16_t    str_start_idx;    /* current param start index in the message */
    uint16_t    str_end_idx;      /* current param end index in the message */
    uint16_t    str_size;         /* current param size */
}at_element_info_t;

/* Exported functions ------------------------------------------------------- */
at_status_t ATParser_initParsers(sysctrl_device_type_t device_type);
at_status_t ATParser_init(at_context_t *p_at_ctxt, IPC_CheckEndOfMsgCallbackTypeDef *p_checkEndOfMsgCallback);
at_status_t ATParser_process_request(at_context_t *p_at_ctxt,
                                     at_msg_t msg_id, at_buf_t *p_cmd_buf);
at_action_send_t ATParser_get_ATcmd(at_context_t *p_at_ctxt,
                               uint8_t *p_ATcmdBuf, uint16_t *p_ATcmdSize, uint32_t *p_ATcmdTimeout);
at_action_rsp_t ATParser_parse_rsp(at_context_t *p_at_ctxt, IPC_RxMessage_t *p_message);
at_status_t ATParser_get_rsp(at_context_t *p_at_ctxt, at_buf_t *p_rsp_buf);
at_status_t ATParser_get_urc(at_context_t *p_at_ctxt, at_buf_t *p_rsp_buf);
void        ATParser_abort_request(at_context_t *p_at_ctxt);

#endif /* AT_PARSER_H_ */
