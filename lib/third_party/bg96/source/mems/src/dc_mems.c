/**
  ******************************************************************************
  * @file    dc_mems.c
  * @author  MCD Application Team
  * @brief   This file contains management of mems data for
  *          Nucleo expansion board X-NUCLEO-IKS01A2
  ******************************************************************************
  * @attention
  *
  * ST Confidential Information released to Verizon under NDA.
  *
  ******************************************************************************
  */

#include "dc_common.h"
#include "dc_service.h"
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "main.h"
#include "plf_config.h"
#include "error_handler.h"

#if (USE_DC_MEMS == 1)
#if defined (USE_STM32L496G_DISCO)
#include "x_nucleo_iks01a2.h"
#include "x_nucleo_iks01a2_accelero.h"
#include "x_nucleo_iks01a2_gyro.h"
#include "x_nucleo_iks01a2_magneto.h"
#include "x_nucleo_iks01a2_humidity.h"
#include "x_nucleo_iks01a2_temperature.h"
#include "x_nucleo_iks01a2_pressure.h"
#elif defined (STM32L475xx)
#include "stm32l475e_iot01.h"
#include "stm32l475e_iot01_accelero.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_psensor.h"
#endif
#endif


#if (USE_TRACE_INTERFACE_DCMEMS == 1)
#include "trace_interface.h"
#define PrintINFO(format, args...) TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P0, "MEMS:" format "\n\r", ## args)
#define PrintDBG(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_P1, "MEMS:" format "\n\r", ## args)
#define PrintErr(format, args...)  TracePrint(DBG_CHAN_ATCMD, DBL_LVL_ERR, "MEMS ERROR:" format "\n\r", ## args)
#elif (USE_PRINTF == 1)
#define PrintINFO(format, args...)  printf("MEMS:" format "\n\r", ## args)
#define PrintDBG(format, args...)   printf("MEMS:" format "\n\r", ## args)
#define PrintErr(format, args...)   printf("MEMS ERROR:" format "\n\r", ## args)
#elif (USE_CONFIGPRINTF_DCMEMS == 1)
#define PrintINFO(format, args...)  vLoggingPrintf("MEMS:" format "\r\n", ## args)
#define PrintDBG(format, args...)   vLoggingPrintf("MEMS:" format "\r\n", ## args)
#define PrintErr(format, args...)   vLoggingPrintf("MEMS ERROR:" format "\r\n", ## args)
#else
#define PrintINFO(format, args...)  do {} while(0)
#define PrintDBG(format, args...)   do {} while(0)
#define PrintErr(format, args...)   do {} while(0)
#endif

#if (USE_SIMU_MEMS == 1)

#define SIMULATED_PRESSURE_INCREMENT 1
#define SIMULATED_PRESSURE_INITIAL   2000
#define SIMULATED_PRESSURE_DELTA     7
#define SIMULATED_PRESSURE_MAX       (SIMULATED_PRESSURE_INITIAL+SIMULATED_PRESSURE_DELTA)
#define SIMULATED_PRESSURE_MIN       (SIMULATED_PRESSURE_INITIAL-SIMULATED_PRESSURE_DELTA)

#define SIMULATED_HUMIDITY_INCREMENT 1
#define SIMULATED_HUMIDITY_INITIAL   85
#define SIMULATED_HUMIDITY_DELTA     10
#define SIMULATED_HUMIDITY_MAX       (SIMULATED_HUMIDITY_INITIAL+SIMULATED_HUMIDITY_DELTA)
#define SIMULATED_HUMIDITY_MIN       (SIMULATED_HUMIDITY_INITIAL-SIMULATED_HUMIDITY_DELTA)

#define SIMULATED_TEMPERATURE_INCREMENT 0.8
#define SIMULATED_TEMPERATURE_INITIAL   80
#define SIMULATED_TEMPERATURE_DELTA     7
#define SIMULATED_TEMPERATURE_MAX       (SIMULATED_TEMPERATURE_INITIAL+SIMULATED_TEMPERATURE_DELTA)
#define SIMULATED_TEMPERATURE_MIN       (SIMULATED_TEMPERATURE_INITIAL-SIMULATED_TEMPERATURE_DELTA)


static float pressure_increment    = SIMULATED_PRESSURE_INCREMENT;
static float humidity_increment    = SIMULATED_HUMIDITY_INCREMENT;
static float temperature_increment = SIMULATED_TEMPERATURE_INCREMENT;

static float sumulated_pressure_value    = SIMULATED_PRESSURE_INITIAL;
static float sumulated_temperature_value = SIMULATED_TEMPERATURE_INITIAL;
static float sumulated_humidity_value    = SIMULATED_HUMIDITY_INITIAL;

#endif /* USE_SIMU_MEMS */

#if (USE_DC_MEMS == 1)
#if defined (USE_STM32L496G_DISCO)
static void *ACCELERO_handle = NULL;
static void *GYRO_handle = NULL;
static void *MAGNETO_handle = NULL;
static void *HUMIDITY_handle = NULL;
static void *TEMPERATURE_handle = NULL;
static void *PRESSURE_handle = NULL;
static SensorAxes_t ACC_Value;                  /*!< Acceleration Value */
static SensorAxes_t GYR_Value;                  /*!< Gyroscope Value */
static SensorAxes_t MAG_Value;                  /*!< Magnetometer Value */
#elif defined (STM32L475xx)
static int16_t ACC_Value[3];                  /*!< Acceleration Value */
static float GYR_Value[3];                    /*!< Gyroscope Value */
static int16_t MAG_Value[3];                  /*!< Magnetometer Value */
#endif
static float PRESSURE_Value;           /*!< Pressure Value */
static float HUMIDITY_Value;           /*!< Humidity Value */
static float TEMPERATURE_Value;        /*!< Temperature Value */
#endif

osThreadId memsDclibTaskTaskId = NULL;

/* StartDefaultTask function */
#if (USE_DC_MEMS == 1) || (USE_SIMU_MEMS == 1)
static void StartMemsDclibTask(void const * argument)
{
  dc_pressure_rt_info_t        pressure_info;
  dc_humidity_rt_info_t        humidity_info;
  dc_temperature_rt_info_t     temperature_info;
  
#if (USE_DC_MEMS == 1)
  dc_accelerometer_rt_info_t   accelerometer_info;
  dc_gyroscope_rt_info_t       gyroscope_info;
  dc_magnetometer_rt_info_t    magnetometer_info;
  
#if defined (USE_STM32L496G_DISCO)
  /* Try to use automatic discovery. By default use LSM6DSL on board */
  BSP_ACCELERO_Init( ACCELERO_SENSORS_AUTO, &ACCELERO_handle );
  /* Try to use automatic discovery. By default use LSM6DSL on board */
  BSP_GYRO_Init( GYRO_SENSORS_AUTO, &GYRO_handle );
  /* Try to use automatic discovery. By default use LSM303AGR on board */
  BSP_MAGNETO_Init( MAGNETO_SENSORS_AUTO, &MAGNETO_handle );
  /* Try to use automatic discovery. By default use HTS221 on board */
  BSP_HUMIDITY_Init( HUMIDITY_SENSORS_AUTO, &HUMIDITY_handle );
  /* Try to use automatic discovery. By default use HTS221 on board */
  BSP_TEMPERATURE_Init( TEMPERATURE_SENSORS_AUTO, &TEMPERATURE_handle );
  /* Try to use automatic discovery. By default use LPS22HB on board */
  BSP_PRESSURE_Init( PRESSURE_SENSORS_AUTO, &PRESSURE_handle );

  BSP_ACCELERO_Sensor_Enable( ACCELERO_handle );
  BSP_GYRO_Sensor_Enable( GYRO_handle );
  BSP_MAGNETO_Sensor_Enable( MAGNETO_handle );
  BSP_HUMIDITY_Sensor_Enable( HUMIDITY_handle );
  BSP_TEMPERATURE_Sensor_Enable( TEMPERATURE_handle );
  BSP_PRESSURE_Sensor_Enable( PRESSURE_handle );
#elif defined (STM32L475xx)
  uint8_t init = 0;
  
  if (ACCELERO_OK == BSP_ACCELERO_Init())
  {
    init |= 0x01u;
  }
  
  if (GYRO_OK == BSP_GYRO_Init())
  {
    init |= 0x02u;
  }
  
  if (MAGNETO_OK == BSP_MAGNETO_Init())
  {
    init |= 0x04u;
  }
  
  if (HSENSOR_OK == BSP_HSENSOR_Init())
  {
    init |= 0x08u;
  }
  
  if (TSENSOR_OK == BSP_TSENSOR_Init())
  {
    init |= 0x10u;
  }
  
  if (PSENSOR_OK == BSP_PSENSOR_Init())
  {
    init |= 0x20u;
  }
#endif
#endif
  
   /* USER CODE BEGIN StartDefaultTask */
  osDelay(1000);

  /* Infinite loop */
  for(;;)
  {
#if defined (USE_STM32L496G_DISCO)
#if (USE_DC_MEMS == 1)
    uint8_t status = 0;

    if(BSP_ACCELERO_IsInitialized(ACCELERO_handle, &status) == COMPONENT_OK && status == 1)
    {
      BSP_ACCELERO_Get_Axes(ACCELERO_handle, &ACC_Value);

      /* DEBUG sensor values */
      PrintINFO("### ACC_Value: X=%d - Y=%d - Z=%d\n\r", ACC_Value.AXIS_X, ACC_Value.AXIS_Y, ACC_Value.AXIS_Z);
      
      accelerometer_info.rt_state              =  DC_SERVICE_ON;
      accelerometer_info.accelerometer.AXIS_X  =  ACC_Value.AXIS_X;
      accelerometer_info.accelerometer.AXIS_Y  =  ACC_Value.AXIS_Y;
      accelerometer_info.accelerometer.AXIS_Z  =  ACC_Value.AXIS_Z;
      dc_com_write(&dc_com_db, DC_COM_ACCELEROMETER, (void *)&accelerometer_info, sizeof(accelerometer_info));
    }

    if(BSP_GYRO_IsInitialized(GYRO_handle, &status) == COMPONENT_OK && status == 1)
    {
      BSP_GYRO_Get_Axes(GYRO_handle, &GYR_Value);

      /* DEBUG sensor values */
      PrintINFO("### GYR_Value: X=%d - Y=%d - Z=%d\n\r", GYR_Value.AXIS_X, GYR_Value.AXIS_Y, GYR_Value.AXIS_Z);
      
      gyroscope_info.rt_state          =  DC_SERVICE_ON;
      gyroscope_info.gyroscope.AXIS_X  =  GYR_Value.AXIS_X;
      gyroscope_info.gyroscope.AXIS_Y  =  GYR_Value.AXIS_Y;
      gyroscope_info.gyroscope.AXIS_Z  =  GYR_Value.AXIS_Z;
      dc_com_write(&dc_com_db, DC_COM_GYROSCOPE, (void *)&gyroscope_info, sizeof(gyroscope_info));      
    }

    if(BSP_MAGNETO_IsInitialized(MAGNETO_handle, &status) == COMPONENT_OK && status == 1)
    {
      BSP_MAGNETO_Get_Axes(MAGNETO_handle, &MAG_Value);

      /* DEBUG sensor values */
      PrintINFO("### MAG_Value: X=%d - Y=%d - Z=%d\n\r", MAG_Value.AXIS_X, MAG_Value.AXIS_Y, MAG_Value.AXIS_Z);

      magnetometer_info.rt_state             =  DC_SERVICE_ON;
      magnetometer_info.magnetometer.AXIS_X  =  MAG_Value.AXIS_X;
      magnetometer_info.magnetometer.AXIS_Y  =  MAG_Value.AXIS_Y;
      magnetometer_info.magnetometer.AXIS_Z  =  MAG_Value.AXIS_Z;
      dc_com_write(&dc_com_db, DC_COM_MAGNETOMETER, (void *)&magnetometer_info, sizeof(magnetometer_info));
    }
#endif

#if (USE_DC_MEMS == 1)
    if(BSP_PRESSURE_IsInitialized(PRESSURE_handle, &status) == COMPONENT_OK && status == 1)
    {
      BSP_PRESSURE_Get_Press(PRESSURE_handle, &PRESSURE_Value);

      /* DEBUG sensor values */
      PrintINFO("### PRESSURE_Value = %f\n\r", PRESSURE_Value);
      
      pressure_info.rt_state         =  DC_SERVICE_ON;
      pressure_info.pressure         =  PRESSURE_Value;
      dc_com_write(&dc_com_db, DC_COM_PRESSURE, (void *)&pressure_info, sizeof(pressure_info));      
    }
    else
#endif
    {
#if (USE_SIMU_MEMS == 1)
      sumulated_pressure_value += pressure_increment;
      if(pressure_info.pressure >= SIMULATED_PRESSURE_MAX)   pressure_increment = -SIMULATED_PRESSURE_INCREMENT;
      if(pressure_info.pressure <= SIMULATED_PRESSURE_MIN)   pressure_increment = SIMULATED_PRESSURE_INCREMENT;

      pressure_info.pressure = sumulated_pressure_value;
      PrintINFO("### Simulated PRESSURE_Value = %f\n\r", pressure_info.pressure);
      dc_com_write(&dc_com_db, DC_COM_PRESSURE, (void *)&pressure_info, sizeof(pressure_info));      
#endif
    }
    
#if (USE_DC_MEMS == 1)
    if(BSP_HUMIDITY_IsInitialized(HUMIDITY_handle, &status) == COMPONENT_OK && status == 1)
    {
      BSP_HUMIDITY_Get_Hum(HUMIDITY_handle, &HUMIDITY_Value);

      /* DEBUG sensor values */
      PrintINFO("### HUMIDITY_Value = %f\n\r", HUMIDITY_Value);
      
      humidity_info.rt_state         =  DC_SERVICE_ON;
      humidity_info.humidity         =  HUMIDITY_Value;
      dc_com_write(&dc_com_db, DC_COM_HUMIDITY, (void *)&humidity_info, sizeof(humidity_info));  
    }
    else
#endif
    {
#if (USE_SIMU_MEMS == 1)
      sumulated_humidity_value += humidity_increment;
      if(humidity_info.humidity >= SIMULATED_HUMIDITY_MAX)   humidity_increment = -SIMULATED_HUMIDITY_INCREMENT;
      if(humidity_info.humidity <= SIMULATED_HUMIDITY_MIN)   humidity_increment  = SIMULATED_HUMIDITY_INCREMENT;

      humidity_info.humidity = sumulated_humidity_value;
      PrintINFO("### Simulated HUMIDITY Value = %f\n\r", humidity_info.humidity);
      dc_com_write(&dc_com_db, DC_COM_HUMIDITY, (void *)&humidity_info, sizeof(humidity_info));      
#endif
    }
    
#if (USE_DC_MEMS == 1)
    if(BSP_TEMPERATURE_IsInitialized(TEMPERATURE_handle, &status) == COMPONENT_OK && status == 1)
    {
        BSP_TEMPERATURE_Get_Temp(TEMPERATURE_handle, &TEMPERATURE_Value);

        /* DEBUG sensor values */
        PrintINFO("### TEMPERATURE_Value = %f\n\r", TEMPERATURE_Value);

        temperature_info.rt_state         =  DC_SERVICE_ON;
        temperature_info.temperature      =  TEMPERATURE_Value;
        dc_com_write(&dc_com_db, DC_COM_TEMPERATURE, (void *)&temperature_info, sizeof(temperature_info));        
    }
    else
#endif
    {
#if (USE_SIMU_MEMS == 1)
      sumulated_temperature_value += temperature_increment;
      if(temperature_info.temperature >= SIMULATED_TEMPERATURE_MAX)   temperature_increment = -SIMULATED_TEMPERATURE_INCREMENT;
      if(temperature_info.temperature <= SIMULATED_TEMPERATURE_MIN)   temperature_increment = SIMULATED_TEMPERATURE_INCREMENT;

      temperature_info.temperature = sumulated_temperature_value;
      PrintINFO("### Simulated TEMPERATURE Value = %f\n\r", temperature_info.temperature);
      dc_com_write(&dc_com_db, DC_COM_TEMPERATURE, (void *)&temperature_info, sizeof(temperature_info));      
#endif
    }
    
#elif defined (STM32L475xx)
    if (init & 0x01)
    {
      BSP_ACCELERO_AccGetXYZ(ACC_Value);
      
      /* DEBUG sensor values */
      PrintINFO("### ACC_Value: X=%d - Y=%d - Z=%d\n\r", ACC_Value[0], ACC_Value[1], ACC_Value[2]);
      
      accelerometer_info.rt_state              =  DC_SERVICE_ON;
      accelerometer_info.accelerometer.AXIS_X  =  ACC_Value[0];
      accelerometer_info.accelerometer.AXIS_Y  =  ACC_Value[1];
      accelerometer_info.accelerometer.AXIS_Z  =  ACC_Value[2];
      dc_com_write(&dc_com_db, DC_COM_ACCELEROMETER, (void *)&accelerometer_info, sizeof(accelerometer_info));
    }
    
    if (init & 0x02)
    {
      BSP_GYRO_GetXYZ(GYR_Value);
      
      /* DEBUG sensor values */
      PrintINFO("### GYR_Value: X=%d - Y=%d - Z=%d\n\r", (int32_t) GYR_Value[0], (int32_t) GYR_Value[1], (int32_t) GYR_Value[2]);
      
      gyroscope_info.rt_state          =  DC_SERVICE_ON;
      gyroscope_info.gyroscope.AXIS_X  =  (int32_t) GYR_Value[0];
      gyroscope_info.gyroscope.AXIS_Y  =  (int32_t) GYR_Value[1];
      gyroscope_info.gyroscope.AXIS_Z  =  (int32_t) GYR_Value[2];
      dc_com_write(&dc_com_db, DC_COM_GYROSCOPE, (void *)&gyroscope_info, sizeof(gyroscope_info));      
    }
    
    if (init & 0x04)
    {
      BSP_MAGNETO_GetXYZ(MAG_Value);
      
      /* DEBUG sensor values */
      PrintINFO("### MAG_Value: X=%d - Y=%d - Z=%d\n\r", MAG_Value[0], MAG_Value[1], MAG_Value[2]);
      
      magnetometer_info.rt_state             =  DC_SERVICE_ON;
      magnetometer_info.magnetometer.AXIS_X  =  MAG_Value[0];
      magnetometer_info.magnetometer.AXIS_Y  =  MAG_Value[1];
      magnetometer_info.magnetometer.AXIS_Z  =  MAG_Value[2];
      dc_com_write(&dc_com_db, DC_COM_MAGNETOMETER, (void *)&magnetometer_info, sizeof(magnetometer_info));
    }
    
    if (init & 0x20)
    {
      PRESSURE_Value = BSP_PSENSOR_ReadPressure();
      
      /* DEBUG sensor values */
      PrintINFO("### PRESSURE_Value = %f\n\r", PRESSURE_Value);
      
      pressure_info.rt_state         =  DC_SERVICE_ON;
      pressure_info.pressure         =  PRESSURE_Value;
      dc_com_write(&dc_com_db, DC_COM_PRESSURE, (void *)&pressure_info, sizeof(pressure_info));      
    }
#if (USE_SIMU_MEMS == 1)
    else
    {
      sumulated_pressure_value += pressure_increment;
      if(pressure_info.pressure >= SIMULATED_PRESSURE_MAX)   pressure_increment = -SIMULATED_PRESSURE_INCREMENT;
      if(pressure_info.pressure <= SIMULATED_PRESSURE_MIN)   pressure_increment = SIMULATED_PRESSURE_INCREMENT;
      
      pressure_info.pressure = sumulated_pressure_value;
      PrintINFO("### Simulated PRESSURE_Value = %f\n\r", pressure_info.pressure);
      dc_com_write(&dc_com_db, DC_COM_PRESSURE, (void *)&pressure_info, sizeof(pressure_info));      
    }
#endif
    
    if (init & 0x08)
    {
      HUMIDITY_Value = BSP_HSENSOR_ReadHumidity();
      
      /* DEBUG sensor values */
      PrintINFO("### HUMIDITY_Value = %f\n\r", HUMIDITY_Value);
      
      humidity_info.rt_state         =  DC_SERVICE_ON;
      humidity_info.humidity         =  HUMIDITY_Value;
      dc_com_write(&dc_com_db, DC_COM_HUMIDITY, (void *)&humidity_info, sizeof(humidity_info));  
    }
#if (USE_SIMU_MEMS == 1)
    else
    {
      sumulated_humidity_value += humidity_increment;
      if(humidity_info.humidity >= SIMULATED_HUMIDITY_MAX)   humidity_increment = -SIMULATED_HUMIDITY_INCREMENT;
      if(humidity_info.humidity <= SIMULATED_HUMIDITY_MIN)   humidity_increment  = SIMULATED_HUMIDITY_INCREMENT;
      
      humidity_info.humidity = sumulated_humidity_value;
      PrintINFO("### Simulated HUMIDITY Value = %f\n\r", humidity_info.humidity);
      dc_com_write(&dc_com_db, DC_COM_HUMIDITY, (void *)&humidity_info, sizeof(humidity_info));      
    }
#endif
    
    if (init & 0x10)
    {
      TEMPERATURE_Value = BSP_TSENSOR_ReadTemp();
      
      /* DEBUG sensor values */
      PrintINFO("### TEMPERATURE_Value = %f\n\r", TEMPERATURE_Value);
      
      temperature_info.rt_state         =  DC_SERVICE_ON;
      temperature_info.temperature      =  TEMPERATURE_Value;
      dc_com_write(&dc_com_db, DC_COM_TEMPERATURE, (void *)&temperature_info, sizeof(temperature_info));
    }
#if (USE_SIMU_MEMS == 1)
    else
    {
      sumulated_temperature_value += temperature_increment;
      if(temperature_info.temperature >= SIMULATED_TEMPERATURE_MAX)   temperature_increment = -SIMULATED_TEMPERATURE_INCREMENT;
      if(temperature_info.temperature <= SIMULATED_TEMPERATURE_MIN)   temperature_increment = SIMULATED_TEMPERATURE_INCREMENT;
      
      temperature_info.temperature = sumulated_temperature_value;
      PrintINFO("### Simulated TEMPERATURE Value = %f\n\r", temperature_info.temperature);
      dc_com_write(&dc_com_db, DC_COM_TEMPERATURE, (void *)&temperature_info, sizeof(temperature_info));      
    }
#endif
#endif
    osDelay(10000);
  }
}

void dc_mems_init()
{
}

void dc_mems_start()
{
    osThreadDef(memsDclibTask, StartMemsDclibTask, DC_MEMS_THREAD_PRIO, 0, DC_MEMS_THREAD_STACK_SIZE);
    memsDclibTaskTaskId = osThreadCreate(osThread(memsDclibTask), NULL);
    if(memsDclibTaskTaskId == NULL)
    {
        ERROR_Handler(DBG_CHAN_MAIN, 11, ERROR_FATAL);
    }
    else
    {
#if (STACK_ANALYSIS_TRACE == 1)
        stackAnalysis_addThreadStackSizeByHandle(memsDclibTaskTaskId, DC_MEMS_THREAD_STACK_SIZE);
#endif /* STACK_ANALYSIS_TRACE */
    }
}
#endif

