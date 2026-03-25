#ifndef __BSP_PLASMA_H
#define __BSP_PLASMA_H
#include "main.h"

#define PLASMA_SetHigh()        LL_GPIO_SetOutputPin(PLASMA_GPIO_Port,PLASMA_Pin)//HAL_GPIO_WritePin(PLASMA_GPIO_Port,PLASMA_Pin,GPIO_PIN_SET)// do{PLASMA_GPIO_Port-> BSRR = PLASMA_Pin;}while(0) //HAL_GPIO_WritePin(PLASMA_GPIO,PLASMA_PIN,GPIO_PIN_SET)    // output high level
#define PLASMA_SetLow()         LL_GPIO_ResetOutputPin(PLASMA_GPIO_Port,PLASMA_Pin)//HAL_GPIO_WritePin(PLASMA_GPIO_Port,PLASMA_Pin,GPIO_PIN_RESET) //do{PLASMA_GPIO_Port-> BSRR =(uint32_t)PLASMA_Pin<<16;}while(0)// HAL_GPIO_WritePin(PLASMA_GPIO,PLASMA_PIN,GPIO_PIN_RESET)    // output low level



void plasma_fun(uint8_t sel);


#endif 

