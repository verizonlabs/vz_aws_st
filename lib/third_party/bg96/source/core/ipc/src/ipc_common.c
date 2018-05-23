/**
  ******************************************************************************
  * @file    Middlewares\ST\core\ipc\src\ipc_common.c
  * @author  MCD Marketing / Vertical Application Team
  * @version V0.4
  * @date    29-June-2017
  * @brief   This file provides common code for IPC
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright � 2017 STMicroelectronics International N.V.
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

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "ipc_common.h"
#if (IPC_USE_UART == 1)
#include "ipc_uart.h"
#endif
/* NOT SUPPORTED YET
#if (IPC_USE_SPI == 1)
#include "ipc_spi.h"
#endif
#if (IPC_USE_I2C == 1)
#include "ipc_i2c.h"
#endif
*/

/* Private constants ----------------------------------------------*/
IPC_ClientDescription_t g_IPC_Devices_List[IPC_MAX_DEVICES];

/* Private functions prototypes ----------------------------------------------*/


/* Exported functions --------------------------------------------------------*/
/**
* @brief  The function initialize the global variables of the IPC.
* @param  device IPC device identifier.
* @param  itf_type Type of the interface.
* @param  h_itf Handle to the HAL structure of the interface.
* @retval status
*/
IPC_Status_t IPC_init(IPC_Device_t device, IPC_Interface_t itf_type, void *h_itf)
{
    IPC_Status_t status;

    /* check that device value is valid */
    if (device >= IPC_MAX_DEVICES)
    {
        /* IPC device id exceed maximum size defined */
        return(IPC_ERROR);
    }

    if (itf_type == IPC_INTERFACE_UART)
    {
#if (IPC_USE_UART == 1)
        status = IPC_init_uart(device, (UART_HandleTypeDef*) h_itf);
#else
        status = IPC_ERROR;
#endif
    }
    else if (itf_type == IPC_INTERFACE_SPI)
    {
#if (IPC_USE_SPI == 1)
        status = IPC_init_spi(device, (SPI_HandleTypeDef*) h_itf);
#else
        status = IPC_ERROR;
#endif
    }
    else
    {
        /* interface not supported yet */
        status = IPC_ERROR;
    }

    return(status);
}

/**
* @brief  Reinitialize the IPC global variables.
* @param  device IPC device identifier.
* @retval status
*/
IPC_Status_t IPC_deinit(IPC_Device_t  device)
{
    IPC_Status_t status;

    status = IPC_deinit_uart(device);

    return(status);
}

/**
* @brief  Open a specific channel.
* @param  hipc IPC handle to open.
* @param  device IPC device identifier.
* @param  mode IPC mode (char or stream).
* @param  pRxClientCallback Callback ptr called when a message has been received.
* @param  pTxClientCallback Callback ptr called when a message has been send.
* @param  pCheckEndOfMsg Callback ptr to the function used to analyze if char received is a termination char
* @retval status
*/
IPC_Status_t IPC_open(IPC_Handle_t *hipc,
                           IPC_Device_t  device,
                           IPC_Mode_t    mode,
                           IPC_RxCallbackTypeDef pRxClientCallback,
                           IPC_TxCallbackTypeDef pTxClientCallback,
                           IPC_CheckEndOfMsgCallbackTypeDef pCheckEndOfMsg)
{
    IPC_Status_t status;

    /* check if the device has been correctly initialized */
    if (g_IPC_Devices_List[device].state != IPC_STATE_INITIALIZED)
    {
        return(IPC_ERROR);
    }

    if (g_IPC_Devices_List[device].phy_int.interface_type == IPC_INTERFACE_UART)
    {
#if (IPC_USE_UART == 1)
      status = IPC_open_uart(hipc, device, mode, pRxClientCallback, pTxClientCallback, pCheckEndOfMsg);
#else
      status = IPC_ERROR;
#endif
    }
    else if (g_IPC_Devices_List[device].phy_int.interface_type == IPC_INTERFACE_SPI)
    {
#if (IPC_USE_SPI == 1)
      status = IPC_open_spi(hipc, device, mode, pRxClientCallback, pTxClientCallback, pCheckEndOfMsg);
#else
      status = IPC_ERROR;
#endif
    }

    return(status);
}

/**
* @brief  Close a specific channel.
* @param  hipc IPC handle to close.
* @retval status
*/
IPC_Status_t IPC_close(IPC_Handle_t *hipc)
{
    IPC_Status_t status;

    status = IPC_close_uart(hipc);

    return(status);
}

/**
* @brief  Reset a specific channel.
* @param  hipc IPC handle to reset.
* @retval status
*/
IPC_Status_t IPC_reset(IPC_Handle_t *hipc)
{
    IPC_Status_t status;

    status = IPC_reset_uart(hipc);

    return(status);
}


/**
* @brief  Select current channel.
* @param  hipc IPC handle to select.
* @retval status
*/
IPC_Status_t IPC_select(IPC_Handle_t *hipc)
{
    IPC_Status_t status;

    status = IPC_select_uart(hipc);

    return(status);
}

/**
* @brief  Get other channel handle if exists.
* @param  hipc IPC handle.
* @retval IPC_Handle_t*
*/
IPC_Handle_t *IPC_get_other_channel(IPC_Handle_t *hipc)
{
    return(IPC_get_other_channel_uart(hipc));
}

/**
* @brief  Send data over a channel.
* @param  hipc IPC handle.
* @param  p_TxBuffer Pointer to the data buffer to transfer.
* @param  bufsize Length of the data buffer.
* @retval status
*/
IPC_Status_t IPC_send(IPC_Handle_t *hipc, uint8_t* p_TxBuffer, uint16_t bufsize)
{
    IPC_Status_t status;

    status = IPC_send_uart(hipc, p_TxBuffer, bufsize);

    return(status);
}

/**
* @brief  Receive a message from a channel.
* @param  hipc IPC handle.
* @param  p_msg Pointer to the IPC message structure to fill with received message.
* @retval status
*/
IPC_Status_t IPC_receive(IPC_Handle_t *hipc, IPC_RxMessage_t *p_msg)
{
    IPC_Status_t status;

    status = IPC_receive_uart(hipc, p_msg);

    return(status);
}

/**
* @brief  Receive a data buffer from a channel.
* @param  hipc IPC handle.
* @param  p_buffer Pointer to the data buffer to transfer.
* @param  p_len Length of the data buffer.
* @retval status
*/
IPC_Status_t IPC_streamReceive(IPC_Handle_t *hipc,  uint8_t *p_buffer, int16_t *p_len)
{
#if (IPC_USE_STREAM_MODE == 1)
      IPC_Status_t status;

    status = IPC_streamReceive_uart(hipc, p_buffer, p_len);
    return(status);
#else
   return(IPC_ERROR);
#endif  /* IPC_USE_STREAM_MODE */
}

/**
* @brief  Dump content of IPC Rx queue (for debug purpose).
* @param  hipc IPC handle.
* @param  readable If equal 1, print special characters explicitly (<CR>, <LF>, <NULL>).
* @retval none
*/
void IPC_dump_RX_queue(IPC_Handle_t *hipc, uint8_t readable)
{
    IPC_dump_RX_queue_uart(hipc, readable);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

