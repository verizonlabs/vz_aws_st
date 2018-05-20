/**
  ******************************************************************************
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
#include "dc_main.h"
#include "dc_service.h"
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

void BG96_HAL_Init(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART4_Init();

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */

  // USER CODE END WHILE

  // USER CODE BEGIN 3

  /* USER CODE END 3 */

}

void BG96_Modem_Init(void) {

	  /* statical init of components */

	  (void)com_init();

	  CST_cellular_service_init();

	  dc_init();

	  nifman_init();

	  dc_com_register_gen_event_cb(&dc_com_db, BG96_net_up_cb, (void*) NULL);

	  osMessageQDef(BG96_modem_queue, 1, uint32_t);
	  BG96_modem_queue = osMessageCreate(osMessageQ(BG96_modem_queue), NULL);

	  if(BG96_modem_queue == NULL)
	  {
	     configPRINTF(("Can't create Queue\r\n"));
	  }

}

/* StartDefaultTask function */
void BG96_Modem_Start ()
{

	dc_start();

	com_start();

	nifman_start();

	CST_cellular_service_start();

	radio_mngt_radio_on();

}

static void BG96_net_up_cb ( dc_com_event_id_t dc_event_id, void* private_gui_data )
{

  switch (dc_event_id)
  {
    case DC_COM_NIFMAN_INFO:
    {
      dc_nifman_rt_info_t  dc_nifman_rt_info;
      dc_com_read( &dc_com_db, DC_COM_NIFMAN_INFO, (void *)&dc_nifman_rt_info, sizeof(dc_nifman_rt_info));
      if(dc_nifman_rt_info.rt_state  ==  DC_SERVICE_ON)
      {
    	  osMessagePut(BG96_modem_queue, dc_event_id, 0);
    	  BG96_network_status = network_is_up;
    	  configPRINTF(("Network is up\r\n"));
      }
      else
      {
    	  configPRINTF(("Network is down\r\n"));
      }
      break;
    }

    default:
      break;
  }
}

