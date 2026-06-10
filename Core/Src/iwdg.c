/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    iwdg.c
  * @brief   This file provides code for the configuration
  *          of the IWDG instances.
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
#include "iwdg.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* IWDG init function */
void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */
  /* T-over = (prev * (reload + 1))/32KHz   */
  /* USER CODE END IWDG_Init 0 */
  //Counter Value between Min_Data=0 and Max_Data=0x0FFF

  /* USER CODE BEGIN IWDG_Init 1 */
   // iwdg = 2s Time_out = (relaod * prescaler)/32000 = s.
  /* USER CODE END IWDG_Init 1 */
  LL_IWDG_Enable(IWDG);//error is 
  LL_IWDG_EnableWriteAccess(IWDG);
  LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_64);
  LL_IWDG_SetReloadCounter(IWDG, 3999);//4000*(64/32000)=8s//4095
  while (LL_IWDG_IsReady(IWDG) != 1)
  {
  }

  LL_IWDG_ReloadCounter(IWDG);
  /* USER CODE BEGIN IWDG_Init 2 */
  // 6. 最后一步：正式使能/启动看门狗
   LL_IWDG_Enable(IWDG);
  /* USER CODE END IWDG_Init 2 */

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
