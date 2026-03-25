#ifndef __BSP_DHT11_H__
#define __BSP_DHT11_H__
#include "main.h"



//IO????
#define DHT11_DATA_IO_IN()      {GPIOB->MODER&=0XFFFFFFFC;GPIOB->MODER|=0<<0;}  //0x00 input mode
#define DHT11_DATA_IO_OUT()     {GPIOB->MODER&=0XFFFFFFFC;GPIOB->MODER|=1<<0;}   //0x01 output  mode 

#define DHT11_DATA      GPIO_PIN_13
#define DHT11_GPIO      GPIOB

#define DHT11_DATA_SetHigh()            HAL_GPIO_WritePin(DHT11_GPIO,DHT11_DATA,GPIO_PIN_SET)    // output high level
#define DHT11_DATA_SetLow()             HAL_GPIO_WritePin(DHT11_GPIO,DHT11_DATA,GPIO_PIN_RESET)    // output low level

#define DHT11_ReadData()	            HAL_GPIO_ReadPin(DHT11_GPIO,DHT11_DATA)

/* ??? -------------------------------------------------------------------*/
/***********************   DHT11 ??????  **************************/
#define DHT11_Dout_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOB_CLK_ENABLE()//__HAL_RCC_GPIOA_CLK_ENABLE()
#define DHT11_Dout_PORT                           GPIOB
#define DHT11_Dout_PIN                            LL_GPIO_PIN_13

/***********************   DHT11 ?????  ****************************/
#define DHT11_Dout_LOW()                         do{DHT11_Dout_PORT->BSRR |=(uint32_t)DHT11_Dout_PIN<<16;}while(0)// HAL_GPIO_WritePin(DHT11_Dout_PORT, DHT11_Dout_PIN, GPIO_PIN_RESET)
#define DHT11_Dout_HIGH()                        do{DHT11_Dout_PORT->BSRR =DHT11_Dout_PIN;}while(0)//HAL_GPIO_WritePin(DHT11_Dout_PORT, DHT11_Dout_PIN, GPIO_PIN_SET)
#define DHT11_Data_IN()	                         LL_GPIO_IsInputPinSet(DHT11_Dout_PORT,DHT11_Dout_PIN)//HAL_GPIO_ReadPin(DHT11_Dout_PORT,DHT11_Dout_PIN)



/* ???? ------------------------------------------------------------------*/
/************************ DHT11 ??????******************************/
typedef struct
{
	uint8_t  humi_high8bit;		//????:???8?
	uint8_t  humi_low8bit;	 	//????:???8?
	uint8_t  temp_high8bit;	 	//????:???8?
	uint8_t  temp_low8bit;	 	//????:???8?
	uint8_t  check_sum;	 	    //???
  float    humidity;            //????
  float    temperature;        //????  
} DHT11_Data_TypeDef;

extern DHT11_Data_TypeDef DHT11;

void DHT11_Init(void);

void updateDht11_sensorData_toDisp(void);

void Update_Dht11_Totencent_Value(void);    
                            
void read_sensorData(void);

#endif 










