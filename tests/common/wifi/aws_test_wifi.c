/*
 * Amazon FreeRTOS
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/* stdlib includes. */
#include "string.h"
#include "stdio.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "aws_secure_sockets.h"

/* Unity framework includes. */
#include "unity_fixture.h"

/* Wi-Fi include. */
#include "aws_wifi.h"
#include "aws_clientcredential.h"

/* Testing variable includes. */
#include "aws_test_tcp.h"
#include "aws_test_runner.h"


/* Testing configurations defintions. */

/* The number of times to loop in the WiFiConnectionLoop test. */
#define testwifiCONNECTION_LOOP_TIMES       5

/* The delay in ms between connection and disconnection. */
#define testwifiCONNECTION_DELAY            pdMS_TO_TICKS( 2000 )

/* The maximum number of networks that can be scanned. */
#define testwifiMAX_SCAN_NUMBER             10

/* WIFI_Ping() test configurations. */
#define testwifiPING_COUNT                  3
#define testwifiPING_INTERVAL_MS            10

/* The WIFF_Ping() testing address is currently set to the echo server see aws_test_tcp.h. */
#define testwifiECHO_SERVER_ADDRESS         SOCKETS_inet_addr_quick( tcptestECHO_SERVER_ADDR0, \
                                                                     tcptestECHO_SERVER_ADDR1, \
                                                                     tcptestECHO_SERVER_ADDR2, \
                                                                     tcptestECHO_SERVER_ADDR3 )
#define testwifiPING_ADDRESS                testwifiECHO_SERVER_ADDRESS

/* WIFI_GetHostIP() test configurations. */
#define testwifiTEST_DOMAIN_NAME            "amazon.com"    /* Valid domain name. */
#define testwifiTEST_INVALID_DOMAIN_NAME    "invalid"       /* Invalid domain name. */

/* Invalid credentials for WIFI_ConnectAP() tests. Valid credentials used are defined in
   aws_clientcredentials.h. */
#define testwifiINVALID_WIFI_SSID           "invalid ssid"      /* This must be a SSID that does not exist in your range. */
#define testwifiINVALID_WIFI_PASSWORD       "invalid password"  /* This must be a password that is mismatched from the valid AP configured in aws_clientcredentials.h. */
#define testwifiINVALID_WIFI_SECURITY       eWiFiSecurityWEP    /* This must be a security type that is mismatched from the valid AP configured in aws_clientcredentials.h. */

/* Second set of valid Wi-Fi credentials for testing the connection loop. */
#define testwifiWIFI_SSID                   "Guest"
#define testwifiWIFI_PASSWORD               "Dummy"
#define testwifiWIFI_SECURITY               eWiFiSecurityOpen

/* 14 total channels in the 2.4 GHz band. Set to the number of channels available in your region. */
#define testwifiMAX_CHANNEL_NUMBER          14

/* 6 total bytes in a MAC address. */
#define testwifiMAC_ADDRESS_LENGTH          6

/* Arbitrary maximum number of networks to save for WIFI_NetworkGet(), WIFI_NetworkSet()
   and WIFI_NetworkAdd() tests. */
#define testwifiMAX_NETWORK_SAVE_NUMBER     100

/* String for loopback testing. */
#define testwifiLOOPBACK_TEST_STRING        "abcd"

/* Wi-Fi connection multitask test paramters. */

/* Task parameters. */
#define testwifiTASK_STACK_SIZE             ( configMINIMAL_STACK_SIZE * 4 )
#define testwifiTASK_PRIORITY               ( tskIDLE_PRIORITY )

/* Timeout for all of the tasks to finish the test. */
#define testwifiMULTITASK_TEST_TIMEOUT      pdMS_TO_TICKS( 60000 )

/* Timeout when one task waits for the other to synchronize with it. */
#define testwifiTASK_SYNC_TIMEOUT           pdMS_TO_TICKS( 10000 )   

/* The event group used to syncrhonize the Wi-Fi multi-task tests. */
static EventGroupHandle_t xTaskFinishEventGroupHandle;

/* Task status for WiFiSeperateTasksConnectingAndDisconnectingAtOnce test. */
typedef struct
{
    void * pvParameters;
    uint16_t usTaskId;
    WIFIReturnCode_t xWiFiStatus;
    TaskHandle_t xTaskHandle;
} testwifiTaskParams_t;

/* Test WIFI_ConfigureAP(). Beware that enabling these tests might affect WIFI_ConnectAP()
 * if in WIFI_ConnectAP() the mode is not switched from SoftAP to Station. */
#define testwifiENABLE_CONFIGURE_AP_TESTS  0

/* Functions that are required to be implemented for AFQP are:
 * - WIFI_On()
 * - WIFI_ConnectAP()
 * - WIFI_Disconnect()
 * - WIFI_Scan() - Microchip did not implement
 * - WIFI_GetIP()
 * - WIFI_GetMAC()
 * - WIFI_GetHostIP()
 */

/* Custom WIFI Test asserts. */
#define TEST_WIFI_ASSERT_REQUIRED_API( condition, result ) \
    if( result == eWiFiNotSupported ) { TEST_FAIL_MESSAGE( "Required Wi-Fi API is not implemented." ); } else { TEST_ASSERT( condition ); }
        
#define TEST_WIFI_ASSERT_OPTIONAL_API( condition, result ) \
    if( result == eWiFiNotSupported ) { TEST_ASSERT( 1 ); } else { TEST_ASSERT( condition ); }

#define TEST_WIFI_ASSERT_REQUIRED_API_MSG( condition, result, message ) \
    if( result == eWiFiNotSupported ) { TEST_FAIL_MESSAGE( "Required Wi-Fi API is not implemented." ); } else { TEST_ASSERT_MESSAGE( condition, message ); }

/* Helper functions. */

/* Set the network parameters for this aws_clientcredential.h defined parameters. */
static inline void prvSetClientNetworkParameters( WIFINetworkParams_t * pxClientNetworkParams )
{
    pxClientNetworkParams->pcSSID = clientcredentialWIFI_SSID;
    pxClientNetworkParams->ucSSIDLength = sizeof( clientcredentialWIFI_SSID );
    pxClientNetworkParams->pcPassword = clientcredentialWIFI_PASSWORD;
    pxClientNetworkParams->ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD );
    pxClientNetworkParams->xSecurity = clientcredentialWIFI_SECURITY;
}

/* Set network parameters for this test's defined parameters. */
static inline void prvSetTestNetworkParameters( WIFINetworkParams_t * pxTestNetworkParams )
{
    pxTestNetworkParams->pcSSID = testwifiWIFI_SSID;
    pxTestNetworkParams->ucSSIDLength = sizeof( testwifiWIFI_SSID );
    pxTestNetworkParams->pcPassword = testwifiWIFI_PASSWORD;
    pxTestNetworkParams->ucPasswordLength = sizeof( testwifiWIFI_PASSWORD );
    pxTestNetworkParams->xSecurity = testwifiWIFI_SECURITY;
}

/* Set the SoftAP network parameters for WIFI_ConfigureAP() test. */
static inline void prvSetSoftAPNetworkParameters( WIFINetworkParams_t * pxSoftAPNetworkParams )
{
    pxSoftAPNetworkParams->pcSSID = wificonfigACCESS_POINT_SSID_PREFIX;
    pxSoftAPNetworkParams->ucSSIDLength = sizeof( wificonfigACCESS_POINT_SSID_PREFIX );
    pxSoftAPNetworkParams->pcPassword = wificonfigACCESS_POINT_PASSKEY;
    pxSoftAPNetworkParams->ucPasswordLength = sizeof( wificonfigACCESS_POINT_PASSKEY );
    pxSoftAPNetworkParams->xSecurity = wificonfigACCESS_POINT_SECURITY;
    pxSoftAPNetworkParams->cChannel = wificonfigACCESS_POINT_CHANNEL;
}

/* Set the network profile for WIFI_NetworkGet() and WIFI_NetworkAdd() tests. */
static inline void prvSetNetworkProfile( WIFINetworkProfile_t * pxNetworkProfile )
{
    pxNetworkProfile->ucSSIDLength = sizeof( clientcredentialWIFI_SSID );
    strncpy( pxNetworkProfile->cSSID, clientcredentialWIFI_SSID, pxNetworkProfile->ucSSIDLength );
    pxNetworkProfile->ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD );
    strncpy( pxNetworkProfile->cPassword, clientcredentialWIFI_PASSWORD, pxNetworkProfile->ucPasswordLength );
    pxNetworkProfile->xSecurity = clientcredentialWIFI_SECURITY;
}

/**
 * @brief Connect to the Wi-Fi access point and verify sucecss.
 */
static BaseType_t prvConnectAPTest()
{
    WIFIReturnCode_t xWiFiStatus;
    WIFINetworkParams_t xNetworkParams;
    BaseType_t xResult = pdPASS;

    prvSetClientNetworkParameters( &xNetworkParams );

    xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
    
    if( eWiFiSuccess != xWiFiStatus )
    {
        configPRINTF( ( "Could not connect to the access point.\r\n" ) );
        xResult = pdFAIL;
    }

    if( eWiFiSuccess == xWiFiStatus )
    {
        vTaskDelay( testwifiCONNECTION_DELAY );
        if( pdTRUE != WIFI_IsConnected() )
        {
            configPRINTF( ( "Wi-Fi is not connected.\r\n" ) );
            xResult = pdFAIL;
        }
    }

    return xResult;
}

/**
 * @brief Send and receive a string to/from the Echo server defined in aws_test_tcp.h.
 */
static BaseType_t prvLoopBackTest()
{
    Socket_t xSocket;
    SocketsSockaddr_t xEchoServerSockaddr;
    BaseType_t xNumBytesSentTotal = 0;
    BaseType_t xNumBytesSent = 0;
    BaseType_t xNumBytesRecvTotal = 0;
    BaseType_t xNumBytesRecv = 0;
    char * cLoopBackTestString = testwifiLOOPBACK_TEST_STRING;
    size_t xLoopBackTestStringLength = sizeof( testwifiLOOPBACK_TEST_STRING );
    char * cRecvBuffer = cBuffer;
    BaseType_t xResult = pdPASS;

    /* Make socket. */
    xSocket = SOCKETS_Socket( SOCKETS_AF_INET, 
                              SOCKETS_SOCK_STREAM, 
                              SOCKETS_IPPROTO_TCP );

    if( xSocket == SOCKETS_INVALID_SOCKET )
    {
        xResult = pdFAIL;
        configPRINTF( ( "Error creating the socket.\r\n" ) );
    }

    if( xResult == pdPASS )
    {
        /* Connect the socket to the echo server. If the Wi-Fi is not connected we may fail here. */
        xEchoServerSockaddr.ulAddress = testwifiECHO_SERVER_ADDRESS;
        xEchoServerSockaddr.usPort = SOCKETS_htons( tcptestECHO_PORT );
        xEchoServerSockaddr.ucLength = sizeof( SocketsSockaddr_t );
        xEchoServerSockaddr.ucSocketDomain = SOCKETS_AF_INET;

        if( SOCKETS_Connect( xSocket, 
                             &xEchoServerSockaddr,
                             sizeof( xEchoServerSockaddr ) ) != SOCKETS_ERROR_NONE )
        {
            xResult = pdFAIL;
            configPRINTF( ( "Error connecting on the socket.\r\n" ) );
        }

    }

    if( xResult == pdPASS )
    {
        /* Send the data. */
        while( ( size_t ) xNumBytesSentTotal < xLoopBackTestStringLength )
        {
            xNumBytesSent = SOCKETS_Send( xSocket,
                                          &cLoopBackTestString[ xNumBytesSentTotal ], 
                                          xLoopBackTestStringLength - xNumBytesSentTotal,
                                          0 );

            if( xNumBytesSent <= 0 )
            {
                xResult = pdFAIL;
                configPRINTF( ( "Error in SOCKETS_Send.  Return value: %d \r\n", xNumBytesSent ) );
                break;
            }

            xNumBytesSentTotal += xNumBytesSent;
        }
    }

    if( xResult == pdPASS )
    {
        /* Clear the recieve buffer. */
        memset( cRecvBuffer, 0, xLoopBackTestStringLength );

        /* Receive the data. */
        while( ( size_t ) xNumBytesRecvTotal < xLoopBackTestStringLength )
        {
            xNumBytesRecv = SOCKETS_Recv( xSocket,
                                          &cRecvBuffer[ xNumBytesRecvTotal ],
                                          xLoopBackTestStringLength - xNumBytesRecvTotal,
                                          0 );

            if( xNumBytesRecv == 0 )
            {
                xResult = pdFAIL;
                configPRINTF( ( "Timed out receiving from echo server \r\n" ) );
                break;
            }
            else if( xNumBytesRecv < 0 )
            {
                xResult = pdFAIL;
                configPRINTF( ( "Error %d while receiving from echo server\r\n", xNumBytesRecv ) );
                break;
            }

            xNumBytesRecvTotal += xNumBytesRecv;
        }

        if( ( size_t ) xNumBytesRecvTotal != xLoopBackTestStringLength )
        {
            xResult = pdFAIL;
        }

        cRecvBuffer[ xLoopBackTestStringLength ] = '\0';
    }

    if( xResult == pdPASS )
    {
        /* Compare the received string with the test string. */
        if( strncmp( cRecvBuffer, cLoopBackTestString, xLoopBackTestStringLength ) != 0 )
        {
            xResult = pdFAIL;
            configPRINTF( ( "Test String: %s, Received String: %s\r\n", testwifiLOOPBACK_TEST_STRING, cRecvBuffer ) );
        }
    }

    if (xSocket != SOCKETS_INVALID_SOCKET)
    {
        /* Finished using the connected socket, initiate a graceful close:
         * FIN, FIN+ACK, ACK. */
        if( SOCKETS_Shutdown( xSocket, SOCKETS_SHUT_RDWR ) != SOCKETS_ERROR_NONE )
        {
            /* The loop back succeeded, which shows connectivity, so failing to close the socket does not indicate failure. */
            configPRINTF( ("Failed to shutdown the socket.\r\n") );
        }

        /* Close the socket. */
        if( SOCKETS_Close( xSocket ) != SOCKETS_ERROR_NONE )
        {
            /* The loop back succeeded, which shows connectivity, so failing to close the socket does not indicate failure. */
            configPRINTF( ("Failed to close the socket.\r\n") );
        }
    }

    return xResult;
}

static void prvFinishWiFiTesting()
{
    /* Disconnect first. */
    WIFI_Disconnect();

    /* Wait for disconnection. */
    vTaskDelay( testwifiCONNECTION_DELAY );

    /* Turn off the Wi-Fi. */
    WIFI_Off();

    /* Turn on the Wi-Fi. */
    WIFI_On();

    /* Turn on the Wi-Fi. */
    if ( prvConnectAPTest() != pdPASS )
    {
        configPRINTF( ( "Could not reconnect the Wi-Fi following testing.\r\n" ) );
    }
    else
    {
        configPRINTF( ( "Wi-Fi reconnected following tests finished. \r\n" ) );
    }
}

/* Unity TEST initializations. */
TEST_GROUP( Full_WiFi );

TEST_SETUP( Full_WiFi )
{
    WIFIReturnCode_t xWiFiStatus;
    
    /* Disconnect first before running any Wi-Fi test. */
    xWiFiStatus = WIFI_Disconnect();
    TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus == eWiFiSuccess, xWiFiStatus );
    
    vTaskDelay( testwifiCONNECTION_DELAY );
}

TEST_TEAR_DOWN( Full_WiFi )
{
}

TEST_GROUP_RUNNER( Full_WiFi )
{
    /* Happy path tests. */
    RUN_TEST_CASE( Full_WiFi, WiFiOnOff );
    RUN_TEST_CASE( Full_WiFi, WiFiMode );
    RUN_TEST_CASE( Full_WiFi, WiFiConnectionLoop );
    RUN_TEST_CASE( Full_WiFi, WiFiNetworkAddGetDelete );
    RUN_TEST_CASE( Full_WiFi, WiFiPowerManagementMode )
    RUN_TEST_CASE( Full_WiFi, WiFiGetIP );
    RUN_TEST_CASE( Full_WiFi, WiFiGetMAC );
    RUN_TEST_CASE( Full_WiFi, WiFiGetHostIP );
    RUN_TEST_CASE( Full_WiFi, WiFiScan );
    RUN_TEST_CASE( Full_WiFi, WiFiReset );
    RUN_TEST_CASE( Full_WiFi, WiFiPing );
    RUN_TEST_CASE( Full_WiFi, WiFiIsConnected );
    RUN_TEST_CASE( Full_WiFi, WiFiConnectMulitpleAP );
    RUN_TEST_CASE( Full_WiFi, WiFiSeperateTasksConnectingAndDisconnectingAtOnce );
    RUN_TEST_CASE( Full_WiFi, WiFiOnOffLoop );

    /* Null parameter tests. */
    RUN_TEST_CASE( Full_WiFi, WIFI_GetMode_NullParameters );
    RUN_TEST_CASE( Full_WiFi, WIFI_GetIP_NullParameters );
    RUN_TEST_CASE( Full_WiFi, WIFI_GetMAC_NullParameters );
    RUN_TEST_CASE( Full_WiFi, WIFI_GetHostIP_NullParameters );
    RUN_TEST_CASE( Full_WiFi, WIFI_Scan_NullParameters );
    RUN_TEST_CASE( Full_WiFi, WIFI_NetworkAdd_NullParameters );
    RUN_TEST_CASE( Full_WiFi, WIFI_NetworkGet_NullParameters );
    RUN_TEST_CASE( Full_WiFi, WIFI_SetPMMode_NullParameters );
    RUN_TEST_CASE( Full_WiFi, WIFI_GetPMMode_NullParameters );
    RUN_TEST_CASE( Full_WiFi, WIFI_Ping_NullParameters );
    RUN_TEST_CASE( Full_WiFi, WIFI_ConnectAP_NullParameters );

    /* Invalid parameters tests. */
    RUN_TEST_CASE( Full_WiFi, WIFI_SetMode_InvalidMode );
    RUN_TEST_CASE( Full_WiFi, WIFI_GetHostIP_InvalidDomainName );
    RUN_TEST_CASE( Full_WiFi, WIFI_GetHostIP_DomainNameLengthExceeded );
    RUN_TEST_CASE( Full_WiFi, WIFI_NetworkDelete_DeleteNonExistingNetwork );
    RUN_TEST_CASE( Full_WiFi, WIFI_NetworkGetNonExistingNetwork );
    RUN_TEST_CASE( Full_WiFi, WIFI_SetPMMode_InvalidPMMode );
    RUN_TEST_CASE( Full_WiFi, WIFI_Ping_ZeroParameters );
    RUN_TEST_CASE( Full_WiFi, WIFI_ConnectAP_InvalidSSID );
    RUN_TEST_CASE( Full_WiFi, WIFI_ConnectAP_InvalidPassword );
    RUN_TEST_CASE( Full_WiFi, WIFI_ConnectAP_InvalidSecurityTypes );
    RUN_TEST_CASE( Full_WiFi, WIFI_ConnectAP_MaxSSIDLengthExceeded );
    RUN_TEST_CASE( Full_WiFi, WIFI_ConnectAP_MaxPasswordLengthExceeded );
    RUN_TEST_CASE( Full_WiFi, WIFI_ConnectAP_ZeroLengthSSID );
    RUN_TEST_CASE( Full_WiFi, WIFI_ConnectAP_ZeroLengthPassword );

    /* Stability tests. */
    RUN_TEST_CASE( Full_WiFi, WIFI_ConnectAP_PasswordLengthLess );
    RUN_TEST_CASE( Full_WiFi, WIFI_Scan_ZeroScanNumber );
    RUN_TEST_CASE( Full_WiFi, WIFI_NetworkGet_GetManyNetworks );
    RUN_TEST_CASE( Full_WiFi, WIFI_NetworkAdd_AddManyNetworks );
    RUN_TEST_CASE( Full_WiFi, WIFI_NetworkDelete_DeleteManyNetworks );
    RUN_TEST_CASE( Full_WiFi, WIFI_ConnectAP_ConnectAllChannels );

    #if ( testwifiENABLE_CONFIGURE_AP_TESTS == 1 )
        RUN_TEST_CASE( Full_WiFi, WiFiConfigureAP );
        RUN_TEST_CASE( Full_WiFi, WIFI_ConfigureAP_NullParameters );
        RUN_TEST_CASE( Full_WiFi, WIFI_ConfigureAP_InvalidSecurityType );
        RUN_TEST_CASE( Full_WiFi, WIFI_ConfigureAP_NullParameters );
        RUN_TEST_CASE( Full_WiFi, WIFI_ConfigureAP_MaxSSIDLengthExceeded );
        RUN_TEST_CASE( Full_WiFi, WIFI_ConfigureAP_MaxPasswordLengthExceeded );
        RUN_TEST_CASE( Full_WiFi, WIFI_ConfigureAP_ZeroLengthSSID );
        RUN_TEST_CASE( Full_WiFi, WIFI_ConfigureAP_ZeroLengthPassword );
        RUN_TEST_CASE( Full_WiFi, WIFI_ConfigureAP_ConfigureAllChannels );
    #endif

    prvFinishWiFiTesting();
}

/**
 * @brief Turn the Wi-Fi off then on in a loop and verify success. The Wi-Fi should be ON
 * after this test is finished.
 */
TEST( Full_WiFi, WiFiOnOffLoop )
{
    WIFIReturnCode_t xWiFiStatus;
    uint32_t ulIndex;

    if( TEST_PROTECT() ) 
    {
        for( ulIndex = 0 ; ulIndex < testwifiCONNECTION_LOOP_TIMES ; ++ulIndex )
        {
            xWiFiStatus = WIFI_Off();
            TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

            xWiFiStatus = WIFI_On();
            TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
        }
    }
    else
    {
        TEST_FAIL();
    }
}



/**
 * @brief A single happy path case of turning the Wi-Fi off, on, then connecting to the
 * AP and disconnecting. Verify API results.
 */
TEST( Full_WiFi, WiFiOnOff )
{
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWiFiStatus;

    prvSetClientNetworkParameters( &xNetworkParams );

    if( TEST_PROTECT() ) 
    {
        xWiFiStatus = WIFI_Off();
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        if( eWiFiSuccess == xWiFiStatus )
        {
            xWiFiStatus = WIFI_On();
            TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
            
            xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
            TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

            vTaskDelay( testwifiCONNECTION_DELAY );

            xWiFiStatus = WIFI_Disconnect();
            TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_GetMode() and WIFI_SetMode() with each of the available 
 * Wi-Fi modes; verify the API return status.
 */
TEST( Full_WiFi, WiFiMode )
{
    WIFIDeviceMode_t xWiFiDeviceMode;
    WIFIReturnCode_t xWiFiStatus;

    /* Valid Modes ----------------------------------------------------------------- */

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetMode( &xWiFiDeviceMode );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        xWiFiStatus = WIFI_SetMode( eWiFiModeAP );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        xWiFiStatus = WIFI_SetMode( eWiFiModeP2P );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        xWiFiStatus = WIFI_SetMode( eWiFiModeStation );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        xWiFiStatus = WIFI_SetMode( xWiFiDeviceMode );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else 
    {
        TEST_FAIL();
    }
    
}

/**
 * @brief Call WIFI_GetMode() with Null parameterrs and verify failure. 
 */
TEST( Full_WiFi, WIFI_GetMode_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;
    
    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetMode( NULL );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_SetMode() with an invalid mode and verify failure. 
 */
TEST( Full_WiFi, WIFI_SetMode_InvalidMode )
{
    WIFIReturnCode_t xWiFiStatus;
    
    if(TEST_PROTECT() ) 
    {
        xWiFiStatus = WIFI_SetMode( eWiFiModeNotSupported );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Connect and disconnect the wireless AP testwifiCONNECTION_LOOP_TIMES and
 * verify the return status each time.
 */
TEST( Full_WiFi, WiFiConnectionLoop )
{
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWiFiStatus;
    int16_t sConnectLoop;

    xNetworkParams.pcSSID = clientcredentialWIFI_SSID;
    xNetworkParams.ucSSIDLength = sizeof( clientcredentialWIFI_SSID );
    xNetworkParams.pcPassword = clientcredentialWIFI_PASSWORD;
    xNetworkParams.ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD );
    xNetworkParams.xSecurity = clientcredentialWIFI_SECURITY;

    /* Try connect and disconnect several times. Reconnect after the test. */
    if( TEST_PROTECT() )
    {
        for( sConnectLoop = 0 ; sConnectLoop < testwifiCONNECTION_LOOP_TIMES ; ++sConnectLoop )
        {
            xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
            TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

            vTaskDelay( testwifiCONNECTION_DELAY );

            TEST_ASSERT( prvLoopBackTest() == pdPASS );

            xWiFiStatus = WIFI_Disconnect();
            TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

            vTaskDelay( testwifiCONNECTION_DELAY );
        }

        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Exercise WIFI_GetIP() and verify the return status.
 */
TEST( Full_WiFi, WiFiGetIP )
{
    uint8_t ucIPAddr[ 4 ];
    WIFIReturnCode_t xWiFiStatus;

    memset( ucIPAddr, 0, sizeof( ucIPAddr ) );
    
    /* Connect to the AP. */
    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        /* Acquire the IP address. */
        xWiFiStatus = WIFI_GetIP( ucIPAddr );

        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        /* Assert that the IP address is found. */
        TEST_ASSERT( *( ( uint32_t * ) ucIPAddr ) != 0 );
    }
    else 
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_GetIP() with NULL parameters and verify failure.
 */
TEST( Full_WiFi, WIFI_GetIP_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;
    
    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetIP( NULL );

        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Exercise WIFI_GetMAC and verify the return status.
 * 
 * The MAC address returned is checked to be non-zero. Checking for manufacturer
 * addresses given the unknown set of network interface vendors is infeasible.
 */
TEST( Full_WiFi, WiFiGetMAC )
{
    uint8_t ucMacAddressVal[ testwifiMAC_ADDRESS_LENGTH ];
    WIFIReturnCode_t xWiFiStatus;
    uint32_t ulIndex;
    uint32_t ulSum = 0;

    memset( ucMacAddressVal, 0, sizeof(ucMacAddressVal) );

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetMAC( ucMacAddressVal );

        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        for( ulIndex = 0; ulIndex < testwifiMAC_ADDRESS_LENGTH; ulIndex++ )
        {
            ulSum += ucMacAddressVal[ulIndex];
        }

        TEST_ASSERT( ulSum != 0 );
    }
    else 
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_GetMAC() with NULL parameters and verify the return status.
 */
TEST( Full_WiFi, WIFI_GetMAC_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetMAC( NULL );
        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Exercise WIFI_GetHostIP and verify the return status.
 */
TEST( Full_WiFi, WiFiGetHostIP )
{
    uint8_t ucIPAddr[ 4 ];
    WIFIReturnCode_t xWiFiStatus;

    memset( ucIPAddr, 0, sizeof(ucIPAddr) );

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetHostIP( testwifiTEST_DOMAIN_NAME, ucIPAddr );

        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        /* Assert that the IP address is found. */
        TEST_ASSERT( *( ( uint32_t * ) ucIPAddr ) != 0 );
    }
    else 
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_GetHOSTIP with NULL parameters and verify the return status.
 */
TEST( Full_WiFi, WIFI_GetHostIP_NullParameters )
{
    uint8_t ucIPAddr[ 4 ];
    WIFIReturnCode_t xWiFiStatus;

    memset( ucIPAddr, 0, sizeof(ucIPAddr) );

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetHostIP( testwifiTEST_DOMAIN_NAME, NULL );
        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetHostIP( NULL, ucIPAddr );
        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_GetHostIP with the invalid domain name defined in testwifiTEST_INVALID_DOMAIN_NAME
 * and verify failure.
 */
TEST( Full_WiFi, WIFI_GetHostIP_InvalidDomainName )
{
    uint8_t ucIPAddr[ 4 ];
    WIFIReturnCode_t xWiFiStatus;

    memset( ucIPAddr, 0, sizeof(ucIPAddr) );

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetHostIP( testwifiTEST_INVALID_DOMAIN_NAME, ucIPAddr );

        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );

        /* Assert that the IP address is NOT found. */
        TEST_ASSERT_EQUAL_INT32( *( ( uint32_t * ) ucIPAddr ), 0 ); 
    }
}

/**
 * @brief Call WIFI_GetHostIP with a domain name that exceeds the standard maximum 
 * domain name length of 253 and verify failure.
 */
TEST( Full_WiFi, WIFI_GetHostIP_DomainNameLengthExceeded )
{
    uint8_t ucIPAddr[ 4 ];
    WIFIReturnCode_t xWiFiStatus;
    /* 256 slots: 0 .. 255 */
    char domainNameLengthExceeded[ securesocketsMAX_DNS_NAME_LENGTH + 3 ];
    
    memset( ucIPAddr, 0, sizeof(ucIPAddr) );

    TEST_ASSERT( prvConnectAPTest() == pdPASS );

    /* Setting 0 .. 254 to 'x'. */
    memset( domainNameLengthExceeded, 'x', sizeof( domainNameLengthExceeded ) );
    
    /* Setting 255 to null termination. */
    domainNameLengthExceeded[ securesocketsMAX_DNS_NAME_LENGTH + 2] = '\0';
    
    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetHostIP( testwifiTEST_INVALID_DOMAIN_NAME, ucIPAddr );

        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );

        /* Assert that the IP address is NOT found. */
        TEST_ASSERT_EQUAL_INT32( *( ( uint32_t * ) ucIPAddr ), 0 ); 
    }
}

/**
 * @brief Exercise WIFI_Scan() and verify the return status.
 */
TEST( Full_WiFi, WiFiScan )
{
    WIFIScanResult_t xScanResults[ testwifiMAX_SCAN_NUMBER ];
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_Scan( xScanResults, testwifiMAX_SCAN_NUMBER );
        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_Scan() with NULL parameters and verify failure.
 */
TEST( Full_WiFi, WIFI_Scan_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_Scan( NULL, testwifiMAX_SCAN_NUMBER );
        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_Scan() to scan for zero total networks and verify stability.
 */
TEST( Full_WiFi, WIFI_Scan_ZeroScanNumber )
{
    WIFIScanResult_t xScanResults[ testwifiMAX_SCAN_NUMBER ];

    if( TEST_PROTECT() )
    {
        /* Ports are allowed to decide to return success or failure depending on their 
         * driver for a scan number of zero. */
        WIFI_Scan( xScanResults, 0 );
    }
}

/**
 * @brief Single test of adding a Wi-Fi network, getting it, then delete it; verify the
 * return status.
 */
TEST( Full_WiFi, WiFiNetworkAddGetDelete )
{
    WIFINetworkProfile_t xNetworkProfile;
    WIFIReturnCode_t xWiFiStatus;
    uint16_t usIndex;

    prvSetNetworkProfile( &xNetworkProfile );

    if( TEST_PROTECT() )
    {
        /* Delete all the networks first. */
        for( usIndex = 0; usIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usIndex++ )
        {
            WIFI_NetworkDelete( usIndex );
        }

        /* Add a network. */
        xWiFiStatus = WIFI_NetworkAdd( &xNetworkProfile, &usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        /* Reset the network profile variable so that we can verify if we get the one that was just added. */
        memset( &xNetworkProfile, 0, sizeof( WIFINetworkProfile_t ) );

        /* Get a network. */
        xWiFiStatus = WIFI_NetworkGet( &xNetworkProfile, usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        /* If successful, then implemented. So check network parameters are what was just added. */
        if ( xWiFiStatus == eWiFiSuccess )
        {
            TEST_ASSERT_EQUAL_INT32( sizeof( clientcredentialWIFI_SSID ), xNetworkProfile.ucSSIDLength );
            TEST_ASSERT_EQUAL_INT32( 0, strncmp( xNetworkProfile.cSSID, clientcredentialWIFI_SSID, sizeof( clientcredentialWIFI_SSID ) ) );
            TEST_ASSERT_EQUAL_INT32( clientcredentialWIFI_SECURITY, xNetworkProfile.xSecurity );
        }

        /* Delete a network. */
        xWiFiStatus = WIFI_NetworkDelete( usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Call WIFI_NetworkAdd() with NULL parameters and verify failure.
 */
TEST( Full_WiFi, WIFI_NetworkAdd_NullParameters )
{
    WIFINetworkProfile_t xNetworkProfile;
    WIFIReturnCode_t xWiFiStatus;
    uint16_t usIndex;

    prvSetNetworkProfile( &xNetworkProfile );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_NetworkAdd( &xNetworkProfile, NULL );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_NetworkAdd( NULL, &usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_NetworkGet() with NULL parameters and verify failure.
 */
TEST( Full_WiFi, WIFI_NetworkGet_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_NetworkGet( NULL, 0 );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_NetworkDelete() on a network that doesn't exist and verify 
 * success.
 */
TEST( Full_WiFi, WIFI_NetworkDelete_DeleteNonExistingNetwork )
{
    WIFINetworkProfile_t xNetworkProfile;
    WIFIReturnCode_t xWiFiStatus;
    uint16_t usIndex;

    prvSetNetworkProfile( &xNetworkProfile );

    if( TEST_PROTECT() )
    {
        /* Delete all the networks first. */
        for( usIndex = 0; usIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usIndex++ )
        {
            WIFI_NetworkDelete( usIndex );
        }

        /* Add a network. */
        xWiFiStatus = WIFI_NetworkAdd( &xNetworkProfile, &usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        /* Delete the network. */
        xWiFiStatus = WIFI_NetworkDelete( usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        /* Delete non-existing network. */
        xWiFiStatus = WIFI_NetworkDelete( usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_NetworkGet() on a non-existing network and verify failure.
 */
TEST( Full_WiFi, WIFI_NetworkGetNonExistingNetwork )
{
    WIFINetworkProfile_t xNetworkProfile;
    WIFIReturnCode_t xWiFiStatus;
    uint16_t usIndex;

    prvSetNetworkProfile( &xNetworkProfile );

    if ( TEST_PROTECT() )
    {
        /* Delete all the networks first. */
        for( usIndex = 0; usIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usIndex++ )
        {
            WIFI_NetworkDelete( usIndex );
        }

        /* Add a network. */
        xWiFiStatus = WIFI_NetworkAdd( &xNetworkProfile, &usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        /* Delete the network. */
        xWiFiStatus = WIFI_NetworkDelete( usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }

    if( TEST_PROTECT() )
    {
        /* Get non-existing network. */
        xWiFiStatus = WIFI_NetworkGet( &xNetworkProfile, usIndex );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess != xWiFiStatus, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_NetworkGet() over the maximum network save number. 
 */
TEST( Full_WiFi, WIFI_NetworkGet_GetManyNetworks )
{
    WIFIReturnCode_t xWiFiStatus;
    WIFINetworkProfile_t xNetworkProfile;
    uint16_t usIndex;
    
    if( TEST_PROTECT() )
    {   
        /* Delete all the networks first. */
        for( usIndex = 0; usIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usIndex++ )
        {
            WIFI_NetworkDelete( usIndex );
        }

        for( usIndex = 0; usIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usIndex++ )
        {
            xWiFiStatus = WIFI_NetworkGet( &xNetworkProfile, usIndex );
            TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess != xWiFiStatus, xWiFiStatus );
        }
    }
}

/**
 * @brief Call WIFI_NetworkAdd() over the maximum network save number.
 * 
 * The number of networks allowed to be saved or if at all vary by platform. Therefore, the
 * success of WIFI_NetworkGet() and WIFI_NetworkAdd() is not checked. Instead only the network
 * returned on success is checked to be correct.
 */
TEST( Full_WiFi, WIFI_NetworkAdd_AddManyNetworks )
{
    WIFIReturnCode_t xWiFiStatus;
    WIFINetworkProfile_t xNetworkProfile;
    uint16_t usNetworkIndex;
    uint16_t usSavedNetworkIndex;
    char pcNetworkSSID[ wificonfigMAX_SSID_LEN + 1 ];
    int lCompResults;
    int lNetworkSSIDLength;

    /* Set some network profile fields. */
    prvSetNetworkProfile( &xNetworkProfile );

    if ( TEST_PROTECT() )
    {
        /* Delete all the networks first. */
        for( usNetworkIndex = 0; usNetworkIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usNetworkIndex++ )
        {
            WIFI_NetworkDelete( usNetworkIndex );
        }
        
        /* Add networks in increasing post-fix order in the SSID string. */
        for( usNetworkIndex = 0; usNetworkIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usNetworkIndex++ )
        {
            /* Create the network SSID to verify was added. */
            lNetworkSSIDLength = sprintf( pcNetworkSSID, "network%d",  usNetworkIndex );
            pcNetworkSSID[ lNetworkSSIDLength ] = '\0';
            strcpy( xNetworkProfile.cSSID, pcNetworkSSID );
            xNetworkProfile.ucSSIDLength = lNetworkSSIDLength;
            
            /* Add the network profile. */
            xWiFiStatus = WIFI_NetworkAdd( &xNetworkProfile, &usSavedNetworkIndex );
        }

        /* Verify added networks */
        for( usNetworkIndex = 0; usNetworkIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usNetworkIndex++ )
        {
            /* Create the network SSID to verify was added. */
            lNetworkSSIDLength = sprintf( pcNetworkSSID, "network%d",  usNetworkIndex );
            pcNetworkSSID[ lNetworkSSIDLength ] = '\0';

            /* Clear the network profile from the last round. */
            memset( &xNetworkProfile, 0, sizeof( WIFINetworkProfile_t ) );

            /* Get the network profile. */
            xWiFiStatus = WIFI_NetworkGet( &xNetworkProfile, usNetworkIndex );
            
            /* The board may not support the defined maximum networks saved so we need to check if success
             * was returned from the last function before we check the saved network. */
            if ( eWiFiSuccess == xWiFiStatus )
            {
                /* Verify the network profile SSID. The returned network SSID may not be null terminated. */
                lCompResults = strncmp( xNetworkProfile.cSSID, pcNetworkSSID, lNetworkSSIDLength );
                TEST_ASSERT_EQUAL_INT32( 0, lCompResults );
            }
        }
    }
}

/**
 * @brief Call WIFI_NetworkDelete over the maximum network save number. This verifies
 * stability.
 */
TEST( Full_WiFi, WIFI_NetworkDelete_DeleteManyNetworks )
{
    uint16_t usIndex;

    if( TEST_PROTECT() )
    {
        for( usIndex = 0; usIndex < testwifiMAX_NETWORK_SAVE_NUMBER; usIndex++ )
        {
            WIFI_NetworkDelete( usIndex );
        }
    }
}

/**
 * @brief Exercise the WIFI_SetPMMode() and WIFI_GetPMMode() APIs for each of 
 * the available modes and verify the return status.
 */
TEST( Full_WiFi, WiFiPowerManagementMode )
{
    WIFIPMMode_t xPMMode;
    WIFIReturnCode_t xWiFiStatus;
    uint32_t ulOptionValue = 0;

    if ( TEST_PROTECT() )
    {
        xPMMode = eWiFiPMLowPower;
        xWiFiStatus = WIFI_SetPMMode( xPMMode, ( void * )&ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
        xWiFiStatus = WIFI_GetPMMode( &xPMMode, ( void * )&ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API( ( eWiFiSuccess == xWiFiStatus && eWiFiPMLowPower == xPMMode ), xWiFiStatus );

        xPMMode = eWiFiPMAlwaysOn;
        xWiFiStatus = WIFI_SetPMMode( xPMMode, ( void * )&ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
        xWiFiStatus = WIFI_GetPMMode( &xPMMode, ( void * )&ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API( ( eWiFiSuccess == xWiFiStatus && eWiFiPMAlwaysOn == xPMMode ), xWiFiStatus );

        xPMMode = eWiFiPMNormal;
        xWiFiStatus = WIFI_SetPMMode( xPMMode, ( void * )&ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
        xWiFiStatus = WIFI_GetPMMode( &xPMMode, ( void * )&ulOptionValue );
        TEST_WIFI_ASSERT_OPTIONAL_API( ( eWiFiSuccess == xWiFiStatus && eWiFiPMNormal == xPMMode ), xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }    
}

/**
 * @brief Call WIFI_SetPMMode() with NULL parameters.
 */
TEST( Full_WiFi, WIFI_SetPMMode_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_SetPMMode( eWiFiPMNormal, NULL);
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief call WIFI_GetPMMode() with NULL parameters.
 */
TEST( Full_WiFi, WIFI_GetPMMode_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;
    WIFIPMMode_t xPMMode;
    uint32_t ulOptionValue = 0;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetPMMode( &xPMMode, NULL);
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_GetPMMode( NULL, ( void *)&ulOptionValue);
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief call WIFI_SetPMMode() with an invalid power mode and verify failure.
 */
TEST( Full_WiFi, WIFI_SetPMMode_InvalidPMMode )
{
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_SetPMMode( eWiFiPMNotSupported, NULL );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Test to configure the Software Access Point and verify the return status of the
 * API.
 * 
 * @note This test can only be verified manually. Set a breakpoint after WIFI_ConfigureAP,
 * which will block for some time. Connect to the Wi-Fi network created by it on your
 * phone and configure. You should expect it to return eWiFiSuccess. 
 */
TEST( Full_WiFi, WiFiConfigureAP )
{
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWiFiStatus;

    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }

}

/**
 * @brief Call WIFI_ConfigureAP with NULL parameters and verify failure.
 */
TEST( Full_WiFi, WIFI_ConfigureAP_NullParameters )
{   
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWiFiStatus;

    /* Null Network Parameters */
    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( NULL);
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );

    /* Null SSID */
    
    xNetworkParams.pcSSID = NULL;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
    
    /* Null password. */

    xNetworkParams.pcSSID = wificonfigACCESS_POINT_SSID_PREFIX;
    xNetworkParams.pcPassword = NULL;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    /* Null password for open security network. */

    xNetworkParams.xSecurity = eWiFiSecurityOpen;
    
    if( TEST_PROTECT() )
    {
        /* Some ports allow this some don't. */
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Configure the AP with an invalid security type an verify failure.
 */
TEST( Full_WiFi, WIFI_ConfigureAP_InvalidSecurityType )
{
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWiFiStatus;

    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );

    xNetworkParams.xSecurity = eWiFiSecurityNotSupported;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Configure the SoftAP with a SSID exceeding the maximum SSID length and verify
 * failure.
 */
TEST( Full_WiFi, WIFI_ConfigureAP_MaxSSIDLengthExceeded )
{
    char cLengthExceedingSSID[ wificonfigMAX_SSID_LEN + 2 ];
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWiFiStatus;
    
    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );

    /* Set some SSID that exceeds the max length. */
    memset( cLengthExceedingSSID, 'x', wificonfigMAX_SSID_LEN + 1 );
    cLengthExceedingSSID[ wificonfigMAX_SSID_LEN + 1 ] = '\0';

    xNetworkParams.pcSSID = cLengthExceedingSSID;
    xNetworkParams.ucSSIDLength = sizeof( cLengthExceedingSSID );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Configure the SoftAP with a password exceeding the maximum password length and
 * verify failure.
 */
TEST( Full_WiFi, WIFI_ConfigureAP_MaxPasswordLengthExceeded )
{
    char cLengthExceedingPassword[ wificonfigMAX_PASSPHRASE_LEN + 2 ];
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWiFiStatus;
    
    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );

    /* Set a password that exceeds the max length. */
    memset( cLengthExceedingPassword, 'x', wificonfigMAX_PASSPHRASE_LEN + 1 );
    cLengthExceedingPassword[ wificonfigMAX_PASSPHRASE_LEN + 1 ] = '\0';

    xNetworkParams.pcPassword = cLengthExceedingPassword;
    xNetworkParams.ucPasswordLength = sizeof( cLengthExceedingPassword );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConfigureAP( &xNetworkParams );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Configure the SoftAP with a zero length SSID and verify stability.
 */
TEST( Full_WiFi, WIFI_ConfigureAP_ZeroLengthSSID )
{
    WIFINetworkParams_t xNetworkParams;
    
    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );

    /* Set a zero length SSID */
    xNetworkParams.ucSSIDLength = 0;

    if( TEST_PROTECT() )
    {
        WIFI_ConfigureAP( &xNetworkParams );
    }
}

/**
 * @brief Configure the SoftAP with a zero length password and verify stability.
 */
TEST( Full_WiFi, WIFI_ConfigureAP_ZeroLengthPassword )
{
    WIFINetworkParams_t xNetworkParams;
    
    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );
    
    /* Set zero length password parameter */
    xNetworkParams.ucPasswordLength = 0;

    if( TEST_PROTECT() )
    {
        WIFI_ConfigureAP( &xNetworkParams );
    }
}


/**
 * @brief Configure the SoftAP over all channels specified in testwifiMAX_CHANNEL_NUMBER
 * and verify stability.
 */
TEST( Full_WiFi, WIFI_ConfigureAP_ConfigureAllChannels )
{  
    WIFINetworkParams_t xNetworkParams;
    uint32_t ulIndex;
    
    /* Set the network parameters with valid parameters */
    prvSetSoftAPNetworkParameters( &xNetworkParams );

    if( TEST_PROTECT() )
    {
        for( ulIndex = 0; ulIndex < testwifiMAX_CHANNEL_NUMBER; ulIndex++ )
        {   
            xNetworkParams.cChannel = ulIndex;
            WIFI_ConfigureAP( &xNetworkParams );
        }
    }
}


/**
 * @brief Exercise WIFI_Reset and verify a success.
 */
TEST( Full_WiFi, WiFiReset )
{
    WIFIReturnCode_t xWiFiStatus;

    xWiFiStatus = WIFI_Reset();
    TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );
}

/**
 * @brief Exercise WIFI_Ping() and verify a success.
 */
TEST( Full_WiFi, WiFiPing )
{
    WIFIReturnCode_t xWiFiStatus;
    uint32_t ulPingAddress = testwifiPING_ADDRESS;
    
    xWiFiStatus = WIFI_Ping( ( uint8_t* )&ulPingAddress, testwifiPING_COUNT, testwifiPING_INTERVAL_MS );
    TEST_WIFI_ASSERT_OPTIONAL_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

}

/**
 * @brief Call WIFI_Ping() with NULL parameters and verify failure.
 */
TEST( Full_WiFi, WIFI_Ping_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_Ping( NULL, testwifiPING_COUNT, testwifiPING_INTERVAL_MS );
        TEST_WIFI_ASSERT_OPTIONAL_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
}

/**
 * @brief Call WIFI_Ping() with zero for the ping interval and zero for the ping count
 * and verify stability.
 */
TEST( Full_WiFi, WIFI_Ping_ZeroParameters )
{
    uint32_t ulPingAddress = testwifiPING_ADDRESS;

    if( TEST_PROTECT() )
    {
        WIFI_Ping( ( uint8_t* )&ulPingAddress, testwifiPING_COUNT, 0 );
    }

    if( TEST_PROTECT() )
    {
        WIFI_Ping( ( uint8_t* )&ulPingAddress, 0, testwifiPING_INTERVAL_MS );
    }
}

/**
 * @brief Test WIFI_IsConnected() after calling WIFI_ConnectAP() and 
 * WIFI_DisconnectAP() and verify success.
 */
TEST( Full_WiFi, WiFiIsConnected )
{
    WIFINetworkParams_t xNetworkParams;
    BaseType_t xIsConnected;
    WIFIReturnCode_t xWiFiStatus;

    prvSetClientNetworkParameters( &xNetworkParams );
    
    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );

        vTaskDelay( testwifiCONNECTION_DELAY );
        
        xIsConnected = WIFI_IsConnected();
        TEST_WIFI_ASSERT_REQUIRED_API( pdTRUE == xIsConnected, xWiFiStatus );

        /* Confirm if we are truly connected with the loopback test. */
        TEST_ASSERT( prvLoopBackTest() == pdPASS );

        xWiFiStatus = WIFI_Disconnect();
        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess == xWiFiStatus, xWiFiStatus );

        vTaskDelay( testwifiCONNECTION_DELAY );

        xIsConnected = WIFI_IsConnected();
        TEST_WIFI_ASSERT_REQUIRED_API( pdFALSE == xIsConnected, xIsConnected );

        TEST_ASSERT( prvLoopBackTest() != pdPASS );
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Test WIFI_ConnectAP() with null parameters. It is expected that null 
 * parameters will go into an assert or fail.
 */
TEST( Full_WiFi, WIFI_ConnectAP_NullParameters )
{
    WIFIReturnCode_t xWiFiStatus;
    WIFINetworkParams_t xNetworkParams;
    
    /* Null network params structure. */
    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( NULL );
        TEST_WIFI_ASSERT_REQUIRED_API( eWiFiSuccess != xWiFiStatus, xWiFiStatus );
    }

    /* Null SSID */
    prvSetClientNetworkParameters( &xNetworkParams );
    xNetworkParams.pcSSID = NULL;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }
    
    /* Test a null password when it is needed. */
    prvSetClientNetworkParameters( &xNetworkParams );
    xNetworkParams.pcPassword = NULL;

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
        TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    }

    /* Test a null password when it is not needed. */
    xNetworkParams.xSecurity = eWiFiSecurityOpen;
    
    if( TEST_PROTECT() )
    {
        /* Every port will implement this differently. */
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
    }
}

/**
 * @brief Test WIFI_ConnectAP() with an invalid SSID.
 * 
 * @note This test may fail for an valid SSID but invalid/valid password if there exists 
 * a Access point, in range, with the same SSID but an Open security type. 
 */
TEST( Full_WiFi, WIFI_ConnectAP_InvalidPassword )
{
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWiFiStatus;
    BaseType_t xIsConnected;

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    /* Set the invalid password. */
    xNetworkParams.pcPassword = testwifiINVALID_WIFI_PASSWORD;
    xNetworkParams.ucPasswordLength = sizeof( testwifiINVALID_WIFI_PASSWORD );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
    }
    xIsConnected = WIFI_IsConnected();
    
    if ( ( xIsConnected == pdFALSE ) && ( xWiFiStatus == eWiFiSuccess ) )
    {
        TEST_FAIL_MESSAGE( "WIFI_ConnectAP() returned success, but failed to connect." );
    }
    
    TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    TEST_WIFI_ASSERT_REQUIRED_API( pdFALSE == xIsConnected, xIsConnected );
}

/**
 * @brief Call WIFI_ConnectAP() with an invalid SSID and verify failure.
 */
TEST( Full_WiFi, WIFI_ConnectAP_InvalidSSID )
{
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWiFiStatus;
    BaseType_t xIsConnected;
    
    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    /* Set the invalid SSID. */
    xNetworkParams.pcSSID = testwifiINVALID_WIFI_SSID;
    xNetworkParams.ucSSIDLength = sizeof( testwifiINVALID_WIFI_SSID );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
    }
    xIsConnected = WIFI_IsConnected();
    
    if ( ( xIsConnected == pdFALSE ) && ( xWiFiStatus == eWiFiSuccess ) )
    {
        TEST_FAIL_MESSAGE( "WIFI_ConnectAP() returned success, but failed to connect." );
    }
    
    TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    TEST_WIFI_ASSERT_REQUIRED_API( pdFALSE == xIsConnected, xIsConnected );
}

/**
 * @brief Call WIFI_ConnectAP() with invalid security types and verify stability.
 */
TEST( Full_WiFi, WIFI_ConnectAP_InvalidSecurityTypes )
{
    WIFINetworkParams_t xNetworkParams;

    /* Get valid client parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    /* Set the user defined invalid security type. */
    xNetworkParams.xSecurity = testwifiINVALID_WIFI_SECURITY;

    if( TEST_PROTECT() )
    {
        /* It is allowed that some ports will infer the security type from the Wi-Fi
         * scan. */
        WIFI_ConnectAP( &xNetworkParams );
    }
    
    /* Truly invalid security type. */
    xNetworkParams.xSecurity = eWiFiSecurityNotSupported;

    if( TEST_PROTECT() )
    {
        /* It is allowed that some ports will infer the security type from the Wi-Fi
         * scan. */
        WIFI_ConnectAP( &xNetworkParams );
    }
}

/**
 * @brief Call WIFI_ConnectAP() with valid credentials over all channels and verify 
 * stability.
 */
TEST( Full_WiFi, WIFI_ConnectAP_ConnectAllChannels )
{
    WIFINetworkParams_t xNetworkParams;
    uint32_t ulIndex;

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    if( TEST_PROTECT() )
    {
        for( ulIndex = 0; ulIndex < testwifiMAX_CHANNEL_NUMBER; ulIndex++ )
        {   
            xNetworkParams.cChannel = ulIndex;
            WIFI_ConnectAP( &xNetworkParams );
        }
    }
}

/**
 * @brief Call WIFI_ConnectAP() with an SSID that exceeds the maximum SSID length and 
 * verify failure.
 */
TEST( Full_WiFi, WIFI_ConnectAP_MaxSSIDLengthExceeded )
{
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWiFiStatus;
    BaseType_t xIsConnected;
    char cLengthExceedingSSID[ wificonfigMAX_SSID_LEN + 2 ];

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    memset( cLengthExceedingSSID, 'x', wificonfigMAX_SSID_LEN + 1 );
    cLengthExceedingSSID[ wificonfigMAX_SSID_LEN + 1 ] = '\0';

    /* Set the SSID exceeding the max length */
    xNetworkParams.pcSSID = cLengthExceedingSSID;
    xNetworkParams.ucSSIDLength = sizeof( cLengthExceedingSSID );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
    }
    
    xIsConnected = WIFI_IsConnected();
    
    if ( ( xIsConnected == pdFALSE ) && ( xWiFiStatus == eWiFiSuccess ) )
    {
        TEST_FAIL_MESSAGE( "WIFI_ConnectAP() returned success, but failed to connect." );
    }
    
    TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    TEST_WIFI_ASSERT_REQUIRED_API( pdFALSE == xIsConnected, xIsConnected );
}

/**
 * @brief Call WIFI_ConnectAP() with a password that exceeds the maximum password length
 * and verify failure.
 */
TEST( Full_WiFi, WIFI_ConnectAP_MaxPasswordLengthExceeded )
{
    WIFINetworkParams_t xNetworkParams;
    WIFIReturnCode_t xWiFiStatus;
    BaseType_t xIsConnected;
    char cLengthExceedingPassword[ wificonfigMAX_PASSPHRASE_LEN + 2 ];

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    /* Set the password exceeding the max length. */
    memset( cLengthExceedingPassword, 'x', wificonfigMAX_PASSPHRASE_LEN + 1 );
    cLengthExceedingPassword[ wificonfigMAX_PASSPHRASE_LEN + 1 ] = '\0';

    xNetworkParams.pcPassword = cLengthExceedingPassword;
    xNetworkParams.ucPasswordLength = sizeof( cLengthExceedingPassword );

    if( TEST_PROTECT() )
    {
        xWiFiStatus = WIFI_ConnectAP( &xNetworkParams );
    }
    
    xIsConnected = WIFI_IsConnected();
    
    if ( ( xIsConnected == pdFALSE ) && ( xWiFiStatus == eWiFiSuccess ) )
    {
        TEST_FAIL_MESSAGE( "WIFI_ConnectAP() returned success, but failed to connect." );
    }
    
    TEST_WIFI_ASSERT_REQUIRED_API( xWiFiStatus != eWiFiSuccess, xWiFiStatus );
    TEST_WIFI_ASSERT_REQUIRED_API( pdFALSE == xIsConnected, xIsConnected );
}

/** 
 * @brief Attempt to connect to an AP with a valid SSID and with a specified length of 
 * zero and verify stability.
 */
TEST( Full_WiFi, WIFI_ConnectAP_ZeroLengthSSID )
{
    WIFINetworkParams_t xNetworkParams;

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    xNetworkParams.ucSSIDLength = 0;

    if( TEST_PROTECT() )
    {
        /* This may not return failure if the max SSID length is copied in the port. */
        WIFI_ConnectAP( &xNetworkParams );
    }
}

/**
 * @brief Attempt to connect to an AP with a valid password and with a specified length
 * of zero and verify stability.
 */
TEST( Full_WiFi, WIFI_ConnectAP_ZeroLengthPassword )
{
    WIFINetworkParams_t xNetworkParams;

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    xNetworkParams.ucPasswordLength = 0;
    
    if( TEST_PROTECT() )
    {
        /* This may not return faulure if the max passphrase length is copied in the port. */
        WIFI_ConnectAP( &xNetworkParams );
    }
}

/**
 * @brief Attempt to connect to an AP with a valid password and the specified password
 * length is less; verify stability.
 */
TEST( Full_WiFi, WIFI_ConnectAP_PasswordLengthLess )
{
    WIFINetworkParams_t xNetworkParams;

    /* Set the network parameters with valid parameters. */
    prvSetClientNetworkParameters( &xNetworkParams );

    /* Set a password less. */
    xNetworkParams.ucPasswordLength = sizeof( clientcredentialWIFI_PASSWORD ) - 1;

    if( TEST_PROTECT() )
    {
        /* May not return false. */
        WIFI_ConnectAP( &xNetworkParams );
    }
}

/**
 * @brief Connect to the Wi-Fi credentials in aws_clientcredential.h and the credentials
 * defined in this test over and over and verify we are still connected.
 */
TEST( Full_WiFi, WiFiConnectMulitpleAP )
{
    WIFINetworkParams_t xClientNetworkParams;
    WIFINetworkParams_t xTestNetworkParams;
    BaseType_t xIsConnected;
    BaseType_t xLoopbackResults;
    uint32_t ulIndex;

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xClientNetworkParams );

    /* Set the valid client parameters defined in this test. */
    prvSetTestNetworkParameters( &xTestNetworkParams );

    if( TEST_PROTECT() )
    {
        for( ulIndex = 0; ulIndex < testwifiCONNECTION_LOOP_TIMES; ulIndex++ )
        {
            WIFI_ConnectAP( &xTestNetworkParams );

            vTaskDelay( testwifiCONNECTION_DELAY );

            xIsConnected = WIFI_IsConnected();
            TEST_WIFI_ASSERT_REQUIRED_API( pdTRUE == xIsConnected, xIsConnected );

            WIFI_ConnectAP( &xClientNetworkParams );

            vTaskDelay( testwifiCONNECTION_DELAY );

            xIsConnected = WIFI_IsConnected();
            TEST_WIFI_ASSERT_REQUIRED_API_MSG( pdTRUE == xIsConnected, xIsConnected, "API is violated, we must connect to the new network.\r\n" );

            /* Perform the loop-back test to verify we are actually connected. The client credential network is
             * the network expected to have the echo server, since the tcp tests use that network. */
            xLoopbackResults = prvLoopBackTest();
            if ( xLoopbackResults == pdFAIL )
            {
                configPRINTF( ("Wi-Fi API claims to be connected to %s, but loopback test on iteration %d failed.\r\n", xClientNetworkParams.pcSSID, ulIndex ) );
            }
            TEST_ASSERT( xLoopbackResults == pdPASS );
        }
    }
    else
    {
        TEST_FAIL();
    }
}

/**
 * @brief Connection loop task.
 */
static void prvConnectionTask( void * pvParameters )
{
    WIFIReturnCode_t xWiFiStatus;
    testwifiTaskParams_t * pxTaskParams;
    WIFINetworkParams_t * pxNetworkParams;
    uint32_t ulIndex;
    BaseType_t xIsConnected;
    BaseType_t xLoopBackResults;
    /* The number of times task 0 connects last. */
    uint32_t task0ConnectLastCount = 0;
    /* There are only two tasks in this test. */
    const uint32_t ulNumTasks = 2;
    const uint32_t ulTaskFinishMask = ( ( 1 << ulNumTasks ) - 1 );

    /* If task0 has the higher count it finished last and the loopback test can be performed. This
       variable should not increment beyond 2. After calling WIFI_ConnectAP() and verifying the 
       connection, this variable shall be restarted. */
    static uint8_t taskCount = 0;
    static SemaphoreHandle_t xTaskCountBinarySem = NULL;
    static StaticSemaphore_t xSemaphoreBuffer;
    static const TickType_t xSemaphoreWaitTicks = pdMS_TO_TICKS( wificonfigMAX_SEMAPHORE_WAIT_TIME_MS );
    
    /* The event groups to synchronize task connection and disconnection. */
    static EventGroupHandle_t xTaskConnectSyncEventGroupHandle = NULL;
    static EventGroupHandle_t xTaskDisconnectSyncEventGroupHandle = NULL;
    
    pxTaskParams = ( testwifiTaskParams_t * )( pvParameters );
    pxNetworkParams = ( WIFINetworkParams_t * )( pxTaskParams->pvParameters );

    portENTER_CRITICAL( );

    /* One time initialization of the semaphore. */
    if( xTaskCountBinarySem == NULL )
    {
        xTaskCountBinarySem = xSemaphoreCreateBinaryStatic( &xSemaphoreBuffer );
        if( xTaskCountBinarySem != NULL )
        {
            xSemaphoreGive( xTaskCountBinarySem );
        }
        else
        {
            configPRINTF( ("Failed to create the semaphore.\r\n" ) );
            pxTaskParams->xWiFiStatus = eWiFiFailure;
        }
    }
    
    /* One time initialization of the task synchronization event groups. */
    if( xTaskConnectSyncEventGroupHandle == NULL )
    {
        xTaskConnectSyncEventGroupHandle = xEventGroupCreate();
        if( xTaskConnectSyncEventGroupHandle == NULL )
        {
            configPRINTF( ("Failed to create the task connection event group.\r\n" ) );
            pxTaskParams->xWiFiStatus = eWiFiFailure;
        }
    }

    if( xTaskDisconnectSyncEventGroupHandle == NULL )
    {
        xTaskDisconnectSyncEventGroupHandle = xEventGroupCreate();
        if( xTaskDisconnectSyncEventGroupHandle == NULL )
        {
            configPRINTF( ("Failed to create the task disconnection event group.\r\n" ) );
            pxTaskParams->xWiFiStatus = eWiFiFailure;
        }
    }
    
    portEXIT_CRITICAL();

    for( ulIndex = 0; ulIndex < testwifiCONNECTION_LOOP_TIMES && pxTaskParams->xWiFiStatus == eWiFiSuccess ; ulIndex++ )
    {

        /* Connect. */
        xWiFiStatus = WIFI_ConnectAP( pxNetworkParams );
        
        /* Delay. */
        vTaskDelay( testwifiCONNECTION_DELAY );

        /* Increment the task count. */
        if( xSemaphoreTake( xTaskCountBinarySem, xSemaphoreWaitTicks ) == pdTRUE )
        {
            taskCount++;
            xSemaphoreGive( xTaskCountBinarySem );
        }

        /* Are we connected? */
        xIsConnected = WIFI_IsConnected();
        if( xIsConnected == pdFALSE )
        {   
            configPRINTF( ("Task %d returned eWiFiSuccess from WIFI_ConnectAP(), but is not connected.\r\n",
                           pxTaskParams->usTaskId ) );
            xWiFiStatus = eWiFiFailure;
            break;
        }

        /* Perform a loop-back test to ensure we are connected. Only the task connected to the echo server can talk to it (task 0). */
        if ( taskCount == 2 && pxTaskParams->usTaskId == 0 )
        {
            task0ConnectLastCount++;
            configPRINTF( ( "Task 0 connected second %d times\r\n", task0ConnectLastCount ) );
            xLoopBackResults = prvLoopBackTest();
            if( xLoopBackResults == pdFALSE )
            {
                configPRINTF( ( "Task %d failed the loop-back test after connect.", pxTaskParams->usTaskId ) );
                break;
            }
        }

        /* Defensive. */
        if( taskCount > 2 )
        {
            configPRINTF( ( "Thread %d incremented the task-count to %d. \r\n", pxTaskParams->usTaskId, taskCount ) );
        }

        /* Wait for the other task. */
        if( xEventGroupSync( xTaskConnectSyncEventGroupHandle,            /* The event group used for the rendezvous. */
                             (0x1 << pxTaskParams->usTaskId ),            /* Set our task ID when we are done. */
                             ulTaskFinishMask,                            /* Wait for both our task and the other task. */
                             testwifiTASK_SYNC_TIMEOUT ) != ulTaskFinishMask )
        {
            break;
        }

        /* Restart the task counter. */
        if( xSemaphoreTake( xTaskCountBinarySem, xSemaphoreWaitTicks ) == pdTRUE )
        {
            if( taskCount != 0)
            {
                taskCount = 0;
            }
            xSemaphoreGive( xTaskCountBinarySem );
        }

        /* Disconnect. */
        xWiFiStatus = WIFI_Disconnect();
        if( xWiFiStatus != eWiFiSuccess )
        {
            configPRINTF( ("Task %d failed to disconnect with Wi-Fi error code %d\r\n",
                           pxTaskParams->usTaskId,
                           xWiFiStatus) );
            break;
        }

        /* Delay. */
        vTaskDelay( testwifiCONNECTION_DELAY );

        /* Perform a loop-back test to ensure we are NOT connected. */
        xLoopBackResults = prvLoopBackTest();
        if( xLoopBackResults == pdPASS )
        {
            configPRINTF( ( "Task %d completed the loop-back test after supposedly disconnecting." ) );
            break;
        }

        /* Are we disconnected? */
        xIsConnected = WIFI_IsConnected();
        if( xIsConnected == pdTRUE )
        {   
            configPRINTF( ("Task %d returned eWiFiSuccess from WIFI_Disconnect(), but is not disconnected.\r\n",
                           pxTaskParams->usTaskId ) );
            xWiFiStatus = eWiFiFailure;
            break;
        }

        /* Wait for the other task. */
        if( xEventGroupSync( xTaskDisconnectSyncEventGroupHandle,         /* The event group used for the rendezvous. */
                             (0x1 << pxTaskParams->usTaskId ),            /* Set our task ID when we are done. */
                             ulTaskFinishMask,                            /* Wait for both our task and the other task. */
                             testwifiTASK_SYNC_TIMEOUT ) != ulTaskFinishMask )
        {
            break;
        }

    }

    /* Set the Wi-Fi status. */
    pxTaskParams->xWiFiStatus = xWiFiStatus;
    
    /* Clear the task synchronization variables. */
    if( xTaskCountBinarySem != NULL )
    {
        vSemaphoreDelete( xTaskCountBinarySem );
    }
    
    if( xTaskConnectSyncEventGroupHandle != NULL )
    {
        vEventGroupDelete( xTaskConnectSyncEventGroupHandle );
    }

    if( xTaskDisconnectSyncEventGroupHandle != NULL )
    {
        vEventGroupDelete( xTaskDisconnectSyncEventGroupHandle );
    }

    /* Flag that the task is done. */
    xEventGroupSync( xTaskFinishEventGroupHandle,
                     ( 1 << pxTaskParams->usTaskId ),
                     ulTaskFinishMask,
                     testwifiMULTITASK_TEST_TIMEOUT );

    vTaskSuspend( NULL ); /* Delete this task. */
}

/**
 * @brief Spawn two threads to connect and disconnect at the same time. This is to
 * verify thread safety in the Wi-Fi driver API.
 */
TEST( Full_WiFi, WiFiSeperateTasksConnectingAndDisconnectingAtOnce )
{
    BaseType_t xTaskCreateResult;
    WIFINetworkParams_t xTask0_NetworkParams; /* Task zero has the clientcredential parameters and therefore can reach the echo server. */
    WIFINetworkParams_t xTask1_NetworkParams; /* Task one cannot reach the echo server. */
    testwifiTaskParams_t xTask0_Params;
    testwifiTaskParams_t xTask1_Params;
    uint32_t ulResultEventMask;

    /* There are only two tasks in this test. */
    const uint32_t ulNumTasks = 2;
    const uint32_t ulTaskFinishMask = ( ( 1 << ulNumTasks ) - 1 );

    /* Set the valid client parameters. */
    prvSetClientNetworkParameters( &xTask0_NetworkParams );
    xTask0_Params.pvParameters = &xTask0_NetworkParams;
    xTask0_Params.usTaskId = 0;
    xTask0_Params.xWiFiStatus = eWiFiSuccess;

    /* Set the valid client parameters defined in this test. */
    prvSetTestNetworkParameters( &xTask1_NetworkParams );
    xTask1_Params.pvParameters = &xTask1_NetworkParams;
    xTask1_Params.usTaskId = 1;
    xTask1_Params.xWiFiStatus = eWiFiSuccess;

    if ( TEST_PROTECT() )
    {
        /* Create an event group for tasks finishing testing. */
        xTaskFinishEventGroupHandle = xEventGroupCreate();
        configASSERT( xTaskFinishEventGroupHandle != NULL );
        
        /* Create the two task that connect at the same time. */
        xTaskCreateResult = xTaskCreate( prvConnectionTask,                       
                                         "ClientCredWiFiConnectionTask",                     
                                         testwifiTASK_STACK_SIZE,   
                                         &xTask0_Params,                                    
                                         testwifiTASK_PRIORITY,     
                                         &( xTask0_Params.xTaskHandle ) );
        TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xTaskCreateResult, "Client credential Wi-Fi connection task creation failed." );
        
        xTaskCreateResult = xTaskCreate( prvConnectionTask,                       
                                         "TestDefinedWiFiConnectionTask",                     
                                         testwifiTASK_STACK_SIZE,   
                                         &xTask1_Params,                                    
                                         testwifiTASK_PRIORITY,     
                                         &( xTask1_Params.xTaskHandle ) );
        TEST_ASSERT_EQUAL_MESSAGE( pdPASS, xTaskCreateResult, "Test credential Wi-Fi connection task creation failed." );

        /* Wait for the two tasks to finish. */
        ulResultEventMask = xEventGroupSync( xTaskFinishEventGroupHandle,
                                             0,
                                             ulTaskFinishMask,
                                             testwifiMULTITASK_TEST_TIMEOUT );

        /* Delete tasks that did not complete. */
        if( ( ulResultEventMask & ( 1 << xTask0_Params.usTaskId ) ) == 0 )
        {
            vTaskDelete( xTask0_Params.xTaskHandle );
        }

        if( ( ulResultEventMask & ( 1 << xTask1_Params.usTaskId ) ) == 0 )
        {
            vTaskDelete( xTask1_Params.xTaskHandle );
        }

        /* Check the task status and assert test results. */
        TEST_WIFI_ASSERT_REQUIRED_API( xTask0_Params.xWiFiStatus == eWiFiSuccess, xTask0_Params.xWiFiStatus );
        TEST_WIFI_ASSERT_REQUIRED_API( xTask1_Params.xWiFiStatus == eWiFiSuccess, xTask1_Params.xWiFiStatus );
    }
    else
    {
        TEST_FAIL();
    }

    /* Clean up the even groups. */
    if( xTaskFinishEventGroupHandle != NULL )
    {
        vEventGroupDelete( xTaskFinishEventGroupHandle );
    }
}
