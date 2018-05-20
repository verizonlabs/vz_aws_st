/**
  ******************************************************************************
  * @file    Middlewares\ST\core\ipc\inc\ipc_rxfifo.h
  * @author  MCD Application Team
  * @version V0.4
  * @date    29-June-2017
  * @brief   This file contains all the functions prototypes common for managing
  *          IPC RX FIFO
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

#ifndef __IPC_RXFIFO_H_
#define __IPC_RXFIFO_H_

#include "ipc_common.h"

void RXFIFO_init(IPC_Handle_t *hipc);
void RXFIFO_writeCharacter(IPC_Handle_t* hipc, uint8_t rxChar);
int16_t RXFIFO_read(IPC_Handle_t *hipc, IPC_RxMessage_t *o_Msg);

#if (IPC_USE_STREAM_MODE == 1)
void RXFIFO_stream_init(IPC_Handle_t *hipc);
void RXFIFO_writeStream(IPC_Handle_t* hipc, uint8_t rxChar);
#endif /* IPC_USE_STREAM_MODE */

/* debug functions */
void RXFIFO_print_data(IPC_Handle_t *hipc, uint16_t index, uint16_t size, uint8_t readable);
void RXFIFO_readMsgHeader_at_pos(IPC_Handle_t *hipc, IPC_RxHeader_t *header, uint16_t pos);
uint16_t RXFIFO_getFreeBytes(IPC_Handle_t *hipc);

#if (DBG_IPC_RX_FIFO != 0)
void dump_RX_dbg_infos(IPC_Handle_t *hipc, uint8_t databuf, uint8_t queue);
#endif /* DBG_IPC_RX_FIFO */

#endif /* __IPC_RXFIFO_H_ */
