/**
  ******************************************************************************
  * @file    board_interrupts.c
  * @author  MCD Application Team
  * @brief   Implements HAL weak functions for Interrupts
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ipc_uart.h"
#include "board_buttons.h"
#include "plf_config.h"

/* NOTE : this code is designed for FreeRTOS */



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_13)
    {
        BB_UserButton_Pressed();
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    if (UartHandle->Instance == MODEM_UART_INSTANCE)
    {
        IPC_UART_RxCpltCallback(UartHandle);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    if (UartHandle->Instance == MODEM_UART_INSTANCE)
    {
        IPC_UART_TxCpltCallback(UartHandle);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
    if (UartHandle->Instance == MODEM_UART_INSTANCE)
    {
        IPC_UART_ErrorCallback(UartHandle);
    }
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
