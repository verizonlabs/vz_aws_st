/**
  ******************************************************************************
  * @file    Application\core\data_cache\Src\dc_main.c
  * @author  MCD Application Team
  * @brief   This file contains data cache initialisation
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#include "dc_data.h"
#include "dc_config.h"
#include "dc_main.h"
#include "dc_control.h"
#include <stdio.h>
#include <string.h>

/* static void *dc_com_db[MMI_DC_MAX_SERV]; */

dc_com_db_t dc_com_db;

static dc_time_date_info_t         time_date_info;
static dc_battery_info_t           battery_info;
static dc_hrm_info_t               hrm_info;
static dc_pedometer_info_t         pedometer_info;
static dc_activity_info_t          activity_info;
static dc_cellular_info_t          cellular_info;
static dc_pressure_info_t          pressure_info;
static dc_humidity_info_t          humidity_info;
static dc_temperature_info_t       temperature_info;
static dc_accelerometer_info_t     accelerometer_info;
static dc_gyroscope_info_t         gyroscope_info;
static dc_megnetometer_info_t      megnetometer_info;
/* dc_mmi_state_info_t              mmi_state_info; */
static dc_gesture_info_t           gesture_info;
static dc_button_info_t            button_info;
static dc_disp_state_info_t        display_info;

static dc_remote_ppp_server_rt_info_t remote_ppp_server_info;
static dc_remote_ppp_server_rt_info_t local_ppp_server_info;
static dc_ppp_client_rt_info_t        ppp_client_info;
static dc_radio_lte_rt_info_t         radio_lte_info;
static dc_nifman_rt_info_t            nifman_info;
static dc_nfmc_rt_info_t              nfmc_info;


dc_main_status_t dc_init ()
{
    int i;

    memset(&dc_com_db,0,sizeof(dc_com_db));

    dc_com_db.user_number = 0;

    dc_com_db.dc_db[DC_COM_TIME_DATE] 	           = &time_date_info;
    dc_com_db.dc_db[DC_COM_CELLULAR]  	           = &cellular_info;
    dc_com_db.dc_db[DC_COM_HRM]		           = &hrm_info;
    dc_com_db.dc_db[DC_COM_BATTERY]	           = &battery_info;
    dc_com_db.dc_db[DC_COM_PEDOMETER]	           = &pedometer_info;
    dc_com_db.dc_db[DC_COM_ACTIVITY]	           = &activity_info;
    dc_com_db.dc_db[DC_COM_GESTURE]	           = &gesture_info;
    dc_com_db.dc_db[DC_COM_DISPLAY]                = &display_info;

    dc_com_db.dc_db[DC_COM_REMOTE_PPP_SERVER_INFO] = &remote_ppp_server_info;
    dc_com_db.dc_db[DC_COM_LOCAL_PPP_SERVER_INFO]  = &local_ppp_server_info;
    dc_com_db.dc_db[DC_COM_PPP_CLIENT_INFO]        = &ppp_client_info;
    dc_com_db.dc_db[DC_COM_RADIO_LTE_INFO]         = &radio_lte_info;
    dc_com_db.dc_db[DC_COM_NIFMAN_INFO]            = &nifman_info;

    dc_com_db.dc_db[DC_COM_PRESSURE_INFO]          = &pressure_info;
    dc_com_db.dc_db[DC_COM_HUMIDITY_INFO]          = &humidity_info;
    dc_com_db.dc_db[DC_COM_TEMPERATURE_INFO]       = &temperature_info;
    dc_com_db.dc_db[DC_COM_ACCELEROMETER_INFO]     = &accelerometer_info;
    dc_com_db.dc_db[DC_COM_GYROSCOPE_INFO]         = &gyroscope_info;
    dc_com_db.dc_db[DC_COM_MAGNETOMETER_INFO]      = &megnetometer_info;
    dc_com_db.dc_db[DC_COM_NFMC_TEMPO_INFO]        = &nfmc_info;

    dc_com_db.dc_db[DC_COM_BUTTON_UP]	           = &button_info;
    dc_com_db.dc_db[DC_COM_BUTTON_DN]	           = &button_info;
    dc_com_db.dc_db[DC_COM_BUTTON_SEL]	           = &button_info;
    dc_com_db.dc_db[DC_COM_BUTTON_BAK]	           = &button_info;
    dc_com_db.dc_db[DC_COM_BUTTON_BAKL]	           = &button_info;



    /* user_reg default values init */
    for (i= 0 ; i < (int)DC_COM_MAX_NB_USERS; i++)
    {
        dc_com_db.user_info[dc_com_db.user_number].user_reg_id   = 0;

        dc_com_db.user_info[i].user_reg_id         = 0;
        dc_com_db.user_info[i].notif_cb            = 0;
        dc_com_db.user_info[i].private_user_data   = 0;
    }

    for (i= 0 ; i < (int)DC_COM_SERV_MAX; i++)
    {
        ((dc_base_rt_info_t*)(dc_com_db.dc_db[i]))->header.res_id      = (dc_com_res_id_t)i;
        ((dc_base_rt_info_t*)(dc_com_db.dc_db[i]))->rt_state           = DC_SERVICE_OFF;
    }

    /* time_date_info datas Init */
    time_date_info.rt_info.month   = 1;
    time_date_info.rt_info.year    = 47;

    /* time_date_info datas Init */

    return DC_MAIN_OK;
}

void dc_start (void)
{
    /* In this function, we start all DC tasks */
    dc_ctrl_event_start();
}
