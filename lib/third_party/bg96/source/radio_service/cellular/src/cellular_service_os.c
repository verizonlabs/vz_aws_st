/**
  ******************************************************************************
  * @file    Application\radio_service\cellular\src\cellular_service_os.c
  * @author  MCD Application Team
  * @brief   This file defines functions for Cellular Service OS dependance
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

#include "cellular_service_task.h"
#include "cellular_service_os.h"

osMutexId CellularServiceMutexHandle = NULL;
static bool CellularServiceInitialized = false;

CS_Status_t osCS_get_signal_quality(CS_SignalQuality_t *p_sig_qual)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);

  result = CS_get_signal_quality(p_sig_qual);

  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}


socket_handle_t osCDS_socket_create(CS_IPaddrType_t addr_type,
                                    CS_TransportProtocol_t protocol,
                                    uint16_t local_port,
                                    CS_PDN_conf_id_t cid)
{
  socket_handle_t socket_handle;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);

  socket_handle = CDS_socket_create(addr_type,
                                    protocol,
                                    local_port,
                                    cid);
  osMutexRelease(CellularServiceMutexHandle);

  return(socket_handle);
}

CS_Status_t osCDS_socket_set_callbacks(socket_handle_t sockHandle,
                                       cellular_socket_data_ready_callback_t data_ready_cb,
                                       cellular_socket_data_sent_callback_t data_sent_cb,
                                       cellular_socket_closed_callback_t remote_close_cb)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);

  result = CDS_socket_set_callbacks(sockHandle,
                                    data_ready_cb,
                                    data_sent_cb,
                                    remote_close_cb);

  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_socket_set_option(socket_handle_t sockHandle,
                                    CS_SocketOptionLevel_t opt_level,
                                    CS_SocketOptionName_t opt_name,
                                    void *p_opt_val)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);

  result = CDS_socket_set_option(sockHandle,
                                 opt_level,
                                 opt_name,
                                 p_opt_val);

  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_socket_get_option(void)
{
  CS_Status_t result = CELLULAR_ERROR;

  if(CST_get_state() == CST_MODEM_DATA_READY_STATE)
  {
    osMutexWait(CellularServiceMutexHandle, osWaitForever);

    result = CDS_socket_get_option();

    osMutexRelease(CellularServiceMutexHandle);
  }

  return(result);
}

CS_Status_t osCDS_socket_connect(socket_handle_t sockHandle,
                                 CS_IPaddrType_t addr_type,
                                 char *p_ip_addr_value,
                                 uint16_t remote_port)
{
  CS_Status_t result = CELLULAR_ERROR;

  if(CST_get_state() == CST_MODEM_DATA_READY_STATE)
  {
    osMutexWait(CellularServiceMutexHandle, osWaitForever);

    result = CDS_socket_connect(sockHandle,
                                addr_type,
                                (CS_CHAR_t*)p_ip_addr_value,
                                remote_port);

    osMutexRelease(CellularServiceMutexHandle);
  }

  return(result);
}

CS_Status_t osCDS_socket_listen(socket_handle_t sockHandle)
{
  CS_Status_t result = CELLULAR_ERROR;

  if(CST_get_state() == CST_MODEM_DATA_READY_STATE)
  {
    osMutexWait(CellularServiceMutexHandle, osWaitForever);

    result = CDS_socket_listen(sockHandle);

    osMutexRelease(CellularServiceMutexHandle);
  }

  return(result);
}

CS_Status_t osCDS_socket_send(socket_handle_t sockHandle,
                              const char *p_buf,
                              uint32_t length)
{
  CS_Status_t result = CELLULAR_ERROR;

  if(CST_get_state() == CST_MODEM_DATA_READY_STATE)
  {
    osMutexWait(CellularServiceMutexHandle, osWaitForever);

    result = CDS_socket_send(sockHandle,
                             (const CS_CHAR_t *)p_buf,
                             length);

    osMutexRelease(CellularServiceMutexHandle);
  }

  return(result);
}

int osCDS_socket_receive(socket_handle_t sockHandle,
                         char *p_buf,
                         uint32_t max_buf_length)
{
  int result = 0;

  if(CST_get_state() == CST_MODEM_DATA_READY_STATE)
  {
    osMutexWait(CellularServiceMutexHandle, osWaitForever);

    result = CDS_socket_receive(sockHandle,
                                (CS_CHAR_t *)p_buf,
                                max_buf_length);

    osMutexRelease(CellularServiceMutexHandle);
  }

  return(result);
}

CS_Status_t osCDS_socket_close(socket_handle_t sockHandle,
                               uint8_t force)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);

  result = CDS_socket_close(sockHandle,
                            force);

  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_socket_cnx_status(socket_handle_t sockHandle,
                                    CS_SocketCnxInfos_t *infos)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);

  result = CDS_socket_cnx_status(sockHandle,
                                 infos);

  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

bool osCDS_cellular_service_init(void)
{
  bool result = true;

  if (CellularServiceInitialized == false)
  {
    osMutexDef(osCellularServiceMutex);
    CellularServiceMutexHandle = osMutexCreate(osMutex(osCellularServiceMutex));
    if (CellularServiceMutexHandle == NULL)
    {
      result = false;
      ERROR_Handler(DBG_CHAN_CELLULAR_SERVICE, 1, ERROR_FATAL);
    }
    else
    {
      CellularServiceInitialized = true;
    }
  }

  return result;
}

/* =========================================================
   ===========      Mode Command services        ===========
   ========================================================= */


CS_Status_t osCDS_get_net_status(CS_RegistrationStatus_t *p_reg_status)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_get_net_status(p_reg_status);
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_get_device_info(CS_DeviceInfo_t *p_devinfo)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_get_device_info(p_devinfo);
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_subscribe_net_event(CS_UrcEvent_t event, cellular_urc_callback_t urc_callback)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_subscribe_net_event( event,  urc_callback);
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}


CS_Status_t osCDS_power_on(void)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_power_on( );
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_reset(CS_Reset_t rst_type)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_reset(rst_type);
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_init_modem(CS_ModemInit_t init, CS_Bool_t reset, const char *pin_code)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_init_modem( init,  reset, (const CS_CHAR_t *)pin_code);
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_register_net(CS_OperatorSelector_t *p_operator, CS_RegistrationStatus_t *p_reg_status)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_register_net(p_operator, p_reg_status);
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_get_attach_status(CS_PSattach_t *p_attach)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_get_attach_status(p_attach);
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}


CS_Status_t osCDS_attach_PS_domain(void)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_attach_PS_domain();
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}


CS_Status_t osCDS_define_pdn(CS_PDN_conf_id_t cid, const char *apn, CS_PDN_configuration_t *pdn_conf)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_define_pdn( cid, (const CS_CHAR_t *)apn,  pdn_conf);
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_register_pdn_event(CS_PDN_conf_id_t cid, cellular_pdn_event_callback_t pdn_event_callback)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_register_pdn_event( cid,  pdn_event_callback);
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_set_default_pdn(CS_PDN_conf_id_t cid)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_set_default_pdn( cid);
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_activate_pdn(CS_PDN_conf_id_t cid)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_activate_pdn( cid);
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}


CS_Status_t osCDS_suspend_data(void)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_suspend_data();
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}

CS_Status_t osCDS_resume_data(void)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_resume_data();
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}


CS_Status_t osCDS_dns_request(CS_PDN_conf_id_t cid,
                              CS_DnsReq_t *dns_req,
                              CS_DnsResp_t *dns_resp)
{
  CS_Status_t result;

  osMutexWait(CellularServiceMutexHandle, osWaitForever);
  result = CS_dns_request(cid, dns_req, dns_resp);
  osMutexRelease(CellularServiceMutexHandle);

  return(result);
}
