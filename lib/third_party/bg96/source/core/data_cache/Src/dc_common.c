/**
  ******************************************************************************
  * File Name          : dc_common.c
  * @author  MCD Application Team
  * Description        : This file contains all the functions for Data cache common
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

#include "dc_common.h"
#include "dc_data.h"
#include <stdio.h>
#include <string.h>

static uint16_t   dc_user_number = 0U;

/* register a data cache callback */
dc_com_reg_id_t dc_com_register_gen_event_cb(dc_com_db_t* dc_db, dc_com_gen_event_callback_t notif_cb, void *private_gui_data)
{
    if(dc_db->user_number < DC_COM_MAX_NB_USERS)
    {
      dc_db->user_info[dc_user_number].user_reg_id       = dc_user_number;
      dc_db->user_info[dc_user_number].notif_cb          = notif_cb;
      dc_db->user_info[dc_user_number].private_user_data = private_gui_data;
      dc_user_number++;
    }
      
    return dc_user_number;
}

/* update a data info in the DC, calls the GUI callback if new value detected and GUI is registered */
dc_com_status_t dc_com_write(void *dc, int32_t res_id, void *data, uint16_t len)
{
    dc_com_reg_id_t reg_id;
    dc_com_event_id_t event_id = (dc_com_event_id_t)res_id;
    dc_base_rt_info_t* dc_base_rt_info;

    dc_com_db_t* com_db = (dc_com_db_t*)dc;
    memcpy((void*)(com_db->dc_db[res_id]),data, (uint32_t)len);
    dc_base_rt_info = (dc_base_rt_info_t*)(com_db->dc_db[res_id]);
    dc_base_rt_info->header.res_id = (dc_com_res_id_t)event_id;
    dc_base_rt_info->header.size   = len;
    
    for(reg_id = 0U; reg_id< DC_COM_MAX_NB_USERS; reg_id++)
    {
      if(com_db->user_info[reg_id].notif_cb != NULL)
      {
          com_db->user_info[reg_id].notif_cb(event_id, com_db->user_info[reg_id].private_user_data);
      }
    }

    
    return DC_COM_OK;
}

/* update a data info in the DC, calls the GUI callback if new value detected and GUI is registered */
dc_com_status_t dc_com_read(void *dc, int32_t res_id, void *data, uint16_t len)
{
    dc_com_db_t* com_db = (dc_com_db_t*)dc;
    memcpy(data, (void*)com_db->dc_db[res_id], (uint32_t)len);

    
    return DC_COM_OK;
}


/* send a data cache event */
dc_com_status_t dc_com_write_event (void *dc, dc_com_event_id_t event_id)
{
    dc_com_reg_id_t reg_id;
    dc_com_db_t* com_db = (dc_com_db_t*)dc;

    for(reg_id = 0U; reg_id< DC_COM_MAX_NB_USERS; reg_id++)
    {
      if(com_db->user_info[reg_id].notif_cb != NULL)
      {
          com_db->user_info[reg_id].notif_cb(event_id, com_db->user_info[reg_id].private_user_data);
      }
    }
    return DC_COM_OK;    
}

/* create the dc and load the data cache to RAM (BACKUPSRAM) with default value if specific setting does not exist. It returns the address of dc */
dc_com_status_t dc_com_init  (void *init_db, void *dc)
{
    return DC_COM_OK;    
}


/* unload the DC from RAM or BACKUP SRAM */
dc_com_status_t dc_com_deinit(void *init_db, void *dc)
{
    return DC_COM_OK;    
}


