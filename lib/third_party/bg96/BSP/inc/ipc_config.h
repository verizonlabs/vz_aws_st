/**
  ******************************************************************************
  * @file    ipc_config.h
  * @author  MCD Marketing / Vertical Application Team
  * @version V1.0.0
  * @date    27-March-2018
  * @brief   This file defines IPC Configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef IPC_CONFIG_H
#define IPC_CONFIG_H 1

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"
#include "features.h"

#define IPC_BUFFER_EXT    ((uint16_t) 400U) /* size addded to RX buffer because of RX queue implementation (using
                                            * headers for messages)
                                            */
#define IPC_RXBUF_MAXSIZE ((uint16_t) 1600U + IPC_BUFFER_EXT) /* maximun size of character queue
                                                              * size has to match ATCMD_MAX_CMD_SIZE
                                                              */

/* IPC tuning parameters */
#if (USE_SOCKETS_MODEM == 1)
/* SOCKET MODE (IP stack in the modem) */
#define IPC_USE_STREAM_MODE (0U)
#else
/*  STREAM MODE (IP stack in MCU) */
#define IPC_USE_STREAM_MODE (1U)
#define IPC_RXBUF_STREAM_MAXSIZE  ((uint16_t) IPC_RXBUF_MAXSIZE) /* maximum size of stream queue (if used) */
#endif

/* IPC_RXBUF_MAXSIZE and IPC_RXBUF_STREAM_MAXSIZE are defined above */
#define IPC_RXBUF_THRESHOLD  ((uint16_t) 20U)

/* IPC interface */
#define IPC_USE_UART (1U) /* UART activated by default */
#define IPC_USE_SPI  (0U) /* SPI NOT SUPPORTED YET */
#define IPC_USE_I2C  (0U) /* I2C NOT SUPPORTED YET */

/* Debug flags */
#define DBG_IPC_RX_FIFO  (0U)             /* additional debug infos */
#define DBG_QUEUE_SIZE ((uint16_t) 1000U) /* debug message history depth */

#ifdef __cplusplus
}
#endif

#endif /* IPC_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
