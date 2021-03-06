/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "queue.h"

#include "stepper.h"

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

/* USER CODE BEGIN PV */
SM_Param motor_param={0};
speedRampData motor_data={0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t buff_is_ready(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_I2C3_Init();
  MX_LTDC_Init();
  MX_SPI5_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_UART5_Init();
  MX_USB_OTG_HS_HCD_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  RetargetInit(&huart5);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  Ser_Queue.Front=-1;
  Ser_Queue.Rear=-1;
  uint8_t buffer[20];
  uint8_t cmd[3];
  int8_t buf_val[4];
  HAL_UART_Receive_IT(&huart5, rec_buff, 1);
 // uint16_t c=0;
  motor_param.accel=10;
  motor_param.decel=10;
  motor_param.speed=100;
  motor_param.steps=-1000;

//  htim7.Instance->ARR= 2500;
//  HAL_TIM_Base_Start(&htim7);
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  HAL_UART_Transmit_IT(&huart5, str, len);
//	  LD3_GPIO_Port->BSRR|=LD3_Pin;
//	  //HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
//	  HAL_Delay(1000);
//
//	  //HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
//	  LD3_GPIO_Port->BSRR|=(uint32_t)LD3_Pin<<16;
//	  HAL_Delay(1000);
	  if (buff_is_ready())
	  {
		  uint8_t *ptr;
		  uint8_t *space;

		  for (int i=0;i<3;i++){
			  cmd[i]= deQueue(&Ser_Queue);
		  }


		  if ((cmd[0]=='S') &&(cmd[1]=='E') &&(cmd[2]=='T'))
		  {
			  uint8_t temp;
			  //uint8_t dum=0;
			  //int len = (Ser_Queue.Rear-Ser_Queue.Front)+1;
			  while (!isQ_Empty(&Ser_Queue))
			  {
				  temp = deQueue(&Ser_Queue);

				  switch (temp) {
					case 'a':

						ptr =&Ser_Queue.Buffer[Ser_Queue.Front];
						space = strchr(ptr,' ');
						if (*space==NULL){
							space = strchr(ptr,'\n');
						}
						*space=0;
						motor_param.accel= atoi(ptr);

						Ser_Queue.Front= Ser_Queue.Front+(space-ptr);
						//printf("setting a : %d\r\n", motor_param.accel);

//
						break;
					case 'd':
						ptr =&Ser_Queue.Buffer[Ser_Queue.Front];
						space = strchr(ptr,' ');
						if (*space==NULL){
							space = strchr(ptr,'\n');
						}
						*space=0;
						motor_param.decel= atoi(ptr);

						Ser_Queue.Front= Ser_Queue.Front+(space-ptr);

						//printf("setting d\r\n");

					   break;
					case 's':
						ptr =&Ser_Queue.Buffer[Ser_Queue.Front];
						space = strchr(ptr,' ');
						if (*space==NULL){
							space = strchr(ptr,'\n');
						}
						*space=0;
						motor_param.speed= atoi(ptr);

						Ser_Queue.Front= Ser_Queue.Front+(space-ptr);

						//printf("setting s\r\n");

						break;

					case 'c':
						ptr =&Ser_Queue.Buffer[Ser_Queue.Front];
						space = strchr(ptr,' ');
						if (*space==NULL){
							space = strchr(ptr,'\n');
						}
						*space=0;
						motor_param.steps= atoi(ptr);

						Ser_Queue.Front= Ser_Queue.Front+(space-ptr);

						//printf("setting c\r\n");

						break;

				}

			  }


			  printf("Setting accel : %d decel : %d speed  : %d steps : %d\r\n",
					  motor_param.accel,motor_param.decel,motor_param.speed,motor_param.steps);
//		  int len = (Ser_Queue.Rear-Ser_Queue.Front)+1;
//		  for (int j =0;j<len;j++)
//		  	 {
//		  		 buffer[j]=deQueue(&Ser_Queue);
//
//		  	 }
//		  HAL_UART_Transmit_IT(&huart5,buffer, len);

		  }
		  else if ((cmd[0]=='R') &&(cmd[1]=='U') &&(cmd[2]=='N'))
		  {
			  runMotor_f=1;

		  }

		  //reset_Queue(&Ser_Queue);
//		  if (deQueue(&Ser_Queue)=='S')
//
//		  {
//			  HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
//			  printf("reply : ");
//			  int len = (Ser_Queue.Rear-Ser_Queue.Front)+1;
//
//			  for (int j =0;j<len;j++)
//			  {
//				  buffer[j]=deQueue(&Ser_Queue);
//
//			  }
//			  //Ser_Queue.Front=Ser_Queue.Front+3;
//
//			  HAL_UART_Transmit_IT(&huart5,buffer, len);
//			  memset(buffer,0,10);
//		  }




	  }
	  if (runMotor_f)
	  {
		  runStepper(&motor_data, &motor_param);
		  runMotor_f=0;
		  printf("min delay : %d sec_start : %d dec_value : %d\r\n",
		  	    		  motor_data.min_delay,motor_data.decel_start,motor_data.decel_val);
		 // printf("s_delay : %d s_count : %d \n", motor_data.step_delay, motor_data.step_count);
	  }




  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

uint8_t buff_is_ready(void)
{
	if(Ser_Queue.Buffer[Ser_Queue.Rear]=='\n')
	{
		return 1;

	}
	return 0;
}


/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if (htim->Instance == TIM7)
  {

	  updateStepper(&motor_data);
	 // printf("step delay : %d set count :%d state : %d\r\n", motor_data.step_delay, motor_data.step_count, motor_data.state);

  }

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
