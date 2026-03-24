/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
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
#define D_SOL_EN_Pin GPIO_PIN_13
#define D_SOL_EN_GPIO_Port GPIOC
#define PRESSURE_SIG_Pin GPIO_PIN_0
#define PRESSURE_SIG_GPIO_Port GPIOA
#define TAG_TX_Pin GPIO_PIN_2
#define TAG_TX_GPIO_Port GPIOA
#define FLOW_METER_SIG_Pin GPIO_PIN_3
#define FLOW_METER_SIG_GPIO_Port GPIOA
#define CONT_PWR_I_Pin GPIO_PIN_4
#define CONT_PWR_I_GPIO_Port GPIOA
#define VCC_Pin GPIO_PIN_5
#define VCC_GPIO_Port GPIOA
#define CONT_GND_I_Pin GPIO_PIN_6
#define CONT_GND_I_GPIO_Port GPIOA
#define CONT_I_COMMS_RLY_Pin GPIO_PIN_7
#define CONT_I_COMMS_RLY_GPIO_Port GPIOA
#define CONT_B_I_Pin GPIO_PIN_0
#define CONT_B_I_GPIO_Port GPIOB
#define CONT_A_I_Pin GPIO_PIN_1
#define CONT_A_I_GPIO_Port GPIOB
#define CONT_O_A_RLY_Pin GPIO_PIN_2
#define CONT_O_A_RLY_GPIO_Port GPIOB
#define CONT_A_O_Pin GPIO_PIN_10
#define CONT_A_O_GPIO_Port GPIOB
#define CONT_B_O_Pin GPIO_PIN_11
#define CONT_B_O_GPIO_Port GPIOB
#define CONT_O_B_RLY_Pin GPIO_PIN_12
#define CONT_O_B_RLY_GPIO_Port GPIOB
#define CONT_GND_O_Pin GPIO_PIN_13
#define CONT_GND_O_GPIO_Port GPIOB
#define CONT_O_GND_RLY_Pin GPIO_PIN_14
#define CONT_O_GND_RLY_GPIO_Port GPIOB
#define CONT_PWR_O_Pin GPIO_PIN_15
#define CONT_PWR_O_GPIO_Port GPIOB
#define CONT_O_PWR_RLY_Pin GPIO_PIN_8
#define CONT_O_PWR_RLY_GPIO_Port GPIOA
#define RPi_LINE_TX_Pin GPIO_PIN_9
#define RPi_LINE_TX_GPIO_Port GPIOA
#define HEARTBEAT_LED_Pin GPIO_PIN_6
#define HEARTBEAT_LED_GPIO_Port GPIOC
#define USER_BTN_Pin GPIO_PIN_7
#define USER_BTN_GPIO_Port GPIOC
#define RPi_LINE_RX_Pin GPIO_PIN_10
#define RPi_LINE_RX_GPIO_Port GPIOA
#define LED_G_Pin GPIO_PIN_3
#define LED_G_GPIO_Port GPIOB
#define LED_R_Pin GPIO_PIN_4
#define LED_R_GPIO_Port GPIOB
#define LED_B_Pin GPIO_PIN_5
#define LED_B_GPIO_Port GPIOB
#define VLV_PWR_CTRL_Pin GPIO_PIN_6
#define VLV_PWR_CTRL_GPIO_Port GPIOB
#define VLV_POL_CTRL_Pin GPIO_PIN_7
#define VLV_POL_CTRL_GPIO_Port GPIOB
#define W_SOL_EN_Pin GPIO_PIN_8
#define W_SOL_EN_GPIO_Port GPIOB
#define A_SOL_EN_Pin GPIO_PIN_9
#define A_SOL_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart1;

extern osMessageQueueId_t pressureResultQueueHandle;
extern osMessageQueueId_t rawMsgQueueHandle;

extern osMutexId_t uartTxMutexHandle;

extern osThreadId_t PressureTaskHandle;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
