/**
  ******************************************************************************
  * @file    Application\at_service\custom\modem_bg96\src\at_custom_modem_specific.c
  * @author  MCD Application Team
  * @brief   This file provides all the specific code to the
  *          BG96 Quectel modem: LTE-cat-M1 or LTE-cat.NB1(=NB-IOT) or GSM
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "at_custom_common.h"
#include "at_custom_modem.h"
#include "at_custom_modem_specific.h"
#include "at_datapack.h"
#include "at_util.h"
#include "plf_config.h"
#include "plf_modem_config.h"
#include "error_handler.h"

/*
AT+<X>=?     : TEST COMMAND
AT+<X>?      : READ COMMAND
AT+<X>=...   : WRITE COMMAND
AT+<X>       : EXECUTION COMMAND
*/
#if defined(USE_MODEM_BG96)
#if (defined(HWREF_AKORIOT_BG96) || defined(HWREF_B_CELL_BG96_V2))
#else
#error Hardware reference not specified
#endif
#endif /* USE_MODEM_BG96 */

/* BG96 COMPILATION FLAGS to define in project option if needed:
*
*  - CHECK_SIM_PIN : if defined, check if PIN code is required in SID_CS_INIT_MODEM
*
*
*/

#if (USE_TRACE_INTERFACE_ATCUSTOM_SPECIFIC == 1)
#include "trace_interface.h"
#define PrintINFO(format, args...) TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P0, "BG96:" format "\r\n", ## args)
#define PrintDBG(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P1, "BG96:" format "\r\n", ## args)
#define PrintAPI(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P2, "BG96 API:" format "\r\n", ## args)
#define PrintErr(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_ERR, "BG96 ERROR:" format "\r\n", ## args)
#define PrintBuf(pbuf, size)       TracePrintBufChar(DBG_CHAN_ATCMD, DBL_LVL_P1, (char *)pbuf, size);
#elif (USE_PRINTF == 1)
#define PrintINFO(format, args...)  printf("BG96:" format "\r\n", ## args)
#define PrintDBG(format, args...)   printf("BG96:" format "\r\n", ## args)
#define PrintAPI(format, args...)   printf("BG96 API:" format "\r\n", ## args)
#define PrintErr(format, args...)   printf("BG96 ERROR:" format "\r\n", ## args)
#define PrintBuf(format, args...)   do {} while(0)
#elif (USE_CONFIGPRINTF_ATCUSTOM_SPECIFIC == 1)
#define PrintINFO(format, args...)  vLoggingPrintf("BG96:" format "\r\n", ## args)
#define PrintDBG(format, args...)   vLoggingPrintf("BG96:" format "\r\n", ## args)
#define PrintAPI(format, args...)   vLoggingPrintf("BG96 API:" format "\r\n", ## args)
#define PrintErr(format, args...)   vLoggingPrintf("BG96 ERROR:" format "\r\n", ## args)
#define PrintBuf(format, args...)   do {} while(0) /* Cannot print a buffer using vLoggingPrintf */
#define IGNORE(x) do {} while(0)
#else
#define PrintINFO(format, args...)  do {} while(0)
#define PrintDBG(format, args...)   do {} while(0)
#define PrintAPI(format, args...)   do {} while(0)
#define PrintErr(format, args...)   do {} while(0)
#define PrintBuf(format, args...)   do {} while(0)
#define IGNORE(x) ((void)(x))
#endif

/* ###########################  START CUSTOMIZATION PART  ########################### */
/* FLAGS:
*
*/

/* Pre-defined Bands configurations:
   To force a band configuration, define one of the following param. If none of them is define, the bands
   configuration will be checked but not modified.
   Forcing a band configuration will set it as permanent (saved in NV memory)

   BG96_BAND_CONFIG_CAT_M1_ONLY_FULL_BANDS      - Cat M1 only, all bands
   BG96_BAND_CONFIG_CAT_NB1_ONLY_FULL_BANDS     - Cat NB1 only, all bands
   BG96_BAND_CONFIG_GSM_ONLY_FULL_BANDS         - GSM only, all bands
   BG96_BAND_CONFIG_CUSTOM                      - Custom config
   BG96_BAND_CONFIG_DEFAULT                     - Default config (the config originally flashed in the modems)

List of bands parameters (cf .h file to see the list of enum values for each parameter)
  - BG96_BAND_GSM    : hexadecimal value that specifies the GSM frequency band (cf AT+QCFG="band")
  - BG96_BAND_CAT_M1 : hexadecimal value that specifies the LTE Cat.M1 frequency band (cf AT+QCFG="band")
  - BG96_BAND_CAT_NB1: hexadecimal value that specifies the LTE Cat.NB1 frequency band (cf AT+QCFG="band")
  - BG96_IOTOPMODE   : network category to be searched under LTE network (cf AT+QCFG="iotopmode")
  - BG96_SCANSEQ     : network search sequence (GSM, Cat.M1, Cat.NB1) (cf AT+QCFG="nwscanseq")
  - BG96_SCANMODE    : network to be searched (cf AT+QCFG="nwscanmode")
*/
#if (BG96_BAND_CONFIG_CAT_M1_ONLY_FULL_BANDS == 1)
/* ----- Cat M1 only, all bands ------ */
#define BG96_BAND_GSM     ((uint32_t)_QCFGbandGSM_any)
#define BG96_BAND_CAT_M1  ((uint64_t)_QCFGbandCatM1_any)
#define BG96_BAND_CAT_NB1 ((uint64_t)_QCFGbandCatNB1_any)
#define BG96_IOTOPMODE    ((uint32_t)_QCFGiotopmodeCatM1)
#define BG96_SCANSEQ      ((uint32_t)_QCFGscanseq_M1_NB1_GSM)
#define BG96_SCANMODE     ((uint32_t)_QCFGscanmodeLTEOnly)

#elif (BG96_BAND_CONFIG_CAT_NB1_ONLY_FULL_BANDS == 1)
/* ----- Cat NB1 only, all bands ------ */
#define BG96_BAND_GSM     ((uint32_t)_QCFGbandGSM_any)
#define BG96_BAND_CAT_M1  ((uint64_t)_QCFGbandCatM1_any)
#define BG96_BAND_CAT_NB1 ((uint64_t)_QCFGbandCatNB1_any)
#define BG96_IOTOPMODE    ((uint32_t)_QCFGiotopmodeCatNB1)
#define BG96_SCANSEQ      ((uint32_t)_QCFGscanseq_NB1_M1_GSM)
#define BG96_SCANMODE     ((uint32_t)_QCFGscanmodeLTEOnly)

#elif (BG96_BAND_CONFIG_GSM_ONLY_FULL_BANDS == 1)
/* ----- GSM only, all bands ------ */
#define BG96_BAND_GSM     ((uint32_t)_QCFGbandGSM_any)
#define BG96_BAND_CAT_M1  ((uint64_t)_QCFGbandCatM1_any)
#define BG96_BAND_CAT_NB1 ((uint64_t)_QCFGbandCatNB1_any)
#define BG96_IOTOPMODE    ((uint32_t)_QCFGiotopmodeCatM1)
#define BG96_SCANSEQ      ((uint32_t)_QCFGscanseq_GSM_M1_NB1)
#define BG96_SCANMODE     ((uint32_t)_QCFGscanmodeGSMOnly)

#elif (BG96_BAND_CONFIG_CAT_M1_GSM_FALLBACK_FULL_BANDS == 1)
/* ----- Cat M1 and GSM fallback, all bands ------ */
#define BG96_BAND_GSM     ((uint32_t)_QCFGbandGSM_any)
#define BG96_BAND_CAT_M1  ((uint64_t)_QCFGbandCatM1_any)
#define BG96_BAND_CAT_NB1 ((uint64_t)_QCFGbandCatNB1_any)
#define BG96_IOTOPMODE    ((uint32_t)_QCFGiotopmodeCatM1)
#define BG96_SCANSEQ      ((uint32_t)_QCFGscanseq_M1_GSM_NB1)
#define BG96_SCANMODE     ((uint32_t)_QCFGscanmodeAuto)

#elif (BG96_BAND_CONFIG_CAT_NB1_GSM_FALLBACK_FULL_BANDS == 1)
/* ----- Cat NB1 and GSM fallback, all bands ------ */
#define BG96_BAND_GSM     ((uint32_t)_QCFGbandGSM_any)
#define BG96_BAND_CAT_M1  ((uint64_t)_QCFGbandCatM1_any)
#define BG96_BAND_CAT_NB1 ((uint64_t)_QCFGbandCatNB1_any)
#define BG96_IOTOPMODE    ((uint32_t)_QCFGiotopmodeCatNB1)
#define BG96_SCANSEQ      ((uint32_t)_QCFGscanseq_NB1_GSM_M1)
#define BG96_SCANMODE     ((uint32_t)_QCFGscanmodeAuto)

#elif (BG96_BAND_CONFIG_DEFAULT == 1)
/* ----- Default config ------ */
#define BG96_BAND_GSM     ((uint32_t)_QCFGbandGSM_any)
#define BG96_BAND_CAT_M1  ((uint64_t)_QCFGbandCatM1_any)
#define BG96_BAND_CAT_NB1 ((uint64_t)_QCFGbandCatNB1_any)
#define BG96_IOTOPMODE    ((uint32_t)_QCFGiotopmodeCatM1CatNB1)
#define BG96_SCANSEQ      ((uint32_t)_QCFGscanseq_M1_NB1_GSM)
#define BG96_SCANMODE     ((uint32_t)_QCFGscanmodeAuto)

#elif (BG96_SET_BANDS == 1)
/* use values defined by user in configuration file */

#else /* BG96_SET_BANDS == 0 */
/* set values to avoid compilation error */
#define BG96_BAND_GSM     ((uint32_t)_QCFGbandGSM_any)
#define BG96_BAND_CAT_M1  ((uint64_t)_QCFGbandCatM1_any)
#define BG96_BAND_CAT_NB1 ((uint64_t)_QCFGbandCatNB1_any)
#define BG96_IOTOPMODE    ((uint32_t)_QCFGiotopmodeCatM1CatNB1)
#define BG96_SCANSEQ      ((uint32_t)_QCFGscanseq_M1_NB1_GSM)
#define BG96_SCANMODE     ((uint32_t)_QCFGscanmodeAuto)
#endif

#define BG96_PDP_DUPLICATECHK_ENABLE ((uint8_t)0) /* parameter of AT+QCFG="PDP/DuplicateChk": 0 to refuse, 1 to allow */

#define BG96_DEFAULT_TIMEOUT  ((uint32_t)15000)
#define BG96_RDY_TIMEOUT      ((uint32_t)30000)
#define BG96_SIMREADY_TIMEOUT ((uint32_t)5000)
#define BG96_ESCAPE_TIMEOUT   ((uint32_t)1000)  /* maximum time allowed to receive a response to an Escape command */
#define BG96_COPS_TIMEOUT     ((uint32_t)180000) /* 180 sec */
#define BG96_CGATT_TIMEOUT    ((uint32_t)140000) /* 140 sec */
#define BG96_CGACT_TIMEOUT    ((uint32_t)150000) /* 150 sec */
#define BG96_ATH_TIMEOUT      ((uint32_t)90000) /* 90 sec */
#define BG96_AT_TIMEOUT       ((uint32_t)3000)  /* timeout for AT */
#define BG96_SOCKET_PROMPT_TIMEOUT ((uint32_t)10000)
#define BG96_QIOPEN_TIMEOUT   ((uint32_t)150000) /* 150 sec */
#define BG96_QICLOSE_TIMEOUT  ((uint32_t)150000) /* 150 sec */
#define BG96_QIACT_TIMEOUT    ((uint32_t)150000) /* 150 sec */
#define BG96_QIDEACT_TIMEOUT  ((uint32_t)40000) /* 40 sec */
#define BG96_QNWINFO_TIMEOUT  ((uint32_t)1000) /* 1000ms */
#define BG96_QIDNSGIP_TIMEOUT ((uint32_t)60000) /* 60 sec */

#if !defined(BG96_OPTION_NETWORK_INFO)
/* set default value */
#define BG96_OPTION_NETWORK_INFO (0)
#endif

#if !defined(BG96_OPTION_ENGINEERING_MODE)
/* set default value */
#define BG96_OPTION_ENGINEERING_MODE (0)
#endif

/* BG96 Modem device context */
atcustom_modem_context_t BG96_ctxt;

/* Commands Look-up table for AT+QCFG */
const char BG96_QCFG_LUT[][32]={
    /* cmd enum - cmd string - cmd timeout (in ms) - build cmd ftion - analyze cmd ftion */
    {"unknown"}, /* _QCFG_gprsattach */
    {"gprsattach"}, /* _QCFG_gprsattach */
    {"nwscanseq"}, /* _QCFG_nwscanseq */
    {"nwscanmode"}, /* _QCFG_nwscanmode */
    {"iotopmode"}, /* _QCFG_iotopmode */
    {"roamservice"}, /* _QCFG_roamservice */
    {"band"}, /* _QCFG_band */
    {"servicedomain"}, /* _QCFG_servicedomain */
    {"sgsn"}, /* _QCFG_sgsn */
    {"msc"}, /* _QCFG_msc */
    {"pdp/duplicatechk"}, /* _QCFG_PDP_DuplicateChk */
    {"urc/ri/ring"}, /* _QCFG_urc_ri_ring */
    {"urc/ri/smsincoming"}, /* _QCFG_urc_ri_smsincoming */
    {"urc/ri/other"}, /* _QCFG_urc_ri_other */
    {"signaltype"}, /* _QCFG_signaltype */
    {"urc/delay"}, /* _QCFG_urc_delay */
};

/* Array to convert AT+QIOPEN service type parameter to a string
*  need to be aligned with ATCustom_BG96_QIOPENservicetype_t
*/
static const char* const bg96_array_QIOPEN_service_type[] = { "TCP", "UDP", "TCP LISTENER", "UDP_LISTENER", }; /* all const */


/* --- VARIABLE SHARD TO SET BG96 SPECIFIC COMMANDS  ------------------------------ */
/* TODO optimize */
static ATCustom_BG96_QCFG_function_t     BG96_QCFG_command_param = _QCFG_unknown;
static at_bool_t                         BG96_QCFG_command_write = AT_FALSE;
static ATCustom_BG96_QINDCFG_function_t  BG96_QINDCFG_command_param = _QINDCFG_unknown;
static ATCustom_BG96_QENGcelltype_t      BG96_QENG_command_param = _QENGcelltype_ServingCell;


/* --- VARIABLES SHARED LOCALLY  ------------------------------ */
/* TODO optimize */
/* memorize current BG96 mode and bands configuration */
static ATCustom_BG96_mode_band_config_t  bg96_mode_and_bands_config;
/* memorize which is current connectID for QIOPEN command received */
static at_bool_t bg96_waiting_qiopen = AT_FALSE;
/* memorize if QICSGP write command is a config or a query command */
static at_bool_t bg96_QICGSP_config_command = AT_TRUE;
static uint8_t bg96_current_qiopen_socket_connected = 0;
#if (USE_SOCKETS_MODEM == 1)
/* check if request PDN to activate is already active (QIACT) */
static at_bool_t bg96_pdn_already_active;
#endif /* USE_SOCKETS_MODEM */

/* SPECIAL CASE START : regroup in a structure ? */
/* TODO optimize */
/* memorize infos received in the URC +QIURC:"dnsgip" */
static at_bool_t bg96_qiurc_dnsgip_finished;
static at_bool_t bg96_qiurc_dnsgip_wait_header;  /* indicate we are waiting for +QIURC: "dnsgip",<err>,<IP_count>,<DNS_ttl> */
static uint32_t  bg96_qiurc_dnsgip_error;
static uint32_t  bg96_qiurc_dnsgip_ip_count;
static uint32_t  bg96_qiurc_dnsgip_ttl;
static char      bg96_qiurc_dnsgip_hostIPaddr[MAX_SIZE_IPADDR] = {0}; /* = host_name parameter from CS_DnsReq_t */
/* SPECIAL CASE END */

/* Socket Data receive: to analyze size received in data header */
static char SocketHeaderDataRx_Buf[4];
static uint8_t SocketHeaderDataRx_Cpt;

/* ---- VARIABLES EXISTING only beacause of AT Parser way of working... ------------------------------ */
/* TODO optimize */
/* memorize which is current QIURC received */
static ATCustom_BG96_QIURC_function_t bg96_current_qiurc_ind = _QIURC_unknown;
/* memorize which is current QCFG command received */
static ATCustom_BG96_QCFG_function_t bg96_current_qcfg_cmd = _QCFG_unknown;
/* memorize which is current QIND command received */
static at_bool_t bg96_current_qind_is_csq = AT_FALSE;
static uint32_t bg96_current_qiopen_connectId = 0;
static at_bool_t bg96_qistate_for_requested_socket = AT_FALSE;


/* ###########################  END CUSTOMIZATION PART  ########################### */

/* Static functions prototypes for parsing ----------------------------------------------*/
/* ###########################  START CUSTOMIZATION PART  ########################### */
static void reinitSyntaxAutomaton_bg96(void);
static void reset_variables_bg96(void);
static void display_decoded_GSM_bands(uint32_t gsm_bands);
static void display_decoded_CatM1_bands(uint64_t CatM1_bands);
static void display_decoded_CatNB1_bands(uint64_t CatNB1_bands);
#if 0
static void display_decoded_iotopmode(uint32_t iotopmode);
static void display_decoded_nwscanmode(uint32_t scanmode);
static void display_decoded_nwscanseq(uint32_t scanseq);
#endif /* 0 */
static void display_user_friendly_mode_and_bands_config(void);
static uint8_t display_if_active_band(uint8_t scanseq, uint8_t rank, uint8_t catM1_on, uint8_t catNB1_on, uint8_t gsm_on);
static void socketHeaderRX_reset(void);
static void SocketHeaderRX_addChar(char *rxchar);
static uint16_t SocketHeaderRX_getSize(void);

/* BG96 build commands overriding common function  or specific */
static at_status_t fCmdBuild_ATD_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_CGSN_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QPOWD_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QCFG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QINDCFG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_CPSMS_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_CEDRXS_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QENG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QICSGP_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QIACT_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QICFG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QIOPEN_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QICLOSE_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QISEND_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QISEND_WRITE_DATA_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QIRD_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QISTATE_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_CGDCONT_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QIDNSCFG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);
static at_status_t fCmdBuild_QIDNSGIP_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt);

/* BG96 specific analyze commands */
static at_action_rsp_t fRspAnalyze_CPIN_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                          IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos);
static at_action_rsp_t fRspAnalyze_CFUN_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                          IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos);
static at_action_rsp_t fRspAnalyze_QIND_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos);
static at_action_rsp_t fRspAnalyze_QIACT_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos);
static at_action_rsp_t fRspAnalyze_QCFG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos);
static at_action_rsp_t fRspAnalyze_QINDCFG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos);
static at_action_rsp_t fRspAnalyze_QIOPEN_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos);
static at_action_rsp_t fRspAnalyze_QIURC_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos);
static at_action_rsp_t fRspAnalyze_QIRD_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos);
static at_action_rsp_t fRspAnalyze_QIRD_data_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos);
static at_action_rsp_t fRspAnalyze_QISTATE_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos);
static at_action_rsp_t fRspAnalyze_Error_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                       IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos);

/* Commands Look-up table */
const atcustom_LUT_t ATCMD_BG96_LUT[]={
    /* cmd enum - cmd string - cmd timeout (in ms) - build cmd ftion - analyze cmd ftion */
    {_AT,             "",             BG96_AT_TIMEOUT,       fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_OK,          "OK",           BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_CONNECT,     "CONNECT",      BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_RING,        "RING",         BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_NO_CARRIER,  "NO CARRIER",   BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_ERROR,       "ERROR",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_Error_BG96},
    {_AT_NO_DIALTONE, "NO DIALTONE",  BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_BUSY,        "BUSY",         BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_NO_ANSWER,   "NO ANSWER",    BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_CME_ERROR,   "+CME ERROR",   BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_Error_BG96},
    {_AT_CMS_ERROR,   "+CMS ERROR",   BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CmsErr},

    /* GENERIC MODEM commands */
    {_AT_CGMI,        "+CGMI",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CGMI},
    {_AT_CGMM,        "+CGMM",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CGMM},
    {_AT_CGMR,        "+CGMR",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CGMR},
    {_AT_CGSN,        "+CGSN",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_CGSN_BG96,  fRspAnalyze_CGSN},
    {_AT_GSN,         "+GSN",         BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_GSN},
    {_AT_CIMI,        "+CIMI",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CIMI},
    {_AT_CEER,        "+CEER",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CEER},
    {_AT_CMEE,        "+CMEE",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_CMEE,       fRspAnalyze_None},
    {_AT_CPIN,        "+CPIN",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_CPIN,       fRspAnalyze_CPIN_BG96},
    {_AT_CFUN,        "+CFUN",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_CFUN,       fRspAnalyze_CFUN_BG96},
    {_AT_COPS,        "+COPS",        BG96_COPS_TIMEOUT,     fCmdBuild_COPS,       fRspAnalyze_COPS},
    {_AT_CNUM,        "+CNUM",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CNUM},
    {_AT_CGATT,       "+CGATT",       BG96_CGATT_TIMEOUT,    fCmdBuild_CGATT,      fRspAnalyze_CGATT},
    {_AT_CGPADDR,     "+CGPADDR",     BG96_DEFAULT_TIMEOUT,  fCmdBuild_CGPADDR,    fRspAnalyze_CGPADDR},
    {_AT_CEREG,       "+CEREG",       BG96_DEFAULT_TIMEOUT,  fCmdBuild_CEREG,      fRspAnalyze_CEREG},
    {_AT_CREG,        "+CREG",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_CREG,       fRspAnalyze_CREG},
    {_AT_CGREG,       "+CGREG",       BG96_DEFAULT_TIMEOUT,  fCmdBuild_CGREG,      fRspAnalyze_CGREG},
    {_AT_CSQ,         "+CSQ",         BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CSQ},
    {_AT_CGDCONT,     "+CGDCONT",     BG96_DEFAULT_TIMEOUT,  fCmdBuild_CGDCONT_BG96,    fRspAnalyze_None},
    {_AT_CGACT,       "+CGACT",       BG96_CGACT_TIMEOUT,    fCmdBuild_CGACT,      fRspAnalyze_None},
    {_AT_CGDATA,      "+CGDATA",      BG96_DEFAULT_TIMEOUT,  fCmdBuild_CGDATA,     fRspAnalyze_None},
    {_AT_CGEREP,      "+CGEREP",      BG96_DEFAULT_TIMEOUT,  fCmdBuild_CGEREP,     fRspAnalyze_None},
    {_AT_CGEV,        "+CGEV",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_CGEV},
    {_ATD,            "D",            BG96_DEFAULT_TIMEOUT,  fCmdBuild_ATD_BG96,   fRspAnalyze_None},
    {_ATE,            "E",            BG96_DEFAULT_TIMEOUT,  fCmdBuild_ATE,        fRspAnalyze_None},
    {_ATH,            "H",            BG96_ATH_TIMEOUT,      fCmdBuild_NoParams,   fRspAnalyze_None},
    {_ATO,            "O",            BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_ATV,            "V",            BG96_DEFAULT_TIMEOUT,  fCmdBuild_ATV,        fRspAnalyze_None},
    {_ATX,            "X",            BG96_DEFAULT_TIMEOUT,  fCmdBuild_ATX,        fRspAnalyze_None},
    {_AT_ESC_CMD,     "+++",          BG96_ESCAPE_TIMEOUT,   fCmdBuild_ESCAPE_CMD, fRspAnalyze_None},
    {_AT_IPR,         "+IPR",         BG96_DEFAULT_TIMEOUT,  fCmdBuild_IPR,        fRspAnalyze_IPR},
    {_AT_IFC,         "+IFC",         BG96_DEFAULT_TIMEOUT,  fCmdBuild_IFC,        fRspAnalyze_None},
    {_AT_AND_W,       "&W",           BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_AND_D,       "&D",           BG96_DEFAULT_TIMEOUT,  fCmdBuild_AT_AND_D,   fRspAnalyze_None},

    /* MODEM SPECIFIC COMMANDS */
    {_AT_QPOWD,       "+QPOWD",       BG96_DEFAULT_TIMEOUT,  fCmdBuild_QPOWD_BG96, fRspAnalyze_None},
    {_AT_QCFG,        "+QCFG",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_QCFG_BG96,  fRspAnalyze_QCFG_BG96},
    {_AT_QINDCFG ,    "+QINDCFG",     BG96_DEFAULT_TIMEOUT,  fCmdBuild_QINDCFG_BG96,  fRspAnalyze_QINDCFG_BG96},
    {_AT_QIND,        "+QIND",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_QIND_BG96},
    {_AT_QUSIM,       "+QUSIM",       BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_CPSMS,       "+CPSMS",       BG96_DEFAULT_TIMEOUT,  fCmdBuild_CPSMS_BG96, fRspAnalyze_None},
    {_AT_CEDRXS,      "+CEDRXS",      BG96_DEFAULT_TIMEOUT,  fCmdBuild_CEDRXS_BG96, fRspAnalyze_None},
    {_AT_QNWINFO,     "+QNWINFO",     BG96_QNWINFO_TIMEOUT,  fCmdBuild_NoParams, fRspAnalyze_None},
    {_AT_QENG,        "+QENG",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_QENG_BG96, fRspAnalyze_None},

    {_AT_QICSGP,      "+QICSGP",      BG96_DEFAULT_TIMEOUT,  fCmdBuild_QICSGP_BG96, fRspAnalyze_None},
    {_AT_QIACT,       "+QIACT",       BG96_QIACT_TIMEOUT,    fCmdBuild_QIACT_BG96,  fRspAnalyze_QIACT_BG96},
  /* TODO  {_AT_QIDEACT,     "+QIDEACT",     BG96_QIDEACT_TIMEOUT,  fCmdBuild_NoParams, fRspAnalyze_None}, */
    {_AT_QIOPEN,      "+QIOPEN",      BG96_QIOPEN_TIMEOUT,   fCmdBuild_QIOPEN_BG96, fRspAnalyze_QIOPEN_BG96},
    {_AT_QICLOSE,     "+QICLOSE",     BG96_QICLOSE_TIMEOUT,  fCmdBuild_QICLOSE_BG96, fRspAnalyze_None},
    {_AT_QISEND,      "+QISEND",      BG96_DEFAULT_TIMEOUT,  fCmdBuild_QISEND_BG96, fRspAnalyze_None},
    {_AT_QISEND_WRITE_DATA,  "",      BG96_DEFAULT_TIMEOUT,  fCmdBuild_QISEND_WRITE_DATA_BG96, fRspAnalyze_None},
    {_AT_QIRD,        "+QIRD",        BG96_DEFAULT_TIMEOUT,  fCmdBuild_QIRD_BG96, fRspAnalyze_QIRD_BG96},
    {_AT_QICFG,       "+QICFG",       BG96_DEFAULT_TIMEOUT,  fCmdBuild_QICFG_BG96, fRspAnalyze_None},
    {_AT_QISTATE,     "+QISTATE",     BG96_DEFAULT_TIMEOUT,  fCmdBuild_QISTATE_BG96, fRspAnalyze_QISTATE_BG96},
    {_AT_QIURC,       "+QIURC",       BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams, fRspAnalyze_QIURC_BG96},
    {_AT_SOCKET_PROMPT, "> ",         BG96_SOCKET_PROMPT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_SEND_OK,      "SEND OK",     BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_SEND_FAIL,    "SEND FAIL",   BG96_DEFAULT_TIMEOUT,  fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_QIDNSCFG,     "+QIDNSCFG",   BG96_DEFAULT_TIMEOUT,  fCmdBuild_QIDNSCFG_BG96, fRspAnalyze_None},
    {_AT_QIDNSGIP,     "+QIDNSGIP",   BG96_QIDNSGIP_TIMEOUT, fCmdBuild_QIDNSGIP_BG96, fRspAnalyze_None},

    /* MODEM SPECIFIC EVENTS */
    {_AT_WAIT_EVENT,     "",          BG96_DEFAULT_TIMEOUT,        fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_BOOT_EVENT,     "",          BG96_RDY_TIMEOUT,            fCmdBuild_NoParams,   fRspAnalyze_None},
    {_AT_RDY_EVENT,      "RDY",       BG96_RDY_TIMEOUT,            fCmdBuild_NoParams,   fRspAnalyze_None},
    /* {_AT_SIMREADY_EVENT, "",          BG96_SIMREADY_TIMEOUT,       fCmdBuild_NoParams,   fRspAnalyze_None}, */
};
#define size_ATCMD_BG96_LUT ((uint16_t) (sizeof (ATCMD_BG96_LUT) / sizeof (atcustom_LUT_t)))

/* ###########################  END CUSTOMIZATION PART  ########################### */

/* Exported functions ------------------------------------------------------- */
void ATCustom_BG96_init(atparser_context_t *p_atp_ctxt)
{
    /* common init */
    atcm_modem_init(&BG96_ctxt);

    /* ###########################  START CUSTOMIZATION PART  ########################### */
    BG96_ctxt.modem_LUT_size = size_ATCMD_BG96_LUT;
    BG96_ctxt.p_modem_LUT = (const atcustom_LUT_t *)ATCMD_BG96_LUT;

    /* override default termination string for AT command: <CR> */
    sprintf((char *)p_atp_ctxt->endstr, "\r");

    /* ###########################  END CUSTOMIZATION PART  ########################### */
}

uint8_t ATCustom_BG96_checkEndOfMsgCallback(uint8_t rxChar)
{
    uint8_t last_char = 0;

    switch (BG96_ctxt.state_SyntaxAutomaton)
    {
     case WAITING_FOR_INIT_CR:
        {
            /* waiting for first valid <CR>, char received before are considered as trash */
            if('\r' == rxChar)
            {
                /* current     : xxxxx<CR>
                *  command format : <CR><LF>xxxxxxxx<CR><LF>
                *  waiting for : <LF>
                */
                BG96_ctxt.state_SyntaxAutomaton = WAITING_FOR_LF;
            }
            break;
        }

    case WAITING_FOR_CR:
        {
            if('\r' == rxChar)
            {
                /* current     : xxxxx<CR>
                *  command format : <CR><LF>xxxxxxxx<CR><LF>
                *  waiting for : <LF>
                */
                BG96_ctxt.state_SyntaxAutomaton = WAITING_FOR_LF;
            }
            break;
        }

    case WAITING_FOR_LF:
        {
            /* waiting for <LF> */
            if('\n' == rxChar)
            {
                /*  current        : <CR><LF>
                *   command format : <CR><LF>xxxxxxxx<CR><LF>
                *   waiting for    : x or <CR>
                */
                BG96_ctxt.state_SyntaxAutomaton = WAITING_FOR_FIRST_CHAR;
                last_char = 1;
            }
            break;
        }

     case WAITING_FOR_FIRST_CHAR:
        {
            if (BG96_ctxt.socket_ctxt.socket_RxData_state == SocketRxDataState_waiting_header)
            {
                /* Socket Data RX - waiting for Header: we are waiting for first <CR>
                *
                * <CR><LF>+QIRD: 522<CR><LF>HTTP/1.1 200 OK<CR><LF><CR><LF>Date: Wed, 21 Feb 2018 14:56:54 GMT<CR><LF><CR><LF>Serve...
                *  ^- waiting this <CR>
                */
                if('\r' == rxChar)
                {
                  /* first <CR> detected, next step */
                  socketHeaderRX_reset();
                  BG96_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_receiving_header;
                }
            }
            else if (BG96_ctxt.socket_ctxt.socket_RxData_state == SocketRxDataState_receiving_header)
            {
                /* Socket Data RX - Header received: we are waiting for second <CR>
                *
                * <CR><LF>+QIRD: 522<CR><LF>HTTP/1.1 200 OK<CR><LF><CR><LF>Date: Wed, 21 Feb 2018 14:56:54 GMT<CR><LF><CR><LF>Serve...
                *                    ^- waiting this <CR>
                */
                if('\r' == rxChar)
                {
                    /* second <CR> detected, we have received data header
                    *  now waiting for <LF>, then start to receive socket datas
                    *  Verify that size received in header is the expected one
                    */
                    uint16_t size_from_header = SocketHeaderRX_getSize();
                    if (BG96_ctxt.socket_ctxt.socket_rx_expected_buf_size != size_from_header)
                    {
                        /* update buffer size received - should not happen */
                        BG96_ctxt.socket_ctxt.socket_rx_expected_buf_size = size_from_header;
                    }
                    BG96_ctxt.state_SyntaxAutomaton = WAITING_FOR_LF;
                    BG96_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_receiving_data;
                }
                else if ((rxChar >= '0') && (rxChar <= '9'))
                {
                    /* receiving size of data in header */
                    SocketHeaderRX_addChar((char *)&rxChar);
                }
            }
            else if (BG96_ctxt.socket_ctxt.socket_RxData_state == SocketRxDataState_receiving_data)
            {
                /* receiving socket data: do not analyze char, just count expected size
                *
                * <CR><LF>+QIRD: 522<CR><LF>HTTP/1.1 200 OK<CR><LF><CR><LF>Date: Wed, 21 Feb 2018 14:56:54 GMT<CR><LF><CR><LF>Serve...
                *.                          ^- start to read data: count char
                */
                BG96_ctxt.state_SyntaxAutomaton = WAITING_FOR_SOCKET_DATA;
                BG96_ctxt.socket_ctxt.socket_rx_count_bytes_received++;
                /* check if full buffer has been received */
                if (BG96_ctxt.socket_ctxt.socket_rx_count_bytes_received == BG96_ctxt.socket_ctxt.socket_rx_expected_buf_size)
                {
                    BG96_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_data_received;
                    BG96_ctxt.state_SyntaxAutomaton = WAITING_FOR_CR;
                }
            }
            /* waiting for <CR> or x */
            else if('\r' == rxChar)
            {
                /*   current        : <CR>
                *   command format : <CR><LF>xxxxxxxx<CR><LF>
                *   waiting for    : <LF>
                */
                BG96_ctxt.state_SyntaxAutomaton = WAITING_FOR_LF;
            }
            break;
        }

    case WAITING_FOR_SOCKET_DATA:
        BG96_ctxt.socket_ctxt.socket_rx_count_bytes_received++;
        /* check if full buffer has been received */
        if (BG96_ctxt.socket_ctxt.socket_rx_count_bytes_received == BG96_ctxt.socket_ctxt.socket_rx_expected_buf_size)
        {
            BG96_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_data_received;
            BG96_ctxt.state_SyntaxAutomaton = WAITING_FOR_CR;
        }
        break;
    }

    /* ###########################  START CUSTOMIZATION PART  ######################### */
    /* if modem does not use standard syntax or has some specificities, replace previous
    *  function by a custom function
    */
    if (last_char == 0)
    {
        /* BG96 special cases
        *
        *  SOCKET MODE: when sending DATA using AT+QISEND, we are waiting for socket prompt "<CR><LF>> "
        *               before to send DATA. Then we should receive "OK<CR><LF>".
        */

        if (BG96_ctxt.socket_ctxt.socket_send_state != SocketSendState_No_Activity)
        {
            switch (BG96_ctxt.socket_ctxt.socket_send_state)
            {
             case SocketSendState_WaitingPrompt1st_greaterthan:
                {
                    /* detecting socket prompt first char: "greater than" */
                    if ('>' == rxChar)
                    {
                        BG96_ctxt.socket_ctxt.socket_send_state = SocketSendState_WaitingPrompt2nd_space;
                    }
                    break;
                }

             case SocketSendState_WaitingPrompt2nd_space:
                {
                    /* detecting socket prompt second char: "space" */
                    if (' ' == rxChar)
                    {
                        BG96_ctxt.socket_ctxt.socket_send_state = SocketSendState_Prompt_Received;
                        last_char = 1;
                    }
                    else
                    {
                        /* if char iommediatly after "greater than" is not a "space", reinit state */
                        BG96_ctxt.socket_ctxt.socket_send_state = SocketSendState_WaitingPrompt1st_greaterthan;
                    }
                    break;
                }

             default:
            	 /* No matching case */
            	 break;
            }
        }
    }

    /* ###########################  END CUSTOMIZATION PART  ########################### */

    return(last_char);
}

at_status_t ATCustom_BG96_getCmd(atparser_context_t *p_atp_ctxt, uint32_t *p_ATcmdTimeout)
{
    at_status_t retval = ATSTATUS_OK;
    at_msg_t curSID = p_atp_ctxt->current_SID;

    PrintAPI("enter ATCustom_BG96_getCmd() for SID %d", curSID);

    /* retrieve parameters from SID command (will update SID_ctxt) */
    if (atcm_retrieve_SID_parameters(&BG96_ctxt, p_atp_ctxt) != ATSTATUS_OK)
    {
      return(ATSTATUS_ERROR);
    }

    /* new command: reset command context */
    atcm_reset_CMD_context(&BG96_ctxt.CMD_ctxt);

    /* For each SID, athe sequence of AT commands to send id defined (it can be dynamic)
    * Determine and prepare the next command to send for this SID
    */

    /* ###########################  START CUSTOMIZATION PART  ########################### */
    if (curSID == SID_CS_CHECK_CNX)
    {
        if (p_atp_ctxt->step == 0)
        {
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _AT, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_MODEM_CONFIG)
    {
        if (p_atp_ctxt->step == 0)
        {
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _AT, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if ((curSID == SID_CS_POWER_ON) || (curSID == SID_CS_RESET))
    {
        uint8_t common_start_sequence_step = 1;
        uint8_t specific_start_sequence_step = common_start_sequence_step + 6;

        /* POWER_ON and RESET are almost the same, specific differences are managed case by case */

        /* for reset, only HW reset is supported */
        if ((curSID == SID_CS_RESET) && (BG96_ctxt.SID_ctxt.reset_type != CS_RESET_HW))
        {
             PrintErr("Reset type (%d) not supported", BG96_ctxt.SID_ctxt.reset_type);
            retval = ATSTATUS_ERROR;
        }
        else
        {
            if (p_atp_ctxt->step == 0)
            {
                /* reset modem specific variables */
                reset_variables_bg96();

                /* check if RDY has been received */
                if (BG96_ctxt.persist.modem_at_ready == AT_TRUE)
                {
                    PrintINFO("Modem START indication already received, continue init sequence...");
                    /* now reset modem_at_ready (in case of reception of reset indication) */
                    BG96_ctxt.persist.modem_at_ready = AT_FALSE;
                    /* if yes, go to next step: jump to POWER ON sequence step */
                    p_atp_ctxt->step = common_start_sequence_step;

                    if (curSID == SID_CS_RESET)
                    {
                        /* reinit context for reset case */
                        atcm_modem_reset(&BG96_ctxt);
                    }
                }
                else
                {
                    if (curSID == SID_CS_RESET)
                    {
                        /* reinit context for reset case */
                        atcm_modem_reset(&BG96_ctxt);
                    }

                    PrintINFO("Modem START indication not received yet, waiting for it...");
                    /* wait for RDY (if not received, this is not an error, see below with autobaud) */
                    atcm_program_TEMPO(p_atp_ctxt, BG96_RDY_TIMEOUT, INTERMEDIATE_CMD);
                }
            }

            /* start common power ON sequence here */
             if (p_atp_ctxt->step == common_start_sequence_step)
            {
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _AT, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == common_start_sequence_step + 1)
            {
                /* disable echo */
                BG96_ctxt.CMD_ctxt.command_echo = AT_FALSE;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _ATE, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == common_start_sequence_step + 2)
            {
                /* request detailled error report */
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CMEE, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == common_start_sequence_step + 3)
            {
                /* enable full response format */
                BG96_ctxt.CMD_ctxt.dce_full_resp_format = AT_TRUE;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _ATV, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == common_start_sequence_step + 4)
            {
                /* deactivate DTR */
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _AT_AND_D, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == common_start_sequence_step + 5)
            {
                /* set flow control */
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_IFC, INTERMEDIATE_CMD);
            }

            /* start specific power ON sequence here */
            /* BG96_AT_Commands_Manual_V2.0 */
            else if (p_atp_ctxt->step == specific_start_sequence_step)
            {
#if (BG96_SEND_READ_CPSMS == 1)
                /* check power saving mode settings */
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_CPSMS, INTERMEDIATE_CMD);
#else
                atcm_program_TEMPO(p_atp_ctxt, 100, INTERMEDIATE_CMD);
#endif
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 1)
            {
#if (BG96_SEND_READ_CEDRXS == 1)
                /* check e-I-DRX settings */
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_CEDRXS, INTERMEDIATE_CMD);
#else
                atcm_program_TEMPO(p_atp_ctxt, 100, INTERMEDIATE_CMD);
#endif
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 2)
            {
#if (BG96_SEND_WRITE_CPSMS == 1)
                /* set power settings mode params */
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CPSMS, INTERMEDIATE_CMD);
#else
                /* do not set power settings mode params */
                atcm_program_TEMPO(p_atp_ctxt, 100, INTERMEDIATE_CMD);
#endif
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 3)
            {
#if (BG96_SEND_WRITE_CEDRXS == 1)
                /* set e-I-DRX params */
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CEDRXS, INTERMEDIATE_CMD);
#else
                /* do not set e-I-DRX params */
                atcm_program_TEMPO(p_atp_ctxt, 300, INTERMEDIATE_CMD);
#endif
            }

#if (BG96_SET_BANDS == 0)
            /* Check bands paramaters */
            else if (p_atp_ctxt->step == specific_start_sequence_step + 4)
            {
                BG96_QCFG_command_write = AT_FALSE;
                BG96_QCFG_command_param = _QCFG_band;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QCFG, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 5)
            {
                BG96_QCFG_command_write = AT_FALSE;
                BG96_QCFG_command_param = _QCFG_iotopmode;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QCFG, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 6)
            {
                BG96_QCFG_command_write = AT_FALSE;
                BG96_QCFG_command_param = _QCFG_nwscanseq;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QCFG, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 7)
            {
                BG96_QCFG_command_write = AT_FALSE;
                BG96_QCFG_command_param = _QCFG_nwscanmode;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QCFG, FINAL_CMD);
            }
            else if (p_atp_ctxt->step > specific_start_sequence_step + 8)
            {
                /* error, invalid step */
                retval = ATSTATUS_ERROR;
            }
#else
            /* Check and set bands paramaters */
            else if (p_atp_ctxt->step == specific_start_sequence_step + 4)
            {
                BG96_QCFG_command_write = AT_TRUE;
                BG96_QCFG_command_param = _QCFG_iotopmode;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QCFG, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 5)
            {
                BG96_QCFG_command_write = AT_FALSE;
                BG96_QCFG_command_param = _QCFG_iotopmode;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QCFG, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 6)
            {
                BG96_QCFG_command_write = AT_TRUE;
                BG96_QCFG_command_param = _QCFG_nwscanseq;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QCFG, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 7)
            {
                BG96_QCFG_command_write = AT_FALSE;
                BG96_QCFG_command_param = _QCFG_nwscanseq;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QCFG, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 8)
            {
                BG96_QCFG_command_write = AT_TRUE;
                BG96_QCFG_command_param = _QCFG_nwscanmode;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QCFG, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 9)
            {
                BG96_QCFG_command_write = AT_FALSE;
                BG96_QCFG_command_param =  _QCFG_nwscanmode;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QCFG, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 10)
            {
                BG96_QCFG_command_write = AT_TRUE;
                BG96_QCFG_command_param = _QCFG_band;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QCFG, INTERMEDIATE_CMD);
            }
            else if (p_atp_ctxt->step == specific_start_sequence_step + 11)
            {
                BG96_QCFG_command_write = AT_FALSE;
                BG96_QCFG_command_param = _QCFG_band;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QCFG, FINAL_CMD);
            }
            else if (p_atp_ctxt->step > specific_start_sequence_step + 12)
            {
                /* error, invalid step */
                retval = ATSTATUS_ERROR;
            }
#endif
        }
    }
    else if (curSID == SID_CS_POWER_OFF)
    {
        /* TODO ? deregister */
        if (p_atp_ctxt->step == 0)
        {
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QPOWD, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_INIT_MODEM)
    {
#if defined(BG96_OPTION_CHECK_CPIN)
       if (p_atp_ctxt->step == 0)
        {
            /* check is CPIN is requested */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_CPIN, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 1)
        {
            if (BG96_ctxt.persist.sim_pin_code_ready == AT_FALSE)
            {
                PrintDBG("CPIN required, we send user value to modem");

                if (strlen((const char *)&BG96_ctxt.SID_ctxt.modem_init.pincode.pincode) != 0)
                {
                    /* send PIN value */
                    atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CPIN, INTERMEDIATE_CMD);
                }
                else
                {
                    /* no PIN provided by user */
                    retval = ATSTATUS_ERROR;
                }
            }
            else
            {
                PrintDBG("CPIN not required");
                /* no PIN required, send CFUN as last command */
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CFUN, FINAL_CMD);
            }
        }
        else if (p_atp_ctxt->step == 2)
        {
            /* PIN was required, now send CFUN as last command */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CFUN, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
#else
        if (p_atp_ctxt->step == 0)
        {
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CFUN, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
#endif /* BG96_OPTION_CHECK_CPIN */
    }
    else if (curSID == SID_CS_GET_DEVICE_INFO)
    {
        if (p_atp_ctxt->step == 0)
        {
            switch (BG96_ctxt.SID_ctxt.device_info->field_requested)
            {
             case CS_DIF_MANUF_NAME_PRESENT:
              atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _AT_CGMI, FINAL_CMD);
              break;

             case CS_DIF_MODEL_PRESENT:
              atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _AT_CGMM, FINAL_CMD);
              break;

             case CS_DIF_REV_PRESENT:
              atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _AT_CGMR, FINAL_CMD);
              break;

             case CS_DIF_SN_PRESENT:
              atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _AT_CGSN, FINAL_CMD);
              break;

             case CS_DIF_IMEI_PRESENT:
              atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _AT_GSN, FINAL_CMD);
              break;

             case CS_DIF_IMSI_PRESENT:
              atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _AT_CIMI, FINAL_CMD);
              break;

             case CS_DIF_PHONE_NBR_PRESENT:
              /* not AT+CNUM not supported by BG96 */
              retval = ATSTATUS_ERROR;
              break;

             default:
              /* error, invalid step */
              retval = ATSTATUS_ERROR;
              break;
            }
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_GET_SIGNAL_QUALITY)
    {
        uint8_t sid_steps = 0;
#if (BG96_OPTION_NETWORK_INFO == 1)
        sid_steps++;
#endif
#if (BG96_OPTION_ENGINEERING_MODE == 1)
        sid_steps++;
#endif

        atcustom_FinalCmd_t last_cmd_info;
        last_cmd_info = (p_atp_ctxt->step == sid_steps) ? FINAL_CMD : INTERMEDIATE_CMD;

        if (p_atp_ctxt->step == 0)
        {
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _AT_CSQ, last_cmd_info);
        }
#if (BG96_OPTION_NETWORK_INFO == 1)
        else if (p_atp_ctxt->step == 1)
        {
            /* NB: cmd answer is optionnal ie no error will be raised if no answer received from modem
            *  indeed, if requested here, it's just a bonus and should not generate an error
            */
            atcm_program_AT_CMD_ANSWER_OPTIONAL(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _AT_QNWINFO, last_cmd_info);
        }
#endif
#if (BG96_OPTION_ENGINEERING_MODE == 1)
        else if ((p_atp_ctxt->step == 2) ||
                 ((p_atp_ctxt->step == 1) && (sid_steps == 1)))
        {
            BG96_QENG_command_param = _QENGcelltype_ServingCell;
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QENG, last_cmd_info);
        }
#endif
    }
    else if (curSID == SID_CS_GET_ATTACHSTATUS)
    {
        if (p_atp_ctxt->step == 0)
        {
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_CGATT, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_REGISTER_NET)
    {
        if (p_atp_ctxt->step == 0)
        {
            /* read registration status */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_COPS, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 1)
        {
#if 0
            /* check if actual registration status is the expected one */
            CS_OperatorSelector_t *operatorSelect = &(BG96_ctxt.SID_ctxt.write_operator_infos);
            if (BG96_ctxt.SID_ctxt.read_operator_infos.mode != operatorSelect->mode)
            {
                /* write registration status */
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_COPS, INTERMEDIATE_CMD);
            }
            else
            {
                /* skip this step */
                atcm_program_SKIP_CMD(p_atp_ctxt);
            }
#else
            /* due to problem observed on simu: does not register after reboot */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_COPS, INTERMEDIATE_CMD);
#endif
        }
        else if (p_atp_ctxt->step == 2)
        {
            /* read registration status */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_CEREG, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 3)
        {
            /* read registration status */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_CREG, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 4)
        {
            /* read registration status */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_CGREG, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_GET_NETSTATUS)
    {
        if (p_atp_ctxt->step == 0)
        {
            /* read registration status */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_CEREG, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 1)
        {
            /* read registration status */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_CREG, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 2)
        {
            /* read registration status */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_CGREG, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 3)
        {
            /* read registration status */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_COPS, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_SUSBCRIBE_NET_EVENT)
    {
        if (p_atp_ctxt->step == 0)
        {
            CS_UrcEvent_t urcEvent = BG96_ctxt.SID_ctxt.urcEvent;

            /* is an event linked to CREG, CGREG or CEREG ? */
            if ((urcEvent == CS_URCEVENT_EPS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_EPS_LOCATION_INFO) ||
                (urcEvent == CS_URCEVENT_GPRS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_GPRS_LOCATION_INFO) ||
                (urcEvent == CS_URCEVENT_CS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_CS_LOCATION_INFO))
            {
                atcm_subscribe_net_event(&BG96_ctxt, p_atp_ctxt);
            }
            else if  (urcEvent == CS_URCEVENT_SIGNAL_QUALITY)
            {
                /* if signal quality URC not yet suscbribe */
                if (BG96_ctxt.persist.urc_subscript_signalQuality == CELLULAR_FALSE)
                {
                    /* set event as suscribed */
                    BG96_ctxt.persist.urc_subscript_signalQuality = CELLULAR_TRUE;

                    /* request the URC we want */
                    BG96_QINDCFG_command_param = _QINDCFG_csq;
                    atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QINDCFG, FINAL_CMD);
                }
                else
                {
                    atcm_program_NO_MORE_CMD(p_atp_ctxt);
                }
            }
            else
            {
                atcm_program_NO_MORE_CMD(p_atp_ctxt);
            }

        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_UNSUSBCRIBE_NET_EVENT)
    {
        if (p_atp_ctxt->step == 0)
        {
            CS_UrcEvent_t urcEvent = BG96_ctxt.SID_ctxt.urcEvent;

            /* is an event linked to CREG, CGREG or CEREG ? */
            if ((urcEvent == CS_URCEVENT_EPS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_EPS_LOCATION_INFO) ||
                (urcEvent == CS_URCEVENT_GPRS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_GPRS_LOCATION_INFO) ||
                (urcEvent == CS_URCEVENT_CS_NETWORK_REG_STAT) || (urcEvent == CS_URCEVENT_CS_LOCATION_INFO))
            {
                atcm_unsubscribe_net_event(&BG96_ctxt, p_atp_ctxt);
            }
            else if  (urcEvent == CS_URCEVENT_SIGNAL_QUALITY)
            {
                /* if signal quality URC suscbribed */
                if (BG96_ctxt.persist.urc_subscript_signalQuality == CELLULAR_TRUE)
                {
                    /* set event as unsuscribed */
                    BG96_ctxt.persist.urc_subscript_signalQuality = CELLULAR_FALSE;

                    /* request the URC we don't want */
                    BG96_QINDCFG_command_param = _QINDCFG_csq;
                    atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QINDCFG, FINAL_CMD);
                }
                else
                {
                    atcm_program_NO_MORE_CMD(p_atp_ctxt);
                }
            }
            else
            {
                atcm_program_NO_MORE_CMD(p_atp_ctxt);
            }
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_REGISTER_PDN_EVENT)
    {
        if (p_atp_ctxt->step == 0)
        {
            if (BG96_ctxt.persist.urc_subscript_pdn_event == CELLULAR_FALSE)
            {
                /* set event as suscribed */
                BG96_ctxt.persist.urc_subscript_pdn_event = CELLULAR_TRUE;

                /* request PDN events */
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CGEREP, FINAL_CMD);
            }
            else
            {
                atcm_program_NO_MORE_CMD(p_atp_ctxt);
            }
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_DEREGISTER_PDN_EVENT)
    {
        if (p_atp_ctxt->step == 0)
        {
            if (BG96_ctxt.persist.urc_subscript_pdn_event == CELLULAR_TRUE)
            {
                /* set event as unsuscribed */
                BG96_ctxt.persist.urc_subscript_pdn_event = CELLULAR_FALSE;

                /* request PDN events */
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CGEREP, FINAL_CMD);
            }
            else
            {
                atcm_program_NO_MORE_CMD(p_atp_ctxt);
            }
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_ATTACH_PS_DOMAIN)
    {
        if (p_atp_ctxt->step == 0)
        {
            BG96_ctxt.CMD_ctxt.cgatt_write_cmd_param = CGATT_ATTACHED;
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CGATT, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_DETACH_PS_DOMAIN)
    {
        if (p_atp_ctxt->step == 0)
        {
            BG96_ctxt.CMD_ctxt.cgatt_write_cmd_param = CGATT_DETACHED;
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CGATT, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_ACTIVATE_PDN)
    {
#if (USE_SOCKETS_MODEM == 1)
        /* SOCKET MODE */
        if (p_atp_ctxt->step == 0)
        {
            /* ckeck PDN state */
            bg96_pdn_already_active = AT_FALSE;
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_QIACT, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 1)
        {
            /* PDN activation */
            if (bg96_pdn_already_active == AT_TRUE)
            {
                /* PDN already active - exit */
                PrintINFO("Skip PDN activation (already active)");
                atcm_program_NO_MORE_CMD(p_atp_ctxt);
            }
            else
            {
                /* request PDN activation */
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QIACT, INTERMEDIATE_CMD);
            }
        }
        else if (p_atp_ctxt->step == 2)
        {
            /* ckeck PDN state */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_QIACT, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
#else
        /* DATA MODE*/
        if (p_atp_ctxt->step == 0)
        {
            /* get IP address */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CGPADDR, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 1)
        {
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _ATD, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
#endif /* USE_SOCKETS_MODEM */
    }
    else if (curSID == SID_CS_DEACTIVATE_PDN)
    {
        /* TODO not managed yet */
        retval = ATSTATUS_ERROR;
    }
    else if (curSID == SID_CS_DEFINE_PDN)
    {
#if (USE_SOCKETS_MODEM == 1)
        /* SOCKET MODE */
        if (p_atp_ctxt->step == 0)
        {
            bg96_QICGSP_config_command = AT_TRUE;
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QICSGP, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 1)
        {
            bg96_QICGSP_config_command = AT_FALSE;
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QICSGP, FINAL_CMD);
        }

        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
#else
        /* DATA MODE*/
        if (p_atp_ctxt->step == 0)
        {
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CGDCONT, FINAL_CMD);
            /* TODO */
            /* add AT+CGAUTH for username and password */
            /* or use AT+QICSGP */
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
#endif /* USE_SOCKETS_MODEM */
    }
    else if (curSID == SID_CS_SET_DEFAULT_PDN)
    {
        /* nothing to do here
         * Indeed, default PDN has been saved automatically during analysis of SID command
         * cf function: atcm_retrieve_SID_parameters()
         */
        atcm_program_NO_MORE_CMD(p_atp_ctxt);
    }
    else if (curSID == SID_CS_GET_IP_ADDRESS)
    {
        /* get IP address */
#if (USE_SOCKETS_MODEM == 1)
        /* SOCKET MODE */
        atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_READ_CMD, _AT_QIACT, FINAL_CMD);
#else
        /* DATA MODE*/
        atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CGPADDR, FINAL_CMD);
#endif /* USE_SOCKETS_MODEM */
    }
    else if (curSID == SID_CS_DIAL_COMMAND)
    {
        /* SOCKET CONNECTION FOR COMMAND DATA MODE */

        /* TODO */
        /* do we need to activate PDP context ? indeed, this parameter is used in _AT_SQNSCFG
           if yes, add CGACT ? */
        /* atcm_program_AT_CMD(&SEQMONARCH_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_CGACT, INTERMEDIATE_CMD); */
        /* or QIACT ? */

        /* TODO: configure socket ? */
        /* atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QICFG, INTERMEDIATE_CMD); */

        if (p_atp_ctxt->step == 0)
        {
            /* reserve a modem CID for this socket_handle */
            bg96_current_qiopen_socket_connected = 0;
            socket_handle_t sockHandle = BG96_ctxt.socket_ctxt.socket_info->socket_handle;
            atcm_socket_reserve_modem_cid(&BG96_ctxt, sockHandle);
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QIOPEN, INTERMEDIATE_CMD);
            PrintINFO("For Client Socket Handle=%d : MODEM CID affected=%d",
                      sockHandle,
                      BG96_ctxt.persist.socket[sockHandle].socket_connId_value );

        }
        else if (p_atp_ctxt->step == 1)
        {
            if (bg96_current_qiopen_socket_connected == 0)
            {
                /* Waiting for +QIOPEN urc indicating that socket is open */
                atcm_program_TEMPO(p_atp_ctxt, BG96_QIOPEN_TIMEOUT, INTERMEDIATE_CMD);
            }
            else
            {
                /* socket opened */
                bg96_waiting_qiopen = AT_FALSE;
                /* socket is connected */
                atcm_socket_set_connected(&BG96_ctxt, BG96_ctxt.socket_ctxt.socket_info->socket_handle);
                atcm_program_NO_MORE_CMD(p_atp_ctxt);
            }
        }
        else  if (p_atp_ctxt->step == 2)
        {
            if (bg96_current_qiopen_socket_connected == 0)
            {
                /* QIOPEN NOT RECEIVED,
                *  cf BG96 TCP/IP AT Commands Manual V1.0, paragraph 2.1.4 - 3/
                *  "if the URC cannot be received within 150 seconds, AT+QICLOSE should be used to close
                *   the socket".
                *
                *  then we will have to return an error to cellular service !!! (see next step)
                */
                bg96_waiting_qiopen = AT_FALSE;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QICLOSE, INTERMEDIATE_CMD);
            }
            else
            {
                /* socket opened */
                bg96_waiting_qiopen = AT_FALSE;
                /* socket is connected */
                atcm_socket_set_connected(&BG96_ctxt, BG96_ctxt.socket_ctxt.socket_info->socket_handle);
                atcm_program_NO_MORE_CMD(p_atp_ctxt);
            }
        }
        else  if (p_atp_ctxt->step == 3)
        {
            /* if we fall here, it means we have send _AT_QICLOSE on previous step
            *  now inform cellular service that opening has failed => return an error
            */
            /* release the modem CID for this socket_handle */
            atcm_socket_release_modem_cid(&BG96_ctxt, BG96_ctxt.socket_ctxt.socket_info->socket_handle);
            atcm_program_NO_MORE_CMD(p_atp_ctxt);
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_SEND_DATA)
    {
       if (p_atp_ctxt->step == 0)
        {
            /* Check data size to send */
            if (BG96_ctxt.SID_ctxt.socketSendData_struct.buffer_size > MODEM_MAX_SOCKET_TX_DATA_SIZE)
            {
                PrintErr("Data size to send %d exceed maximum size %d",
                         BG96_ctxt.SID_ctxt.socketSendData_struct.buffer_size,
                         MODEM_MAX_SOCKET_TX_DATA_SIZE);
                atcm_program_NO_MORE_CMD(p_atp_ctxt);
                retval = ATSTATUS_ERROR;
                /* TODO : possible evo is to split data in many commands */
            }
            else
            {
                BG96_ctxt.socket_ctxt.socket_send_state = SocketSendState_WaitingPrompt1st_greaterthan;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QISEND, INTERMEDIATE_CMD);
            }
        }
        else if (p_atp_ctxt->step == 1)
        {
            /* waiting for socket prompt: "<CR><LF>> " */
            if (BG96_ctxt.socket_ctxt.socket_send_state == SocketSendState_Prompt_Received)
            {
                PrintDBG("SOCKET PROMPT ALREADY RECEIVED");
                /* go to next step */
                p_atp_ctxt->step++;
            }
            else
            {
                PrintDBG("WAITING FOR SOCKET PROMPT");
                atcm_program_WAIT_EVENT(p_atp_ctxt, BG96_SOCKET_PROMPT_TIMEOUT, INTERMEDIATE_CMD);
            }
        }

        if (p_atp_ctxt->step == 2)
        {
            /* socket prompt received, send DATA */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_RAW_CMD, _AT_QISEND_WRITE_DATA, FINAL_CMD);

            /* reinit automaton to receive answer */
            reinitSyntaxAutomaton_bg96();
        }
    }
    else if (curSID == SID_CS_RECEIVE_DATA)
    {
        if (p_atp_ctxt->step == 0)
        {
            BG96_ctxt.socket_ctxt.socket_receive_state = SocketRcvState_RequestSize;
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QIRD, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 1)
        {
            /* check that data size to receive is not null */
            if (BG96_ctxt.socket_ctxt.socket_rx_expected_buf_size != 0)
            {
                /* check that data size to receive does not exceed maximum size
                *  if it's the case, only request maximum size we can receive
                */
                if (BG96_ctxt.socket_ctxt.socket_rx_expected_buf_size >
                    BG96_ctxt.socket_ctxt.socketReceivedata.max_buffer_size)
                {
                    PrintINFO("Data size available (%d) exceed buffer maximum size (%d)",
                              BG96_ctxt.socket_ctxt.socket_rx_expected_buf_size,
                              BG96_ctxt.socket_ctxt.socketReceivedata.max_buffer_size);
                    BG96_ctxt.socket_ctxt.socket_rx_expected_buf_size = BG96_ctxt.socket_ctxt.socketReceivedata.max_buffer_size;
                }

                /* receive datas */
                BG96_ctxt.socket_ctxt.socket_receive_state = SocketRcvState_RequestData_Header;
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QIRD, FINAL_CMD);
            }
            else
            {
                /* no datas to receive */
                 atcm_program_NO_MORE_CMD(p_atp_ctxt);
            }
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_SOCKET_CLOSE)
    {
        if (p_atp_ctxt->step == 0)
        {
            /* is socket connected ?
            * due to BG96 socket connection mechanism (waiting URC QIOPEN), we can fall here but socket
            * has been already closed if error occurs during connection
            */
            if ( atcm_socket_is_connected(&BG96_ctxt, BG96_ctxt.socket_ctxt.socket_info->socket_handle) == AT_TRUE)
            {
                atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QICLOSE, INTERMEDIATE_CMD);
            }
            else
            {
                /* release the modem CID for this socket_handle */
                atcm_socket_release_modem_cid(&BG96_ctxt, BG96_ctxt.socket_ctxt.socket_info->socket_handle);
                atcm_program_NO_MORE_CMD(p_atp_ctxt);
            }
        }
        else if (p_atp_ctxt->step == 1)
        {
            /* release the modem CID for this socket_handle */
            atcm_socket_release_modem_cid(&BG96_ctxt, BG96_ctxt.socket_ctxt.socket_info->socket_handle);
            atcm_program_NO_MORE_CMD(p_atp_ctxt);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
   else if (curSID == SID_CS_DATA_SUSPEND)
    {
        if (p_atp_ctxt->step == 0)
        {
            /* wait for 1 second */
           atcm_program_TEMPO(p_atp_ctxt, 1000, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 1)
        {
            /* send escape sequence +++ (RAW command type)
            *  CONNECT expected before 1000 ms
            */
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_RAW_CMD, _AT_ESC_CMD, FINAL_CMD);
            reinitSyntaxAutomaton_bg96();
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_SOCKET_CNX_STATUS)
    {
        if (p_atp_ctxt->step == 0)
        {
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QISTATE, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_DATA_RESUME)
    {
        if (p_atp_ctxt->step == 0)
        {
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_EXECUTION_CMD, _ATO, FINAL_CMD);
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    else if (curSID == SID_CS_DNS_REQ)
    {
        if (p_atp_ctxt->step == 0)
        {
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QIDNSCFG, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 1)
        {
            /* intialize +QIURC "dnsgip" parameters */
            bg96_qiurc_dnsgip_finished = AT_FALSE;
            bg96_qiurc_dnsgip_wait_header = AT_TRUE;
            bg96_qiurc_dnsgip_error = 0;
            bg96_qiurc_dnsgip_ip_count = 0;
            bg96_qiurc_dnsgip_ttl = 0;
            memset((void *)bg96_qiurc_dnsgip_hostIPaddr, 0, MAX_SIZE_IPADDR);
            atcm_program_AT_CMD(&BG96_ctxt, p_atp_ctxt, ATTYPE_WRITE_CMD, _AT_QIDNSGIP, INTERMEDIATE_CMD);
        }
        else if (p_atp_ctxt->step == 2)
        {
            /* do we have received a valid DNS response ? */
            if ((bg96_qiurc_dnsgip_finished == AT_TRUE) && (bg96_qiurc_dnsgip_error == 0))
            {
                /* yes */
                atcm_program_NO_MORE_CMD(p_atp_ctxt);
            }
            else
            {
                /* not yet, waiting for DNS informations */
                atcm_program_TEMPO(p_atp_ctxt, BG96_QIDNSGIP_TIMEOUT, FINAL_CMD);
            }
        }
        else
        {
            /* error, invalid step */
            retval = ATSTATUS_ERROR;
        }
    }
    /* ###########################  END CUSTOMIZATION PART  ########################### */
    else
    {
        PrintErr("Error, invalid command ID");
        retval = ATSTATUS_ERROR;
    }

    /* if no error, build the command to send */
    if (retval == ATSTATUS_OK)
    {
        retval = atcm_modem_build_cmd(&BG96_ctxt, p_atp_ctxt, p_ATcmdTimeout);
    }

    return (retval);
}

at_endmsg_t ATCustom_BG96_extractElement(atparser_context_t *p_atp_ctxt,
                                           IPC_RxMessage_t *p_msg_in,
                                           uint16_t *p_parseIndex,
                                           at_element_info_t *element_infos)
{
    at_endmsg_t retval_msg_end_detected = ATENDMSG_NO;
    uint8_t exit_loop = 0;
    uint16_t idx, start_idx;

    PrintAPI("enter ATCustom_BG96_extractElement()");
    PrintDBG("input message: size=%d ", p_msg_in->size);

    /* if this is beginning of message, check that message header is correct and jump over it */
    if (*p_parseIndex == 0)
    {
        /* ###########################  START CUSTOMIZATION PART  ########################### */
        /* MODEM RESPONSE SYNTAX:
        * <CR><LF><response><CR><LF>
        *
        */
        start_idx = 0;
        /* search initial <CR><LF> sequence (for robustness) */
        if ((p_msg_in->buffer[0] == '\r') && (p_msg_in->buffer[1] == '\n'))
        {
            /* <CR><LF> sequence has been found, it is a command line */
            PrintDBG("cmd init sequence <CR><LF> found - break");
            *p_parseIndex = 2;
            start_idx = 2;
        }
        for (idx=start_idx; idx < (p_msg_in->size - 1); idx++)
        {
            if ((p_atp_ctxt->current_atcmd.id == _AT_QIRD) &&
                (BG96_ctxt.socket_ctxt.socket_receive_state == SocketRcvState_RequestData_Payload) &&
                (BG96_ctxt.socket_ctxt.socket_RxData_state != SocketRxDataState_finished))
            {
                PrintDBG("receiving socket data (real size=%d)", SocketHeaderRX_getSize());
                element_infos->str_start_idx = 0;
                element_infos->str_end_idx = BG96_ctxt.socket_ctxt.socket_rx_count_bytes_received;
                element_infos->str_size = BG96_ctxt.socket_ctxt.socket_rx_count_bytes_received;
                BG96_ctxt.socket_ctxt.socket_RxData_state = SocketRxDataState_finished;
                return(ATENDMSG_YES);
            }
        }
        /* ###########################  END CUSTOMIZATION PART  ########################### */
    }

    element_infos->str_start_idx = *p_parseIndex;
    element_infos->str_end_idx = *p_parseIndex;
    element_infos->str_size = 0;

    /* reach limit of input buffer ? (empty message received) */
    if (*p_parseIndex >= p_msg_in->size)
    {
        return (ATENDMSG_YES);
    };

    /* extract parameter from message */
    do {
        switch (p_msg_in->buffer[*p_parseIndex])
        {
            /* ###########################  START CUSTOMIZATION PART  ########################### */
            /* ----- test separators ----- */
          case ' ':
             if ((p_atp_ctxt->current_atcmd.id == _AT_CGDATA) || (p_atp_ctxt->current_atcmd.id == _ATD))
             {
                 /* specific case for BG96 which answer CONNECT <value> even when ATX0 sent before */
                 exit_loop = 1;
             }
             else
             {
                 element_infos->str_end_idx = *p_parseIndex;
                 element_infos->str_size++;
             }
             break;

          case ':':
          case ',':
             exit_loop = 1;
             break;

            /* ----- test end of message ----- */
          case '\r':
            exit_loop = 1;
            retval_msg_end_detected = ATENDMSG_YES;
            break;

          default:
            /* increment end position */
            element_infos->str_end_idx = *p_parseIndex;
            element_infos->str_size++;
            break;
            /* ###########################  END CUSTOMIZATION PART  ########################### */
        }

        /* increment index */
        (*p_parseIndex)++;

        /* reach limit of input buffer ? */
        if (*p_parseIndex >= p_msg_in->size)
        {
            exit_loop = 1;
            retval_msg_end_detected = ATENDMSG_YES;
        };
    } while (!exit_loop);

    /* increase parameter rank */
    element_infos->param_rank = (element_infos->param_rank + 1);

    return(retval_msg_end_detected);
}

at_action_rsp_t ATCustom_BG96_analyzeCmd(atparser_context_t  *p_atp_ctxt,
                                       IPC_RxMessage_t *p_msg_in,
                                       at_element_info_t *element_infos)
{
    at_action_rsp_t retval = ATACTION_RSP_ERROR;

    PrintAPI("enter ATCustom_BG96_analyzeCmd()");

    /* search in LUT the ID corresponding to command received */
    if (ATSTATUS_OK != atcm_searchCmdInLUT(&BG96_ctxt, p_atp_ctxt, p_msg_in, element_infos))
    {
        /* if cmd_id not found in the LUT, may be it's a text line to analyze */

        /* 1st STEP: search in common modems commands

          NOTE: if this modem has a specific behaviour for one of the common commands, bypass this
          step and manage all command in the 2nd step
        */
        retval = atcm_check_text_line_cmd(&BG96_ctxt, p_atp_ctxt, p_msg_in, element_infos);

        /* 2nd STEP: search in specific modems commands if not found at 1st STEP */
        if (retval == ATACTION_RSP_NO_ACTION)
        {
            switch (p_atp_ctxt->current_atcmd.id)
            {
                /* ###########################  START CUSTOMIZED PART  ########################### */
             case _AT_QIRD:
                if (fRspAnalyze_QIRD_data_BG96(p_atp_ctxt, &BG96_ctxt, p_msg_in, element_infos)!= ATACTION_RSP_ERROR)
                {
                    /* received a valid intermediate answer */
                    retval = ATACTION_RSP_INTERMEDIATE;
                }
                break;

             case _AT_QISTATE:
                if (fRspAnalyze_QISTATE_BG96(p_atp_ctxt, &BG96_ctxt, p_msg_in, element_infos)!= ATACTION_RSP_ERROR)
                {
                    /* received a valid intermediate answer */
                    retval = ATACTION_RSP_INTERMEDIATE;
                }
                break;

                /* ###########################  END CUSTOMIZED PART  ########################### */
             default:
                /* this is not one of modem common command, need to check if this is an answer to a modem's specific cmd */
                PrintDBG("receive an un-expected line... is it a text line ?");
                retval = ATACTION_RSP_IGNORED;
                break;
            }
        }

        /* we fall here when cmd_id not found in the LUT
        * 2 cases are possible:
        *  - this is a valid line: ATACTION_RSP_INTERMEDIATE
        *  - this is an invalid line: ATACTION_RSP_ERROR
        */
        return(retval);
    }

    /* cmd_id has been found in the LUT: determine next action */
    switch (element_infos->cmd_id_received)
    {
      case _AT_OK:
         if (p_atp_ctxt->current_SID == SID_CS_DATA_SUSPEND)
         {
             PrintINFO("MODEM SWITCHES TO COMMAND MODE");
             atcm_set_modem_data_mode(&BG96_ctxt, AT_FALSE);
         }
         else if ((p_atp_ctxt->current_SID == SID_CS_POWER_ON) ||  (p_atp_ctxt->current_SID == SID_CS_RESET))
         {
             if (p_atp_ctxt->current_atcmd.id == _AT)
             {
                 /* bg96_boot_synchro = AT_TRUE; */
                 PrintDBG("modem synchro established");
             }
             else if (p_atp_ctxt->current_atcmd.id == _ATE)
             {
                 PrintDBG("Echo successfully disabled");
             }
         }
        retval = ATACTION_RSP_FRC_END;
        break;

      case _AT_RING:
      case _AT_NO_CARRIER:
      case _AT_NO_DIALTONE:
      case _AT_BUSY:
      case _AT_NO_ANSWER:
         /* VALUES NOT MANAGED IN CURRENT IMPLEMENTATION BECAUSE NOT EXPECTED */
        retval = ATACTION_RSP_ERROR;
        break;

      case _AT_CONNECT:
        PrintINFO("MODEM SWITCHES TO DATA MODE");
        atcm_set_modem_data_mode(&BG96_ctxt, AT_TRUE);
        retval = (at_action_rsp_t)(ATACTION_RSP_FLAG_DATA_MODE | ATACTION_RSP_FRC_END);
        break;

        /* ###########################  START CUSTOMIZATION PART  ########################### */
     case _AT_CEREG:
     case _AT_CREG:
     case _AT_CGREG:
        /* check if response received corresponds to the command we have send
        *  if not => this is an URC
        */
        if (element_infos->cmd_id_received == p_atp_ctxt->current_atcmd.id)
        {
            retval = ATACTION_RSP_INTERMEDIATE;
        }
        else
        {
            retval = ATACTION_RSP_URC_FORWARDED;
        }
        break;

      case _AT_RDY_EVENT:
        PrintDBG(" MODEM READY TO RECEIVE AT COMMANDS");
        /* modem is ready */
        BG96_ctxt.persist.modem_at_ready = AT_TRUE;

        /* if we were waiting for this event, we can continue the sequence */
        if ((p_atp_ctxt->current_SID == SID_CS_POWER_ON) || (p_atp_ctxt->current_SID == SID_CS_RESET))
        {
            /* UNLOCK the WAIT EVENT : as there are still some commands to send after, use CONTINUE */
            BG96_ctxt.persist.modem_at_ready = AT_FALSE;
            retval = ATACTION_RSP_FRC_CONTINUE;
        }
        else
        {
            /* TODO : what to do if reset initiated by the modem */
            retval = ATACTION_RSP_URC_IGNORED;
        }
        break;

     case _AT_SOCKET_PROMPT:
        PrintINFO(" SOCKET PROMPT RECEIVED");
        /* if we were waiting for this event, we can continue the sequence */
        if (p_atp_ctxt->current_SID == SID_CS_SEND_DATA)
        {
            /* UNLOCK the WAIT EVENT */
            retval = ATACTION_RSP_FRC_END;
        }
        else
        {
            retval = ATACTION_RSP_URC_IGNORED;
        }
        break;

      case _AT_SEND_OK:
         if (p_atp_ctxt->current_SID == SID_CS_SEND_DATA)
         {
             retval = ATACTION_RSP_FRC_END;
         }
         else
         {
             retval = ATACTION_RSP_ERROR;
         }
        break;

      case _AT_SEND_FAIL:
        retval = ATACTION_RSP_ERROR;
        break;

      case _AT_QIURC:
        /* retval will be override in analyze of +QUIRC content
        *  indeed, QIURC can be considered as an URC or a normal msg (for DNS request)
        */
        retval = ATACTION_RSP_INTERMEDIATE;
    	break;

     case _AT_QIOPEN:
        /* now waiting for an URC  */
        retval = ATACTION_RSP_INTERMEDIATE;
        break;

     case _AT_QIND:
        retval = ATACTION_RSP_URC_FORWARDED;
        break;

     case _AT_CFUN:
        retval = ATACTION_RSP_URC_IGNORED;
        break;

     case _AT_CPIN:
        retval = ATACTION_RSP_URC_IGNORED;
        break;

     case _AT_QCFG:
        retval = ATACTION_RSP_INTERMEDIATE;
        break;

     case _AT_QUSIM:
        retval = ATACTION_RSP_URC_IGNORED;
        break;

     case _AT_CPSMS:
        retval = ATACTION_RSP_INTERMEDIATE;
        break;

     case _AT_CEDRXS:
        retval = ATACTION_RSP_INTERMEDIATE;
        break;

     case _AT_CGEV:
        retval = ATACTION_RSP_URC_FORWARDED;
        break;

        /* ###########################  END CUSTOMIZATION PART  ########################### */

      case _AT:
        retval = ATACTION_RSP_IGNORED;
        break;

      case _AT_INVALID:
        retval = ATACTION_RSP_ERROR;
        break;

      case _AT_ERROR:
        /* ERROR does not contains parameters, call the analyze function explicity */
        retval = fRspAnalyze_Error_BG96(p_atp_ctxt, &BG96_ctxt, p_msg_in, element_infos);
        break;

      case _AT_CME_ERROR:
      case _AT_CMS_ERROR:
        /* as CME_ERROR and CMS_ERROR contains a parameter, the analyze function will be called
        * automatically by the parser
        */
        retval = ATACTION_RSP_ERROR;
        break;

      default:
        /* check if response received corresponds to the command we have send
        *  if not => this is an ERROR
        */
        if (element_infos->cmd_id_received == p_atp_ctxt->current_atcmd.id)
        {
            retval = ATACTION_RSP_INTERMEDIATE;
        }
        else
        {
            PrintINFO("UNEXPECTED MESSAGE RECEIVED");
            retval = ATACTION_RSP_IGNORED;
        }
        break;
    }

    return (retval);
}

at_action_rsp_t ATCustom_BG96_analyzeParam(atparser_context_t  *p_atp_ctxt,
                                         IPC_RxMessage_t *p_msg_in,
                                         at_element_info_t *element_infos)
{
    at_action_rsp_t retval;
    PrintAPI("enter ATCustom_BG96_analyzeParam()");

    /* analyse parameters of the command we received:
    * call the corresponding function from the LUT
    */
    retval = (atcm_get_CmdAnalyzeFunc(&BG96_ctxt, element_infos->cmd_id_received))(p_atp_ctxt,
                                                                                   &BG96_ctxt,
                                                                                   p_msg_in,
                                                                                   element_infos);

    return (retval);
}

/* function called to finalize an AT command */
at_action_rsp_t ATCustom_BG96_terminateCmd(atparser_context_t *p_atp_ctxt, at_element_info_t *element_infos)
{
    at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PrintAPI("enter ATCustom_BG96_terminateCmd()");

    /* ###########################  START CUSTOMIZATION PART  ########################### */
    /* additional tests */
    if (BG96_ctxt.socket_ctxt.socket_send_state != SocketSendState_No_Activity)
    {
        /* special case for SID_CS_SEND_DATA
        * indeed, this function is called when an AT cmd is finished
        * but for AT+QISEND, it is called a 1st time when prompt is received
        * and a second time when data have been sent.
        */
        if (p_atp_ctxt->current_SID != SID_CS_SEND_DATA)
        {
            /* reset socket_send_state */
            BG96_ctxt.socket_ctxt.socket_send_state = SocketSendState_No_Activity;
        }
    }

    if ((p_atp_ctxt->current_atcmd.id == _ATD) ||
        (p_atp_ctxt->current_atcmd.id == _ATO) ||
        (p_atp_ctxt->current_atcmd.id == _AT_CGDATA))
    {
        if (element_infos->cmd_id_received != _AT_CONNECT)
        {
            PrintErr("expected CONNECT not received !!!");
            return (ATACTION_RSP_ERROR);
        }
        else
        {
            /* force last command (no command can be sent in DATA mode) */
            p_atp_ctxt->is_final_cmd = 1;
            PrintDBG("CONNECT received");
        }
    }

    /* ###########################  END CUSTOMIZATION PART  ########################### */
    return (retval);
}

/* function called to finalize a SID */
at_status_t ATCustom_BG96_get_rsp(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter ATCustom_BG96_get_rsp()");

    /* prepare response for a SID - common part */
    retval = atcm_modem_get_rsp(&BG96_ctxt, p_atp_ctxt, p_rsp_buf);

    /* ###########################  START CUSTOMIZATION PART  ########################### */
    /* prepare response for a SID
    *  all specific behaviors for SID which are returning datas in rsp_buf have to be implemented here
    */
    switch (p_atp_ctxt->current_SID)
    {
    case SID_CS_DNS_REQ:
        /* PACK data to response buffer */
        if (DATAPACK_writeStruct(&p_rsp_buf[0], CMST_DNS_REQ, sizeof(bg96_qiurc_dnsgip_hostIPaddr), (void *)bg96_qiurc_dnsgip_hostIPaddr) != DATAPACK_OK)
        {
            retval = ATSTATUS_OK;
        }
        break;

     case SID_CS_POWER_ON:
     case SID_CS_RESET:
        display_user_friendly_mode_and_bands_config();
        break;
    }
    /* ###########################  END CUSTOMIZATION PART  ########################### */

    /* reset SID context */
    atcm_reset_SID_context(&BG96_ctxt.SID_ctxt);

    /* reset socket context */
    atcm_reset_SOCKET_context(&BG96_ctxt);

    return (retval);
}

at_status_t ATCustom_BG96_get_urc(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter ATCustom_BG96_get_urc()");

    /* prepare response for an URC - common part */
    retval = atcm_modem_get_urc(&BG96_ctxt, p_atp_ctxt, p_rsp_buf);

    /* ###########################  START CUSTOMIZATION PART  ########################### */
    /* prepare response for an URC
    *  all specific behaviors for an URC have to be implemented here
    */

     /* ###########################  END CUSTOMIZATION PART  ########################### */

    return (retval);
}

/* ###########################  START CUSTOMIZATION PART  ########################### */
/* Build command functions ------------------------------------------------------- */
static at_status_t fCmdBuild_ATD_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_ATD_BG96()");

    /* only for execution command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_EXECUTION_CMD)
    {
        CS_PDN_conf_id_t current_conf_id = atcm_get_cid_current_SID(p_modem_ctxt);
        uint8_t modem_cid = atcm_get_affected_modem_cid(&p_modem_ctxt->persist, current_conf_id);
        PrintINFO("Activate PDN (user cid = %d, modem cid = %d)", (uint8_t)current_conf_id, modem_cid);

        sprintf((char *)p_atp_ctxt->current_atcmd.params , "*99***%d#", modem_cid);

        /* sprintf((char *)p_atp_ctxt->current_atcmd.params , "*99#" ); */
    }
    return (retval);
}

static at_status_t fCmdBuild_CGSN_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    UNUSED(p_modem_ctxt);

    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_CGSN_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        /* BG96 only supports EXECUTION form of CGSN */
        retval = ATSTATUS_ERROR;
    }
    return (retval);
}

static at_status_t fCmdBuild_QPOWD_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    UNUSED(p_modem_ctxt);

    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QPOWD_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        /* Immediatly Power Down */
        /* sprintf((char *)p_atp_ctxt->current_atcmd.params , "0"); */
        /* Normal Power Down */
        sprintf((char *)p_atp_ctxt->current_atcmd.params , "1");
    }

    return (retval);
}

static at_status_t fCmdBuild_QCFG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    UNUSED(p_modem_ctxt);

    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QCFG_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        char cmd_param1[16] = "\0";
        char cmd_param2[16] = "\0";
        char cmd_param3[16] = "\0";
        char cmd_param4[16] = "\0";
        char cmd_param5[16] = "\0";
        uint8_t  cmd_nb_params = 0;

        /* TODO write parameters not implemented */
        if (BG96_QCFG_command_write == AT_TRUE)
        {
            /* BG96_AT_Commands_Manual_V2.0 */
            switch(BG96_QCFG_command_param)
            {
             case _QCFG_gprsattach:
                /* cmd_nb_params = 1; */
                /* TODO */
                break;
             case _QCFG_nwscanseq:
                cmd_nb_params = 2;
                /* param 1 = scanseq */
                sprintf((char *)&cmd_param1 , "0%lx", BG96_SCANSEQ); /* print as hexa but without prefix, need to add 1st digit = 0*/
                /* param 2 = effect */
                sprintf((char *)&cmd_param2 , "%d", 1); /* 1 means take effect immediately */
                break;
             case _QCFG_nwscanmode:
                cmd_nb_params = 2;
                /* param 1 = scanmode */
                sprintf((char *)&cmd_param1 , "%ld", BG96_SCANMODE);
                /* param 2 = effect */
                sprintf((char *)&cmd_param2 , "%d", 1); /* 1 means take effect immediately */
                break;
             case _QCFG_iotopmode:
                cmd_nb_params = 2;
                /* param 1 = iotopmode */
                sprintf((char *)&cmd_param1 , "%ld", BG96_IOTOPMODE);
                /* param 2 = effect */
                sprintf((char *)&cmd_param2 , "%d", 1); /* 1 means take effect immediately */
                break;
             case _QCFG_roamservice:
                /* cmd_nb_params = 2; */
                /* TODO */
                break;
             case _QCFG_band:
                cmd_nb_params = 4;
                /* param 1 = gsmbandval */
                sprintf((char *)&cmd_param1 , "%lx", BG96_BAND_GSM);
                /* param 2 = catm1bandval */
                sprintf((char *)&cmd_param2 , "%llx", BG96_BAND_CAT_M1);
                /* param 3 = catnb1bandval */
                sprintf((char *)&cmd_param3 , "%llx", BG96_BAND_CAT_NB1);
                /* param 4 = effect */
                sprintf((char *)&cmd_param4 , "%d", 1); /* 1 means take effect immediatly */
                break;
             case _QCFG_servicedomain:
                /* cmd_nb_params = 2; */
                /* TODO */
                break;
             case _QCFG_sgsn:
                /* cmd_nb_params = 1; */
                /* TODO */
                break;
             case _QCFG_msc:
                /* cmd_nb_params = 1; */
                /* TODO */
                break;
             case _QCFG_PDP_DuplicateChk:
                cmd_nb_params = 1;
                /* param 1 = enable */
                sprintf((char *)&cmd_param1 , "%d", BG96_PDP_DUPLICATECHK_ENABLE);
                break;
             case _QCFG_urc_ri_ring:
                /* cmd_nb_params = 5; */
                /* TODO */
                break;
             case _QCFG_urc_ri_smsincoming:
                /* cmd_nb_params = 2; */
                /* TODO */
                break;
             case _QCFG_urc_ri_other:
                /* cmd_nb_params = 2; */
                /* TODO */
                break;
             case _QCFG_signaltype:
                /* cmd_nb_params = 1; */
                /* TODO */
                break;
             case _QCFG_urc_delay:
                /* cmd_nb_params = 1; */
                /* TODO */
                break;
             default:
                break;
            }
        }

        if (cmd_nb_params == 5)
        {
            /* command has 5 parameters (this is a WRITE command) */
            sprintf((char *)p_atp_ctxt->current_atcmd.params , "\"%s\",%s,%s,%s,%s,%s", BG96_QCFG_LUT[BG96_QCFG_command_param], cmd_param1, cmd_param2, cmd_param3, cmd_param4, cmd_param5);
        }
        else if (cmd_nb_params == 4)
        {
            /* command has 4 parameters (this is a WRITE command) */
            sprintf((char *)p_atp_ctxt->current_atcmd.params , "\"%s\",%s,%s,%s,%s", BG96_QCFG_LUT[BG96_QCFG_command_param], cmd_param1, cmd_param2, cmd_param3, cmd_param4);
        }
        else if (cmd_nb_params == 3)
        {
            /* command has 3 parameters (this is a WRITE command) */
            sprintf((char *)p_atp_ctxt->current_atcmd.params , "\"%s\",%s,%s,%s", BG96_QCFG_LUT[BG96_QCFG_command_param], cmd_param1, cmd_param2, cmd_param3);
        }
        else if (cmd_nb_params == 2)
        {
            /* command has 2 parameters (this is a WRITE command) */
            sprintf((char *)p_atp_ctxt->current_atcmd.params , "\"%s\",%s,%s", BG96_QCFG_LUT[BG96_QCFG_command_param], cmd_param1, cmd_param2);
        }
        else if (cmd_nb_params == 1)
        {
            /* command has 1 parameters (this is a WRITE command) */
            sprintf((char *)p_atp_ctxt->current_atcmd.params , "\"%s\",%s", BG96_QCFG_LUT[BG96_QCFG_command_param], cmd_param1);
        }
        else
        {
            /* command has 0 parameters (this is a READ command) */
            sprintf((char *)p_atp_ctxt->current_atcmd.params , "\"%s\"", BG96_QCFG_LUT[BG96_QCFG_command_param]);
        }
    }


    return (retval);
}

static at_status_t fCmdBuild_QINDCFG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    UNUSED(p_modem_ctxt);

    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QINDCFG_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        switch (BG96_QINDCFG_command_param)
        {
         case _QINDCFG_csq:
            if (p_atp_ctxt->current_SID == SID_CS_SUSBCRIBE_NET_EVENT)
            {
                /* subscribe to CSQ URC event, do not save to nvram */
                sprintf((char *)p_atp_ctxt->current_atcmd.params , "\"csq\",1,0");
            }
            else
            {
                /* unsubscribe to CSQ URC event, do not save to nvram */
                sprintf((char *)p_atp_ctxt->current_atcmd.params , "\"csq\",0,0");
            }
            break;

         case _QINDCFG_all:
         case _QINDCFG_smsfull:
         case _QINDCFG_ring:
         case _QINDCFG_smsincoming:
         default:
            /* not implemented yet or error */
            break;
        }
    }

    return (retval);
}

static at_status_t fCmdBuild_CPSMS_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    UNUSED(p_modem_ctxt);

    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_CPSMS_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        /* cf BG96 AT commands manual V2.0 (paragraph 6.7)
        AT+CPSMS=[<mode>[,<Requested_Periodic-RAU>[,<Requested_GPRS-READY-timer>[,<Requested_Periodic-TAU>[
        ,<Requested_Active-Time>]]]]]

        mode: 0 disable PSM, 1 enable PSM, 2 disable PSM and discard all params or reset to manufacturer values
        Requested_Periodic-RAU:
        Requested_GPRS-READY-timer:
        Requested_Periodic-TAU:
        Requested_Active-Time:

        exple:
        AT+CPSMS=1,,,?00000100?,?00001111?
        Set the requested T3412 value to 40 minutes, and set the requested T3324 value to 30 seconds
        */
#if (BG96_ENABLE_PSM == 1)
        sprintf((char *)p_atp_ctxt->current_atcmd.params , "1,,,\"%s\",\"%s\"",BG96_CPSMS_REQ_PERIODIC_TAU, BG96_CPSMS_REQ_ACTIVE_TIME);
#else
        sprintf((char *)p_atp_ctxt->current_atcmd.params , "0");
#endif

    }

    return (retval);
}

static at_status_t fCmdBuild_CEDRXS_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    UNUSED(p_modem_ctxt);

    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_CEDRXS_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        /* cf BG96 AT commands manual V2.0 (paragraph 6.7)
        AT+CEDRXS=[<mode>,[,<AcT-type>[,<Requested_eDRX_value>]]]

        mode: 0 disable e-I-DRX, 1 enable e-I-DRX, 2 enable e-I-DRX and URC +CEDRXP
        AcT-type: 0 Act not using e-I-DRX, 1 LTE cat.M1, 2 GSM, 3 UTRAN, 4 LTE, 5 LTE cat.NB1
        Requested_eDRX_value>:

        exple:
        AT+CEDRX=1,5,?0000?
        Set the requested e-I-DRX value to 5.12 second
        */
#if (BG96_ENABLE_E_I_DRX == 1)
        sprintf((char *)p_atp_ctxt->current_atcmd.params , "1,%d,\"0000\"", BG96_CEDRXS_ACT_TYPE);
#else
        sprintf((char *)p_atp_ctxt->current_atcmd.params , "0");
#endif
    }

    return (retval);
}

static at_status_t fCmdBuild_QENG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    UNUSED(p_modem_ctxt);

    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QENG_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        /* cf addendum for descirption of QENG AT command
         * AT+QENG=<celltype>
        */

        switch (BG96_QENG_command_param)
        {

         case _QENGcelltype_ServingCell:
            sprintf((char *)p_atp_ctxt->current_atcmd.params , "\"servingcell\"");
            break;

         case _QENGcelltype_NeighbourCell:
            sprintf((char *)p_atp_ctxt->current_atcmd.params , "\"neighbourcell\"");
            break;

         case _QENGcelltype_PSinfo:
            sprintf((char *)p_atp_ctxt->current_atcmd.params , "\"psinfo\"");
            break;

         default:
            /* not implemented or supported */
            retval = ATSTATUS_ERROR;
            break;
        }
    }

    return (retval);
}

static at_status_t fCmdBuild_QICSGP_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QICSGP_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        /* cf BG96 TCP/IP AT commands manual v1.0
        *  AT+QICSGP: Configure parameters of a TCP/IP context
        *  AT+QICSGP=<cid>[,<context_type>,<APN>[,<username>,<password>)[,<authentication>]]]
        *  - cid: context id (rang 1 to 16)
        *  - context_type: 1 for IPV4, 2 for IPV6
        *  - APN: string for access point name
        *  - username: string
        *  - password: string
        *  - authentication: 0 for NONE, 1 for PAP, 2 for CHAP, 3 for PAP or CHAP
        *
        * example: AT+QICSGP=1,1,"UNINET","","",1
        */

        if (bg96_QICGSP_config_command == AT_TRUE)
        {
            /* Write command is a config command */
            CS_PDN_conf_id_t current_conf_id = atcm_get_cid_current_SID(p_modem_ctxt);
            uint8_t modem_cid = atcm_get_affected_modem_cid(&p_modem_ctxt->persist, current_conf_id);
            PrintINFO("user cid = %d, modem cid = %d", (uint8_t)current_conf_id, modem_cid);

            uint8_t context_type_value;
            uint8_t authentication_value;

            /* convert context type to numeric value */
            switch(p_modem_ctxt->persist.pdp_ctxt_infos[current_conf_id].pdn_conf.pdp_type)
            { case CS_PDPTYPE_IP:
               context_type_value = 1;
               break;
             case CS_PDPTYPE_IPV6:
             case CS_PDPTYPE_IPV4V6:
                context_type_value = 2;
                break;

             default :
                context_type_value = 1;
                break;
            }

            /* TODO
            * authentication : 0,1,2 or 3 (see above)
            */
            authentication_value = 0;

            /* build command */
            sprintf((char *)p_atp_ctxt->current_atcmd.params, "%d,%d,\"%s\",\"%s\",\"%s\",%d",
                    modem_cid,
                    context_type_value,
                    p_modem_ctxt->persist.pdp_ctxt_infos[current_conf_id].apn,
                    p_modem_ctxt->persist.pdp_ctxt_infos[current_conf_id].pdn_conf.username,
                    p_modem_ctxt->persist.pdp_ctxt_infos[current_conf_id].pdn_conf.password,
                    authentication_value
                        );
        }
        else
        {
            /* Write command is a query command */
            CS_PDN_conf_id_t current_conf_id = atcm_get_cid_current_SID(p_modem_ctxt);
            uint8_t modem_cid = atcm_get_affected_modem_cid(&p_modem_ctxt->persist, current_conf_id);
            PrintINFO("user cid = %d, modem cid = %d", (uint8_t)current_conf_id, modem_cid);
            sprintf((char *)p_atp_ctxt->current_atcmd.params, "%d", modem_cid);
        }

    }

    return (retval);
}

static at_status_t fCmdBuild_QIACT_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QIACT_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        CS_PDN_conf_id_t current_conf_id = atcm_get_cid_current_SID(p_modem_ctxt);
        uint8_t modem_cid = atcm_get_affected_modem_cid(&p_modem_ctxt->persist, current_conf_id);
        PrintINFO("user cid = %d, modem cid = %d", (uint8_t)current_conf_id, modem_cid);
        /* check if this PDP context has been defined */
        if  (p_modem_ctxt->persist.pdp_ctxt_infos[current_conf_id].conf_id == CS_PDN_NOT_DEFINED)
        {
            PrintINFO("PDP context not explicitly defined for conf_id %d (using modem params)", current_conf_id);
        }

        sprintf((char *)p_atp_ctxt->current_atcmd.params , "%d",  modem_cid);
    }

    return (retval);
}

static at_status_t fCmdBuild_QICFG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    UNUSED(p_modem_ctxt);

    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QICFG_BG96()");

    /* TODO */

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        retval = ATSTATUS_ERROR;
    }

    return (retval);
}

static at_status_t fCmdBuild_QIOPEN_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QIOPEN_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        if (p_modem_ctxt->socket_ctxt.socket_info != NULL)
        {
            /* IP AT Commands manual - LTE Module Series - V1.0
            * AT+QIOPEN=<contextID>,<connectId>,<service_type>,<IP_address>/<domain_name>,<remote_port>[,
            *           <local_port>[,<access_mode>]]
            *
            * <contextID> is the PDP context ID
            */
            /* convert user cid (CS_PDN_conf_id_t) to PDP modem cid (value) */
            uint8_t pdp_modem_cid = atcm_get_affected_modem_cid(&p_modem_ctxt->persist, p_modem_ctxt->socket_ctxt.socket_info->conf_id);
            PrintDBG("user cid = %d, PDP modem cid = %d", (uint8_t)p_modem_ctxt->socket_ctxt.socket_info->conf_id, pdp_modem_cid);
            uint8_t _access_mode = 0; /* 0=buffer acces mode, 1=direct push mode, 2=transparent access mode */

            if (p_atp_ctxt->current_SID == SID_CS_DIAL_COMMAND)
            {
                /* client mode: "TCP" of "UDP" */
                uint8_t _service_type_index = ((p_modem_ctxt->socket_ctxt.socket_info->protocol == CS_TCP_PROTOCOL)? _QIOPENservicetype_TCP_Client : _QIOPENservicetype_UDP_Client);

                sprintf((char *)p_atp_ctxt->current_atcmd.params , "%d,%ld,\"%s\",\"%s\",%d,%d,%d",
                        pdp_modem_cid,
                        atcm_socket_get_modem_cid(p_modem_ctxt, p_modem_ctxt->socket_ctxt.socket_info->socket_handle),
                        bg96_array_QIOPEN_service_type[_service_type_index],
                        p_modem_ctxt->socket_ctxt.socket_info->ip_addr_value,
                        p_modem_ctxt->socket_ctxt.socket_info->remote_port,
                        p_modem_ctxt->socket_ctxt.socket_info->local_port,
                        _access_mode );

                /* waiting for +QIOPEN now */
                bg96_waiting_qiopen = AT_TRUE;
            }
            /*            TODO: QIOPEN for server mode */
#if 0
            else if (curSID == "???") for server mode (corresponding to CDS_socket_listen)
            {
                /* server mode: "TCP LISTENER" of "UDP LISTENER" */
                uint8_t _service_type_index = ((p_modem_ctxt->socket_ctxt.socket_info->protocol == CS_TCP_PROTOCOL)? _QIOPENservicetype_TCP_Server : _QIOPENservicetype_UDP_Server);

                sprintf((char *)p_atp_ctxt->current_atcmd.params , "%d,%d,\"%s\",\"%s\",%d,%d,%d",
                        modem_cid,
                        atcm_socket_get_modem_cid(p_modem_ctxt, p_modem_ctxt->socket_ctxt.socket_info->socket_handle),
                        bg96_array_QIOPEN_service_type[_service_type_index],
                        "127.0.0.1",
                        p_modem_ctxt->socket_ctxt.socket_info->remote_port, /* set value but not valid - cf doc */
                        p_modem_ctxt->socket_ctxt.socket_info->local_port,
                        _access_mode );
            }
#endif
            else
            {
                /* error */
                retval = ATSTATUS_ERROR;
            }
        }
        else
        {
            retval = ATSTATUS_ERROR;
        }
    }

    return (retval);
}

static at_status_t fCmdBuild_QICLOSE_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QICLOSE_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        if (p_modem_ctxt->socket_ctxt.socket_info != NULL)
        {
            /* IP AT Commands manual - LTE Module Series - V1.0
            * AT+QICLOSE=connectId>[,<timeout>]
            */
            uint32_t connID = atcm_socket_get_modem_cid(p_modem_ctxt, p_modem_ctxt->socket_ctxt.socket_info->socket_handle);
            sprintf((char *)p_atp_ctxt->current_atcmd.params , "%ld", connID );
        }
        else
        {
            retval = ATSTATUS_ERROR;
        }
    }
    return (retval);
}

static at_status_t fCmdBuild_QISEND_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QISEND_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        /*IP AT Commands manual - LTE Module Series - V1.0
        * send data in command mode:
        * AT+QISEND=<connId>,<send_length><CR>
        * > ...DATA...
        *
        * DATA are sent using fCmdBuild_QISEND_WRITE_DATA_BG96()
        */
        sprintf((char *)p_atp_ctxt->current_atcmd.params, "%ld,%ld",
                atcm_socket_get_modem_cid(p_modem_ctxt, p_modem_ctxt->SID_ctxt.socketSendData_struct.socket_handle),
                p_modem_ctxt->SID_ctxt.socketSendData_struct.buffer_size
                );
    }

    return (retval);
}

static at_status_t fCmdBuild_QISEND_WRITE_DATA_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QISEND_WRITE_DATA_BG96()");

    /* after having send AT+QISEND and prompt received, now send DATA */

    /* only for raw command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_RAW_CMD)
    {
        uint32_t str_size = p_modem_ctxt->SID_ctxt.socketSendData_struct.buffer_size;
        memcpy((void *)p_atp_ctxt->current_atcmd.params,
               (void *)p_modem_ctxt->SID_ctxt.socketSendData_struct.p_buffer_addr,
               str_size);

        /* set raw command size */
        p_atp_ctxt->current_atcmd.raw_cmd_size = str_size;
    }

    return (retval);
}

static at_status_t fCmdBuild_QIRD_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QIRD_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        if (BG96_ctxt.socket_ctxt.socket_receive_state == SocketRcvState_RequestSize)
        {
            /* requesting socket data size (set length = 0) */
            sprintf((char *)p_atp_ctxt->current_atcmd.params, "%ld,0",
                    atcm_socket_get_modem_cid(p_modem_ctxt, p_modem_ctxt->SID_ctxt.socketSendData_struct.socket_handle) );
        }
        else if (BG96_ctxt.socket_ctxt.socket_receive_state == SocketRcvState_RequestData_Header)
        {
            uint32_t requested_data_size = 0;
#if defined(USE_C2C_CS_ADAPT)
            /* read only the requested size */
            requested_data_size = p_modem_ctxt->socket_ctxt.socketReceivedata.max_buffer_size;
#else
            requested_data_size = p_modem_ctxt->socket_ctxt.socket_rx_expected_buf_size;
#endif /* USE_C2C_CS_ADAPT */

            /* requesting socket data with correct size */
            sprintf((char *)p_atp_ctxt->current_atcmd.params, "%ld,%ld",
                    atcm_socket_get_modem_cid(p_modem_ctxt, p_modem_ctxt->SID_ctxt.socketSendData_struct.socket_handle),
                    requested_data_size );

            /* ready to start receive socket buffer */
            p_modem_ctxt->socket_ctxt.socket_RxData_state = SocketRxDataState_waiting_header;
        }
        else
        {
            PrintErr("Unexpected socket receiving state");
            retval = ATSTATUS_ERROR;
        }
    }

    return (retval);
}

static at_status_t fCmdBuild_QISTATE_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QISTATE_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        /* AT+QISTATE=<query_type>,<connectID>
        *
        * <query_type> = 1
        */

        sprintf((char *)p_atp_ctxt->current_atcmd.params, "1,%ld",
                atcm_socket_get_modem_cid(p_modem_ctxt, p_modem_ctxt->SID_ctxt.socketSendData_struct.socket_handle) );
    }

    return (retval);
}

static at_status_t fCmdBuild_CGDCONT_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_CGDCONT_BG96()");

    /* normal case */
    retval = fCmdBuild_CGDCONT(p_atp_ctxt, p_modem_ctxt);

    return (retval);
}


static at_status_t fCmdBuild_QIDNSCFG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QIDNSCFG_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        /* cf TCP/IP AT commands manual v1.0
        *  1- Configure DNS server address for specified PDP context
        *  *  AT+QIDNSCFG=<contextID>,<pridnsaddr>[,<secdnsaddr>]
        *
        *  2- Query DNS server address of specified PDP context
        *  AT+QIDNSCFG=<contextID>
        *  response:
        *  +QIDNSCFG: <contextID>,<pridnsaddr>,<secdnsaddr>
        *
        */
        CS_PDN_conf_id_t current_conf_id = atcm_get_cid_current_SID(p_modem_ctxt);
        uint8_t modem_cid = atcm_get_affected_modem_cid(&p_modem_ctxt->persist, current_conf_id);
        /* configure DNS server address for the specfied PDP context */
        sprintf((char *)p_atp_ctxt->current_atcmd.params, "%d,\"%s\"",
                modem_cid,
                p_modem_ctxt->SID_ctxt.dns_req->dns_request.primary_dns_addr);
    }

    return (retval);
}

static at_status_t fCmdBuild_QIDNSGIP_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt)
{
    at_status_t retval = ATSTATUS_OK;
    PrintAPI("enter fCmdBuild_QIDNSGIP_BG96()");

    /* only for write command, set parameters */
    if (p_atp_ctxt->current_atcmd.type == ATTYPE_WRITE_CMD)
    {
        /* cf TCP/IP AT commands manual v1.0
        * AT+QIDNSGIP=<contextID>,<hostname>
        */
        CS_PDN_conf_id_t current_conf_id = atcm_get_cid_current_SID(p_modem_ctxt);
        uint8_t modem_cid = atcm_get_affected_modem_cid(&p_modem_ctxt->persist, current_conf_id);
        sprintf((char *)p_atp_ctxt->current_atcmd.params, "%d,\"%s\"",
                modem_cid,
                p_modem_ctxt->SID_ctxt.dns_req->dns_request.host_name);
    }
    return (retval);
}

/* Analyze command functions ------------------------------------------------------- */
static at_action_rsp_t fRspAnalyze_CPIN_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                          IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
    at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PrintAPI("enter fRspAnalyze_CPIN_BG96()");

    /*  Quectel BG96 AT Commands Manual V1.0
    *   analyze parameters for +CPIN
    *
    *   if +CPIN is not received after AT+CPIN request, it's an URC
    */

    if (p_atp_ctxt->current_atcmd.id == _AT_CPIN)
    {
        retval = fRspAnalyze_CPIN(p_atp_ctxt, p_modem_ctxt, p_msg_in, element_infos);
    }
    else
    {
        /* this is an URC */
        if (element_infos->param_rank == 2)
        {
        PrintDBG("URC +CPIN received");
        PrintBuf((const void *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
        }
    }

    return (retval);
}

static at_action_rsp_t fRspAnalyze_CFUN_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                          IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
    UNUSED(p_atp_ctxt);
    UNUSED(p_modem_ctxt);

    at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PrintAPI("enter fRspAnalyze_CFUN_BG96()");

    /*  Quectel BG96 AT Commands Manual V1.0
    *   analyze parameters for +CFUN
    *
    *   if +CFUN is received, it's an URC
    */

    /* this is an URC */
    if (element_infos->param_rank == 2)
    {
        PrintDBG("URC +CFUN received");
        PrintBuf((const void *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
    }

    return (retval);
}

static at_action_rsp_t fRspAnalyze_QIND_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
    UNUSED(p_atp_ctxt);

    at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PrintAPI("enter fRspAnalyze_QIND_BG96()");

    /*  Quectel BG96 AT Commands Manual V1.0
    *   analyze parameters for +QIND
    *
    *   it's an URC
    */

    /* this is an URC */
    if (element_infos->param_rank == 2)
    {
        char line[32] = {0};

        /* init param received info */
        bg96_current_qind_is_csq = AT_FALSE;

        PrintDBG("URC +QIND received");
        PrintBuf((const void *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);

        /* copy element to line for parsing */
        if (element_infos->str_size <= 32)
        {
            memcpy((void*)&line[0], (void*)&(p_msg_in->buffer[element_infos->str_start_idx]), element_infos->str_size);
        }
        else
        {
            PrintErr("line exceed maximum size, line ignored...");
            return(ATACTION_RSP_IGNORED);
        }

        /* extract value and compare it to expected value */
        if((char *) strstr((const char *)&line[0],"csq") != NULL)
        {
            PrintDBG("SIGNAL QUALITY INFORMATION");
            bg96_current_qind_is_csq = AT_TRUE;
        }
        else
        {
           retval = ATACTION_RSP_URC_IGNORED;
           PrintDBG("QIND info not managed: urc ignored");
        }
    }
    else if (element_infos->param_rank == 3)
    {
        if (bg96_current_qind_is_csq == AT_TRUE)
        {
            uint32_t rssi = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintDBG("+CSQ rssi=%d", rssi);
            p_modem_ctxt->persist.urc_avail_signal_quality = AT_TRUE;
            p_modem_ctxt->persist.signal_quality.rssi = rssi;
            p_modem_ctxt->persist.signal_quality.ber = 99; /* in case ber param is not present */
        }
    }
    else if (element_infos->param_rank == 4)
    {
        if (bg96_current_qind_is_csq == AT_TRUE)
        {
            uint32_t ber = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintDBG("+CSQ ber=%d", ber);
            p_modem_ctxt->persist.signal_quality.ber = ber;
        }
    }

    return (retval);
}


static at_action_rsp_t fRspAnalyze_QIACT_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)

{
    at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PrintAPI("enter fRspAnalyze_QIACT_BG96()");

    if (p_atp_ctxt->current_atcmd.type == ATTYPE_READ_CMD)
    {
        /* Answer to AT+QIACT?
        *  Returns the list of current activated contexts and their IP addresses
        *  format: +QIACT: <cid>,<context_state>,<context_type>,[,<IP_address>]
        *
        *  where:
        *  <cid>: context ID, range is 1 to 16
        *  <context_state>: 0 for deactivated, 1 for activated
        *  <context_type>: 1 for IPV4, 2 for IPV6
        *  <IP_address>: string type
        */

        if (element_infos->param_rank == 2)
        {
            /* analyze <cid> */
            uint32_t modem_cid = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintDBG("+QIACT cid=%d", modem_cid);
            p_modem_ctxt->CMD_ctxt.modem_cid = modem_cid;
        }
        else if (element_infos->param_rank == 3)
        {
            /* analyze <context_state> */
            uint32_t context_state = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintDBG("+QIACT context_state=%d", context_state);

            /* If we are trying to activate a PDN and we see that it is already active, do not request the activation to avoid an error */
            if ((p_atp_ctxt->current_SID == SID_CS_ACTIVATE_PDN) && (context_state == 1))
            {
                /* is it the required PDN to activate ? */
                CS_PDN_conf_id_t current_conf_id = atcm_get_cid_current_SID(p_modem_ctxt);
                uint8_t modem_cid = atcm_get_affected_modem_cid(&p_modem_ctxt->persist, current_conf_id);
                if (p_modem_ctxt->CMD_ctxt.modem_cid == modem_cid)
                {
                    PrintDBG("Modem CID to activate (%d) is already activated", modem_cid);
#if (USE_SOCKETS_MODEM == 1)
                    bg96_pdn_already_active = AT_TRUE;
#endif /* USE_SOCKETS_MODEM */
                }
            }
        }
        else if (element_infos->param_rank == 4)
        {
            /* analyze <context_type> */
            uint32_t context_type = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintDBG("+QIACT context_type=%d", context_type);
            /* Avoid Compiler Warning */
            IGNORE(context_type);
        }
        else if (element_infos->param_rank == 5)
        {
            /* analyze <IP_address> */
            csint_ip_addr_info_t  ip_addr_info;
            memset((void *)&ip_addr_info, 0, sizeof(csint_ip_addr_info_t));

            /* retrive IP address value */
            memcpy((void *)&(ip_addr_info.ip_addr_value),
                   (const void *)&p_msg_in->buffer[element_infos->str_start_idx],
                   element_infos->str_size);
            PrintDBG("+QIACT addr=%s", (char *)&ip_addr_info.ip_addr_value);

            /* determine IP address type (IPv4 or IPv6): count number of '.' */
            char* pTmp = (char *)&ip_addr_info.ip_addr_value;
            uint8_t count_dots = 0;
            while(pTmp != NULL)
            {
                pTmp = strchr(pTmp, '.');
                if(pTmp)
                {
                    pTmp++;
                    count_dots++;
                }
            }
            PrintDBG("+QIACT nbr of points in address %d", count_dots);
            if (count_dots == 3)
            {
                ip_addr_info.ip_addr_type = CS_IPAT_IPV4;
            }
            else
            {
                ip_addr_info.ip_addr_type = CS_IPAT_IPV6;
            }

            /* save IP address infos in modem_cid_table */
            atcm_put_IP_address_infos(&p_modem_ctxt->persist, (uint8_t)p_modem_ctxt->CMD_ctxt.modem_cid, &ip_addr_info);
        }
    }

    return (retval);
}

static at_action_rsp_t fRspAnalyze_QCFG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
    UNUSED(p_atp_ctxt);
    UNUSED(p_modem_ctxt);

    at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PrintAPI("enter fRspAnalyze_QCFG_BG96()");

    if (element_infos->param_rank == 2)
    {
        char line[32] = {0};

        /* init param received info */
        bg96_current_qcfg_cmd = _QCFG_unknown;

        PrintBuf((const void *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);

        /* copy element to line for parsing */
        if (element_infos->str_size <= 32)
        {
            memcpy((void*)&line[0], (void*)&(p_msg_in->buffer[element_infos->str_start_idx]), element_infos->str_size);
        }
        else
        {
            PrintErr("line exceed maximum size, line ignored...");
            return(ATACTION_RSP_IGNORED);
        }

        /* extract value and compare it to expected value */
        if((char *) strstr((const char *)&line[0],"nwscanseq") != NULL)
        {
            PrintDBG("+QCFG nwscanseq infos received");
            bg96_current_qcfg_cmd = _QCFG_nwscanseq;
        }
        else if((char *) strstr((const char *)&line[0],"nwscanmode") != NULL)
        {
            PrintDBG("+QCFG nwscanmode infos received");
            bg96_current_qcfg_cmd = _QCFG_nwscanmode;
        }
        else if((char *) strstr((const char *)&line[0],"iotopmode") != NULL)
        {
            PrintDBG("+QCFG iotopmode infos received");
            bg96_current_qcfg_cmd = _QCFG_iotopmode;
        }
        else if((char *) strstr((const char *)&line[0],"band") != NULL)
        {
            PrintDBG("+QCFG band infos received");
            bg96_current_qcfg_cmd = _QCFG_band;
        }
        else
        {
           PrintErr("+QCFDG field not managed");
        }
    }
    else if (element_infos->param_rank == 3)
    {
        switch (bg96_current_qcfg_cmd)
        {
         case _QCFG_nwscanseq:
            bg96_mode_and_bands_config.nw_scanseq = ATutil_convertHexaStringToInt64(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            /* display_decoded_nwscanseq(bg96_mode_and_bands_config.nw_scanseq); */
            break;
         case _QCFG_nwscanmode:
            bg96_mode_and_bands_config.nw_scanmode = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            /* display_decoded_nwscanmode(bg96_mode_and_bands_config.nw_scanmode); */
            break;
         case _QCFG_iotopmode:
            bg96_mode_and_bands_config.iot_op_mode = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            /* display_decoded_iotopmode(bg96_mode_and_bands_config.iot_op_mode); */
            break;
         case _QCFG_band:
            bg96_mode_and_bands_config.gsm_bands = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            /* display_decoded_GSM_bands(bg96_mode_and_bands_config.gsm_bands); */
            break;
         default:
        	 /* No Matching Case */
        	break;
        }
    }
    else if (element_infos->param_rank == 4)
    {
        switch (bg96_current_qcfg_cmd)
        {
         case _QCFG_band:
            bg96_mode_and_bands_config.CatM1_bands = ATutil_convertStringToInt64(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            /* display_decoded_CatM1_bands(bg96_mode_and_bands_config.CatM1_bands); */
            break;
         default:
        	/* No Matching Case */
        	break;
        }
    }
    else if (element_infos->param_rank == 5)
    {
        switch (bg96_current_qcfg_cmd)
        {
         case _QCFG_band:
            bg96_mode_and_bands_config.CatNB1_bands = ATutil_convertStringToInt64(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            /* display_decoded_CatNB1_bands(bg96_mode_and_bands_config.CatNB1_bands); */
            break;
         default:
        	 /* No Matching Case */
        	break;
        }
    }

    return (retval);
}

static at_action_rsp_t fRspAnalyze_QINDCFG_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
    UNUSED(p_atp_ctxt);
    UNUSED(p_modem_ctxt);
    UNUSED(p_msg_in);
    UNUSED(element_infos);

    at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PrintAPI("enter fRspAnalyze_QINDCFG_BG96()");

    /* not implemented yet */

    return (retval);
}

static at_action_rsp_t fRspAnalyze_QIOPEN_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
    UNUSED(p_atp_ctxt);

    at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PrintAPI("enter fRspAnalyze_QIOPEN_BG96()");

    /* are we waiting for QIOPEN ? */
    if ( bg96_waiting_qiopen == AT_TRUE)
    {
        if (element_infos->param_rank == 2)
        {
            uint32_t connectID;
            connectID = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            bg96_current_qiopen_connectId = connectID;
            bg96_current_qiopen_socket_connected = 0;
        }
        else if (element_infos->param_rank == 3)
        {
            uint32_t err_value;
            err_value = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);

            /* compare QIOPEN connectID with the value requested by user (ie in current SID)
            *  and check if err=0
            */
            if ((bg96_current_qiopen_connectId == atcm_socket_get_modem_cid(p_modem_ctxt, p_modem_ctxt->socket_ctxt.socket_info->socket_handle)) &&
                (err_value == 0))
            {
                PrintINFO("socket (connectId=%d) opened", bg96_current_qiopen_connectId);
                bg96_current_qiopen_socket_connected = 1;
                bg96_waiting_qiopen = AT_FALSE;
                retval = ATACTION_RSP_FRC_END;
            }
            else
            {
                if (err_value != 0)
                {
                    PrintErr("+QIOPEN returned error #%d", err_value);
                }
                else
                {
                    PrintErr("+QIOPEN problem");
                }
                retval = ATACTION_RSP_ERROR;
            }
        }
    }
    else
    {
        PrintINFO("+QIOPEN not expected, ignore it");
    }
    return (retval);
}

static at_action_rsp_t fRspAnalyze_QIURC_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
    UNUSED(p_atp_ctxt);

    at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PrintAPI("enter fRspAnalyze_QIURC_BG96()");

    /*IP AT Commands manual - LTE Module Series - V1.0
    * URC
    * +QIURC:"closed",<connectID> : URC of connection closed
    * +QIURC:"recv",<connectID> : URC of incoming data
    * +QIURC:"incoming full" : URC of incoming connection full
    * +QIURC:"incoming",<connectID> ,<serverID>,<remoteIP>,<remote_port> : URC of incoming connection
    * +QIURC:"pdpdeact",<contextID> : URC of PDP deactivation
    *
    * for DNS request:
    * header: +QIURC: "dnsgip",<err>,<IP_count>,<DNS_ttl>
    * infos:  +QIURC: "dnsgip",<hostIPaddr>]
    */

    /* this is an URC */
    if (element_infos->param_rank == 2)
    {
        char line[32] = {0};

        /* init param received info */
        bg96_current_qiurc_ind = _QIURC_unknown;

        PrintBuf((const void *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);

        /* copy element to line for parsing */
        if (element_infos->str_size <= 32)
        {
            memcpy((void*)&line[0], (void*)&(p_msg_in->buffer[element_infos->str_start_idx]), element_infos->str_size);
        }
        else
        {
            PrintErr("line exceed maximum size, line ignored...");
            return(ATACTION_RSP_IGNORED);
        }

        /* extract value and compare it to expected value */
        if((char *) strstr((const char *)&line[0],"closed") != NULL)
        {
            PrintDBG("+QIURC closed info received");
            bg96_current_qiurc_ind = _QIURC_closed;
        }
        else if((char *) strstr((const char *)&line[0],"recv") != NULL)
        {
            PrintDBG("+QIURC recv info received");
            bg96_current_qiurc_ind = _QIURC_recv;
        }
        else if((char *) strstr((const char *)&line[0],"incoming full") != NULL)
        {
            PrintDBG("+QIURC incoming full info received");
            bg96_current_qiurc_ind = _QIURC_incoming_full;
        }
        else if((char *) strstr((const char *)&line[0],"incoming") != NULL)
        {
            PrintINFO("+QIURC incoming info received");
            bg96_current_qiurc_ind = _QIURC_incoming;
        }
        else if((char *) strstr((const char *)&line[0],"pdpdeact") != NULL)
        {
            PrintDBG("+QIURC pdpdeact info received");
            bg96_current_qiurc_ind = _QIURC_pdpdeact;
        }
        else if((char *) strstr((const char *)&line[0],"dnsgip") != NULL)
        {
            PrintDBG("+QIURC dnsgip info received");
            bg96_current_qiurc_ind = _QIURC_dnsgip;
            if (p_atp_ctxt->current_SID != SID_CS_DNS_REQ)
            {
                /* URC not expected */
                retval = ATACTION_RSP_URC_IGNORED;
            }
        }
        else
        {
            PrintErr("+QIURC field not managed");
        }
    }
    else if (element_infos->param_rank == 3)
    {
        uint32_t connectID, contextID;
        socket_handle_t sockHandle;

        switch (bg96_current_qiurc_ind)
        {
         case _QIURC_recv:
            /* <connectID> */
            connectID = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            sockHandle = atcm_socket_get_socket_handle(p_modem_ctxt, connectID);
            atcm_socket_set_urc_data_pending(p_modem_ctxt, sockHandle);
            PrintDBG("+QIURC received data for connId=%d (socket handle=%d)", connectID, sockHandle);
            /* last param */
            retval = ATACTION_RSP_URC_FORWARDED;
            break;

         case _QIURC_closed:
            /* <connectID> */
            connectID = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            sockHandle = atcm_socket_get_socket_handle(p_modem_ctxt, connectID);
            atcm_socket_set_urc_closed_by_remote(p_modem_ctxt, sockHandle);
            PrintDBG("+QIURC closed for connId=%d (socket handle=%d)", connectID, sockHandle);
            /* last param */
            retval = ATACTION_RSP_URC_FORWARDED;
            break;

         case _QIURC_incoming:
            /* <connectID> */
            connectID = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintDBG("+QIURC incoming for connId=%d", connectID);
            break;

         case _QIURC_pdpdeact:
            /* <contextID> */
            contextID = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintDBG("+QIURC pdpdeact for contextID=%d", contextID);
            /* Avoid Compiler Warning */
            IGNORE(contextID);
            /* last param */
            retval = ATACTION_RSP_URC_FORWARDED;
            break;

         case _QIURC_dnsgip:
            /* <err> or <hostIPaddr>]> */
            if (bg96_qiurc_dnsgip_wait_header == AT_TRUE)
            {
                /* <err> expected */
                bg96_qiurc_dnsgip_error = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
                PrintDBG("+QIURC dnsgip with error=%d", bg96_qiurc_dnsgip_error);
                if (bg96_qiurc_dnsgip_error != 0)
                {
                    /* Error when trying to get host address */
                    bg96_qiurc_dnsgip_finished = AT_TRUE;
                    retval = ATACTION_RSP_ERROR;
                }
            }
            else
            {
                /* <hostIPaddr> expected
                *  with the current implementation, in case of many possible host IP address, we use
                *  the last one received
                */
                memcpy((void*)bg96_qiurc_dnsgip_hostIPaddr, (void*)&(p_msg_in->buffer[element_infos->str_start_idx]), element_infos->str_size);
                PrintDBG("+QIURC dnsgip Host address #%d =%s", bg96_qiurc_dnsgip_ip_count, bg96_qiurc_dnsgip_hostIPaddr);
                bg96_qiurc_dnsgip_ip_count--;
                if (bg96_qiurc_dnsgip_ip_count == 0)
                {
                    /* all expected URC have been reecived */
                    bg96_qiurc_dnsgip_finished = AT_TRUE;
                    /* last param */
                    retval = ATACTION_RSP_FRC_END;
                }
                else
                {
                    retval = ATACTION_RSP_IGNORED;
                }
            }
            break;

         case _QIURC_incoming_full:
         default:
            /* no parameter expected */
            PrintErr("parameter not expected for this URC message");
            break;
        }
    }
    else if (element_infos->param_rank == 4)
    {
        uint32_t serverID;

        switch (bg96_current_qiurc_ind)
        {
         case _QIURC_incoming:
            /* <serverID> */
            serverID = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintDBG("+QIURC incoming for serverID=%d", serverID);
            /* Avoid Compiler Warning */
            IGNORE(serverID);
            break;

         case _QIURC_dnsgip:
            /* <IP_count> */
            if (bg96_qiurc_dnsgip_wait_header == AT_TRUE)
            {
                /* <bg96_qiurc_dnsgip_ip_count> expected */
                bg96_qiurc_dnsgip_ip_count = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
                PrintDBG("+QIURC dnsgip IP count=%d", bg96_qiurc_dnsgip_ip_count);
                if (bg96_qiurc_dnsgip_ip_count == 0)
                {
                    /* No host address available */
                    bg96_qiurc_dnsgip_finished = AT_TRUE;
                    retval = ATACTION_RSP_ERROR;
                }
            }
            break;

         case _QIURC_recv:
         case _QIURC_closed:
         case _QIURC_pdpdeact:
         case _QIURC_incoming_full:
         default:
            /* no parameter expected */
            PrintErr("parameter not expected for this URC message");
            break;
        }
    }
    else if (element_infos->param_rank == 5)
    {
        char remoteIP[32] = {0};

        switch (bg96_current_qiurc_ind)
        {
         case _QIURC_incoming:
            /* <remoteIP> */
            memcpy((void*)&remoteIP[0], (void*)&(p_msg_in->buffer[element_infos->str_start_idx]), element_infos->str_size);
            PrintDBG("+QIURC remoteIP for remoteIP=%s", remoteIP);
            break;

         case _QIURC_dnsgip:
            /* <DNS_ttl> */
            if (bg96_qiurc_dnsgip_wait_header == AT_TRUE)
            {
                /* <bg96_qiurc_dnsgip_ttl> expected */
                bg96_qiurc_dnsgip_ttl = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
                PrintDBG("+QIURC dnsgip time-to-live=%d", bg96_qiurc_dnsgip_ttl);
                /* no error, now waiting for URC with IP address */
                bg96_qiurc_dnsgip_wait_header = AT_FALSE;
            }
            break;

         case _QIURC_recv:
         case _QIURC_closed:
         case _QIURC_pdpdeact:
         case _QIURC_incoming_full:
         default:
            /* no parameter expected */
            PrintErr("parameter not expected for this URC message");
            break;
        }
    }
    else if (element_infos->param_rank == 6)
    {
        uint32_t remote_port;

        switch (bg96_current_qiurc_ind)
        {
         case _QIURC_incoming:
            /* <remote_port> */
            remote_port = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintDBG("+QIURC incoming for remote_port=%d", remote_port);
            /* Avoid Compiler Warning */
            IGNORE(remote_port);
            /* last param */
            retval = ATACTION_RSP_URC_FORWARDED;
            break;

         case _QIURC_recv:
         case _QIURC_closed:
         case _QIURC_pdpdeact:
         case _QIURC_incoming_full:
         case _QIURC_dnsgip:
         default:
            /* no parameter expected */
            PrintErr("parameter not expected for this URC message");
            break;
        }
    }
    else
    {
        PrintErr("parameter not expected for this URC message");
    }

    return (retval);
}

static at_action_rsp_t fRspAnalyze_QIRD_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
    UNUSED(p_atp_ctxt);

    at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PrintAPI("enter fRspAnalyze_QIRD_BG96()");

    /*IP AT Commands manual - LTE Module Series - V1.0
    *
    * Received after having send AT+QIRD=<connectID>[,<read_length>]
    *
    * if <read_length> was present and equal to 0, it was a request to get status:
    * +QIRD:<total_receive_length>,<have_read_length>,<unread_length>
    *
    * if <read_length> was absent or != 0
    * +QIRD:<read_actual_length><CR><LF><data>
    *
    */
    if (BG96_ctxt.socket_ctxt.socket_receive_state == SocketRcvState_RequestSize)
    {
        if (element_infos->param_rank == 2)
        {
            /* <total_receive_length> */
            uint32_t buff_in = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintINFO("+QIRD: total_receive_length = %d", buff_in);
            /* Avoid Compiler Warning */
            IGNORE(buff_in);
        }
        else if (element_infos->param_rank == 3)
        {
            /* <have_read_length> */
            uint32_t buff_in = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintINFO("+QIRD: have_read_length = %d", buff_in);
            /* Avoid Compiler Warning */
            IGNORE(buff_in);
        }
        else if (element_infos->param_rank == 4)
        {
            /* <unread_length> */
            uint32_t buff_in = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintINFO("+QIRD: unread_length = %d", buff_in);
            /* this is the size we have to request to read */
            p_modem_ctxt->socket_ctxt.socket_rx_expected_buf_size = buff_in;
        }
    }
    else  if (BG96_ctxt.socket_ctxt.socket_receive_state == SocketRcvState_RequestData_Header)
    {
        /* Receiving socket DATA HEADER which include data size received */
        if (element_infos->param_rank == 2)
        {
            /* <read_actual_length> */
            uint32_t buff_in = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
            PrintDBG("+QIRD: received data size = %d", buff_in);
            /* Avoid Compiler Warning */
            IGNORE(buff_in);
            /* NOTE !!! the size is purely informative in current implementation
            *  indeed, due to real time constraints, the socket data header is analyzed directly in ATCustom_BG96_checkEndOfMsgCallback()
            */
            /* data header analyzed, ready to analyze data payload */
            BG96_ctxt.socket_ctxt.socket_receive_state = SocketRcvState_RequestData_Payload;
        }
    }
    else
    {
        PrintErr("+QIRD: should not fall here");
    }

    return (retval);
}

static at_action_rsp_t fRspAnalyze_QIRD_data_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
    UNUSED(p_atp_ctxt);

    at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PrintAPI("enter fRspAnalyze_QIRD_data_BG96()");

    PrintDBG("DATA received: size=%d vs %d",p_modem_ctxt->socket_ctxt.socket_rx_expected_buf_size, element_infos->str_size);
    if (p_modem_ctxt->socket_ctxt.socketReceivedata.p_buffer_addr != NULL)
    {
        /* recopy data to client buffer */
        memcpy((void *)p_modem_ctxt->socket_ctxt.socketReceivedata.p_buffer_addr ,(void *)&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
        p_modem_ctxt->socket_ctxt.socketReceivedata.buffer_size = element_infos->str_size;
    }
    else
    {
        PrintErr("ERROR, NULL ptr !!!");
        retval = ATACTION_RSP_ERROR;
    }

    return (retval);
}

static at_action_rsp_t fRspAnalyze_QISTATE_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                                         IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
    UNUSED(p_atp_ctxt);

    at_action_rsp_t retval = ATACTION_RSP_IGNORED;
    PrintAPI("enter fRspAnalyze_QISTATE_BG96()");

    /* format:
    * +QISTATE:<connectID>,<service_type>,<IP_adress>,<remote_port>,<local_port>,<socket_state>,<contextID>,<serverID>,<access_mode>,<AT_port>
    *
    * where:
    *
    * exple: +QISTATE: 0,?TCP?,?220.180.239.201?,8705,65514,2,1,0,0,?usbmodem?
    */

    if (element_infos->param_rank == 2)
    {
        /* <connId> */
        uint32_t connId = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
        socket_handle_t sockHandle = atcm_socket_get_socket_handle(p_modem_ctxt, connId);
        /* if this connection ID corresponds to requested socket handle, we will report the following infos */
        if (sockHandle == p_modem_ctxt->socket_ctxt.socket_cnx_infos->socket_handle)
        {
            bg96_qistate_for_requested_socket = AT_TRUE;
        }
        else
        {
            bg96_qistate_for_requested_socket = AT_FALSE;
        }
        PrintDBG("+QISTATE: <connId>=%d (requested=%d)", connId, ((bg96_qistate_for_requested_socket == AT_TRUE) ? 1 : 0) );
    }
    else if (element_infos->param_rank == 3)
    {
        /* <service_type> */
        char _ServiceType[16] = {0};
        memcpy((void*)&_ServiceType[0], (void*)&(p_msg_in->buffer[element_infos->str_start_idx]), element_infos->str_size);
        PrintDBG("+QISTATE: <service_type>=%s", _ServiceType);
    }
    else if (element_infos->param_rank == 4)
    {
        /* <IP_adress> */
        char remIP[MAX_IP_ADDR_SIZE];
        memset((void *)remIP, 0, MAX_IP_ADDR_SIZE);
        memcpy((void*)&remIP[0], (void*)&(p_msg_in->buffer[element_infos->str_start_idx]), element_infos->str_size);
        PrintDBG("+QISTATE: <remote IP_adress>=%s", remIP);
        if (bg96_qistate_for_requested_socket == AT_TRUE)
        {
            uint16_t src_idx;
            uint16_t dest_idx = 0;
            for (src_idx=element_infos->str_start_idx; src_idx<=element_infos->str_end_idx; src_idx++)
            {
                if (p_msg_in->buffer[src_idx] != 0x22) /* remove quotes from the string */
                {
                    p_modem_ctxt->socket_ctxt.socket_cnx_infos->infos->rem_ip_addr_value[dest_idx] = p_msg_in->buffer[src_idx];
                    dest_idx++;
                }
            }
        }
    }
    else if (element_infos->param_rank == 5)
    {
        /* <remote_port> */
        uint32_t remPort = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
        PrintDBG("+QISTATE: <remote_port>=%d", remPort);
        if (bg96_qistate_for_requested_socket == AT_TRUE)
        {
            p_modem_ctxt->socket_ctxt.socket_cnx_infos->infos->rem_port = remPort;
        }
    }
    else if (element_infos->param_rank == 6)
    {
        /* <local_port> */
        uint32_t locPort = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
        PrintDBG("+QISTATE: <local_port>=%d", locPort);
        if (bg96_qistate_for_requested_socket == AT_TRUE)
        {
            p_modem_ctxt->socket_ctxt.socket_cnx_infos->infos->rem_port = locPort;
        }
    }
    else if (element_infos->param_rank == 7)
    {
        /*<socket_state> */
        uint32_t sockState = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
        PrintDBG("+QISTATE: <socket_state>=%d", sockState);
        /* Avoid Compiler Warning */
        IGNORE(sockState);
    }
    else if (element_infos->param_rank == 8)
    {
        /* <contextID> */
        uint32_t contextID = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
        PrintDBG("+QISTATE: <contextID>=%d", contextID);
        /* Avoid Compiler Warning */
        IGNORE(contextID);
    }
    else if (element_infos->param_rank == 9)
    {
        /* <serverID> */
        uint32_t serverID = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
        PrintDBG("+QISTATE: <serverID>=%d", serverID);
        /* Avoid Compiler Warning */
        IGNORE(serverID);
    }
    else if (element_infos->param_rank == 9)
    {
        /* <access_mode> */
        uint32_t access_mode = ATutil_convertStringToInt(&p_msg_in->buffer[element_infos->str_start_idx], element_infos->str_size);
        PrintDBG("+QISTATE: <access_mode>=%d", access_mode);
        /* Avoid Compiler Warning */
        IGNORE(access_mode);
    }
    else if (element_infos->param_rank == 9)
    {
        /* <AT_port> */
        char _ATport[16] = {0};
        memcpy((void*)&_ATport[0], (void*)&(p_msg_in->buffer[element_infos->str_start_idx]), element_infos->str_size);
        PrintDBG("+QISTATE: <AT_port>=%s", _ATport);
    }
    return (retval);
}

at_action_rsp_t fRspAnalyze_Error_BG96(atparser_context_t *p_atp_ctxt, atcustom_modem_context_t *p_modem_ctxt,
                             IPC_RxMessage_t *p_msg_in, at_element_info_t *element_infos)
{
    at_action_rsp_t retval = ATACTION_RSP_ERROR;
    PrintAPI("enter fRspAnalyze_Error_BG96()");

    switch(p_atp_ctxt->current_SID)
    {
     case SID_CS_DIAL_COMMAND:
        /* in case of error during socket connection,
        * release the modem CID for this socket_handle
        */
        atcm_socket_release_modem_cid(p_modem_ctxt, p_modem_ctxt->socket_ctxt.socket_info->socket_handle);
        break;
    }

    /* analyze Error for BG96 */
    switch(p_atp_ctxt->current_atcmd.id)
    {
     case _AT_CREG:
     case _AT_CGREG:
     case _AT_CEREG:
        /* error is ignored */
        retval = ATACTION_RSP_FRC_END;
        break;

     case _AT_CPSMS:
     case _AT_CEDRXS:
     case _AT_QNWINFO:
     case _AT_QENG:
        /* error is ignored */
        retval = ATACTION_RSP_FRC_END;
        break;

     default:
          retval = fRspAnalyze_Error(p_atp_ctxt, p_modem_ctxt, p_msg_in, element_infos);
          break;
    }

    return (retval);
}


/* -------------------------------------------------------------------------- */
static void reinitSyntaxAutomaton_bg96(void)
{
    BG96_ctxt.state_SyntaxAutomaton = WAITING_FOR_INIT_CR;
}

static void reset_variables_bg96(void)
{
    /* Set default values of BG96 specific variables after SWITCH ON or RESET */
    /* bg96_boot_synchro = AT_FALSE; */
    bg96_mode_and_bands_config.nw_scanseq = 0xFFFFFFFF;
    bg96_mode_and_bands_config.nw_scanmode = 0xFFFFFFFF;
    bg96_mode_and_bands_config.iot_op_mode = 0xFFFFFFFF;
    bg96_mode_and_bands_config.gsm_bands = 0xFFFFFFFF;
    bg96_mode_and_bands_config.CatM1_bands = 0xFFFFFFFFFFFFFFFF;
    bg96_mode_and_bands_config.CatNB1_bands = 0xFFFFFFFFFFFFFFFF;
}

static void display_decoded_GSM_bands(uint32_t gsm_bands)
{
    PrintINFO("GSM BANDS config = 0x%x", gsm_bands);

    if (gsm_bands & _QCFGbandGSM_900)
    {
        PrintINFO("GSM_900");
    }
    if (gsm_bands & _QCFGbandGSM_1800)
    {
        PrintINFO("GSM_1800");
    }
    if (gsm_bands & _QCFGbandGSM_850)
    {
        PrintINFO("GSM_850");
    }
    if (gsm_bands & _QCFGbandGSM_1900)
    {
        PrintINFO("GSM_1900");
    }
}

static void display_decoded_CatM1_bands(uint64_t CatM1_bands)
{
    PrintINFO("Cat.M1 BANDS config = 0x%llx", CatM1_bands);

    if (CatM1_bands & _QCFGbandCatM1_B1)
    {
        PrintINFO("CatM1_B1");
    }
    if (CatM1_bands & _QCFGbandCatM1_B2)
    {
        PrintINFO("CatM1_B2");
    }
    if (CatM1_bands & _QCFGbandCatM1_B3)
    {
        PrintINFO("CatM1_B3");
    }
    if (CatM1_bands & _QCFGbandCatM1_B4)
    {
        PrintINFO("CatM1_B4");
    }
    if (CatM1_bands & _QCFGbandCatM1_B5)
    {
        PrintINFO("CatM1_B5");
    }
    if (CatM1_bands & _QCFGbandCatM1_B8)
    {
        PrintINFO("CatM1_B8");
    }
    if (CatM1_bands & _QCFGbandCatM1_B12)
    {
        PrintINFO("CatM1_B12");
    }
    if (CatM1_bands & _QCFGbandCatM1_B13)
    {
        PrintINFO("CatM1_B13");
    }
    if (CatM1_bands & _QCFGbandCatM1_B18)
    {
        PrintINFO("CatM1_B18");
    }
    if (CatM1_bands & _QCFGbandCatM1_B19)
    {
        PrintINFO("CatM1_B19");
    }
    if (CatM1_bands & _QCFGbandCatM1_B20)
    {
        PrintINFO("CatM1_B20");
    }
    if (CatM1_bands & _QCFGbandCatM1_B26)
    {
        PrintINFO("CatM1_B26");
    }
    if (CatM1_bands & _QCFGbandCatM1_B28)
    {
        PrintINFO("CatM1_B28");
    }
    if (CatM1_bands & _QCFGbandCatM1_B39)
    {
        PrintINFO("CatM1_B39");
    }
}

static void display_decoded_CatNB1_bands(uint64_t CatNB1_bands)
{
    PrintINFO("Cat.NB1 BANDS config = 0x%llx", CatNB1_bands);

    if (CatNB1_bands & _QCFGbandCatNB1_B1)
    {
        PrintINFO("CatNB1_B1");
    }
    if (CatNB1_bands & _QCFGbandCatNB1_B2)
    {
        PrintINFO("CatNB1_B2");
    }
    if (CatNB1_bands & _QCFGbandCatNB1_B3)
    {
        PrintINFO("CatNB1_B3");
    }
    if (CatNB1_bands & _QCFGbandCatNB1_B4)
    {
        PrintINFO("CatNB1_B4");
    }
    if (CatNB1_bands & _QCFGbandCatNB1_B5)
    {
        PrintINFO("CatNB1_B5");
    }
    if (CatNB1_bands & _QCFGbandCatNB1_B8)
    {
        PrintINFO("CatNB1_B8");
    }
    if (CatNB1_bands & _QCFGbandCatNB1_B12)
    {
        PrintINFO("CatNB1_B12");
    }
    if (CatNB1_bands & _QCFGbandCatNB1_B13)
    {
        PrintINFO("CatNB1_B13");
    }
    if (CatNB1_bands & _QCFGbandCatNB1_B18)
    {
        PrintINFO("CatNB1_B18");
    }
    if (CatNB1_bands & _QCFGbandCatNB1_B19)
    {
        PrintINFO("CatNB1_B19");
    }
    if (CatNB1_bands & _QCFGbandCatNB1_B20)
    {
        PrintINFO("CatNB1_B20");
    }
    if (CatNB1_bands & _QCFGbandCatNB1_B26)
    {
        PrintINFO("CatNB1_B26");
    }
    if (CatNB1_bands & _QCFGbandCatNB1_B28)
    {
        PrintINFO("CatNB1_B28");
    }
}

#if 0
static void display_decoded_iotopmode(uint32_t iotopmode)
{
    PrintDBG("Iotopmode config = 0x%x", iotopmode);

    if (iotopmode == _QCFGiotopmodeCatM1)
    {
        PrintINFO("iotopmode: LTE networks searched = Cat.M1 only");
    }
    else if (iotopmode == _QCFGiotopmodeCatNB1)
    {
        PrintINFO("iotopmode: LTE networks searched = Cat.NB1 only");
    }
    else if (iotopmode == _QCFGiotopmodeCatM1CatNB1)
    {
        PrintINFO("iotopmode: LTE networks searched = Cat.M1 and Cat.NB1");
    }
    else
    {
        PrintINFO("iotopmode: invalid value 0x%x", iotopmode);
    }
}

static void display_decoded_nwscanmode(uint32_t scanmode)
{
    PrintDBG("nwscanmode config = 0x%x", scanmode);

    if (scanmode == _QCFGscanmodeAuto)
    {
        PrintINFO("scanmode: Automatique (LTE + GSM)");
    }
    else if (scanmode == _QCFGscanmodeGSMOnly)
    {
        PrintINFO("scanmode: GSM network only");
    }
    else if (scanmode == _QCFGscanmodeLTEOnly)
    {
        PrintINFO("scanmode: LTE network only");
    }
    else
    {
        PrintINFO("scanmode: invalid value 0x%x", scanmode);
    }
}

static void display_decoded_nwscanseq(uint32_t scanseq)
{
    PrintDBG("nwscanseq config = 0x%x", scanseq);

    if (scanseq == _QCFGscanseq_GSM_M1_NB1)
    {
        PrintINFO("scanseq: Search order = GSM -> M1 -> NB1");
    }
    else if (scanseq == _QCFGscanseq_GSM_NB1_M1)
    {
        PrintINFO("scanseq: Search order = GSM -> NB1 -> M1");
    }
    else if (scanseq == _QCFGscanseq_M1_GSM_NB1)
    {
        PrintINFO("scanseq: Search order = M1 -> GSM -> NB1");
    }
    else if (scanseq == _QCFGscanseq_M1_NB1_GSM)
    {
        PrintINFO("scanseq: Search order = M1 -> NB1 -> GSM");
    }
    else if (scanseq == _QCFGscanseq_NB1_GSM_M1)
    {
        PrintINFO("scanseq: Search order = NB1 -> GSM -> M1");
    }
    else if (scanseq == _QCFGscanseq_NB1_M1_GSM)
    {
        PrintINFO("scanseq: Search order = NB1 -> M1-> GSM");
    }
    else
    {
        PrintINFO("scanseq: undecoded value 0x%x", scanseq);
    }
}
#endif /* 0 */

static void display_user_friendly_mode_and_bands_config(void)
{
    uint8_t catM1_on = 0;
    uint8_t catNB1_on = 0;
    uint8_t gsm_on = 0;
    uint8_t scanseq_1st, scanseq_2nd, scanseq_3rd;

#if 0
    /* display modem raw values */
    display_decoded_nwscanmode(bg96_mode_and_bands_config.nw_scanmode);
    display_decoded_iotopmode(bg96_mode_and_bands_config.iot_op_mode);
    display_decoded_nwscanseq(bg96_mode_and_bands_config.nw_scanseq);
    display_decoded_CatM1_bands(bg96_mode_and_bands_config.CatM1_bands);
    display_decoded_CatNB1_bands(bg96_mode_and_bands_config.CatNB1_bands);
    display_decoded_GSM_bands(bg96_mode_and_bands_config.gsm_bands);
#endif

    PrintINFO(">>>>> BG96 mode and bands configuration <<<<<");
    /* LTE bands */
    if ((bg96_mode_and_bands_config.nw_scanmode == _QCFGscanmodeAuto) ||
        (bg96_mode_and_bands_config.nw_scanmode == _QCFGscanmodeLTEOnly))
    {
        if ((bg96_mode_and_bands_config.iot_op_mode == _QCFGiotopmodeCatM1CatNB1) ||
            (bg96_mode_and_bands_config.iot_op_mode == _QCFGiotopmodeCatM1))
        {
            /* LTE Cat.M1 active */
            catM1_on = 1;
        }
        if ((bg96_mode_and_bands_config.iot_op_mode == _QCFGiotopmodeCatM1CatNB1) ||
            (bg96_mode_and_bands_config.iot_op_mode == _QCFGiotopmodeCatNB1))
        {
            /* LTE Cat.NB1 active */
            catNB1_on = 1;
        }
    }

    /* GSM bands */
    if ((bg96_mode_and_bands_config.nw_scanmode == _QCFGscanmodeAuto) ||
        (bg96_mode_and_bands_config.nw_scanmode == _QCFGscanmodeGSMOnly))
    {
        /* GSM active */
        gsm_on = 1;
    }

    /* Search active techno */
    scanseq_1st = (bg96_mode_and_bands_config.nw_scanseq & 0x00FF0000) >> 16;
    scanseq_2nd = (bg96_mode_and_bands_config.nw_scanseq & 0x0000FF00) >> 8;
    scanseq_3rd = (bg96_mode_and_bands_config.nw_scanseq & 0x000000FF);

    uint8_t rank = 1;
    if (1 == display_if_active_band(scanseq_1st, rank, catM1_on, catNB1_on, gsm_on))
    {
        rank++;
    }
    if (1 == display_if_active_band(scanseq_2nd, rank, catM1_on, catNB1_on, gsm_on))
    {
        rank++;
    }
    display_if_active_band(scanseq_3rd, rank, catM1_on, catNB1_on, gsm_on);

    PrintINFO(">>>>> ................................. <<<<<");
}

static uint8_t display_if_active_band(uint8_t scanseq, uint8_t rank, uint8_t catM1_on, uint8_t catNB1_on, uint8_t gsm_on)
{
    uint8_t retval = 0;

    if (scanseq == _QCFGscanseqGSM)
    {
        if (gsm_on == 1)
        {
            PrintINFO("GSM band active (scan rank = %d)", rank);
            display_decoded_GSM_bands(bg96_mode_and_bands_config.gsm_bands);

            retval = 1;
        }
    }
    else if (scanseq == _QCFGscanseqLTECatM1)
    {
        if (catM1_on == 1)
        {
            PrintINFO("LTE Cat.M1 band active (scan rank = %d)", rank);
            display_decoded_CatM1_bands(bg96_mode_and_bands_config.CatM1_bands);
            retval = 1;
        }
    }
    else if (scanseq == _QCFGscanseqLTECatNB1)
    {
        if (catNB1_on == 1)
        {
            PrintINFO("LTE Cat.NB1 band active (scan rank = %d)", rank);
            display_decoded_CatNB1_bands(bg96_mode_and_bands_config.CatNB1_bands);
            retval = 1;
        }
    }

    return(retval);
}

static void socketHeaderRX_reset(void)
{
    memset((void *)SocketHeaderDataRx_Buf, 0, 4);
    SocketHeaderDataRx_Cpt = 0;
}
static void SocketHeaderRX_addChar(char *rxchar)
{
    if (SocketHeaderDataRx_Cpt < 4)
    {
        memcpy((void*)&SocketHeaderDataRx_Buf[SocketHeaderDataRx_Cpt], (void *)rxchar, sizeof(char));
        SocketHeaderDataRx_Cpt++;
    }
}
static uint16_t SocketHeaderRX_getSize(void)
{
    return (ATutil_convertStringToInt((uint8_t *)&SocketHeaderDataRx_Buf[0], SocketHeaderDataRx_Cpt));
}
/* ###########################  END CUSTOMIZATION PART  ########################### */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

