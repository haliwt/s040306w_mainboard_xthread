/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g0xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "main.h"
#include "stm32g0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim14;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel 1 interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */
     if(LL_DMA_IsActiveFlag_TC1(DMA1)){
      LL_DMA_ClearFlag_TC1(DMA1);

   	}
  /* USER CODE END DMA1_Channel1_IRQn 0 */
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */
  if(LL_DMA_IsActiveFlag_TE1(DMA1)){
        LL_DMA_ClearFlag_TE1(DMA1);
      
   }

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 2 and channel 3 interrupts.
  */
void DMA1_Channel2_3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 0 */
  if(LL_DMA_IsActiveFlag_TC2(DMA1) != RESET) {
		  LL_DMA_ClearFlag_TC2(DMA1);
		  
  	}
   else if(LL_DMA_IsActiveFlag_TC3(DMA1) != RESET) {
		 LL_DMA_ClearFlag_TC3(DMA1);
		 
	 }
	 

  /* USER CODE END DMA1_Channel2_3_IRQn 0 */
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 1 */
  if(LL_DMA_IsActiveFlag_TE2(DMA1) != RESET) {
        LL_DMA_ClearFlag_TE2(DMA1);
        // 处理DMA传输错误
   }
  else if(LL_DMA_IsActiveFlag_TE3(DMA1) != RESET) {
        LL_DMA_ClearFlag_TE3(DMA1);
        // 处理DMA传输错误
    }

  /* USER CODE END DMA1_Channel2_3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel 4, channel 5 and DMAMUX1 interrupts.
  */
void DMA1_Ch4_5_DMAMUX1_OVR_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Ch4_5_DMAMUX1_OVR_IRQn 0 */
  if(LL_DMA_IsActiveFlag_TC4(DMA1)){
	 LL_DMA_ClearFlag_TC4(DMA1);
	 }
    else if(LL_DMA_IsActiveFlag_TC5(DMA1)){
	LL_DMA_ClearFlag_TC5(DMA1);
  	}
  /* USER CODE END DMA1_Ch4_5_DMAMUX1_OVR_IRQn 0 */
  /* USER CODE BEGIN DMA1_Ch4_5_DMAMUX1_OVR_IRQn 1 */
	
   if(LL_DMA_IsActiveFlag_TE4(DMA1)){
			LL_DMA_ClearFlag_TE4(DMA1);
		  
	}
   else if(LL_DMA_IsActiveFlag_TE5(DMA1)){
		 LL_DMA_ClearFlag_TE5(DMA1);
	   
	  }

  /* USER CODE END DMA1_Ch4_5_DMAMUX1_OVR_IRQn 1 */
}

/**
  * @brief This function handles TIM14 global interrupt.
  */
void TIM14_IRQHandler(void)
{
  /* USER CODE BEGIN TIM14_IRQn 0 */

  /* USER CODE END TIM14_IRQn 0 */
  HAL_TIM_IRQHandler(&htim14);
  /* USER CODE BEGIN TIM14_IRQn 1 */

  /* USER CODE END TIM14_IRQn 1 */
}

/**
  * @brief This function handles TIM17 global interrupt.
  */
void TIM17_IRQHandler(void)
{
  /* USER CODE BEGIN TIM17_IRQn 0 */
   if(LL_TIM_IsActiveFlag_UPDATE(TIM17)){
  	
      LL_TIM_ClearFlag_UPDATE(TIM17); // ✅ 清除更新中断标志
      //tim17_isr_callback_handler();
       //tim17_invoke_callback();   // 调用回调函数
       tim17_isr_callback_handler();

  }
  /* USER CODE END TIM17_IRQn 0 */
  /* USER CODE BEGIN TIM17_IRQn 1 */

  /* USER CODE END TIM17_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
#if 0
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}
#endif 
/**
  * @brief This function handles USART2 global interrupt 
  * USART2 wake-up interrupt through EXTI line 26.
  * wifi 
  */
void USART2_IRQHandler(void)
{

 /* USER CODE BEGIN USART2_IRQn 0 */
 //wifi USART2
 volatile uint8_t data;

  if(LL_USART_IsActiveFlag_RXNE_RXFNE(USART2)){

     //LL_USART_ClearFlag_RXNE(USART2);
    data = LL_USART_ReceiveData8(USART2);
    //usart2_isr_callback_handler(data);
    usart2_rx_callback_invoke(data);

  }


  /* USER CODE END USART2_IRQn 0 */
  /* USER CODE BEGIN USART2_IRQn 1 */
   if(LL_USART_IsActiveFlag_ORE(USART2)){

       LL_USART_ClearFlag_ORE(USART2);
   }
//   if(LL_USART_IsActiveFlag_FE(USART2)){
//       LL_USART_ClearFlag_FE(USART2);
//   }
//   if(LL_USART_IsActiveFlag_NE(USART2)){
//      LL_USART_ClearFlag_NE(USART2);
//   }

  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
