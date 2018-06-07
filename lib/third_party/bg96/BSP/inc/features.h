/**
  ******************************************************************************
  * @file    project_config/inc/features.h
  * @author  MCD Application Team
  * @brief   This file contains the features included in firmware
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FEATURES_H
#define __FEATURES_H

/* ================================================= */
/*          USER MODE                                */
/* ================================================= */

/* ===================================== */
/* BEGIN - Cellular data mode            */
/* ===================================== */
#define USE_SOCKETS_MODEM (1) /* 0: not activated, 1: activated */
/* Next feature is under development */
#define COM_SOCKETS_ERRNO_COMPAT (0) /* 0: not activated, 1: activated */
/* ===================================== */
/* END - Cellular data mode              */
/* ===================================== */

/* ===================================== */
/* BEGIN - Applications to include       */
/* ===================================== */
#if (USE_SOCKETS_MODEM == 1)
/* Only Http Client TCP is supported for the moment */
#define USE_HTTP_CLIENT    (0) /* 0: not activated, 1: activated */
#define USE_HTTP_SERVER    (0) /* Server TCP not yet supported   */
#define USE_COAP           (0) /* Server UDP not yet supported   */
#define USE_LIVEOBJECTS    (0) /* LiveObjects not yet supported  */
#define USE_PING_CLIENT    (0) /* Ping not yet supported         */
#define USE_PPPOSIF_CLIENT (0) /* Not needed                     */
#define USE_DC_TEST        (0) /* Internal socket not supported  */
#else
#define USE_HTTP_CLIENT    (0) /* 0: not activated, 1: activated */
#define USE_HTTP_SERVER    (0) /* 0: not activated, 1: activated */
#define USE_COAP           (0) /* 0: not activated, 1: activated */
#define USE_LIVEOBJECTS    (0) /* 0: not activated, 1: activated */
#define USE_PING_CLIENT    (0) /* 0: not activated, 1: activated */
#define USE_PPPOSIF_CLIENT (1) /* 0: not activated, 1: activated */
#define USE_DC_TEST        (0) /* 0: not activated, 1: activated */
#endif

/* MEMS not available for for AkorloT board */
#define USE_DC_MEMS        (1) /* 0: not activated, 1: activated */
#define USE_SIMU_MEMS      (0) /* 0: not activated, 1: activated */

#define USE_DC_EMUL        (0) /* 0: not activated, 1: activated */
/* ===================================== */
/* END - Applications to include         */
/* ===================================== */


/* ================================================= */
/*          EXPERT MODE                              */
/* ================================================= */

/* ================================================= */
/* BEGIN - Middleware components used (export mode)  */
/* ================================================= */
#define RTOS_USED         (1) /* 0: not activated, 1: activated */
#define USE_DATACACHE     (1) /* 0: not activated, 1: activated */

/* Socket interface is used */
#define USE_IAC_THROUGH_SOCKET (1) /* 0: not activated, 1: activated */
/* PCB interface is used    */
#define USE_IAC_THROUGH_PCB    (0) /* 0: not activated, 1: activated */

/* IAC Wait Network is On   */
#define IAC_WAIT_NETWORK_ON    (1) /* 0: not activated, 1: activated */

/* ================================================= */
/* END - Middleware components used                  */
/* ================================================= */

/* ============================================= */
/* BEGIN - Button polling                        */
/* ============================================= */
#if (USE_DC_MEMS == 1)
#define BB_LEFT_POLLING  (1) /* 0: not activated, 1: activated */
#endif
#define BB_RIGHT_POLLING (0) /* 0: not activated, 1: activated */
#define BB_UP_POLLING    (0) /* 0: not activated, 1: activated */
#define BB_DOWN_POLLING  (0) /* 0: not activated, 1: activated */

#if (BB_LEFT_POLLING == 1) || (BB_RIGHT_POLLING == 1) || (BB_UP_POLLING == 1) || (BB_DOWN_POLLING == 1)
#define BB_BUTTON_POLLING (1)
#else
#define BB_BUTTON_POLLING (0)
#endif
/* ============================================= */
/* BEGIN - Button polling                        */
/* ============================================= */


#endif /* __FEATURES_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
