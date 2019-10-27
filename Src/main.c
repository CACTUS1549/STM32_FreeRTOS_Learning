
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


UART_HandleTypeDef huart3;

//static funtion prototypes for global availability.
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void print(char string[]);


//other functions prototypes.
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);


//Global variables go here!
TaskHandle_t xTask2Handle = NULL;

int main(void)
{


  HAL_Init();

  
  MX_GPIO_Init();

  MX_USART3_UART_Init();

  xTaskCreate(vTask1, "Task 1", 1000, NULL, 2, NULL);

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

void vTask1(void *pvParameters){


	for(;;){
		print("Task 1 is running!\r\n");

		//Create Task 2 @ higher priority
		xTaskCreate(vTask2, "Task 2", 1000, NULL, 2, &xTask2Handle);

		//By now Task 2 must be running and should delete itself. Give some delay for the idle task to clean up resources.
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

void vTask2(void *pvParameters){
	print("Task 2 is running and is about to delete itself!\r\n");
	vTaskDelete(xTask2Handle);
}






























