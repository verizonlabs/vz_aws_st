/**
  ******************************************************************************
  * @file    board_buttons.c
  * @author  MCD Application Team
  * @brief   Implements functions for user buttons actions
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board_buttons.h"
#include "dc_config.h"
#include "dc_control.h"
#include "plf_config.h"

void BB_UserButton_Pressed(void)
{
    /* USER can implement button action here
    * WARNING ! this function is called under an IT, treatment has to be short !
    * (like posting an event into a queue for example)
    */
    dc_ctrl_post_event_debounce(DC_COM_BUTTON_SEL_EVENT);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
