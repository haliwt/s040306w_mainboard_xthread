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

#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_exti.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_usart.h"
#include "stm32g0xx_ll_gpio.h"

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

/* USER CODE BEGIN Private defines */

#define OLDER_PCB   0

#define NEW_PCB    1 

#if NEW_PCB

#define WIFI_TX_Pin           LL_GPIO_PIN_2
#define WIFI_TX_GPIO_Port      GPIOA

#define WIFI_RX_Pin           LL_GPIO_PIN_3
#define WIFI_RX_GPIO_Port     GPIOA



#define FAN_CW_Pin            LL_GPIO_PIN_5 //COM 
#define FAN_CW_GPIO_Port        GPIOA

#define PLASMA_Pin              LL_GPIO_PIN_4
#define PLASMA_GPIO_Port         GPIOA


#define FAN_COM_Pin                 LL_GPIO_PIN_7
#define FAN_COM_GPIO_Port           GPIOA

#define RELAY_Pin           			LL_GPIO_PIN_0
#define RELAY_GPIO_Port 				GPIOB



#define TEMP_SENSOR_Pin         LL_GPIO_PIN_13
#define TEMP_SENSOR_GPIO_Port GPIOB


//USART2 
#define DISP_TX_Pin         LL_GPIO_PIN_9
#define DISP_TX_GPIO_Port GPIOA

#define DISP_RX_Pin     LL_GPIO_PIN_10
#define DISP_RX_GPIO_Port GPIOA

#define WIFI_EN_Pin        LL_GPIO_PIN_12
#define WIFI_EN_GPIO_Port GPIOA
//TIM
#define BEEP_Pin              LL_GPIO_PIN_1
#define BEEP_GPIO_Port        GPIOB

#else  //older PCB

#define WIFI_TX_Pin           LL_GPIO_PIN_2
#define WIFI_TX_GPIO_Port      GPIOA

#define WIFI_RX_Pin           LL_GPIO_PIN_3
#define WIFI_RX_GPIO_Port     GPIOA



#define PLASMA_Pin            LL_GPIO_PIN_5
#define PLASMA_GPIO_Port      GPIOA

#define FAN_CW_Pin            LL_GPIO_PIN_7 //COM 
#define FAN_CW_GPIO_Port        GPIOA

#define FAN_CCW_Pin               LL_GPIO_PIN_6 //RUN
#define FAN_CCW_GPIO_Port         GPIOA


/*older version be used to IC */
#define FAN_COM_Pin                 LL_GPIO_PIN_7
#define FAN_COM_GPIO_Port           GPIOA

//

#define RELAY_Pin           			LL_GPIO_PIN_0
#define RELAY_GPIO_Port 				GPIOB



#define TEMP_SENSOR_Pin         LL_GPIO_PIN_13
#define TEMP_SENSOR_GPIO_Port GPIOB


//USART2 
#define DISP_TX_Pin         LL_GPIO_PIN_9
#define DISP_TX_GPIO_Port GPIOA

#define DISP_RX_Pin     LL_GPIO_PIN_10
#define DISP_RX_GPIO_Port GPIOA

#define WIFI_EN_Pin        LL_GPIO_PIN_12
#define WIFI_EN_GPIO_Port GPIOA
//TIM
#define BEEP_Pin              LL_GPIO_PIN_1
#define BEEP_GPIO_Port        GPIOB

#endif 


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
