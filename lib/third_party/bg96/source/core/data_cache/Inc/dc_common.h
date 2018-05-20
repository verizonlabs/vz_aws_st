/**
  ******************************************************************************
  * @file    dc_common.h
  * @author  MCD Application Team
  * @brief   This file contains all the functions prototypes for Data cache common
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

#ifndef __DC_COMMON_H
#define __DC_COMMON_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "dc_config.h"


/* Exported types ------------------------------------------------------------*/

typedef enum
{
    DC_COM_OK = 0x00,
    DC_COM_ERROR
}dc_com_status_t;

typedef enum
{
    DC_COM_REG_EVENT_OFF, /* Upper-layer not ready to the receive event notification from dc_com  */
    DC_COM_REG_EVENT_ON   /* Upper-layer is ready receive event notification from dc_com */
} dc_com_reg_state_notif_t;

typedef void   (*dc_com_gen_event_callback_t)(
		const dc_com_event_id_t event_id,
		void *private_user_data);

typedef void   (*dc_com_spe_event_callback_t)(
		void *data, 
		void *private_user_data);

typedef struct
{
	dc_com_reg_state_notif_t state_notif;
	dc_com_spe_event_callback_t event_cb;
	uint32_t timestamp; /* time stamp based on system tick from local MCU */
	uint8_t pending_notif; /* to track if GUI has read pending notification */
} dc_com_reg_notif_t;

typedef struct {
	uint16_t user_reg_id;
	dc_com_gen_event_callback_t notif_cb;
	void *private_user_data;
} dc_com_user_info_t;

typedef struct {
	uint16_t   user_number;
	dc_com_user_info_t user_info[DC_COM_MAX_NB_USERS];
	void *dc_db[DC_COM_SERV_MAX];
} dc_com_db_t;


/* Exported vars ------------------------------------------------------------*/

extern dc_com_db_t dc_com_db;

/* Exported functions ------------------------------------------------------- */

typedef uint16_t dc_com_reg_id_t;

dc_com_reg_id_t dc_com_register_gen_event_cb( /* to register a generic cb to a given dc_db */
	dc_com_db_t*dc_db,
	dc_com_gen_event_callback_t notif_cb, /* the user event callback */
	void *private_gui_data);				  /* user private data */

dc_com_status_t dc_com_register_gen_event_notif(
	dc_com_db_t*dc_db,
	dc_com_reg_state_notif_t com_reg_notif, 
	dc_com_res_id_t res_id); /* enable event notif from a resource */
 
dc_com_status_t dc_com_deregister_cb(dc_com_db_t*dc_db, dc_com_reg_id_t reg_id);

dc_com_reg_id_t dc_com_register_spe_event_cb (
	dc_com_db_t*dc_db,
	dc_com_spe_event_callback_t spe_notif_cb, 
	void *private_user_data, 
	dc_com_res_id_t res_id); /* register a specific callback notification par event */



dc_com_status_t dc_com_init  (void *init_db, void *dc); /* create the dc and load the data cache to RAM (BACKUPSRAM) with default value if specific setting does not exist. It returns the address of dc */
dc_com_status_t dc_com_deinit(void *init_db, void *dc); /* unload the DC from RAM or BACKUP SRAM  */
dc_com_status_t dc_com_write (void *dc, int32_t res_id, void *data, uint16_t len); /* update a data info in the DC, calls the GUI callback if new value detected and GUI is registered */
dc_com_status_t dc_com_reset (void *dc, int32_t res_id, void *data, uint16_t len); /* reset data info in the DC with default setting */
dc_com_status_t dc_com_read  (void *dc, int32_t res_id, void *data, uint16_t len); /* read current data info, before updating new data. it will be called by MMI but also can be called */
dc_com_status_t dc_com_write_event (void *dc, dc_com_event_id_t event_id); /* event */

#ifdef __cplusplus
}
#endif


#endif /* __DC_COMMON_H */

/***************************** (C) COPYRIGHT STMicroelectronics *******END OF FILE ************/
