/**
******************************************************************************
* File Name          : dc_control.c
  * @author  MCD Application Team
* Description        : This file contains all the functions for dc_control
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


#include "cmsis_os.h"
#include "error_handler.h"
#include "plf_config.h"
#include "dc_common.h"
#include "dc_control.h"
#include "dc_data.h"
#include <stdio.h>
#include "stm32l4xx_hal.h"
#include "main.h"
#include "dc_common.h"
#include "plf_stack_size.h"

static osMessageQId dc_ctrl_event_queue;
#define EVENT_QUEUE_SIZE (uint32_t) (8)
osThreadId dc_CtrlEventTaskId = NULL;

/* debounce timer */
#define DEBOUNCE_TIMEOUT  (200) /* in millisec */
osTimerId DebounceTimerHandle;
__IO uint8_t debounce_ongoing = 0U;
void dc_ctrl_osTimerDebounceCallback(void const * argument);

/* static functions prototypes ------------------------------------------------------- */
static void dc_ctrl_event_handle(void);
static void dc_CtrlEventStartTask(void const * argument);
void dc_ctrl_osTimerDebounceCallback(void const * argument);

/* static functions ------------------------------------------------------- */
/* dc_ctrl task core function */
static void dc_ctrl_event_handle(void)
{
  osEvent event;
  dc_com_event_id_t event_id;

  /* definition and creation of DebounceTimer */
  osTimerDef(DebounceTimer, dc_ctrl_osTimerDebounceCallback);
  DebounceTimerHandle = osTimerCreate(osTimer(DebounceTimer), osTimerOnce, NULL);

  /* queue creation */
  osMessageQDef(ctrl_dc_event_queue, EVENT_QUEUE_SIZE, uint32_t);
  dc_ctrl_event_queue = osMessageCreate(osMessageQ(ctrl_dc_event_queue), NULL);

  for(;;)
  {
      event = osMessageGet(dc_ctrl_event_queue, osWaitForever);
      if (event.status == osEventMessage)
      {
          event_id = (dc_com_event_id_t)event.value.v;
          dc_com_write_event(&dc_com_db, event_id) ;
      }
  }
  /* USER CODE END mmmMngtStartTask */
}

static void dc_CtrlEventStartTask(void const * argument)
{
   dc_ctrl_event_handle();
}

/* dc_ctrl_osTimerDebounceCallback function */
void dc_ctrl_osTimerDebounceCallback(void const * argument)
{
    (void) argument;

    debounce_ongoing = 0U;
}

/* external functions ------------------------------------------------------- */
dc_ctrl_status_t dc_ctrl_event_init(void)
{
  return DC_CTRL_OK;
}

void dc_ctrl_post_event_normal(dc_com_event_id_t event_id)
{
    /* always post event to the queue */
    if (osMessagePut(dc_ctrl_event_queue, event_id, 0) != osOK)
    {
        ERROR_Handler(DBG_CHAN_DATA_CACHE, 7, ERROR_WARNING);
    }
}

void dc_ctrl_post_event_debounce(dc_com_event_id_t event_id)
{
    /* post event to the queue only if no ongoing debounce
     * limitation: all events with debounce are sharing same timer
     */
    (void) event_id;

    if (!debounce_ongoing)
    {
        if(osMessagePut (dc_ctrl_event_queue, event_id, 0) != osOK)
        {
            ERROR_Handler(DBG_CHAN_DATA_CACHE, 7, ERROR_WARNING);
        }
        else
        {
            osTimerStart(DebounceTimerHandle, DEBOUNCE_TIMEOUT);
            debounce_ongoing = 1;
        }
    }
}


dc_ctrl_status_t dc_ctrl_event_start(void)
{
  /* definition and creation of dc_CtrlEventTask */
  osThreadDef(DCCET, dc_CtrlEventStartTask, DC_CTRL_THREAD_PRIO, 0, DC_CTRL_THREAD_STACK_SIZE);
  dc_CtrlEventTaskId = osThreadCreate(osThread(DCCET), NULL);
  if(dc_CtrlEventTaskId == NULL)
  {
     ERROR_Handler(DBG_CHAN_DATA_CACHE, 7, ERROR_WARNING);
  }
  else
  {
#if (STACK_ANALYSIS_TRACE == 1)
      stackAnalysis_addThreadStackSizeByHandle(dc_CtrlEventTaskId, DC_CTRL_THREAD_STACK_SIZE);
#endif /* STACK_ANALYSIS_TRACE */
  }

  return DC_CTRL_OK;
}
