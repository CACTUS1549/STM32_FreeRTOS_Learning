/*BLOCKING WHEN RECEIVING FROM QUEUE.

Function - This example demonstrates a queue being created, data being sent to the queue from multiple tasks,
and data being received from the queue. The queue is created to hold data items of type int32_t.
The tasks that send to the queue do not specify a block time, whereas the task that receives from the queue does.
The priority of the tasks that send to the queue are lower than the priority of the task that receives from the queue.
This means the queue should never contain more than one item because, as soon as data is sent to the queue
the receiving task will unblock, pre-empt the sending task, and remove the data—leaving the queue empty once again.
Two instances of this task are created, one that writes continuously the value 100 to the queue,
and another that writes continuously the value 200 to the same queue. The task parameter is used to pass
these values into each task instance.

For more info refer - 161204_Mastering_the_FreeRTOS_Real_Time_Kernel-A_Hands-On_Tutorial_Guide --> Pg - 115

 */





#include "main.h"
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


UART_HandleTypeDef huart3;

//always used funtion prototypes for global availability.
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void print(char string[]);
static void printInteger(int i);


//other created functions prototypes.
static void vSenderTask(void *pvParameters);
static void vReceiverTask(void *pvParameters);


//Global variables go here!

/* Declare a variable of type QueueHandle_t.
   This is used to store the handle to the queue that is accessed by all three tasks. */
QueueHandle_t xQueue;


int main(void)
{


  HAL_Init();
  
  MX_GPIO_Init();

  MX_USART3_UART_Init();

  /* The queue is created to hold a maximum of 5 values, each of which is large
   	 enough to hold a variable of type int32_t. */

  xQueue = xQueueCreate(5, sizeof(int32_t));

  if(xQueue != NULL){
	  /* Create two instances of the task that will send to the queue.
	    The task parameter is used to pass the value that the task will write
	    to the queue, so one task will continuously write 100 to the queue while
	    the other task will continuously write 200 to the queue. Both tasks are created at priority 1. */

	  xTaskCreate(vSenderTask, "Sender 1", 1000, (void *)100, 1, NULL);
	  xTaskCreate(vSenderTask, "Sender 2", 1000, (void *)200, 1, NULL);

	  /* Create the task that will read from the queue.
	     The task is created with priority 2, so above the priority of the sender tasks. */

	  xTaskCreate(vReceiverTask, "Receiver", 1000, NULL, 2, NULL);
  }

  else{
	  print("The queue could not be created! \r\n");
  }

  vTaskStartScheduler();

  for(;;);

}

static void print(char string[]){
	char buffer[300];
	int len;
	sprintf(buffer, string);
	len = strlen(buffer);
	HAL_UART_Transmit(&huart3, buffer, len, 1000);

}

static void printInteger(int i){
	char buffer[300];
	int len;
	sprintf(buffer, "%i", i);
	len = strlen(buffer);
	HAL_UART_Transmit(&huart3, buffer, len, 1000);
}


static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}


static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOG_CLK_ENABLE();

  __HAL_RCC_GPIOD_CLK_ENABLE();

  __HAL_RCC_GPIOK_CLK_ENABLE();


  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LED3_Pin|LED2_Pin, GPIO_PIN_SET); //PORT D

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET); //PORT K

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); //PORT G



  /*Configure GPIO pins : LED3_Pin LED2_Pin */
  GPIO_InitStruct.Pin = LED3_Pin|LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);


  /*Configure GPIO pin : LED4_Pin */
  GPIO_InitStruct.Pin = LED4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED4_GPIO_Port, &GPIO_InitStruct);


  /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

}



void Error_Handler(void)
{
	print("Error in USART3\r\n");
}


static void vSenderTask(void *pvParameters){

	int32_t lValueToSend;
	BaseType_t xStatus;

	lValueToSend = (int32_t) pvParameters;

	for(;;){

		//Send date to the queue.
		xStatus = xQueueSendToBack(xQueue, &lValueToSend, 0);

		if(xStatus != pdPASS){
			print("Could not send to the queue!\r\n");
		}
	}
}


static void vReceiverTask(void *pvParameters){

	int32_t lReceivedValue;
	BaseType_t xStatus;
	const TickType_t xTicksToWait = pdMS_TO_TICKS(100);

	for(;;){
		/* This call should always find the queue empty because this task will
		 immediately remove any data that is written to the queue. */
		if(uxQueueMessagesWaiting(xQueue) != 0){
			print("Queue should have been empty!\r\n");
		}

		//Receive Data from Queue

		xStatus = xQueueReceive(xQueue, &lReceivedValue, xTicksToWait);

		if(xStatus == pdPASS){
			print("Received  ");
			printInteger(lReceivedValue);
			print("\r\n");
		}

		else {
			print("Could not receive the value from the queue.\r\n");
		}
	}
}





























