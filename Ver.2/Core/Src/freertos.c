/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "device_state.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for key */
osThreadId_t keyHandle;
const osThreadAttr_t key_attributes = {
  .name = "key",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for into_device_Tas */
osThreadId_t into_device_TasHandle;
const osThreadAttr_t into_device_Tas_attributes = {
  .name = "into_device_Tas",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for output_devic_Ta */
osThreadId_t output_devic_TaHandle;
const osThreadAttr_t output_devic_Ta_attributes = {
  .name = "output_devic_Ta",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for data_into_Queue */
osMessageQueueId_t data_into_QueueHandle;
const osMessageQueueAttr_t data_into_Queue_attributes = {
  .name = "data_into_Queue"
};
/* Definitions for data_output_Queue */
osMessageQueueId_t data_output_QueueHandle;
const osMessageQueueAttr_t data_output_Queue_attributes = {
  .name = "data_output_Queue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Start_KeyTask(void *argument);
void data_into_device_Task(void *argument);
void data_output_device_Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of data_into_Queue */
  data_into_QueueHandle = osMessageQueueNew (16, sizeof(Device_Queue_t), &data_into_Queue_attributes);

  /* creation of data_output_Queue */
  data_output_QueueHandle = osMessageQueueNew (16, sizeof(Device_Queue_t), &data_output_Queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of key */
  keyHandle = osThreadNew(Start_KeyTask, NULL, &key_attributes);

  /* creation of into_device_Tas */
  into_device_TasHandle = osThreadNew(data_into_device_Task, NULL, &into_device_Tas_attributes);

  /* creation of output_devic_Ta */
  output_devic_TaHandle = osThreadNew(data_output_device_Task, NULL, &output_devic_Ta_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_Start_KeyTask */
/**
  * @brief  Function implementing the key thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Start_KeyTask */
__weak void Start_KeyTask(void *argument)
{
  /* USER CODE BEGIN Start_KeyTask */
  /* Infinite loop */
  for(;;)
  {

    osDelay(10);
  }
  /* USER CODE END Start_KeyTask */
}

/* USER CODE BEGIN Header_data_into_device_Task */
/**
* @brief Function implementing the into_device_Tas thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_data_into_device_Task */
__weak void data_into_device_Task(void *argument)
{
  /* USER CODE BEGIN data_into_device_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END data_into_device_Task */
}

/* USER CODE BEGIN Header_data_output_device_Task */
/**
* @brief Function implementing the output_devic_Ta thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_data_output_device_Task */
__weak void data_output_device_Task(void *argument)
{
  /* USER CODE BEGIN data_output_device_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END data_output_device_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

