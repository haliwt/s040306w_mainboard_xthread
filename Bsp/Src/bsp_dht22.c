#include "bsp.h"

#define Bit_RESET 0
#define Bit_SET   1

uint8_t read_flag;

static void DHT11_Mode_IPU(void);
static void DHT11_Mode_Out_PP(void);
static uint8_t DHT11_ReadByte(void);
DHT22_Data_TypeDef DHT22;
//void static Dht22_Read_TempHumidity_Handler(DHT11_Data_TypeDef * pdth22);

void static Dht22_Read_TempHumidity_Handler(DHT22_Data_TypeDef * pdth22);


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

	__HAL_RCC_GPIOA_CLK_ENABLE();
	DHT11_Mode_Out_PP();
	
	DHT11_Dout_HIGH();  // ??GPIO
}

/**
  * ????: ?DHT11-DATA??????????
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
static void DHT11_Mode_IPU(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Dout_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_Dout_PORT, &GPIO_InitStruct);
}

/**
  * ????: ?DHT11-DATA??????????
  * ????: ?
  * ? ? ?: ?
  * ?    ?:?
  */
static void DHT11_Mode_Out_PP(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DHT11_Dout_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_Dout_PORT, &GPIO_InitStruct);
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

		/*DHT11 ?26~28us??????锟?0锟?,?70us?????锟?1锟?,
		 *???? x us???????????? ,x ?????? 
		 */
		delay_us(40); //??x us ??????????0???????	   	  

		if(DHT11_Data_IN()==Bit_SET)/* x us??????????锟?1锟? */
		{
			/* ????1?????? */
			while(DHT11_Data_IN()==Bit_SET);

			temp|=(uint8_t)(0x01<<(7-i));  //??7-i??1,MSB?? 
		}
		else	 // x us?????????锟?0锟?
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
uint8_t DHT22_Read_TempAndHumidity(DHT22_Data_TypeDef *DHT22_Data)
{  
  uint8_t temp;
  int16_t raw_temp;
  uint16_t raw_humi;

  DHT11_Mode_Out_PP();       // 设置为推挽输出
  DHT11_Dout_LOW();          // 拉低开始信号
  delay_ms(1);//HAL_Delay(1);              // DHT22只需 >1ms，推荐1~2ms

  DHT11_Dout_HIGH();         // 拉高
  delay_us(30);              // 等待响应

  DHT11_Mode_IPU();          // 设置为上拉输入
  delay_us(40);              // 等待DHT22响应

  if(DHT11_Data_IN() == Bit_RESET)     
  {
    while(DHT11_Data_IN() == Bit_RESET); // 等待DHT22拉高
    while(DHT11_Data_IN() == Bit_SET);   // 等待DHT22拉低

    // 读取5字节数据
    DHT22_Data->humi_high8bit = DHT11_ReadByte();
    DHT22_Data->humi_low8bit  = DHT11_ReadByte();
    DHT22_Data->temp_high8bit = DHT11_ReadByte();
    DHT22_Data->temp_low8bit  = DHT11_ReadByte();
    DHT22_Data->check_sum     = DHT11_ReadByte();

    DHT11_Mode_Out_PP();
    DHT11_Dout_HIGH();

    // 湿度处理（无符号）
    raw_humi = ((uint16_t)DHT22_Data->humi_high8bit << 8) | DHT22_Data->humi_low8bit;
    DHT22_Data->humidity = raw_humi / 10.0f;

    // 温度处理（高位符号位）
    raw_temp = ((uint16_t)(DHT22_Data->temp_high8bit & 0x7F) << 8) | DHT22_Data->temp_low8bit;
    if (DHT22_Data->temp_high8bit & 0x80) {
      raw_temp = -raw_temp;
    }
    DHT22_Data->temperature = raw_temp / 10.0f;

    // 校验和验证
    temp = DHT22_Data->humi_high8bit + DHT22_Data->humi_low8bit + 
           DHT22_Data->temp_high8bit + DHT22_Data->temp_low8bit;
    if(DHT22_Data->check_sum == temp)
    { 
      return SUCCESS;
    }
    else 
      return ERROR;
  } 
  else
    return ERROR;
}



void static Dht22_Read_TempHumidity_Handler(DHT22_Data_TypeDef * pdth22)
{
	read_flag = DHT22_Read_TempAndHumidity(pdth22);  // 调用 DHT22 读取函数

    if (read_flag == 0) {
        gctl_t.gDht11_humidity = pdth22->humidity;         // 直接使用 float 湿度
        gctl_t.gDht11_temperature = pdth22->temperature;   // 直接使用 float 温度
    }
	

}

void updateDht11_sensorData_toDisp(void)
{
	
	    Dht22_Read_TempHumidity_Handler(&DHT22);
	    sendData_Real_TimeHum(gctl_t.gDht11_humidity ,gctl_t.gDht11_temperature);
		tx_thread_sleep(100);//tx_thread_sleep(20);
	

     //dht11_read_data(&gctl_t.gDht11_temperature, &gctl_t.gDht11_humidity);
//sendData_Real_TimeHum(gctl_t.gDht11_humidity ,gctl_t.gDht11_temperature);
	 //tx_thread_sleep(20);

}



void Update_Dht11_Totencent_Value(void)
{

  
	Dht22_Read_TempHumidity_Handler(&DHT22);
	
	// dht11_read_data(&gctl_t.gDht11_temperature, &gctl_t.gDht11_humidity);


	MqttData_Publis_ReadTempHum(gctl_t.gDht11_temperature, gctl_t.gDht11_humidity);
    tx_thread_sleep(200);//HAL_Delay(100);

}






