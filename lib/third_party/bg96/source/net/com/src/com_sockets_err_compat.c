/**
  ******************************************************************************
  * @file    ST/net/com/src/com_sockets_err_compat.c
  * @author  MCD Application Team
  * @version V1.0
  * @date    28-Mar-2018
  * @brief   This file implements Communication Socket Error Compatibility 
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


/* Includes ------------------------------------------------------------------*/
#include "plf_config.h"
#include "com_sockets_err_compat.h"

#if (USE_SOCKETS_MODEM == 1)

/** Table to quickly map an com_sockets_err_t to to a socket error
  * by using -err as an index */
#if (COM_SOCKETS_ERRNO_COMPAT == 1)
static const int32_t com_sockets_err_to_errno_table[] = {
  0,             /* COM_SOCKETS_ERR_OK             0  Everything OK              */
  -1,            /* COM_SOCKETS_ERR_GENERAL       -1  General / Low level error  */
  EIO,           /* COM_SOCKETS_ERR_DESCRIPTOR    -2  Invalid socket descriptor  */
  EINVAL,        /* COM_SOCKETS_ERR_PARAMETER     -3  Invalid parameter          */
  EWOULDBLOCK,   /* COM_SOCKETS_ERR_WOULDBLOCK    -4  Operation would block      */
  ENOMEM,        /* COM_SOCKETS_ERR_NOMEMORY      -5  Out of memory error        */
  ENOTCONN,      /* COM_SOCKETS_ERR_CLOSING       -6  Connection is closing      */
  EADDRINUSE,    /* COM_SOCKETS_ERR_LOCKED        -7  Address in use             */
  EAGAIN,        /* COM_SOCKETS_ERR_TIMEOUT       -8  Timeout                    */
  EINPROGRESS,   /* COM_SOCKETS_ERR_INPROGRESS    -9  Operation in progress      */
  EHOSTUNREACH,  /* COM_SOCKETS_ERR_NONAME       -10  Host Name not existing     */

  /* Added value */  
  EHOSTUNREACH,  /* COM_SOCKETS_ERR_NONETWORK    -11   No network to proceed      */
  -1,            /* COM_SOCKETS_ERR_UNSUPPORTED  -12   Unsupported functionnality */
  -1             /* COM_SOCKETS_ERR_STATE        -13   Connect rqt but Socket is already connected
                                                       Send/Recv rqt but Socket is not connected
                                                       ... */
};

int32_t com_sockets_err_to_errno(com_sockets_err_t err)
{
  if (   (err > 0) 
      || (-err >= (sizeof(com_sockets_err_to_errno_table)/sizeof((com_sockets_err_to_errno_table[0])))))
  {
    return((int32_t)EIO);
  }
  else
  {
    return((int32_t)com_sockets_err_to_errno_table[-err]);
  }
}
#else
int32_t com_sockets_err_to_errno(com_sockets_err_t err)
{
  return((int32_t)err);
}
#endif /* COM_SOCKETS_ERRNO_COMPAT */

#else /* USE_SOCKETS_MODEM == 0 */
/* Just for compilation - should not be used */
int32_t com_sockets_err_to_errno(com_sockets_err_t err)
{
  return((int32_t)err);
}

#endif /* USE_SOCKETS_MODEM */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/