/**
  ******************************************************************************
  * @file    Middlewares\ST\core\ipc\inc\ipc_uart.h
  * @author  MCD Application Team
  * @version V0.4
  * @date    29-June-2017
  * @brief   This file contains all the functions prototypes for uart IPC
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

#ifndef __IPC_UART_H_
#define __IPC_UART_H_

#include "ipc_common.h"


/* Exported functions ------------------------------------------------------- */
IPC_Status_t IPC_init_uart(IPC_Device_t  device, UART_HandleTypeDef  *huart);
IPC_Status_t IPC_deinit_uart(IPC_Device_t  device);
IPC_Status_t IPC_open_uart(IPC_Handle_t *hipc,
                                IPC_Device_t  device,
                                IPC_Mode_t    mode,
                                IPC_RxCallbackTypeDef pRxClientCallback,
                                IPC_TxCallbackTypeDef pTxClientCallback,
                                IPC_CheckEndOfMsgCallbackTypeDef pCheckEndOfMsg);
IPC_Status_t IPC_close_uart(IPC_Handle_t *hipc);
IPC_Status_t IPC_select_uart(IPC_Handle_t *hipc);
IPC_Status_t IPC_reset_uart(IPC_Handle_t *hipc);
IPC_Handle_t *IPC_get_other_channel_uart(IPC_Handle_t *hipc);
IPC_Status_t IPC_send_uart(IPC_Handle_t *hipc, uint8_t *p_TxBuffer, uint16_t bufsize);
IPC_Status_t IPC_receive_uart(IPC_Handle_t *hipc, IPC_RxMessage_t *p_msg);
IPC_Status_t IPC_streamReceive_uart(IPC_Handle_t *hipc,  uint8_t *p_buffer, int16_t *p_len);
void IPC_dump_RX_queue_uart(IPC_Handle_t *hipc, uint8_t readable);

void IPC_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle);
void IPC_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle);
void IPC_UART_ErrorCallback(UART_HandleTypeDef *UartHandle);


#endif /* __IPC_UART_H_ */
