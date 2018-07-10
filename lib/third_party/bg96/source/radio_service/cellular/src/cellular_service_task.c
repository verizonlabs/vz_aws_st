/**
  ******************************************************************************
  * @file    Application\radio_service\cellular\src\cellular_service_task.c
  * @author  MCD Application Team
  * @brief   This file defines functions for Cellular Service
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"
#include "error_handler.h"
#include "plf_config.h"
#include "plf_stack_size.h"
#include "dc_common.h"
#include "dc_data.h"
#include "cellular_service.h"
#include "cellular_service_os.h"
#include "cellular_service_task.h"
#include "string.h"


#define CST_SETUP_NFMC       (0)

/* Test NFMC activation */
#define CST_TEST_NFMC        (0)
#if (CST_TEST_NFMC == 1)
static uint32_t CST_nfmc_test_count = 0;
#define CST_NFMC_TEST_COUNT_MAX 9
#endif

#define CST_TEST_REGISTER_FAIL        (0)
#if (CST_TEST_REGISTER_FAIL == 1)
static uint32_t CST_register_fail_test_count = 0;
#define CST_REGISTER_FAIL_TEST_COUNT_MAX 2
#endif

#define CST_MODEM_POWER_ON_LABEL "Modem power on"


#define CST_COUNT_FAIL_MAX 5

#define GOOD_PINCODE "1234"
#define CST_MODEM_POLLING_PERIOD_DEFAULT 5000

#define CST_BAD_SIG_RSSI 99

#define CST_POWER_ON_RESET_MAX      5
#define CST_RESET_MAX               5
#define CST_INIT_MODEM_RESET_MAX    5
#define CST_CSQ_MODEM_RESET_MAX     5
#define CST_GNS_MODEM_RESET_MAX     5
#define CST_ATTACH_RESET_MAX        5
#define CST_DEFINE_PDN_RESET_MAX    5
#define CST_ACTIVATE_PDN_RESET_MAX  5
#define CST_PPP_CONFIG_RETRY_MAX    5
#define CST_GLOBAL_RETRY_MAX        5


#define CST_LABEL "Cellular Service"
#define CST_VERSION_APPLI     (uint16_t)1

#define CST_DEFAULT_APN_STRING                 PDP_CONTEXT_DEFAULT_APN
#define CST_DEFAULT_CID_STRING                 PDP_CONTEXT_DEFAULT_MODEM_CID_STRING
#define CST_DEFAULT_MODE_STRING                "0"
#define CST_DEFAULT_SIM_SLOT_STRING            "0"
#define CST_DEFAULT_NFMC_ACTIVATION_STRING     "0"
#define CST_DEFAULT_NFMC_TEMPO1_STRING         "60000"
#define CST_DEFAULT_NFMC_TEMPO2_STRING        "120000"
#define CST_DEFAULT_NFMC_TEMPO3_STRING        "240000"
#define CST_DEFAULT_NFMC_TEMPO4_STRING        "480000"
#define CST_DEFAULT_NFMC_TEMPO5_STRING        "960000"
#define CST_DEFAULT_NFMC_TEMPO6_STRING       "1920000"
#define CST_DEFAULT_NFMC_TEMPO7_STRING       "3840000"
/* TODO : right default values */

#define CST_INPUT_STRING_SIZE    20
#define CST_DISPLAY_STRING_SIZE  20
#define CST_APN_SIZE             20

#define CST_DEFAULT_PARAMA_NB    12
#define CST_NFMC_BOOL_SIZE        2
#define CST_NFMC_VALUES_SIZE      6
#define CST_NFMC_TEMPO_NB         7U

#define CST_PDN_ACTIVATE_RETRY_DELAY 30000
#define CST_WAITING_FOR_NETWORK_STATUS_DELAY 15000




#if (USE_TRACE_INTERFACE_CELLULAR_SERVICE == 1)
#define PrintCellularService(format, args...)      TracePrint(DBG_CHAN_CELLULAR_SERVICE, DBL_LVL_P0, format, ## args)
#define PrintCellularServiceErr(format, args...)   TracePrint(DBG_CHAN_CELLULAR_SERVICE, DBL_LVL_ERR, "ERROR " format, ## args)
#elif (USE_PRINTF == 1)
#define PrintCellularService(format, args...)      printf(format , ## args)
#define PrintCellularServiceErr(format, args...)   printf(format , ## args)
#elif (USE_CONFIGPRINTF_CELLULAR_SERVICE == 1)
#define PrintCellularService(format, args...)      vLoggingPrintf("CST: " format "\r\n",## args)
#define PrintCellularServiceErr(format, args...)   vLoggingPrintf("CST ERR: " format "\r\n",## args)
#else
#define PrintCellularService(format, args...)   	do {} while(0)
#define PrintCellularServiceErr(format, args...)    do {} while(0)
#endif

#define CST_RESET_DEBUG (0)

typedef enum
{
    CST_MESSAGE_CS_EVENT    = 0,
    CST_MESSAGE_DC_EVENT    = 1,
    CST_MESSAGE_URC_EVENT   = 2,
    CST_MESSAGE_TIMER_EVENT = 3,
    CST_MESSAGE_CMD         = 4
} CST_message_type_t;


typedef enum
{
    CST_OFF_EVENT                  ,
    CST_INIT_EVENT                 ,
    CST_MODEM_POWER_ON_EVENT       ,
    CST_MODEM_POWERED_ON_EVENT     ,
    CST_MODEM_INITIALIZED_EVENT    ,
    CST_NETWORK_CALLBACK_EVENT     ,
    CST_SIGNAL_QUALITY_EVENT       ,
    CST_NW_REG_TIMEOUT_TIMER_EVENT ,
    CST_NETWORK_STATUS_EVENT       ,
    CST_NETWORK_STATUS_OK_EVENT    ,
    CST_MODEM_ATTACHED_EVENT       ,
    CST_PDP_ACTIVATED_EVENT        ,
    CST_MODEM_PDN_ACTIVATE_RETRY_TIMER_EVENT,
    CST_PDN_STATUS_EVENT           ,
    CST_MODEM_PPP_CONFIG_OK_EVENT  ,
    CST_MODEM_PPP_FAIL_EVENT       ,
    CST_FAIL_EVENT                 ,
    CST_POLLING_TIMER              ,
    CST_MODEM_URC                  ,
    CST_NO_EVENT                   ,
    CST_CMD_UNKWONW_EVENT
} CST_autom_event_t;

#define     CST_MODEM_NETWORK_STATUS_FAIL_STATE (CST_state_t)100 /* TODO : to add in cellular_service_task.h */

typedef enum
{
    CST_NO_FAIL                    ,
    CST_MODEM_POWER_ON_FAIL        ,
    CST_MODEM_RESET_FAIL           ,
    CST_MODEM_CSQ_FAIL             ,
    CST_MODEM_GNS_FAIL             ,
    CST_MODEM_REGISTER_FAIL        ,
    CST_MODEM_ATTACH_FAIL          ,
    CST_MODEM_PDP_DEFINE_FAIL      ,
    CST_MODEM_PDP_ACTIVATION_FAIL  ,
    CST_MODEM_PPP_CONFIG_FAIL_FAIL
} CST_fail_cause_t;

typedef enum
{
    CST_SIM_SLOT_MODEM_SOCKET         = 0,
    CST_SIM_SLOT_MODEM_EMBEDDED_SIM   = 1,
    CST_SIM_SLOT_STM32_EMBEDDED_SIM   = 2
} CST_sim_slot_type_t;

typedef struct
{
    uint16_t  type ;
    uint16_t  id  ;
} CST_message_t;

typedef struct
{
    CST_state_t          current_state;
    CST_fail_cause_t     fail_cause;
    CS_PDN_event_t       pdn_status;
    CS_SignalQuality_t   signal_quality;
    CS_NetworkRegState_t current_EPS_NetworkRegState;
    CS_NetworkRegState_t current_GPRS_NetworkRegState;
    CS_NetworkRegState_t current_CS_NetworkRegState;
    uint16_t             set_pdn_mode;
    uint16_t             activate_pdn_nfmc_tempo_count;
    uint16_t             register_retry_tempo_count;
    uint16_t             power_on_reset_count ;
    uint16_t             reset_reset_count ;
    uint16_t             init_modem_reset_count ;
    uint16_t             csq_reset_count ;
    uint16_t             gns_reset_count ;
    uint16_t             attach_reset_count ;
    uint16_t             activate_pdn_reset_count ;
    uint16_t             ppp_config_retry_count ;
    uint16_t             global_retry_count ;
} CST_context_t;

typedef struct
{
  uint32_t  active;
  uint32_t  value[CST_NFMC_TEMPO_NB];
  uint32_t  tempo[CST_NFMC_TEMPO_NB];
} CST_nfmc_context_t;

typedef struct
{
  uint8_t*            APN;
  CS_PDN_conf_id_t    CID;
  CST_sim_slot_type_t SIM_SLOT;
}CST_cellular_params_t;

static CST_cellular_params_t CST_cellular_params =
{
  NULL,
  (CS_PDN_conf_id_t)0,
  (CST_sim_slot_type_t)0    
};

static osMessageQId      cst_queue_id;
static osTimerId         cst_polling_timer_handle;
static osTimerId         CST_pdn_activate_retry_timer_handle;
static osTimerId         CST_waiting_for_network_status_timer_handle;
static osTimerId         CST_register_retry_timer_handle;


static uint64_t CST_IMSI;
static uint8_t CST_input_string[CST_INPUT_STRING_SIZE];
static uint8_t  CST_APN_string[CST_APN_SIZE];

#if (CST_SETUP_NFMC == 1)
static uint8_t CST_display_string[CST_DISPLAY_STRING_SIZE];
#endif

static CST_nfmc_context_t CST_nfmc_context;
static CST_context_t     cst_context = {    CST_MODEM_INIT_STATE, CST_NO_FAIL, CS_PDN_EVENT_NW_DETACH,    /* Automaton State, FAIL Cause,  */
                                            { 0, 0},                              /* signal quality */
                                            CS_NRS_NOT_REGISTERED_NOT_SEARCHING, CS_NRS_NOT_REGISTERED_NOT_SEARCHING, CS_NRS_NOT_REGISTERED_NOT_SEARCHING,
                                            0,                                     /* set_pdn_mode */
                                            0,                                     /* activate_pdn_nfmc_tempo_count */
                                            0,                                     /* register_retry_tempo_count */
                                            0, 0, 0, 0, 0, 0 ,0 ,0 ,0              /* Reset counters */
                                       };

uint8_t CST_signal_quality =0;

static CS_OperatorSelector_t    ctxt_operator = {
    .mode = CS_NRM_AUTO,
    .format = CS_ONF_NOT_PRESENT,
    .operator_name = "00101", };

static int CST_polling_timer_flag = 0;
static int CST_csq_count_fail     = 0;

/* external variable definition ----------------------------------------------------------*/
osThreadId CST_cellularServiceThreadId = NULL;


/* Static functions prototypes ----------------------------------------------------------*/
static CS_Status_t  CST_send_message(uint16_t  type, uint16_t cmd);
static void CST_setup_handler(void);
static void CST_select_sim(CST_sim_slot_type_t sim_slot);
static void CST_reset_fail_count(void);
static void CST_pdn_event_callback(CS_PDN_conf_id_t cid, CS_PDN_event_t pdn_event);
static void CST_network_reg_callback(void);
static void CST_location_info_callback(void);
static void  CST_data_cache_set(dc_service_rt_state_t dc_service_state);
static void CST_location_info_callback(void);
static void CST_config_fail_mngt(char* msg_fail, CST_fail_cause_t fail_cause,uint16_t* fail_count, uint16_t fail_max );
static void CST_modem_init(void);
static CS_Status_t CST_set_signal_quality(void);
static CS_Status_t  CST_send_message(uint16_t  type, uint16_t cmd);
static void CST_get_device_all_infos(void);
static void CST_subscribe_all_net_events(void);
static void CST_notif_cb(dc_com_event_id_t dc_event_id, void* private_gui_data);
static CST_autom_event_t CST_get_autom_event(osEvent event);
static void CST_power_on_only_modem_mngt(void);
static void CST_power_on_modem_mngt(void);
static CS_Status_t CST_reset_modem(void);
static void CST_reset_modem_mngt(void);
static void CST_init_modem_mngt(void);
static void CST_net_register_mngt(void);
static void CST_signal_quality_test_mngt(void);
static void CST_network_status_test_mngt(void);
static void CST_network_event_mngt(void);
static void CST_attach_modem_mngt(void);
static void CST_modem_define_pdn(void);
static CS_Status_t CST_modem_activate_pdn(void);
static void CST_config_ppp_fail_mngt(void);
static void CST_pdn_event_mngt(void);
static void CST_init_state(CST_autom_event_t autom_event);
static void CST_reset_state(CST_autom_event_t autom_event);
static void CST_modem_on_state(CST_autom_event_t autom_event);
static void CST_modem_powered_on_state(CST_autom_event_t autom_event);
static void CST_waiting_for_signal_quality_ok_state(CST_autom_event_t autom_event);
static void CST_waiting_for_network_status_state(CST_autom_event_t autom_event);
static void CST_network_status_ok_state(CST_autom_event_t autom_event);
static void CST_modem_registered_state(CST_autom_event_t autom_event);
static void CST_modem_pdn_activate_state(CST_autom_event_t autom_event);
static void CST_ppp_config_on_going_state(CST_autom_event_t autom_event);
static void CST_data_ready_state(CST_autom_event_t autom_event);
static void CST_fail_state(CST_autom_event_t autom_event);
static void CST_timer_handler(void);
static void CST_cellular_service_task(void const * argument);
static void CST_polling_timer_callback(void const * argument);
static void CST_pdn_activate_retry_timer_callback(void const * argument);
static void CST_waiting_for_network_status_timer_callback(void const * argument);
static void CST_register_retry_timer_callback(void const * argument);


#if (USE_SOCKETS_MODEM == 0)
static void CST_config_ppp_modem(void);
#endif

/* static functions ------------------------------------------------------- */
static void CST_setup_handler(void)
{
  //menu_utils_get_string("APN ",(uint8_t*)CST_APN_string, sizeof(CST_APN_string)); //AVK
  memset((uint8_t*)&CST_APN_string, 0 ,CST_APN_SIZE);
  CST_cellular_params.APN = CST_APN_string;

 // menu_utils_get_string("CID ",(uint8_t*)CST_input_string, sizeof(CST_input_string));
  CST_input_string[0]= 0x31;
  CST_cellular_params.CID = (CS_PDN_conf_id_t)(CST_input_string[0]-'0');

 // menu_utils_get_string("add pdn config (0: no / 1: yes) ",(uint8_t*)CST_input_string, sizeof(CST_input_string)); //AVK
  if(CST_input_string[0] == '1')
  {
     cst_context.set_pdn_mode = 1;
  }
  else
  {
     cst_context.set_pdn_mode = 0;
  }

  //menu_utils_get_string("Sim Slot (0: socket / 1: embedded sim / 2: host) ",(uint8_t*)CST_input_string, sizeof(CST_input_string)); //AVK
  //CST_cellular_params.SIM_SLOT = (CST_sim_slot_type_t)(CST_input_string[0]-'0');
   CST_cellular_params.SIM_SLOT = CST_SIM_SLOT_MODEM_SOCKET;


#if (CST_SETUP_NFMC == 1)
  menu_utils_get_string("NFMC activation (0: inactive / 1: active) ",(uint8_t*)CST_input_string, sizeof(CST_input_string));
  if(CST_input_string[0] == '1')
  {
     CST_nfmc_context.active = 1;
  }
  else
  {
     CST_nfmc_context.active = 0;
  }
  
  for (uint32_t i=0; i<CST_NFMC_TEMPO_NB ; i++)
  {
    sprintf((char*)CST_display_string,"NFMC value %d  ",i+1);
    menu_utils_get_string(CST_display_string,(uint8_t*)CST_input_string, sizeof(CST_input_string));
    CST_nfmc_context.value[i] = (uint32_t)atoi((char*)CST_input_string);
  }
#else
  CST_nfmc_context.active = 1;
  CST_nfmc_context.value[0] = (uint32_t)atoi((char*)CST_DEFAULT_NFMC_TEMPO1_STRING);
  CST_nfmc_context.value[1] = (uint32_t)atoi((char*)CST_DEFAULT_NFMC_TEMPO2_STRING);
  CST_nfmc_context.value[2] = (uint32_t)atoi((char*)CST_DEFAULT_NFMC_TEMPO3_STRING);
  CST_nfmc_context.value[3] = (uint32_t)atoi((char*)CST_DEFAULT_NFMC_TEMPO4_STRING);
  CST_nfmc_context.value[4] = (uint32_t)atoi((char*)CST_DEFAULT_NFMC_TEMPO5_STRING);
  CST_nfmc_context.value[5] = (uint32_t)atoi((char*)CST_DEFAULT_NFMC_TEMPO6_STRING);
  CST_nfmc_context.value[6] = (uint32_t)atoi((char*)CST_DEFAULT_NFMC_TEMPO7_STRING);
#endif
}


static void CST_select_sim(CST_sim_slot_type_t sim_slot)
{
    switch (sim_slot) {

      case CST_SIM_SLOT_MODEM_EMBEDDED_SIM:
        HAL_GPIO_WritePin(MDM_SIM_SELECT_1_GPIO_Port, MDM_SIM_SELECT_1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MDM_SIM_SELECT_0_GPIO_Port, MDM_SIM_SELECT_0_Pin, GPIO_PIN_RESET);
        PrintCellularService("STM32 ESIM SELECTED \r\n");
        break;

      case CST_SIM_SLOT_MODEM_SOCKET:
        HAL_GPIO_WritePin(MDM_SIM_SELECT_1_GPIO_Port, MDM_SIM_SELECT_1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MDM_SIM_SELECT_0_GPIO_Port, MDM_SIM_SELECT_0_Pin, GPIO_PIN_RESET);
        PrintCellularService("MODEM SIM SOCKET SELECTED \r\n");
        break;

      case CST_SIM_SLOT_STM32_EMBEDDED_SIM:
      default:
        HAL_GPIO_WritePin(MDM_SIM_SELECT_1_GPIO_Port, MDM_SIM_SELECT_1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MDM_SIM_SELECT_0_GPIO_Port, MDM_SIM_SELECT_0_Pin, GPIO_PIN_SET);

        PrintCellularService("MODEM SIM ESIM SELECTED \r\n");
        break;
    }
}

static void CST_reset_fail_count(void)
{
  cst_context.power_on_reset_count       = 0;
  cst_context.reset_reset_count          = 0;
  cst_context.init_modem_reset_count     = 0;
  cst_context.csq_reset_count            = 0;
  cst_context.gns_reset_count            = 0;
  cst_context.attach_reset_count         = 0;
  cst_context.activate_pdn_reset_count   = 0;
  cst_context.ppp_config_retry_count     = 0;
  cst_context.global_retry_count         = 0;
}


/* PDN event callback */
static void CST_pdn_event_callback(CS_PDN_conf_id_t cid, CS_PDN_event_t pdn_event)
{
   PrintCellularService("====================================CST_pdn_event_callback (cid=%d / event=%d)\n\r",
                        cid, pdn_event);
    cst_context.pdn_status = pdn_event;
    CST_send_message(CST_MESSAGE_CS_EVENT, CST_PDN_STATUS_EVENT);

}

/* URC callback */
static void CST_network_reg_callback(void)
{
    PrintCellularService("==================================CST_network_reg_callback\n\r");
    CST_send_message(CST_MESSAGE_CS_EVENT, CST_NETWORK_CALLBACK_EVENT);
}

static void CST_location_info_callback(void)
{
    PrintCellularService("CST_location_info_callback\n\r");
    /* TODO implement */
}

static void  CST_data_cache_set(dc_service_rt_state_t dc_service_state)
{
    dc_ppp_client_rt_info_t ppp_client_rt_info;

    dc_com_read(&dc_com_db, DC_COM_PPP_CLIENT_INFO, (void*)&ppp_client_rt_info, sizeof(ppp_client_rt_info));
    ppp_client_rt_info.rt_state = dc_service_state;
    dc_com_write(&dc_com_db, DC_COM_PPP_CLIENT_INFO, (void*)&ppp_client_rt_info, sizeof(ppp_client_rt_info));
}

/* failing configuration management */
static void CST_config_fail_mngt(char* msg_fail, CST_fail_cause_t fail_cause,uint16_t* fail_count, uint16_t fail_max )
{
    PrintCellularService("=== %s Fail !!! === \r\n", msg_fail);
    ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 1, ERROR_WARNING);

    *fail_count = *fail_count+1;
    cst_context.global_retry_count++;

    CST_data_cache_set(DC_SERVICE_FAIL);
    if((*fail_count <= fail_max) && (cst_context.global_retry_count <= CST_GLOBAL_RETRY_MAX))
    {
       cst_context.current_state = CST_MODEM_RESET_STATE;
       CST_send_message(CST_MESSAGE_CS_EVENT, CST_INIT_EVENT);
    }
    else
    {
       cst_context.current_state = CST_MODEM_FAIL_STATE;
       cst_context.fail_cause    = CST_MODEM_POWER_ON_FAIL;

       PrintCellularServiceErr("=== CST_set_fail_state %d - count %d/%d FATAL !!! ===\n\r",fail_cause,cst_context.global_retry_count, *fail_count);
       ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 1, ERROR_FATAL);
    }
}

/* init modem processing */
static void CST_modem_init(void)
{
    CS_init();
    if (atcore_task_start(ATCORE_THREAD_STACK_PRIO, ATCORE_THREAD_STACK_SIZE) != ATSTATUS_OK)
    {
       ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 2, ERROR_WARNING);
    }
}

/* init modem processing */
static CS_Status_t CST_set_signal_quality(void)
{
    CS_Status_t cs_status = CELLULAR_ERROR;
    dc_cellular_rt_info_t cellular_rt_info;
    CS_SignalQuality_t sig_quality;
    if (osCS_get_signal_quality(&sig_quality) == CELLULAR_OK)
    {
        CST_csq_count_fail = 0;
        if((sig_quality.rssi != cst_context.signal_quality.rssi) || (sig_quality.ber != cst_context.signal_quality.ber))
        {
           cst_context.signal_quality.rssi = sig_quality.rssi;
           cst_context.signal_quality.ber  = sig_quality.ber;

           dc_com_read(&dc_com_db, DC_COM_CELLULAR_INFO, (void *)&cellular_rt_info, sizeof(cellular_rt_info));
           /* if((sig_quality.rssi == 0) || (sig_quality.rssi == CST_BAD_SIG_RSSI)) */
           if(sig_quality.rssi == CST_BAD_SIG_RSSI)
           {
              cellular_rt_info.cs_signal_level    = DC_NO_ATTACHED;
              cellular_rt_info.cs_signal_level_db = DC_NO_ATTACHED;
           }
           else
           {
               cs_status = CELLULAR_OK;
               CST_signal_quality = sig_quality.rssi;
/*               cellular_rt_info.cs_signal_level = (dc_cs_signal_level_t)(sig_quality.rssi/20+1);   */ /* range 1..99 -> 1..5 */
               cellular_rt_info.cs_signal_level     = sig_quality.rssi;             /*  range 0..99 */
               cellular_rt_info.cs_signal_level_db  = (-113 + 2*sig_quality.rssi);  /* dBm */
           }
           dc_com_write(&dc_com_db, DC_COM_CELLULAR_INFO, (void *)&cellular_rt_info, sizeof(cellular_rt_info));
        }

        PrintCellularService(" -Sig quality rssi : %d\n\r", sig_quality.rssi);
        PrintCellularService(" -Sig quality ber  : %d\n\r", sig_quality.ber);
    }
    else
    {
        CST_csq_count_fail++;
        PrintCellularService("Modem signal quality error\n\r");
        if(CST_csq_count_fail >= CST_COUNT_FAIL_MAX)
        {
           PrintCellularService("Modem signal quality error max\n\r");
           CST_csq_count_fail = 0;
           CST_config_fail_mngt(  "CS_get_signal_quality",
                        CST_MODEM_CSQ_FAIL,
                        &cst_context.csq_reset_count,
                        CST_CSQ_MODEM_RESET_MAX );
        }
    }
    return cs_status;
}

#if 0
/* set network status in CST context */
static void CST_set_network_status(void)
{
    CS_Status_t cs_status = CELLULAR_ERROR;
    CS_RegistrationStatus_t reg_status;

    cs_status = osCDS_get_net_status(&reg_status);
    if(cs_status == CELLULAR_OK)
    {
      cst_context.current_EPS_NetworkRegState  = reg_status.EPS_NetworkRegState;
      cst_context.current_GPRS_NetworkRegState = reg_status.GPRS_NetworkRegState;
      cst_context.current_CS_NetworkRegState   = reg_status.CS_NetworkRegState;
    }
    else
    {
        CST_config_fail_mngt(  "osCDS_get_net_status",
                        CST_MODEM_GNS_FAIL,
                        &cst_context.gns_reset_count,
                        CST_GNS_MODEM_RESET_MAX );
    }
}
#endif

/* send message to cellular service task */
static CS_Status_t  CST_send_message(uint16_t  type, uint16_t cmd)
{
    CST_message_t cmd_message;
    cmd_message.type = type;
    cmd_message.id   = cmd;
    osMessagePut(cst_queue_id, *(uint32_t*)&cmd_message, 0);

    return CELLULAR_OK;
}

static uint64_t CST_util_ipow(uint32_t base, uint32_t exp)
{
    /* implementation of power function */
    uint64_t result = 1;
    while (exp)
    {
        if (exp & 1)
        {
            result *= base;
        }
        exp >>= 1;
        base *= base;
    }

    return result;
}

static uint64_t CST_util_convertStringToInt64(uint8_t *p_string, uint32_t size)
{
  uint16_t idx, nb_digit_ignored = 0, loop = 0;
  uint64_t conv_nbr = 0;


  /* decimal value */
  for (idx=0; idx < size; idx++)
  {
    loop++;
    conv_nbr = conv_nbr + ((p_string[idx] - 48) * CST_util_ipow(10, (size - loop - nb_digit_ignored)));
  }

  return(conv_nbr);
}


static uint32_t CST_calculate_tempo_value (uint32_t nb, uint32_t value, uint64_t imsi)
{
  return value + imsi%value;
}

static void CST_fill_nfmc_tempo(uint64_t imsi_p)
{
  uint32_t i;
 dc_nfmc_rt_info_t nfmc_info;
  
  if(CST_nfmc_context.active != 0)
  {
    nfmc_info.activate = 1;
    for (i=0U ; i< CST_NFMC_TEMPO_NB; i++)
    {
      CST_nfmc_context.tempo[i] = CST_calculate_tempo_value(i, CST_nfmc_context.value[i], imsi_p);
      nfmc_info.tempo[i] = CST_nfmc_context.tempo[i];
      PrintCellularService("VALUE/TEMPO %d/%d\n\r",  CST_nfmc_context.value[i], CST_nfmc_context.tempo[i]);
    }
    nfmc_info.rt_state = DC_SERVICE_ON;
  }
  else
  {
    nfmc_info.activate = 0;
    nfmc_info.rt_state = DC_SERVICE_OFF;
  }
  dc_com_write(&dc_com_db, DC_COM_NFMC_TEMPO, (void *)&nfmc_info, sizeof(nfmc_info));

}


/* update device info in data cache */
static void CST_get_device_all_infos(void)
{
    dc_cellular_rt_info_t cellular_rt_info;
    CS_DeviceInfo_t       cst_device_info;

    memset((void *)&cst_device_info, 0, sizeof(CS_DeviceInfo_t));

    cst_device_info.field_requested = CS_DIF_IMEI_PRESENT;
    if (osCDS_get_device_info(&cst_device_info) == CELLULAR_OK)
    {
        PrintCellularService(" -IMEI: %s\n\r", cst_device_info.u.imei);
    }
    else
    {
        PrintCellularService("IMEI error\n\r");
    }

    cst_device_info.field_requested = CS_DIF_MANUF_NAME_PRESENT;
    if (osCDS_get_device_info(&cst_device_info) == CELLULAR_OK)
    {
        PrintCellularService(" -MANUFACTURER: %s\n\r", cst_device_info.u.manufacturer_name);

        dc_com_read(&dc_com_db, DC_COM_CELLULAR_INFO, (void *)&cellular_rt_info, sizeof(cellular_rt_info));
        memcpy(cellular_rt_info.mno_name, cst_device_info.u.manufacturer_name, MAX_SIZE_MNO_NAME);
        cellular_rt_info.rt_state              = DC_SERVICE_ON;
        dc_com_write(&dc_com_db, DC_COM_CELLULAR_INFO, (void *)&cellular_rt_info, sizeof(cellular_rt_info));

    }
    else
    {
        PrintCellularService("Manufacturer Name error\n\r");
    }

    cst_device_info.field_requested = CS_DIF_MODEL_PRESENT;
    if (osCDS_get_device_info(&cst_device_info) == CELLULAR_OK)
    {
        PrintCellularService(" -MODEL: %s\n\r", cst_device_info.u. model);
    }
    else
    {
        PrintCellularService("Model error\n\r");
    }

    cst_device_info.field_requested = CS_DIF_REV_PRESENT;
    if (osCDS_get_device_info(&cst_device_info) == CELLULAR_OK)
    {
        PrintCellularService(" -REVISION: %s\n\r", cst_device_info.u.revision);
    }
    else
    {
        PrintCellularService("Revision error\n\r");
    }

    cst_device_info.field_requested = CS_DIF_SN_PRESENT;
    if (osCDS_get_device_info(&cst_device_info) == CELLULAR_OK)
    {
        PrintCellularService(" -SERIAL NBR: %s\n\r", cst_device_info.u.serial_number);
    }
    else
    {
        PrintCellularService("Serial Number error\n\r");
    }

    cst_device_info.field_requested = CS_DIF_IMSI_PRESENT;
    if (osCDS_get_device_info(&cst_device_info) == CELLULAR_OK)
    {
        CST_IMSI = CST_util_convertStringToInt64(cst_device_info.u.imsi, 15);
        PrintCellularService(" -IMSI: %llu\n\r", CST_IMSI);
        CST_fill_nfmc_tempo(CST_IMSI);
    }
    else
    {
        PrintCellularService("IMSI error\n\r");
    }

    cst_device_info.field_requested = CS_DIF_PHONE_NBR_PRESENT;
    if (osCDS_get_device_info(&cst_device_info) == CELLULAR_OK)
    {
        PrintCellularService(" -PHONE NBR: %s\n\r", cst_device_info.u.phone_number);
    }
    else
    {
        PrintCellularService("Phone number error\n\r");
    }
}

/* subscribe to modem event */
static void CST_subscribe_all_net_events(void)
{
    PrintCellularService("Subscribe URC events: Network registration\n\r");
    osCDS_subscribe_net_event(CS_URCEVENT_CS_NETWORK_REG_STAT, CST_network_reg_callback);
    osCDS_subscribe_net_event(CS_URCEVENT_GPRS_NETWORK_REG_STAT, CST_network_reg_callback);
    osCDS_subscribe_net_event(CS_URCEVENT_EPS_NETWORK_REG_STAT, CST_network_reg_callback);
    PrintCellularService("Subscribe URC events: Location info\n\r");
    osCDS_subscribe_net_event(CS_URCEVENT_EPS_LOCATION_INFO, CST_location_info_callback);
    osCDS_subscribe_net_event(CS_URCEVENT_GPRS_LOCATION_INFO, CST_location_info_callback);
    osCDS_subscribe_net_event(CS_URCEVENT_CS_LOCATION_INFO, CST_location_info_callback);
}



/* Data cache callback */
static void CST_notif_cb(dc_com_event_id_t dc_event_id, void* private_gui_data)
{
    if((dc_event_id == DC_COM_REMOTE_PPP_SERVER_INFO) || (dc_event_id == DC_COM_PPP_CLIENT_INFO))
    {
      CST_message_t cmd_message;
      cmd_message.type = CST_MESSAGE_DC_EVENT;
      cmd_message.id   = (uint32_t)dc_event_id;
      osMessagePut(cst_queue_id, *(uint32_t*)&cmd_message, 0);
    }
}

/* Convert received message to automation event */
static CST_autom_event_t CST_get_autom_event(osEvent event)
{
    CST_autom_event_t autom_event;
    CST_message_t message;
    dc_remote_ppp_server_rt_info_t remote_ppp_server_rt_info;
    dc_ppp_client_rt_info_t ppp_client_rt_info;

    autom_event = CST_NO_EVENT;

    message = *(CST_message_t*)&event.value.v;

    /* 4 types of possible messages: */
    /*
         -> CS automaton event
         -> CS CMD    (ON/OFF)
         -> DC EVENT  (DC_COM_REMOTE_PPP_SERVER_INFO: PPP CONFIG OK/PPP FAIL)
         -> DC TIMER  (Polling handle)
    */
    if(message.type == CST_MESSAGE_TIMER_EVENT)
    {
      autom_event = CST_POLLING_TIMER;
    }
    else if(message.type == CST_MESSAGE_CS_EVENT)
    {
      autom_event = (CST_autom_event_t)message.id;
    }
    else if(message.type == CST_MESSAGE_URC_EVENT)
    {
      autom_event = CST_MODEM_URC;
    }
    else if(message.type == CST_MESSAGE_CMD)
    {
      switch(message.id)
      {
        case CST_INIT_EVENT:
        {
          autom_event = (CST_autom_event_t)CST_INIT_EVENT;
          break;
        }
        case CST_MODEM_POWER_ON_EVENT:
        {
          autom_event = (CST_autom_event_t)CST_MODEM_POWER_ON_EVENT;
          break;
        }
        default:
        {
          autom_event = CST_CMD_UNKWONW_EVENT;
          break;
        }
      }
    }
    else if(message.type == CST_MESSAGE_DC_EVENT)
    {
      if(message.id == DC_COM_REMOTE_PPP_SERVER_INFO)
      {
        dc_com_read(&dc_com_db, DC_COM_REMOTE_PPP_SERVER_INFO, (void*)&remote_ppp_server_rt_info,sizeof(remote_ppp_server_rt_info));
        if(DC_SERVICE_FAIL == remote_ppp_server_rt_info.rt_state)
        {
          autom_event = CST_MODEM_PPP_FAIL_EVENT;
        }
      }
      if(message.id == DC_COM_PPP_CLIENT_INFO)
      {
        dc_com_read(&dc_com_db, DC_COM_PPP_CLIENT_INFO, (void*)&ppp_client_rt_info,sizeof(ppp_client_rt_info));
        if(DC_SERVICE_FAIL == ppp_client_rt_info.rt_state)
        {
          autom_event = CST_MODEM_PPP_FAIL_EVENT;
        }
        else if(DC_SERVICE_ON == ppp_client_rt_info.rt_state)
        {
          autom_event = CST_MODEM_PPP_CONFIG_OK_EVENT;
        }
        else
        {
          /* Nothing to do */
        }
      }
    }
    else
    {
      PrintCellularService("CST_get_autom_event : No type matching\n\r");
    }

    return autom_event;
}


/* ===================================================================
   Automaton functions  Begin
   =================================================================== */

/* power on modem processing */
static void CST_power_on_only_modem_mngt(void)
{
    PrintCellularService("*********** CST_power_on_only_modem_mngt ********\n\r");
    osCDS_power_on();
    PrintCellularService("*********** MODEM ON ********\n\r");
}

static void CST_power_on_modem_mngt(void)
{
    CS_Status_t cs_status;
    dc_radio_lte_rt_info_t radio_lte_rt_info;

    PrintCellularService("*********** CST_power_on_modem_mngt ********\n\r");
    cs_status = osCDS_power_on();

#if (CST_RESET_DEBUG == 1)
    if(cst_context.power_on_reset_count == 0) cs_status=CELLULAR_ERROR;
#endif

    if(cs_status != CELLULAR_OK)
    {
      CST_config_fail_mngt(  "CST_power_on_modem_mngt",
                        CST_MODEM_POWER_ON_FAIL,
                        &cst_context.power_on_reset_count,
                        CST_POWER_ON_RESET_MAX );

    }
    else
    {
       /* Data Cache -> Radio ON */
       dc_com_read(&dc_com_db, DC_COM_RADIO_LTE_INFO, (void*)&radio_lte_rt_info,sizeof(radio_lte_rt_info));
       radio_lte_rt_info.rt_state = DC_SERVICE_ON;
       dc_com_write(&dc_com_db, DC_COM_RADIO_LTE_INFO, (void*)&radio_lte_rt_info,sizeof(radio_lte_rt_info));
       CST_get_device_all_infos();
       CST_send_message(CST_MESSAGE_CS_EVENT, CST_MODEM_POWERED_ON_EVENT);
    }
}

static CS_Status_t CST_reset_modem(void)
{
  return osCDS_reset(CS_RESET_AUTO);
}

/* power on modem processing */
static void CST_reset_modem_mngt(void)
{
    CS_Status_t cs_status;
    dc_radio_lte_rt_info_t radio_lte_rt_info;

    PrintCellularService("*********** CST_power_on_modem_mngt ********\n\r");
    cs_status = CST_reset_modem();
#if (CST_RESET_DEBUG == 1)
    if(cst_context.reset_reset_count == 0) cs_status=CELLULAR_ERROR;
#endif

    if(cs_status != CELLULAR_OK)
    {
      CST_config_fail_mngt(  "CST_reset_modem_mngt",
                        CST_MODEM_RESET_FAIL,
                        &cst_context.reset_reset_count,
                        CST_RESET_MAX );

    }
    else
    {
       /* Data Cache -> Radio ON */
       dc_com_read(&dc_com_db, DC_COM_RADIO_LTE_INFO, (void*)&radio_lte_rt_info,sizeof(radio_lte_rt_info));
       radio_lte_rt_info.rt_state = DC_SERVICE_ON;
       dc_com_write(&dc_com_db, DC_COM_RADIO_LTE_INFO, (void*)&radio_lte_rt_info,sizeof(radio_lte_rt_info));
       CST_get_device_all_infos();
       CST_send_message(CST_MESSAGE_CS_EVENT, CST_MODEM_POWERED_ON_EVENT);
    }
}

/* init modem management */
static void  CST_init_modem_mngt(void)
{
    CS_Status_t cs_status;

    PrintCellularService ("*********** CST_init_modem_mngt ********\n\r");

    cs_status = osCDS_init_modem(CS_CMI_FULL, CELLULAR_FALSE, GOOD_PINCODE);
#if (CST_RESET_DEBUG == 1)
    if(cst_context.init_modem_reset_count == 0) cs_status=CELLULAR_ERROR;
#endif
    if(cs_status != CELLULAR_OK)
    {
       CST_config_fail_mngt(  "CST_init_modem_mngt",
                         CST_MODEM_REGISTER_FAIL,
                         &cst_context.init_modem_reset_count,
                         CST_INIT_MODEM_RESET_MAX );
    }
    else
    {
       CST_subscribe_all_net_events();

       /* overwrite operator parameters */
       ctxt_operator.mode   = CS_NRM_AUTO;
       ctxt_operator.format = CS_ONF_NOT_PRESENT;
       /* ctxt_operator.operator_name not set (use memcpy if needed) */

       CST_send_message(CST_MESSAGE_CS_EVENT, CST_MODEM_INITIALIZED_EVENT);
    }
}

/* registration modem management */
static void  CST_net_register_mngt(void)
{
    CS_Status_t cs_status;
    CS_RegistrationStatus_t  cst_ctxt_reg_status;

    PrintCellularService("=== CST_net_register_mngt ===\n\r");
    cs_status = osCDS_register_net(&ctxt_operator, &cst_ctxt_reg_status);
    if(cs_status == CELLULAR_OK)
    {
      cst_context.current_EPS_NetworkRegState  = cst_ctxt_reg_status.EPS_NetworkRegState;
      cst_context.current_GPRS_NetworkRegState = cst_ctxt_reg_status.GPRS_NetworkRegState;
      cst_context.current_CS_NetworkRegState   = cst_ctxt_reg_status.CS_NetworkRegState;

      CST_send_message(CST_MESSAGE_CS_EVENT, CST_SIGNAL_QUALITY_EVENT);
    }
    else
    {
      PrintCellularService("===CST_net_register_mngt - FAIL !!! ===\n\r");
      ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 3, ERROR_WARNING);
    }
}



/* test if modem catch right signal */
static void  CST_signal_quality_test_mngt(void)
{
    PrintCellularService("*********** CST_signal_quality_test_mngt ********\n\r");

    if((cst_context.signal_quality.rssi != 0) && (cst_context.signal_quality.rssi != CST_BAD_SIG_RSSI))
    {
        osTimerStart(CST_waiting_for_network_status_timer_handle, CST_WAITING_FOR_NETWORK_STATUS_DELAY);
        PrintCellularService("-----> Start NW REG TIMEOUT TIMER   : %d\n\r",CST_WAITING_FOR_NETWORK_STATUS_DELAY);    
        
        cst_context.current_state = CST_WAITING_FOR_NETWORK_STATUS_STATE;        
        CST_send_message(CST_MESSAGE_CS_EVENT, CST_NETWORK_STATUS_EVENT);
    }
}

/* test if network status is OK */
static void  CST_network_status_test_mngt(void)
{
    PrintCellularService("*********** CST_network_status_test_mngt ********\n\r");

#if (CST_TEST_REGISTER_FAIL == 1)
    if(CST_register_fail_test_count >= CST_REGISTER_FAIL_TEST_COUNT_MAX)
#endif
    {
        if(    (cst_context.current_EPS_NetworkRegState  == CS_NRS_REGISTERED_HOME_NETWORK)
            || (cst_context.current_EPS_NetworkRegState  == CS_NRS_REGISTERED_ROAMING)
            || (cst_context.current_GPRS_NetworkRegState == CS_NRS_REGISTERED_HOME_NETWORK)
            || (cst_context.current_GPRS_NetworkRegState == CS_NRS_REGISTERED_ROAMING) )
        {
                cst_context.current_state = CST_NETWORK_STATUS_OK_STATE;
                cst_context.register_retry_tempo_count = 0;
                CST_send_message(CST_MESSAGE_CS_EVENT, CST_NETWORK_STATUS_OK_EVENT);
        }
    }
}


static void  CST_network_event_mngt(void)
{
    CS_Status_t cs_status;
    CS_RegistrationStatus_t reg_status;
    PrintCellularService("*********** CST_network_event_mngt ********\n\r");
    cs_status = osCDS_get_net_status(&reg_status);
    if(cs_status == CELLULAR_OK)
    {
         cst_context.current_EPS_NetworkRegState  = reg_status.EPS_NetworkRegState;
         cst_context.current_GPRS_NetworkRegState = reg_status.GPRS_NetworkRegState;
         cst_context.current_CS_NetworkRegState   = reg_status.CS_NetworkRegState;

         if(    (cst_context.current_EPS_NetworkRegState  != CS_NRS_REGISTERED_HOME_NETWORK)
             && (cst_context.current_EPS_NetworkRegState  != CS_NRS_REGISTERED_ROAMING)
             && (cst_context.current_GPRS_NetworkRegState != CS_NRS_REGISTERED_HOME_NETWORK)
             && (cst_context.current_GPRS_NetworkRegState != CS_NRS_REGISTERED_ROAMING) )
         {
             CST_data_cache_set(DC_SERVICE_OFF);
             cst_context.current_state = CST_WAITING_FOR_NETWORK_STATUS_STATE;
             CST_send_message(CST_MESSAGE_CS_EVENT, CST_NETWORK_CALLBACK_EVENT);
         }
    }
    else
    {
      /* modem fail TODO */
    }

}

/* attach modem management */
static void  CST_attach_modem_mngt(void)
{
    CS_Status_t cs_status;
    CS_PSattach_t            cst_ctxt_attach_status;

    PrintCellularService("*********** CST_attach_modem_mngt ********\n\r");

    cs_status = osCDS_get_attach_status(&cst_ctxt_attach_status);
#if (CST_RESET_DEBUG == 1)
    if(cst_context.attach_reset_count == 0) cs_status=CELLULAR_ERROR;
#endif
    if(cs_status != CELLULAR_OK)
    {
       PrintCellularService("*********** CST_attach_modem_mngt fail ********\n\r");
       CST_config_fail_mngt(  "CS_get_attach_status FAIL",
                         CST_MODEM_ATTACH_FAIL,
                         &cst_context.attach_reset_count,
                         CST_ATTACH_RESET_MAX );
       CST_send_message(CST_MESSAGE_CS_EVENT, CST_FAIL_EVENT);
    }
    else
    {
        if(cst_ctxt_attach_status == CS_PS_ATTACHED)
        {
            PrintCellularService("*********** CST_attach_modem_mngt OK ********\n\r");
            cst_context.current_state = CST_MODEM_REGISTERED_STATE;
            CST_send_message(CST_MESSAGE_CS_EVENT, CST_MODEM_ATTACHED_EVENT);
        }

        else
        {
           PrintCellularService("===CST_attach_modem_mngt - NOT ATTACHED !!! ===\n\r");
/* Workaround waiting for Modem behaviour clarification */
           CST_config_fail_mngt(  "CST_pdn_event_mngt",
                        CST_MODEM_ATTACH_FAIL,
                        &cst_context.power_on_reset_count,
                        CST_POWER_ON_RESET_MAX );
#if 0
           cs_status = osCDS_attach_PS_domain();
           if(cs_status != CELLULAR_OK)
           {
              PrintCellularService("===CST_attach_modem_mngt - osCDS_attach_PS_domain FAIL !!! ===\n\r");
              CST_config_fail_mngt(  "CS_get_attach_status FAIL",
                                CST_MODEM_ATTACH_FAIL,
                                &cst_context.attach_reset_count,
                                CST_ATTACH_RESET_MAX );
              CST_send_message(CST_MESSAGE_CS_EVENT, CST_FAIL_EVENT);
           }
           else
           {
              osDelay(100);
              CST_send_message(CST_MESSAGE_CS_EVENT, CST_NETWORK_STATUS_OK_EVENT);
           }
#endif
        }
    }
}

/* pdn definition modem management */
static void CST_modem_define_pdn(void)
{
    CS_PDN_configuration_t pdn_conf;

    CS_Status_t cs_status;
/*
#if (USE_SOCKETS_MODEM == 1)
    cs_status = CELLULAR_OK;
#else
*/
    /* define user PDN configurations */

    /* common user name and password */
    memset((void*)&pdn_conf, 0, sizeof(CS_PDN_configuration_t));
    void *temp = NULL;
    memcpy((void*)pdn_conf.username, temp, 0);
    memcpy((void*)pdn_conf.password, temp, 0);

    /* exemple for CS_PDN_USER_CONFIG_1 with access point name =  "PDN CONFIG 1" */
    cs_status = osCDS_define_pdn(CST_cellular_params.CID, (char*)CST_cellular_params.APN, &pdn_conf);

/* #endif */

#if (CST_RESET_DEBUG == 1)
    if(cst_context.activate_pdn_reset_count == 0) cs_status=CELLULAR_ERROR;
#endif

    if(cs_status != CELLULAR_OK)
    {
        CST_config_fail_mngt(  "CST_modem_define_pdn",
                             CST_MODEM_PDP_DEFINE_FAIL,
                             &cst_context.activate_pdn_reset_count,
                             CST_DEFINE_PDN_RESET_MAX );
    }
/*
    else
    {
        CST_send_message(CST_MESSAGE_CS_EVENT, CST_PDP_ACTIVATED_EVENT);
    }
*/
}

/* pdn activation modem management */
static CS_Status_t CST_modem_activate_pdn(void)
{
    CS_Status_t cs_status;
    cs_status = osCDS_set_default_pdn(CST_cellular_params.CID);
    
    /* register to PDN events for this CID*/
    osCDS_register_pdn_event(CST_cellular_params.CID, CST_pdn_event_callback);
    
    cs_status = osCDS_activate_pdn(CS_PDN_CONFIG_DEFAULT);
#if (CST_TEST_NFMC == 1)
    CST_nfmc_test_count++;
    if(CST_nfmc_test_count < CST_NFMC_TEST_COUNT_MAX)
    {
      cs_status = (CS_Status_t)1;
    }
#endif

#if (CST_RESET_DEBUG == 1)
    if(cst_context.activate_pdn_reset_count == 0) cs_status=CELLULAR_ERROR;
#endif

    if(cs_status != CELLULAR_OK)
    {
       if(CST_nfmc_context.active == 0)
       {
         osTimerStart(CST_pdn_activate_retry_timer_handle, CST_PDN_ACTIVATE_RETRY_DELAY);
         PrintCellularService("-----> CST_modem_activate_pdn NOK - retry tempo  : %d\n\r",CST_PDN_ACTIVATE_RETRY_DELAY);
       }
       else
       {
         osTimerStart(CST_pdn_activate_retry_timer_handle, CST_nfmc_context.tempo[cst_context.activate_pdn_nfmc_tempo_count]);
         PrintCellularService("-----> CST_modem_activate_pdn NOK - retry tempo %d : %d\n\r", cst_context.activate_pdn_nfmc_tempo_count+1, 
                                                                                        CST_nfmc_context.tempo[cst_context.activate_pdn_nfmc_tempo_count]);
       }

       cst_context.activate_pdn_nfmc_tempo_count++;
       if(cst_context.activate_pdn_nfmc_tempo_count >= CST_NFMC_TEMPO_NB)
       {
         cst_context.activate_pdn_nfmc_tempo_count = 0;
       }
    }
    else
    {
       cst_context.activate_pdn_nfmc_tempo_count = 0;
       CST_send_message(CST_MESSAGE_CS_EVENT, CST_PDP_ACTIVATED_EVENT);
    }
    return cs_status;
}

/* start pppos config */
#if (USE_SOCKETS_MODEM == 0)
static void CST_config_ppp_modem(void)
{
    dc_remote_ppp_server_rt_info_t dc_remote_ppp_server_rt_info;

    /* Modem activated : start ppp client config */
    dc_com_read(&dc_com_db, DC_COM_REMOTE_PPP_SERVER_INFO, (void*)&dc_remote_ppp_server_rt_info,sizeof(dc_remote_ppp_server_rt_info));
    dc_remote_ppp_server_rt_info.rt_state = DC_SERVICE_ON;
    dc_com_write(&dc_com_db, DC_COM_REMOTE_PPP_SERVER_INFO, (void*)&dc_remote_ppp_server_rt_info,sizeof(dc_remote_ppp_server_rt_info));

}
#endif
/*  pppos config fail */

static void CST_config_ppp_fail_mngt(void)
{
    CST_config_fail_mngt(  "CST_config_ppp_fail_mngt",
                         CST_MODEM_PPP_CONFIG_FAIL_FAIL,
                         &cst_context.ppp_config_retry_count,
                         CST_PPP_CONFIG_RETRY_MAX );
}

static void CST_pdn_event_mngt(void)
{
   if(cst_context.pdn_status == CS_PDN_EVENT_NW_DETACH)
   {
/* Workaround waiting for Modem behaviour clarification */
      CST_config_fail_mngt(  "CST_pdn_event_mngt",
                        CST_MODEM_ATTACH_FAIL,
                        &cst_context.power_on_reset_count,
                        CST_POWER_ON_RESET_MAX );
#if 0
      cst_context.current_state = CST_WAITING_FOR_NETWORK_STATUS_STATE;
      CST_send_message(CST_MESSAGE_CS_EVENT, CST_NETWORK_CALLBACK_EVENT);
#endif
   }
   else if(
             (cst_context.pdn_status == CS_PDN_EVENT_NW_DEACT)
          || (cst_context.pdn_status == CS_PDN_EVENT_NW_PDN_DEACT))
   {
      PrintCellularService("=========CST_pdn_event_mngt CS_PDN_EVENT_NW_DEACT\n\r");
      cst_context.current_state = CST_MODEM_REGISTERED_STATE;
      CST_send_message(CST_MESSAGE_CS_EVENT, CST_MODEM_ATTACHED_EVENT);
   }
   else
   {
      cst_context.current_state = CST_WAITING_FOR_NETWORK_STATUS_STATE;
      CST_send_message(CST_MESSAGE_CS_EVENT, CST_NETWORK_CALLBACK_EVENT);
   }
}

/* =================================================================
   Management automaton functions according current automation state
   ================================================================= */


static void CST_init_state(CST_autom_event_t autom_event)
{
    switch(autom_event)
    {
        case CST_INIT_EVENT:
        {
           cst_context.current_state = CST_MODEM_ON_STATE;
           CST_power_on_modem_mngt();
           break;
        }
        case CST_MODEM_POWER_ON_EVENT:
        {
           cst_context.current_state = CST_MODEM_ON_ONLY_STATE;
           CST_power_on_only_modem_mngt();
           break;
        }
        default:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 7, ERROR_WARNING);
           break;
        }
    }
}

static void CST_reset_state(CST_autom_event_t autom_event)
{
    switch(autom_event)
    {
        case CST_INIT_EVENT:
        {
           cst_context.current_state = CST_MODEM_ON_STATE;
           CST_reset_modem_mngt();
           break;
        }
        default:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 7, ERROR_WARNING);
           break;
        }
    }
}

static void CST_modem_on_state(CST_autom_event_t autom_event)
{
    switch(autom_event)
    {
        case CST_MODEM_POWERED_ON_EVENT:
        {          
          cst_context.current_state = CST_MODEM_POWERED_ON_STATE;
          CST_init_modem_mngt();
          break;
        }
        default:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 8, ERROR_WARNING);
           break;
        }
    }
}



static void CST_modem_powered_on_state(CST_autom_event_t autom_event)
{
    switch(autom_event)
    {
        case CST_MODEM_INITIALIZED_EVENT:
        {
           if (cst_context.set_pdn_mode)
	   {
              PrintCellularService("CST_modem_on_state : CST_modem_define_pdn\n\r");      
              CST_modem_define_pdn();
           }
           cst_context.current_state = CST_WAITING_FOR_SIGNAL_QUALITY_OK_STATE;
           CST_net_register_mngt();
           break;
        }
        default:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 9, ERROR_WARNING);
           break;
        }
    }
}

static void CST_waiting_for_signal_quality_ok_state(CST_autom_event_t autom_event)
{

    switch(autom_event)
    {
        case CST_NETWORK_CALLBACK_EVENT:
        case CST_SIGNAL_QUALITY_EVENT:
        {
           CST_signal_quality_test_mngt();
           break;
        }
        default:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 9, ERROR_WARNING);
           break;
        }
    }
}

static void CST_waiting_for_network_status_state(CST_autom_event_t autom_event)
{
    switch(autom_event)
    {
        case CST_NETWORK_CALLBACK_EVENT:
        case CST_NETWORK_STATUS_EVENT:
        {
           CST_network_status_test_mngt();
           break;
        }
        case CST_NW_REG_TIMEOUT_TIMER_EVENT:
        {       
           PrintCellularService("-----> NW REG TIMEOUT TIMER EXPIRY WE PWDN THE MODEM \n\r");  
           cst_context.current_state = CST_MODEM_NETWORK_STATUS_FAIL_STATE;
           CS_power_off();
           
           osTimerStart(CST_register_retry_timer_handle, CST_nfmc_context.tempo[cst_context.register_retry_tempo_count]);
           PrintCellularService("-----> CST_waiting_for_network_status NOK - retry tempo %d : %d\n\r", cst_context.register_retry_tempo_count+1, 
                                                                                               CST_nfmc_context.tempo[cst_context.register_retry_tempo_count]);
#if (CST_TEST_REGISTER_FAIL == 1)
           CST_register_fail_test_count++;
#endif
           cst_context.register_retry_tempo_count++;
           if(cst_context.register_retry_tempo_count >= CST_NFMC_TEMPO_NB)
           {
             cst_context.register_retry_tempo_count = 0;
           }
           break;
        }      
        default:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 9, ERROR_WARNING);
           break;
        }
    }
}

static void CST_network_status_ok_state(CST_autom_event_t autom_event)
{
    switch(autom_event)
    {
        case CST_NETWORK_STATUS_OK_EVENT:
        {
           CST_attach_modem_mngt();
           break;
        }
        default:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 9, ERROR_WARNING);
           break;
        }
    }
}


static void CST_modem_registered_state(CST_autom_event_t autom_event)
{
    switch(autom_event)
    {
        case CST_MODEM_ATTACHED_EVENT:
        {
           cst_context.current_state = CST_MODEM_PDN_ACTIVATE_STATE;
           CST_modem_activate_pdn();
           /* CS_Status_t CS_get_dev_IP_address(CS_PDN_conf_id_t cid, CS_IPaddrType_t *ip_addr_type, char *p_ip_addr_value) */
           break;
        }
        case CST_NETWORK_CALLBACK_EVENT:
        {
            CST_network_event_mngt();
            break;
        }
        default:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 10, ERROR_WARNING);
           break;
        }
    }
}
static void CST_modem_pdn_activate_state(CST_autom_event_t autom_event)
{
    switch(autom_event)
    {
        case CST_PDP_ACTIVATED_EVENT:
        {
#if (USE_SOCKETS_MODEM == 1)
           cst_context.current_state = CST_MODEM_DATA_READY_STATE;
           CST_reset_fail_count();
           CST_data_cache_set(DC_SERVICE_ON);
#else
           cst_context.current_state = CST_MODEM_PPP_CONFIG_ON_GOING_STATE;
           CST_config_ppp_modem();
#endif
           break;
        }
        case CST_MODEM_PDN_ACTIVATE_RETRY_TIMER_EVENT:
        {
           CST_modem_activate_pdn();
           break;
        }
        case CST_NETWORK_CALLBACK_EVENT:
        {
           CST_network_event_mngt();
           break;
        }
        case CST_PDN_STATUS_EVENT:
        {
           CST_pdn_event_mngt();
           break;
        }
        default:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 10, ERROR_WARNING);
           break;
        }
    }
}

static void CST_ppp_config_on_going_state(CST_autom_event_t autom_event)
{
    switch(autom_event)
    {
        case CST_MODEM_PPP_CONFIG_OK_EVENT:
        {
            PrintCellularService("-----> CST_MODEM_DATA_READY_STATE\n\r");
            cst_context.current_state = CST_MODEM_DATA_READY_STATE;
            CST_reset_fail_count();
            break;
        }
        case CST_MODEM_PPP_FAIL_EVENT:
        {
           CST_config_ppp_fail_mngt();
           break;
        }
        default:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 12, ERROR_WARNING);
           break;
        }
    }
}

static void CST_data_ready_state(CST_autom_event_t autom_event)
{
    switch(autom_event)
    {
        case CST_NETWORK_CALLBACK_EVENT:
        {
           CST_network_event_mngt();
           break;
        }
        case CST_MODEM_PPP_FAIL_EVENT:
        {
           CST_config_ppp_fail_mngt();
           break;
        }
        case CST_PDN_STATUS_EVENT:
        {
           CST_pdn_event_mngt();
           break;
        }

        default:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 13, ERROR_WARNING);
           break;
        }
    }
}

static void CST_fail_state(CST_autom_event_t autom_event)
{
    switch(autom_event)
    {
        case CST_FAIL_EVENT:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 25, ERROR_WARNING);
           break;
        }
        default:
        {
           ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 26, ERROR_WARNING);
           break;
        }
    }
}

/* ===================================================================
   Automaton functions  END
   =================================================================== */


/* Timer handler
     During configuration :  Signal quality  and network status Polling
     After configuration  :  Modem monitoring (Signal quality checking)
 */
static void CST_timer_handler(void)
{
    CS_Status_t cs_status ;
    CS_RegistrationStatus_t reg_status;
/*    PrintCellularService("-----> CST_timer_handler  <-----\n\r"); */
    CST_polling_timer_flag = 0;
    if(cst_context.current_state == CST_WAITING_FOR_NETWORK_STATUS_STATE)
    {
        cs_status = osCDS_get_net_status(&reg_status);
        if(cs_status == CELLULAR_OK)
        {
/*
          if(    (reg_status.EPS_NetworkRegState  != cst_context.current_EPS_NetworkRegState)
              || (reg_status.GPRS_NetworkRegState != cst_context.current_GPRS_NetworkRegState)
              || (reg_status.CS_NetworkRegState   != cst_context.current_CS_NetworkRegState) )
*/               {
                    cst_context.current_EPS_NetworkRegState  = reg_status.EPS_NetworkRegState;
                    cst_context.current_GPRS_NetworkRegState = reg_status.GPRS_NetworkRegState;
                    cst_context.current_CS_NetworkRegState   = reg_status.CS_NetworkRegState;
                    PrintCellularService("-----> CST_timer_handler - CST_NETWORK_STATUS_EVENT <-----\n\r");

                    CST_send_message(CST_MESSAGE_CS_EVENT, CST_NETWORK_STATUS_EVENT);
                }
        }
        else
        {
            CST_config_fail_mngt(  "osCDS_get_net_status",
                        CST_MODEM_GNS_FAIL,
                        &cst_context.gns_reset_count,
                        CST_GNS_MODEM_RESET_MAX );
        }
    }

    if(cst_context.current_state == CST_WAITING_FOR_SIGNAL_QUALITY_OK_STATE)
    {
       CST_set_signal_quality();
       CST_send_message(CST_MESSAGE_CS_EVENT, CST_SIGNAL_QUALITY_EVENT);
    }

    if((cst_context.current_state == CST_MODEM_DATA_READY_STATE) && (CST_MODEM_POLLING_PERIOD != 0))
    {
#if (USE_SOCKETS_MODEM == 0)
       osCDS_suspend_data();
#endif
       CST_set_signal_quality();
#if (USE_SOCKETS_MODEM == 0)
       /* CS_check_connection(); */
       osCDS_resume_data();
#endif
    }
}


/* Cellular Service Task : autmaton management */
static void CST_cellular_service_task(void const * argument)
{
  osEvent event;
  CST_autom_event_t autom_event;

  /* Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);


  for(;;)
  {
    event = osMessageGet(cst_queue_id, osWaitForever);
    autom_event = CST_get_autom_event(event);
    
    if(autom_event == CST_POLLING_TIMER)
    {
      CST_timer_handler();
    }
    else if (autom_event != CST_NO_EVENT)
    {
      switch(cst_context.current_state)
      {
        case CST_MODEM_INIT_STATE:
        {
           PrintCellularService("-----> State : CST_MODEM_INIT_STATE <-----\n\r");
           CST_init_state(autom_event);
           break;
        }
        case CST_MODEM_RESET_STATE:
        {
           PrintCellularService("-----> State : CST_MODEM_RESET_STATE <-----\n\r");
           CST_reset_state(autom_event);
           break;
        }
        case CST_MODEM_ON_STATE:
        {
           PrintCellularService("-----> State : CST_MODEM_ON_STATE <-----\n\r");
           CST_modem_on_state(autom_event);
           break;
        }
        case CST_MODEM_POWERED_ON_STATE:
        {
           PrintCellularService("-----> State : CST_MODEM_POWERED_ON_STATE <-----\n\r");
           CST_modem_powered_on_state(autom_event);
           break;
        }
        case CST_WAITING_FOR_SIGNAL_QUALITY_OK_STATE:
        {
           PrintCellularService("-----> State : CST_WAITING_FOR_SIGNAL_QUALITY_OK_STATE <-----\n\r");
           CST_waiting_for_signal_quality_ok_state(autom_event);
           break;
        }
        case CST_WAITING_FOR_NETWORK_STATUS_STATE:
        {
           PrintCellularService("-----> State : CST_WAITING_FOR_NETWORK_STATUS_STATE <-----\n\r");
           CST_waiting_for_network_status_state(autom_event);
           break;
        }
      
        case CST_NETWORK_STATUS_OK_STATE:
        {
           PrintCellularService("-----> State : CST_NETWORK_STATUS_OK_STATE <-----\n\r");
           CST_network_status_ok_state(autom_event);
           break;
        }
      
        case CST_MODEM_REGISTERED_STATE:
        {
           PrintCellularService("-----> State : CST_MODEM_REGISTERED_STATE <-----\n\r");
           CST_modem_registered_state(autom_event);
           break;
        }
        case CST_MODEM_PDN_ACTIVATE_STATE:
        {
           PrintCellularService("-----> State : CST_MODEM_PDN_ACTIVATE_STATE <-----\n\r");
           CST_modem_pdn_activate_state(autom_event);
           break;
        }
      
        case CST_MODEM_PPP_CONFIG_ON_GOING_STATE:
        {
           PrintCellularService("-----> State : CST_MODEM_PPP_CONFIG_ON_GOING_STATE <-----\n\r");
           CST_ppp_config_on_going_state(autom_event);
           break;
        }
        case CST_MODEM_DATA_READY_STATE:
        {
           PrintCellularService("-----> State : CST_MODEM_DATA_READY_STATE <-----\n\r");
           CST_data_ready_state(autom_event);
           break;
        }
        case CST_MODEM_FAIL_STATE:
        {
           PrintCellularService("-----> State : CST_MODEM_FAIL_STATE <-----\n\r");
           CST_fail_state(autom_event);
           break;
        }
        default:
        {
           PrintCellularService("-----> State : Not State <-----\n\r");
           break;
        }
      }
    }
    else
    {
      PrintCellularService("CST_cellular_service_task : autom_event Unkwown\n\r");      
    }
  }
}


/* CST_polling_timer_callback function */
static void CST_polling_timer_callback(void const * argument)
{
  if(cst_context.current_state!=CST_MODEM_INIT_STATE)
  {
    CST_message_t cmd_message;
    cmd_message.type = CST_MESSAGE_TIMER_EVENT;
    cmd_message.id   = 0;
    if(CST_polling_timer_flag == 0)
    {
      CST_polling_timer_flag = 1;
      osMessagePut(cst_queue_id, *(uint32_t*)&cmd_message, 0);
    }
  }
}

/* CST_pdn_activate_retry_timer_callback */
static void CST_pdn_activate_retry_timer_callback(void const * argument)
{
  if(cst_context.current_state==CST_MODEM_PDN_ACTIVATE_STATE)
  {
    CST_send_message(CST_MESSAGE_CS_EVENT, CST_MODEM_PDN_ACTIVATE_RETRY_TIMER_EVENT);
  }
}

/* CST_pdn_activate_retry_timer_callback */
static void CST_register_retry_timer_callback(void const * argument)
{
  if(cst_context.current_state == CST_MODEM_NETWORK_STATUS_FAIL_STATE)
  {
    cst_context.current_state = CST_MODEM_INIT_STATE;
    CST_send_message(CST_MESSAGE_CS_EVENT, CST_INIT_EVENT);
  }
}

static void CST_waiting_for_network_status_timer_callback(void const * argument)
{
  if(cst_context.current_state==CST_WAITING_FOR_NETWORK_STATUS_STATE)
  {
    CST_send_message(CST_MESSAGE_CS_EVENT, CST_NW_REG_TIMEOUT_TIMER_EVENT);
  }
}

#if 0
static void CST_waiting_for_network_status_timer_callback(void const * argument)
{
  if(cst_context.current_state == CST_WAITING_FOR_NETWORK_STATUS_STATE)
  {
    cst_context.current_state = CST_MODEM_NETWORK_STATUS_FAIL_STATE;
    CS_power_off();

    osTimerStart(CST_register_retry_timer_handle, CST_nfmc_context.tempo[cst_context.register_retry_tempo_count]);
    PrintCellularService("-----> CST_waiting_for_network_status NOK - retry tempo %d : %d\n\r", cst_context.register_retry_tempo_count+1, 
                                                                                        CST_nfmc_context.tempo[cst_context.register_retry_tempo_count]);
    CST_register_fail_test_count++;
    cst_context.register_retry_tempo_count++;
    if(cst_context.register_retry_tempo_count >= CST_NFMC_TEMPO_NB)
    {
      cst_context.register_retry_tempo_count = 0;
    }
  }
}
#endif

/* Exported functions ----------------------------------------------------------*/
CS_Status_t  CST_radio_on(void)
{
    return CST_send_message(CST_MESSAGE_CMD, CST_INIT_EVENT);
}

CS_Status_t  CST_modem_power_on(void)
{
    return CST_send_message(CST_MESSAGE_CMD, CST_MODEM_POWER_ON_EVENT);
}

CS_Status_t CST_cellular_service_init(void)
{
    cst_context.current_state = CST_MODEM_INIT_STATE;
    CST_modem_init();

    osCDS_cellular_service_init();
    CST_csq_count_fail = 0;
    // AVK here we must take the modem set up parameters
    CST_setup_handler();

   // app_select_record(CST_MODEM_POWER_ON_LABEL, APP_SELECT_CST_MODEM_START);
   // setup_record(SETUP_APPLI_CST, CST_VERSION_APPLI, CST_LABEL, CST_setup_handler, CST_setup_dump, CST_default_setup_table, CST_DEFAULT_PARAMA_NB);


    osMessageQDef(cst_queue_id, 10, sizeof(CST_message_t));
    cst_queue_id = osMessageCreate(osMessageQ(cst_queue_id), NULL);
    if(cst_queue_id == NULL)
    {
      ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 18, ERROR_FATAL);
    }
    return CELLULAR_OK;
}


CST_state_t CST_get_state(void)
{
   return cst_context.current_state;
}


CS_Status_t CST_cellular_service_start(void)
{
    dc_nfmc_rt_info_t nfmc_info;
    dc_cellular_rt_info_t  cellular_info;
    uint32_t cst_polling_period = 0;

    CST_select_sim(CST_cellular_params.SIM_SLOT);

    dc_com_register_gen_event_cb(&dc_com_db, CST_notif_cb, (void*)NULL);
    cellular_info.mno_name[0]           = '0';
    cellular_info.rt_state              = DC_SERVICE_UNAVAIL;

      
    dc_com_write(&dc_com_db, DC_COM_CELLULAR, (void *)&cellular_info, sizeof(cellular_info));

    nfmc_info.rt_state = DC_SERVICE_UNAVAIL;
    dc_com_write(&dc_com_db, DC_COM_NFMC_TEMPO, (void *)&nfmc_info, sizeof(nfmc_info));

    osTimerDef(cs_polling_timer, CST_polling_timer_callback);
    cst_polling_timer_handle = osTimerCreate(osTimer(cs_polling_timer), osTimerPeriodic, NULL);
    if(CST_MODEM_POLLING_PERIOD == 0)
    {
        cst_polling_period = CST_MODEM_POLLING_PERIOD_DEFAULT;
    }
    else
    {
        cst_polling_period = CST_MODEM_POLLING_PERIOD;
    }

    osTimerStart(cst_polling_timer_handle, cst_polling_period);

    osTimerDef(CST_pdn_activate_retry_timer, CST_pdn_activate_retry_timer_callback);
    CST_pdn_activate_retry_timer_handle = osTimerCreate(osTimer(CST_pdn_activate_retry_timer), osTimerOnce, NULL);

    osTimerDef(CST_waiting_for_network_status_timer, CST_waiting_for_network_status_timer_callback);
    CST_waiting_for_network_status_timer_handle = osTimerCreate(osTimer(CST_waiting_for_network_status_timer), osTimerOnce, NULL);

    osTimerDef(CST_register_retry_timer, CST_register_retry_timer_callback);
    CST_register_retry_timer_handle = osTimerCreate(osTimer(CST_register_retry_timer), osTimerOnce, NULL);
      
    osThreadDef(CST, CST_cellular_service_task, CELLULAR_SERVICE_THREAD_PRIO, 0, CELLULAR_SERVICE_THREAD_STACK_SIZE);
    CST_cellularServiceThreadId = osThreadCreate(osThread(CST), NULL);

    if(CST_cellularServiceThreadId == NULL)
    {
      ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 19, ERROR_FATAL);
    }
  else
  {
#if (STACK_ANALYSIS_TRACE == 1)
            stackAnalysis_addThreadStackSizeByHandle(CST_cellularServiceThreadId, CELLULAR_SERVICE_THREAD_STACK_SIZE);
#endif /* STACK_ANALYSIS_TRACE */
  }

    return CELLULAR_OK;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
