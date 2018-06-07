/**
  ******************************************************************************
  * @file    Application\radio_service\cellular\inc\cellular_service_os.h
  * @author  MCD Application Team
  * @brief   This file defines functions prototypes for Cellular Service Os dependance
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#ifndef CELLULAR_SERVICE_OS_H_
#define CELLULAR_SERVICE_OS_H_

#include <stdbool.h>

#include "plf_config.h"
#include "cellular_service.h"


/* Cellular Service Library Init */
bool osCDS_cellular_service_init(void);

CS_Status_t osCS_get_signal_quality(CS_SignalQuality_t *p_sig_qual);

/* SOCKET API */
socket_handle_t osCDS_socket_create(CS_IPaddrType_t addr_type,
                                    CS_TransportProtocol_t protocol,
                                    uint16_t local_port,
                                    CS_PDN_conf_id_t cid);
                                  
CS_Status_t osCDS_socket_set_callbacks(socket_handle_t sockHandle,
                                       cellular_socket_data_ready_callback_t data_ready_cb,
                                       cellular_socket_data_sent_callback_t data_sent_cb,
                                       cellular_socket_closed_callback_t remote_close_cb);
                                     
CS_Status_t osCDS_socket_set_option(socket_handle_t sockHandle,
                                    CS_SocketOptionLevel_t opt_level,
                                    CS_SocketOptionName_t opt_name,
                                    void *p_opt_val);
                                  
CS_Status_t osCDS_socket_get_option(void);

CS_Status_t osCDS_socket_connect(socket_handle_t sockHandle,
                                 CS_IPaddrType_t addr_type,
                                 char *p_ip_addr_value,
                                 uint16_t remote_port); /* for socket client mode */
                               
CS_Status_t osCDS_socket_listen(socket_handle_t sockHandle);

CS_Status_t osCDS_socket_send(socket_handle_t sockHandle,
                              const char *p_buf,
                              uint32_t length);
                            
int osCDS_socket_receive(socket_handle_t sockHandle,
                         char *p_buf,
                         uint32_t max_buf_length);
                       
CS_Status_t osCDS_socket_cnx_status(socket_handle_t sockHandle,
                                    CS_SocketCnxInfos_t *infos);

CS_Status_t osCDS_socket_close(socket_handle_t sockHandle,
                               uint8_t force);


/* =========================================================
   ===========      Mode Command services        ===========
   ========================================================= */


CS_Status_t osCDS_get_net_status(CS_RegistrationStatus_t *p_reg_status);
CS_Status_t osCDS_get_device_info(CS_DeviceInfo_t *p_devinfo);
CS_Status_t osCDS_subscribe_net_event(CS_UrcEvent_t event, cellular_urc_callback_t urc_callback);
CS_Status_t osCDS_power_on(void);
CS_Status_t osCDS_reset(CS_Reset_t rst_type);
CS_Status_t osCDS_init_modem(CS_ModemInit_t init, CS_Bool_t reset, const char *pin_code);
CS_Status_t osCDS_register_net(CS_OperatorSelector_t *p_operator,
                            CS_RegistrationStatus_t *p_reg_status);
CS_Status_t osCDS_get_attach_status(CS_PSattach_t *p_attach);
CS_Status_t osCDS_attach_PS_domain(void);
CS_Status_t osCDS_define_pdn(CS_PDN_conf_id_t cid, const char *apn, CS_PDN_configuration_t *pdn_conf);
CS_Status_t  osCDS_register_pdn_event(CS_PDN_conf_id_t cid, cellular_pdn_event_callback_t pdn_event_callback);
CS_Status_t  osCDS_set_default_pdn(CS_PDN_conf_id_t cid);
CS_Status_t osCDS_activate_pdn(CS_PDN_conf_id_t cid);
CS_Status_t osCDS_suspend_data(void);
CS_Status_t osCDS_resume_data(void);

CS_Status_t osCDS_dns_request(CS_PDN_conf_id_t cid,
                              CS_DnsReq_t  *dns_req,
                              CS_DnsResp_t *dns_resp);

#endif /* CELLULAR_SERVICE_LIB_H_ */