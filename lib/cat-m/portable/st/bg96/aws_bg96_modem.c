/*
 * Copyright (C) 2018 pureIntegration. All Rights Reserved
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://pureintegration.com
 */

/* Includes ------------------------------------------------------------------*/
#include "aws_bg96_modem.h"
#include "board_buttons.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "plf_config.h"
#include "rtc.h"
#include "usart.h"
#include "dc_common.h"
#include "dc_control.h"
#include "cellular_init.h"
#include "dc_cellular.h"
#include "ipc_uart.h"
#include "com_sockets.h"
#include "nifman.h"
#include "radio_mngt.h"
#include "cellular_service.h"
#include "cellular_service_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cellular_service_task.h"

#define GOOD_PINCODE "1234"
#define BAD_PINCODE  "6789"
#define STACK_ANALYSIS_COUNTER  50  /*  Stack analysed every 5s */


/* Function prototypes -------------------------------------------------------*/

static void BG96_net_up_cb ( dc_com_event_id_t dc_event_id, void* private_gui_data );

#define mainLOGGING_MESSAGE_QUEUE_LENGTH    ( 15 )

/**
 * @brief Initialize the BG96 HAL.
 *
 * This function initializes the low level hardware drivers and must be called
 * before calling any other modem API
 *
 */

void BG96_HAL_Init(void)
{


  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART4_Init();


}

/**
 * @brief Initialize the BG96 drivers.
 *
 * This function initializes the low level drivers and must be called
 * before calling BG96_Modem_Start
 *
 */

void BG96_Modem_Init(void) {

	/* Do not change order */

	  (void)com_init();

	  CST_cellular_service_init();

	  cellular_init();

	  dc_com_register_gen_event_cb(&dc_com_db, BG96_net_up_cb, (void*) NULL);

	  osMessageQDef(BG96_modem_queue, 1, uint32_t);
	  BG96_modem_queue = osMessageCreate(osMessageQ(BG96_modem_queue), NULL);

	  if(BG96_modem_queue == NULL)
	  {
	     configPRINTF(("Can't create Queue\r\n"));
	  }

}

/**
 * @brief Starts the BG96 drivers.
 *
 * This function starts the low level  drivers and must be called last in the sequence
 * e.g. after BG96_HAL_Init and BG96_Modem_Init
 *
 */

void BG96_Modem_Start ()
{

	cellular_start();

}

/**
 * @brief Callback function.
 *
 * This function is called whenever there is an event on the network interface
 * manager. Only interested when the modem is up.
 *
 */

static void BG96_net_up_cb ( dc_com_event_id_t dc_event_id, void* private_gui_data )
{


    if( dc_event_id == DC_COM_NIFMAN_INFO)
    {
      dc_nifman_info_t  dc_nifman_info;
      dc_com_read( &dc_com_db, DC_COM_NIFMAN_INFO, (void *)&dc_nifman_info, sizeof(dc_nifman_info));
      if(dc_nifman_info.rt_state  ==  DC_SERVICE_ON)
      {
    	  osMessagePut(BG96_modem_queue, dc_event_id, 0);
    	  BG96_network_status = network_is_up;
    	  configPRINTF(("Network is up\r\n"));
      }
      else
      {
    	  configPRINTF(("Network is down\r\n"));
      }

    }

}

