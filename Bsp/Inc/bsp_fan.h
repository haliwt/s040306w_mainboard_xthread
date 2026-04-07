#ifndef __BSP_FAN_H__
#define __BSP_FAN_H__
#include "main.h"


#if NEW_PCB
 

#define FAN_COM_SetHigh()            LL_GPIO_SetOutputPin(FAN_COM_GPIO_Port,FAN_COM_Pin)//(FAN_GPIO,FAN_CCW,GPIO_PIN_SET)    // output high level
#define FAN_COM_SetLow()             LL_GPIO_ResetOutputPin(FAN_COM_GPIO_Port,FAN_COM_Pin)//HAL_GPIO_WritePin(FAN_GPIO,FAN_CCW,GPIO_PIN_RESET)    // output low level

#define FAN_RUN_SetHigh()            LL_GPIO_SetOutputPin(FAN_CW_GPIO_Port,FAN_CW_Pin) //HAL_GPIO_WritePin(FAN_GPIO,FAN_CW,GPIO_PIN_SET)    // output high level
#define FAN_RUN_SetLow()             LL_GPIO_ResetOutputPin(FAN_CW_GPIO_Port,FAN_CW_Pin)//HAL_GPIO_WritePin(FAN_GPIO,FAN_CW,GPIO_PIN_RESET)    // output low level


#else

#define FAN_RUN_SetHigh()            LL_GPIO_SetOutputPin(GPIOA,FAN_CCW_Pin)//(FAN_GPIO,FAN_CCW,GPIO_PIN_SET)    // output high level
#define FAN_RUN_SetLow()             LL_GPIO_ResetOutputPin(GPIOA,FAN_CCW_Pin)//HAL_GPIO_WritePin(FAN_GPIO,FAN_CCW,GPIO_PIN_RESET)    // output low level

#define FAN_COM_SetHigh()            LL_GPIO_SetOutputPin(GPIOA,FAN_CW_Pin) //HAL_GPIO_WritePin(FAN_GPIO,FAN_CW,GPIO_PIN_SET)    // output high level
#define FAN_COM_SetLow()             LL_GPIO_ResetOutputPin(GPIOA,FAN_CW_Pin)//HAL_GPIO_WritePin(FAN_GPIO,FAN_CW,GPIO_PIN_RESET)    // outp



#endif 






void FAN_Stop(void);






//void ShutDown_AllFunction(void);

//void Fan_Slowly_Speed(void);
void Fan_One_Speed(void);


void Fan_Two_Speed(void);
void Fan_One_Power_Off_Speed(void);


void Fan_RunSpeed_Fun(void);
void Fan_Full_Speed(void);




void fan_run_fun(void);

void SetLevel_Fan_PWMA(uint8_t levelval);

//void fan_start_fun(void);
//void fan_run(void);
void fan_full_run(void);


#endif 

