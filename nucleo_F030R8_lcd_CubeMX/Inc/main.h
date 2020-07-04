/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_BUTTON_Pin GPIO_PIN_13
#define USER_BUTTON_GPIO_Port GPIOC
#define RF_RESET_Pin GPIO_PIN_0
#define RF_RESET_GPIO_Port GPIOA
#define STLINK_TX_Pin GPIO_PIN_2
#define STLINK_TX_GPIO_Port GPIOA
#define STLINK_RX_Pin GPIO_PIN_3
#define STLINK_RX_GPIO_Port GPIOA
#define LCD_RS_Pin GPIO_PIN_4
#define LCD_RS_GPIO_Port GPIOF
#define LCD_RW_Pin GPIO_PIN_5
#define LCD_RW_GPIO_Port GPIOF
#define RF_SCK_Pin GPIO_PIN_5
#define RF_SCK_GPIO_Port GPIOA
#define RF_MISO_Pin GPIO_PIN_6
#define RF_MISO_GPIO_Port GPIOA
#define RF_MOSI_Pin GPIO_PIN_7
#define RF_MOSI_GPIO_Port GPIOA
#define LCD_E_Pin GPIO_PIN_4
#define LCD_E_GPIO_Port GPIOC
#define GRN_LED_Pin GPIO_PIN_5
#define GRN_LED_GPIO_Port GPIOC
#define LCD_DB7_Pin GPIO_PIN_1
#define LCD_DB7_GPIO_Port GPIOB
#define LCD_DB4_Pin GPIO_PIN_13
#define LCD_DB4_GPIO_Port GPIOB
#define LCD_DB5_Pin GPIO_PIN_14
#define LCD_DB5_GPIO_Port GPIOB
#define LCD_DB6_Pin GPIO_PIN_15
#define LCD_DB6_GPIO_Port GPIOB
#define MODE_JUMPER_Pin GPIO_PIN_8
#define MODE_JUMPER_GPIO_Port GPIOC
#define BUZZ_N_Pin GPIO_PIN_8
#define BUZZ_N_GPIO_Port GPIOA
#define BUZZ_P_Pin GPIO_PIN_9
#define BUZZ_P_GPIO_Port GPIOA
#define RF_DIO0_Pin GPIO_PIN_10
#define RF_DIO0_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define RF_DIO1_Pin GPIO_PIN_3
#define RF_DIO1_GPIO_Port GPIOB
#define RF_DIO3_Pin GPIO_PIN_4
#define RF_DIO3_GPIO_Port GPIOB
#define RF_DIO2_Pin GPIO_PIN_5
#define RF_DIO2_GPIO_Port GPIOB
#define RF_CS_N_Pin GPIO_PIN_6
#define RF_CS_N_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
