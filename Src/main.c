/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>
#include <stdio.h>
#include "RFM96.h"
#include "lcd.h"


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
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

char msg[MSG_SIZE];
uint16_t msg_size;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_NVIC_Init(void);

static uint8_t tx_mode;
static uint8_t buffer[64];
static uint8_t toggle;
static uint8_t tx_count = 0;

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

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
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_SPI1_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */

  print("\r\nPOR");
  print("Build: " __DATE__ ", " __TIME__);
  beep(250,1);

  LCD_Init();
  LCD_ClearDisplay();
  LCD_ReturnHome();

  const uint8_t m[] =
  {
      0x1f, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00,
      0x11, 0x1f, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00,
      0x11, 0x11, 0x1f, 0x11, 0x11, 0x11, 0x11, 0x00,
      0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11, 0x00,
      0x11, 0x11, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x00,
      0x11, 0x11, 0x11, 0x11, 0x11, 0x1f, 0x11, 0x00,
      0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1f, 0x00,
      0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1f,
  };

  LCD_SetCGramAddress(0x00);
  for(uint8_t i = 0; i<64; i++)
  {
      LCD_WriteData(m[i]);
  }

  //uint8_t c = 0x01;
  LCD_SetDataAddress(0x00);
  for(uint8_t i=0; i<16; i++)
  {
      LCD_WriteData(i);
   }


//  uint8_t  c = 0;
//  for(uint8_t i=0; i<0x68; i++)
//  {
//      LCD_SetDataAddress(i);
//      LCD_WriteData(c);
//      ++c;
//      if (i == 0x13) i= 0x3F;
//      if (i == 0x53) i= 0x13;
//      if (i == 0x27) i= 0x53;
//  }

//
//  LCD_SetDataAddress(0x00);
//  LCD_WriteData('7');
//  LCD_WriteData('0');
//  LCD_WriteData(LCD_CHAR_DEGREE);
//  LCD_WriteData('F');
//
//  LCD_SetDataAddress(0x40);
//  LCD_WriteData('6');
//  LCD_WriteData('8');
//  LCD_WriteData(LCD_CHAR_DEGREE);
//  LCD_WriteData('F');
//
//  LCD_SetDataAddress(0x14);
//  LCD_WriteData('7');
//  LCD_WriteData('2');
//  LCD_WriteData(LCD_CHAR_DEGREE);
//  LCD_WriteData('F');

  while(1);

  RFM96_Init();

  tx_mode = (uint8_t) HAL_GPIO_ReadPin(MODE_JUMPER_GPIO_Port, MODE_JUMPER_Pin);
  print1("TX Mode", tx_mode);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  toggle = 0;

  if (tx_mode)
  {
	  // Tx Mode
	  while (1)
	  {
		  // turn on LED
		  HAL_GPIO_WritePin(GRN_LED_GPIO_Port, GRN_LED_Pin, GPIO_PIN_SET);

		  print1("--SEND--", tx_count);
		  ++tx_count;

		  RFM96_Send((uint8_t *) "0123456789ABCDEF", 16);

		  // turn off LED after 100ms (pulses = Tx mode)
		  Delay_ms(100);
		  HAL_GPIO_WritePin(GRN_LED_GPIO_Port, GRN_LED_Pin, GPIO_PIN_RESET);

		  // wait a full second
		  Delay_ms(900);

		  RFM96_ClearInt();

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	  }
  }
  else
  {
	  // Rx Mode
	  while(1)
	  {
		  print("--RECV--");
		  RFM96_Receive(buffer, 64);
		  printstr("data:", buffer);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}


/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* USART2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* SPI1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(SPI1_IRQn);
}


/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}


/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}


/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, LCD_RS_Pin|LCD_RW_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LCD_E_Pin|GRN_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_DB7_Pin|LCD_DB4_Pin|LCD_DB5_Pin|LCD_DB6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BUZZ_N_Pin|BUZZ_P_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RF_CS_N_GPIO_Port, RF_CS_N_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : USER_BUTTON_Pin */
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RF_RESET_Pin RF_DIO0_Pin */
  GPIO_InitStruct.Pin = RF_RESET_Pin|RF_DIO0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RS_Pin LCD_RW_Pin */
  GPIO_InitStruct.Pin = LCD_RS_Pin|LCD_RW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_E_Pin GRN_LED_Pin */
  GPIO_InitStruct.Pin = LCD_E_Pin|GRN_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_DB7_Pin LCD_DB4_Pin LCD_DB5_Pin LCD_DB6_Pin */
  GPIO_InitStruct.Pin = LCD_DB7_Pin|LCD_DB4_Pin|LCD_DB5_Pin|LCD_DB6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : MODE_JUMPER_Pin */
  GPIO_InitStruct.Pin = MODE_JUMPER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(MODE_JUMPER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BUZZ_N_Pin BUZZ_P_Pin */
  GPIO_InitStruct.Pin = BUZZ_N_Pin|BUZZ_P_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : RF_DIO1_Pin RF_DIO3_Pin RF_DIO2_Pin */
  GPIO_InitStruct.Pin = RF_DIO1_Pin|RF_DIO3_Pin|RF_DIO2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RF_CS_N_Pin */
  GPIO_InitStruct.Pin = RF_CS_N_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(RF_CS_N_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void print(const char *text)
{
	  sprintf(msg, "%s\r\n", text );
	  msg_size = strlen(msg);
	  HAL_UART_Transmit(&huart2, (uint8_t *) msg, msg_size, TIMEOUT_1_SEC);
}


void print1(const char *text, uint8_t x)
{
	  sprintf(msg, "%s 0x%02X\r\n", text, (int) x );
	  msg_size = strlen(msg);
	  HAL_UART_Transmit(&huart2, (uint8_t *) msg, msg_size, TIMEOUT_1_SEC);
}


void print2(const char *text, uint8_t x, uint8_t y)
{
	  sprintf(msg, "%s 0x%02X 0x%02X\r\n", text, (int) x, (int) y );
	  msg_size = strlen(msg);
	  HAL_UART_Transmit(&huart2, (uint8_t *) msg, msg_size, TIMEOUT_1_SEC);
}


void printstr(const char *text, uint8_t * data)
{
	  sprintf(msg, "%s: %s\r\n", text, (char *) data );
	  msg_size = strlen(msg);
	  HAL_UART_Transmit(&huart2, (uint8_t *) msg, msg_size, TIMEOUT_1_SEC);
}


void printhex(uint8_t x)
{
      sprintf(msg, "%02X", (int) x );
      msg_size = strlen(msg);
      HAL_UART_Transmit(&huart2, (uint8_t *) msg, msg_size, TIMEOUT_1_SEC);
}


/* set pitch to 0,1,2,3 with 0 highest pitch */
void beep(uint32_t duration_ms, uint8_t pitch)
{
	uint32_t tick;

	uint32_t mask = (1 << pitch);

	uint32_t start = HAL_GetTick();
	do {
		tick = HAL_GetTick();
		if (tick & mask)
		{
			// Piezo/Speaker positive voltage
			HAL_GPIO_WritePin(BUZZ_P_GPIO_Port, BUZZ_P_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(BUZZ_N_GPIO_Port, BUZZ_N_Pin, GPIO_PIN_RESET);
		}
		else
		{
			// Piezo/Speaker negative voltage
			HAL_GPIO_WritePin(BUZZ_P_GPIO_Port, BUZZ_P_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(BUZZ_N_GPIO_Port, BUZZ_N_Pin, GPIO_PIN_SET);
		}
	} while ((tick - start) < duration_ms);

	// Peizo/Speaker zero voltage
	HAL_GPIO_WritePin(BUZZ_P_GPIO_Port, BUZZ_P_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BUZZ_N_GPIO_Port, BUZZ_N_Pin, GPIO_PIN_RESET);

	return;
}


void Delay_ms(uint32_t delay_ms)
{
    /**
     * This should correctly handle SysTic roll-overs.
     * See:
     *   https://stackoverflow.com/questions/61443/rollover-safe-timer-tick-comparisons
     */
    uint32_t start_time_ms = HAL_GetTick();
    while ( (HAL_GetTick() - start_time_ms) < delay_ms)
    {
        // spin wait
    }

    return;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
