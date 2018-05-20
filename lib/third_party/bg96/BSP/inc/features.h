/**
  ******************************************************************************
  * @file    project_config/inc/features.h
  * @author  MCD Marketing / Vertical Application Team
  * @brief   This file contains the features included in firmware
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#define USE_DC_MEMS        (0) /* 0: not activated, 1: activated */
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
