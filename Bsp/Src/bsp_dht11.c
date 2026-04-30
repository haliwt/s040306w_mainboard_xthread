#include "bsp.h"

#define Bit_RESET 0
#define Bit_SET   1

uint8_t read_flag;

static void DHT11_Mode_IPU(void);
static void DHT11_Mode_Out_PP(void);
static uint8_t DHT11_ReadByte(void);
DHT11_Data_TypeDef DHT11;
void static Dht11_Read_TempHumidity_Handler(DHT11_Data_TypeDef * pdth11);
uint8_t dht11_read_flag;

//??us??
//void delay_us(unsigned long i)
//{
//	unsigned long j;
//	for(;i>0;i--)
//	{
//			for(j=6;j>0;j--);
//	}
//}


/**
  * ????: DHT11 ?????
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
void DHT11_Init(void)
{

	//__HAL_RCC_GPIOA_CLK_ENABLE();
	//DHT11_Mode_Out_PP();
	
	///DHT11_Dout_HIGH();  // ??GPIO
}

/**
  * ????: ?DHT11-DATA??????????
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
static void DHT11_Mode_IPU(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
 	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

     /**/
	  GPIO_InitStruct.Pin = TEMP_SENSOR_Pin;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
	  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/**
  * ????: ?DHT11-DATA??????????
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
static void DHT11_Mode_Out_PP(void)
{
	

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
/**/
	GPIO_InitStruct.Pin = TEMP_SENSOR_Pin ;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/**
  * ????: ?DHT11??????,MSB??
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
static uint8_t DHT11_ReadByte ( void )
{
	uint8_t i, temp=0;
	
	for(i=0;i<8;i++)    
	{	 
		/*?bit?50us???????,???????? ?50us ??? ??*/  
		while(DHT11_Data_IN()==Bit_RESET);

		/*DHT11 ?26~28us??????ï¿?0ï¿?,?70us?????ï¿?1ï¿?,
		 *???? x us???????????? ,x ?????? 
		 */
		delay_us(40); //??x us ??????????0???????	   	  

		if(DHT11_Data_IN()==Bit_SET)/* x us??????????ï¿?1ï¿? */
		{
			/* ????1?????? */
			while(DHT11_Data_IN()==Bit_SET);

			temp|=(uint8_t)(0x01<<(7-i));  //??7-i??1,MSB?? 
		}
		else	 // x us?????????ï¿?0ï¿?
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //??7-i??0,MSB??
		}
	}
	return temp;
}

/**
  * ????: ??????????40bit,????
  * ????: DHT11_Data:DHT11????
  * ? ? ?: ERROR:  ????
  *           SUCCESS:????
  * ?    ?:8bit ???? + 8bit ???? + 8bit ???? + 8bit ???? + 8bit ??? 
  */
uint8_t DHT11_Read_TempAndHumidity(DHT11_Data_TypeDef *DHT11_Data)
{  
  uint8_t temp;
  uint16_t humi_temp;
  
	/*????*/
	DHT11_Mode_Out_PP();
	/*????*/
	DHT11_Dout_LOW();
	/*??18ms*/
	delay_ms(18);//HAL_Delay(20);//tx_thread_sleep(pdMS_TO_TICKS(20));//HAL_Delay(20);

	/*???? ????30us*/
	DHT11_Dout_HIGH(); 

	delay_us(30);   //??30us

	/*?????? ????????*/ 
	DHT11_Mode_IPU();
      delay_us(30);   //??30us
	/*?????????????? ???????,???????*/   
	if(DHT11_Data_IN()==Bit_RESET)     
	{
		/*???????? ?80us ??? ??????*/  
		while(DHT11_Data_IN()==Bit_RESET);

		/*????????? 80us ??? ??????*/
		while(DHT11_Data_IN()==Bit_SET);

		/*??????*/   
		DHT11_Data->humi_high8bit= DHT11_ReadByte();
		DHT11_Data->humi_low8bit = DHT11_ReadByte();
		DHT11_Data->temp_high8bit= DHT11_ReadByte();
		DHT11_Data->temp_low8bit = DHT11_ReadByte();
		DHT11_Data->check_sum    = DHT11_ReadByte();

		/*????,????????*/
		DHT11_Mode_Out_PP();
		/*????*/
		DHT11_Dout_HIGH();
		
		/* ??????? */
		humi_temp=DHT11_Data->humi_high8bit*100+DHT11_Data->humi_low8bit;
		DHT11_Data->humidity =(float)humi_temp/100;
		
		humi_temp=DHT11_Data->temp_high8bit*100+DHT11_Data->temp_low8bit;
		DHT11_Data->temperature=(float)humi_temp/100;    
		
		/*???????????*/
		temp = DHT11_Data->humi_high8bit + DHT11_Data->humi_low8bit + 
			DHT11_Data->temp_high8bit+ DHT11_Data->temp_low8bit;
		if(DHT11_Data->check_sum==temp)
		{ 
		return SUCCESS;
		}
		else 
		return ERROR;
	}	
	else
		return ERROR;
}

/**
*@breif :
*@note:
*@param:
*@return:
*
*/
void static Dht11_Read_TempHumidity_Handler(DHT11_Data_TypeDef * pdth11)
{
   

	if(dht11_read_flag==0){

	 read_flag =DHT11_Read_TempAndHumidity(pdth11);
    if(read_flag == 0){
		   
		   gctl_t.gDht11_humidity = (pdth11->humi_high8bit);
		   
		   gctl_t.gDht11_temperature = (pdth11->temp_high8bit);
	   
	 }
	 else{

	    dht11_read_flag=1;
		gpro_t.gTimer_read_dth11_sensor =0;

	 }
	}

	if(gpro_t.gTimer_read_dth11_sensor > 2 && dht11_read_flag==1){
            dht11_read_flag=0;

	}
	

}
/**
*@breif :
*@note:
*@param:
*@return:
*
*/
void updateDht11_sensorData_toDisp(void)
{
	
	    Dht11_Read_TempHumidity_Handler(&DHT11);
	    sendData_Real_TimeHum(gctl_t.gDht11_humidity ,gctl_t.gDht11_temperature);
		tx_thread_sleep(10);
	
}
/**
*@breif :
*@note:
*@param:
*@return:
*
*/
void read_sensorData(void)
{
	
	    Dht11_Read_TempHumidity_Handler(&DHT11);
	    sendData_Real_TimeHum(gctl_t.gDht11_humidity ,gctl_t.gDht11_temperature);
		tx_thread_sleep(10);
	
}

/**
*@breif :
*@note:
*@param:
*@return:
*
*/
void Update_Dht11_Totencent_Value(void)
{

  
	Dht11_Read_TempHumidity_Handler(&DHT11);
	
	// dht11_read_data(&gctl_t.gDht11_temperature, &gctl_t.gDht11_humidity);


	MqttData_Publis_ReadTempHum(gctl_t.gDht11_temperature, gctl_t.gDht11_humidity);
    tx_thread_sleep(20);//HAL_Delay(100);

}






