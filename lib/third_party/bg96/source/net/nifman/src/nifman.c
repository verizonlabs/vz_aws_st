/**
  ******************************************************************************
  * @brief   nifman.c
  * @author  MCD Application Team
  * @brief   This file contains all the functions of NIFMAN
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

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "cmsis_os.h"
#include "dc_common.h"
#include "dc_data.h"
#if (USE_PPPOSIF_CLIENT == 1)
#include "ppposif_client.h"
#endif
#include "error_handler.h"
#include "main.h"
#include "nifman.h"
#include "plf_config.h"
#include "plf_stack_size.h"

/* Internal Vars ------------------------------------------------------- */

static osMessageQId nifman_queue;

osThreadId  nifman_ThreadId = NULL;

/* Internal functions ------------------------------------------------------- */

/* Data cache callback ------------------------------------------------------- */
static void   nifman_notif_cb(dc_com_event_id_t dc_event_id, void* private_gui_data)
{
    if((dc_event_id == DC_COM_REMOTE_PPP_SERVER_INFO) ||(dc_event_id == DC_COM_PPP_CLIENT_INFO))
    {
      osMessagePut(nifman_queue, dc_event_id, 0);
    }
}


/* nifman_thread function */
static void nifman_thread(void const * argument)
{
  osEvent event;
  dc_com_event_id_t          dc_event_id;
  dc_remote_ppp_server_rt_info_t    server_rt_info;
  dc_ppp_client_rt_info_t    ppp_client_rt_info;
  dc_nifman_rt_info_t    nifman_rt_info;


  /* Modem activated : start ppp client config */

  int err = 0;
  if(err != 0)
  {
    ERROR_Handler(DBG_CHAN_NIFMAN, 1, ERROR_FATAL);
  }

  for(;;)
  {
    event = osMessageGet(nifman_queue, osWaitForever);
    dc_event_id = (dc_com_event_id_t)event.value.v;

    if(dc_event_id == DC_COM_REMOTE_PPP_SERVER_INFO)
    {
      dc_com_read(&dc_com_db, DC_COM_REMOTE_PPP_SERVER_INFO, (void*)&server_rt_info,sizeof(server_rt_info));
      switch(server_rt_info.rt_state)
      {
        case DC_SERVICE_ON:
        {
#if (USE_PPPOSIF_CLIENT == 1)
          err = ppposif_client_config();
          if(err != 0)
          {
            ERROR_Handler(DBG_CHAN_NIFMAN, 2, ERROR_FATAL);
            server_rt_info.rt_state   =  DC_SERVICE_FAIL;
            dc_com_write(&dc_com_db, DC_COM_REMOTE_PPP_SERVER, (void *)&server_rt_info, sizeof(server_rt_info));
          }
#endif
          break;
        }
        default:
        {
          server_rt_info.rt_state   =  DC_SERVICE_OFF;
          dc_com_write(&dc_com_db, DC_COM_REMOTE_PPP_SERVER_INFO, (void *)&server_rt_info, sizeof(server_rt_info));
          break;
        }
      }
    }

    else if(dc_event_id == DC_COM_PPP_CLIENT_INFO)
    {
      dc_com_read(&dc_com_db, DC_COM_PPP_CLIENT_INFO, (void*)&ppp_client_rt_info,sizeof(ppp_client_rt_info));
      switch(ppp_client_rt_info.rt_state)
      {
        case DC_SERVICE_ON:
        {
            dc_com_read( &dc_com_db, DC_COM_NIFMAN_INFO, (void *)&nifman_rt_info, sizeof(nifman_rt_info));
            nifman_rt_info.rt_state   =  DC_SERVICE_ON;
            nifman_rt_info.network    =  DC_CELLULAR_NETWORK;
            nifman_rt_info.ip_addr    =  ppp_client_rt_info.ip_addr;
            nifman_rt_info.gw         =  ppp_client_rt_info.gw;
            nifman_rt_info.netmask    =  ppp_client_rt_info.netmask;

            dc_com_write(&dc_com_db, DC_COM_NIFMAN_INFO, (void *)&nifman_rt_info, sizeof(nifman_rt_info));
            break;
        }
        default:
        {
            dc_com_read( &dc_com_db, DC_COM_NIFMAN_INFO, (void *)&nifman_rt_info, sizeof(nifman_rt_info));
            nifman_rt_info.rt_state   =  ppp_client_rt_info.rt_state;
            nifman_rt_info.network    =  DC_CELLULAR_NETWORK;
            nifman_rt_info.ip_addr    =  ppp_client_rt_info.ip_addr;
            nifman_rt_info.gw         =  ppp_client_rt_info.gw;
            nifman_rt_info.netmask    =  ppp_client_rt_info.netmask;

            dc_com_write(&dc_com_db, DC_COM_NIFMAN_INFO, (void *)&nifman_rt_info, sizeof(nifman_rt_info));
            break;
        }
      }
    }
  }
}

/* Exported functions ------------------------------------------------------- */
nifman_status_t nifman_init(void)
{
  osMessageQDef(nifman_queue, 1, uint32_t);
  nifman_queue = osMessageCreate(osMessageQ(nifman_queue), NULL);
  if(nifman_queue == NULL)
  {
    ERROR_Handler(DBG_CHAN_NIFMAN, 2, ERROR_FATAL);
    return NIFMAN_ERROR;
  }

  return NIFMAN_OK;
}

nifman_status_t nifman_start(void)
{
  dc_com_register_gen_event_cb(&dc_com_db, nifman_notif_cb, (void*) NULL);

  osThreadDef(NIFMAN, nifman_thread, NIFMAN_THREAD_PRIO, 0, NIFMAN_THREAD_STACK_SIZE);
  nifman_ThreadId = osThreadCreate(osThread(NIFMAN), NULL);
  if(nifman_ThreadId == NULL)
  {
    ERROR_Handler(DBG_CHAN_NIFMAN, 2, ERROR_FATAL);
    return NIFMAN_ERROR;
  }
  else
  {
#if (STACK_ANALYSIS_TRACE == 1)
            stackAnalysis_addThreadStackSizeByHandle(nifman_ThreadId, NIFMAN_THREAD_STACK_SIZE);
#endif /* STACK_ANALYSIS_TRACE */
  }

  return NIFMAN_OK;
}


#ifdef __cplusplus
}
#endif

/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/
