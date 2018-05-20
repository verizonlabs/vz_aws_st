#ifndef _AWS_DEMO_CONFIG_H_
#define _AWS_DEMO_CONFIG_H_

/* Number of sub pub tasks that connect to a broker that is not using TLS. */
#define democonfigMQTT_SUB_PUB_NUM_UNSECURE_TASKS         ( 1 )

/* Number of sub pub tasks that connect to a broker that is using TLS. */
#define democonfigMQTT_SUB_PUB_NUM_SECURE_TASKS           ( 1 )

#define democonfigSHADOW_DEMO_NUM_TASKS                   ( 2 )

/* IoT simple subscribe/publish example task parameters. */
#define democonfigMQTT_SUB_PUB_TASK_STACK_SIZE            ( configMINIMAL_STACK_SIZE * 5 )
#define democonfigMQTT_SUB_PUB_TASK_PRIORITY              ( tskIDLE_PRIORITY )

/* Greengrass discovery example task parameters. */
#define democonfigGREENGRASS_DISCOVERY_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 16 )
#define democonfigGREENGRASS_DISCOVERY_TASK_PRIORITY      ( tskIDLE_PRIORITY )

/* Shadow lightbulb example task parameters. */
#define democonfigSHADOW_LIGHTBULB_TASK_STACK_SIZE        ( configMINIMAL_STACK_SIZE * 16 )
#define democonfigSHADOW_LIGHTBULB_TASK_PRIORITY          ( tskIDLE_PRIORITY )


/* TCP Echo Client tasks single example parameters. */
#define democonfigTCP_ECHO_TASKS_SINGLE_TASK_STACK_SIZE    ( configMINIMAL_STACK_SIZE * 4 )
#define democonfigTCP_ECHO_TASKS_SINGLE_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1 )

/* OTA Update task example parameters. */
#define democonfigOTA_UPDATE_TASK_STACK_SIZE               ( 4 * configMINIMAL_STACK_SIZE )
#define democonfigOTA_UPDATE_TASK_TASK_PRIORITY            ( tskIDLE_PRIORITY )

/* Simple TCP Echo Server task example parameters */
#define democonfigTCP_ECHO_SERVER_TASK_STACK_SIZE          ( configMINIMAL_STACK_SIZE * 6 )
#define democonfigTCP_ECHO_SERVER_TASK_PRIORITY            ( tskIDLE_PRIORITY )

#endif /* _AWS_DEMO_CONFIG_H_ */
