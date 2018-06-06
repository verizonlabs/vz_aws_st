/**
  ******************************************************************************
  * @file    Application\at_service\custom\modem_bg96\inc\at_custom_modem_specific.h
  * @author  MCD Application Team
  * @brief   This file defines all the parameters specific to the
  *          BG96 Quectel modem: LTE-cat-M1 or LTE-cat.NB1(=NB-IOT) or GSM
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#ifndef AT_CUSTOM_MODEM_BG96_H
#define AT_CUSTOM_MODEM_BG96_H

#include "at_core.h"
#include "at_parser.h"
#include "at_custom_common.h"
#include "at_custom_modem.h"
#include "cellular_service.h"
#include "cellular_service_int.h"
#include "IPC_common.h"

typedef enum
{
    /* modem specific commands */
    _AT_QPOWD = (_AT_LAST_GENERIC + 1), /* */
    _AT_QCFG,                           /* Extended configuration settings */
    _AT_QINDCFG,                        /* URC indication configuration settings */
    _AT_QIND,                           /* URC indication */
    _AT_QUSIM,                          /* URC indication SIM card typpe (SIM or USIM) */
    _AT_CPSMS,                          /* Power Saving Mode Setting */
    _AT_CEDRXS,                         /* e-I-DRX Setting */
    _AT_QNWINFO,                        /* Query Network Information */
    _AT_QENG,                           /* Switch on or off Engineering Mode */
    /* BG96 specific TCP/IP commands */
    _AT_QICSGP,                         /* Configure parameters of a TCP/IP context */
    _AT_QIACT,                          /* Activate a PDP context */
    _AT_QIDEACT,                        /* Deactivate a PDP context */
    _AT_QIOPEN,                         /* Open a socket service */
    _AT_QICLOSE,                        /* Close a socket service */
    _AT_QISEND,                         /* Send Data - waiting for prompt */
    _AT_QISEND_WRITE_DATA,              /* Send Data - writing data */
    _AT_QIRD,                           /* Retrieve the received TCP/IP Data */
    _AT_QICFG,                          /* Configure optionnal parameters */
    _AT_QISTATE,                        /* Query socket service status */
    _AT_QIURC,                          /* TCP/IP URC */
    _AT_SOCKET_PROMPT,                  /* when sending socket data : prompt = "> " */
    _AT_SEND_OK,                        /* socket send data OK */
    _AT_SEND_FAIL,                      /* socket send data problem */
    _AT_QIDNSCFG,                       /* configure address of DNS server */
    _AT_QIDNSGIP,                       /* get IP address by Domain Name */

    /* modem specific events (URC, BOOT, ...) */
    _AT_WAIT_EVENT,
    _AT_BOOT_EVENT,
    _AT_SIMREADY_EVENT,
    _AT_RDY_EVENT,
}ATCustom_BG96_cmdlist_t;

/* device specific parameters */
typedef enum
{
    _QIURC_unknown,
    _QIURC_closed,
    _QIURC_recv,
    _QIURC_incoming_full,
    _QIURC_incoming,
    _QIURC_pdpdeact,
    _QIURC_dnsgip,
} ATCustom_BG96_QIURC_function_t;

typedef enum
{
    _QCFG_unknown,
    _QCFG_gprsattach,
    _QCFG_nwscanseq,
    _QCFG_nwscanmode,
    _QCFG_iotopmode,
    _QCFG_roamservice,
    _QCFG_band,
    _QCFG_servicedomain,
    _QCFG_sgsn,
    _QCFG_msc,
    _QCFG_PDP_DuplicateChk,
    _QCFG_urc_ri_ring,
    _QCFG_urc_ri_smsincoming,
    _QCFG_urc_ri_other,
    _QCFG_signaltype,
    _QCFG_urc_delay,
} ATCustom_BG96_QCFG_function_t;

typedef enum
{
    _QINDCFG_unknown,
    _QINDCFG_all,
    _QINDCFG_csq,
    _QINDCFG_smsfull,
    _QINDCFG_ring,
    _QINDCFG_smsincoming,
} ATCustom_BG96_QINDCFG_function_t;

typedef enum
{
    _QCFGbandGSM_NoChange = 0x0,
    _QCFGbandGSM_900      = 0x1,
    _QCFGbandGSM_1800     = 0x2,
    _QCFGbandGSM_850      = 0x4,
    _QCFGbandGSM_1900     = 0x8,
    _QCFGbandGSM_any      = 0xF,
} ATCustom_BG96_QCFGbandGSM_t;

typedef enum
{
    _QCFGbandCatM1_B1       = 0x1,
    _QCFGbandCatM1_B2       = 0x2,
    _QCFGbandCatM1_B3       = 0x4,
    _QCFGbandCatM1_B4       = 0x8,
    _QCFGbandCatM1_B5       = 0x10,
    _QCFGbandCatM1_B8       = 0x80,
    _QCFGbandCatM1_B12      = 0x800,
    _QCFGbandCatM1_B13      = 0x1000,
    _QCFGbandCatM1_B18      = 0x20000,
    _QCFGbandCatM1_B19      = 0x40000,
    _QCFGbandCatM1_B20      = 0x80000,
    _QCFGbandCatM1_B26      = 0x2000000,
    _QCFGbandCatM1_B28      = 0x8000000,
    _QCFGbandCatM1_B39      = 0x4000000000,
    _QCFGbandCatM1_NoChange = 0x40000000,
    _QCFGbandCatM1_any      = 0x400a0e189f,
} ATCustom_BG96_QCFGbandCatM1_t;

typedef enum
{
    _QCFGbandCatNB1_B1       = 0x1,
    _QCFGbandCatNB1_B2       = 0x2,
    _QCFGbandCatNB1_B3       = 0x4,
    _QCFGbandCatNB1_B4       = 0x8,
    _QCFGbandCatNB1_B5       = 0x10,
    _QCFGbandCatNB1_B8       = 0x80,
    _QCFGbandCatNB1_B12      = 0x800,
    _QCFGbandCatNB1_B13      = 0x1000,
    _QCFGbandCatNB1_B18      = 0x20000,
    _QCFGbandCatNB1_B19      = 0x40000,
    _QCFGbandCatNB1_B20      = 0x80000,
    _QCFGbandCatNB1_B26      = 0x2000000,
    _QCFGbandCatNB1_B28      = 0x8000000,
    _QCFGbandCatNB1_NoChange = 0x40000000,
    _QCFGbandCatNB1_any      = 0xA0E189F,
} ATCustom_BG96_QCFGbandCatNB1_t;

typedef enum
{
    _QCFGiotopmodeCatM1       = 0x0,
    _QCFGiotopmodeCatNB1      = 0x1,
    _QCFGiotopmodeCatM1CatNB1 = 0x2,
} ATCustom_BG96_QCFGiotopmode_t;

typedef enum
{
    _QCFGscanmodeAuto     = 0x0,
    _QCFGscanmodeGSMOnly  = 0x1,
    _QCFGscanmodeLTEOnly  = 0x3,
} ATCustom_BG96_QCFGscanmode_t;

typedef enum
{
    /* individual values */
    _QCFGscanseqAuto      = 0x0,
    _QCFGscanseqGSM       = 0x1,
    _QCFGscanseqLTECatM1  = 0x2,
    _QCFGscanseqLTECatNB1 = 0x3,
    /* combined values */
    _QCFGscanseq_GSM_M1_NB1 = 0x10203,
    _QCFGscanseq_GSM_NB1_M1 = 0x10302,
    _QCFGscanseq_M1_GSM_NB1 = 0x20103,
    _QCFGscanseq_M1_NB1_GSM = 0x20301,
    _QCFGscanseq_NB1_GSM_M1 = 0x30102,
    _QCFGscanseq_NB1_M1_GSM = 0x30201,
} ATCustom_BG96_QCFGscanseq_t;

typedef enum
{
    /* Engineering Mode - Cell Type information */
    _QENGcelltype_ServingCell    = 0x0, /* get 2G or 3G serving cell information */
    _QENGcelltype_NeighbourCell  = 0x1, /* get 2G or 3G neighbour cell information */
    _QENGcelltype_PSinfo         = 0x2, /* get 2G or 3G cell information during packet switch connected */
} ATCustom_BG96_QENGcelltype_t;

typedef enum
{
    /* QIOPEN servic type parameter */
    _QIOPENservicetype_TCP_Client  = 0x0, /* get 2G or 3G serving cell information */
    _QIOPENservicetype_UDP_Client  = 0x1, /* get 2G or 3G neighbour cell information */
    _QIOPENservicetype_TCP_Server  = 0x2, /* get 2G or 3G cell information during packet switch connected */
    _QIOPENservicetype_UDP_Server  = 0x3, /* get 2G or 3G cell information during packet switch connected */
} ATCustom_BG96_QIOPENservicetype_t;

typedef struct
{
    uint32_t nw_scanseq;
    uint32_t nw_scanmode;
    uint32_t iot_op_mode;
    uint32_t gsm_bands;
    uint64_t CatM1_bands;
    uint64_t CatNB1_bands;
} ATCustom_BG96_mode_band_config_t;

/* device specific parameters */
#define MODEM_MAX_SOCKET_TX_DATA_SIZE ((uint32_t)1460) /* cf AT+QISEND */
#define MODEM_MAX_SOCKET_RX_DATA_SIZE ((uint32_t)1500) /* cf AT+QIRD */

/* Exported functions ------------------------------------------------------- */
void        ATCustom_BG96_init(atparser_context_t *p_atp_ctxt);
uint8_t     ATCustom_BG96_checkEndOfMsgCallback(uint8_t rxChar);
at_status_t ATCustom_BG96_getCmd(atparser_context_t *p_atp_ctxt, uint32_t *p_ATcmdTimeout);
at_endmsg_t ATCustom_BG96_extractElement(atparser_context_t *p_atp_ctxt,
                                         IPC_RxMessage_t *p_msg_in,
                                         uint16_t *p_parseIndex,
                                         at_element_info_t *element_infos);
at_action_rsp_t ATCustom_BG96_analyzeCmd(atparser_context_t *p_atp_ctxt,
                                     IPC_RxMessage_t *p_msg_in,
                                     at_element_info_t *element_infos);
at_action_rsp_t ATCustom_BG96_analyzeParam(atparser_context_t *p_atp_ctxt,
                                       IPC_RxMessage_t *p_msg_in,
                                       at_element_info_t *element_infos);
at_action_rsp_t ATCustom_BG96_terminateCmd(atparser_context_t *p_atp_ctxt, at_element_info_t *element_infos);

at_status_t ATCustom_BG96_get_rsp(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf);
at_status_t ATCustom_BG96_get_urc(atparser_context_t *p_atp_ctxt, at_buf_t *p_rsp_buf);

#endif /* AT_CUSTOM_MODEM_BG96_H */
