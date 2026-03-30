#include "bsp.h"
#include "adc.h"

#define SAMPLE_COUNT 6

//uint16_t mean_fan_buf[SAMPLE_COUNT];  // 存放5次采样值
//uint8_t fan_counter = 0;              // 当前存储位置
//uint8_t sample_ready = 0;             // 标记是否采满5次


// ADC相关变量定义

uint16_t adc_buffer[ADC_BUFFER_SIZE]; // DMA传输缓冲区
volatile uint8_t adc_conversion_complete = 0;


/* USER CODE BEGIN 0 */




uint16_t fan_detect_voltage = 1000;
uint16_t ptc_detect_voltage;


static uint16_t compute_voltage(uint16_t raw_value) ;
static uint16_t ADC_FAN_ReadVoltage(void);
static uint16_t ADC_PTC_ReadVoltage(void);
static uint8_t ADC_StartConversion(void);
static void ADC_GetValues(void);
uint16_t mean_fan_buf[SAMPLE_COUNT];


/**********************************************************************
	*
	*Functin Name: void adc_detected_hundler(void)
	*Function :
	*Input Ref:  key of value
	*Return Ref: NO
	*
**********************************************************************/
void adc_detected_hundler(void)
{
    if(gpro_t.fan_rx_stop_flag ==1  || gpro_t.stopTwoHours_flag ==1) return ;//WT.EDIT 2026.03.03
    if(gctl_t.gTimer_fan_adc_times > 4 && gpro_t.stopTwoHours_flag ==0 && gpro_t.fan_warning_flag==0){ //detected 3 times is 60s 
        gctl_t.gTimer_fan_adc_times =0;
        Fan_Full_Speed();

       fan_detect_voltage=(adc_buffer[0] * 3300 )/4095;
       tx_thread_sleep(5);

	   if(gpro_t.fan_counter_error > 10)gpro_t.fan_counter_error =0;

	   if(gpro_t.fan_warning_flag > 1)gpro_t.fan_warning_flag=0;

	   if(fan_detect_voltage < 150  &&  gpro_t.fan_warning_flag==0){


		  	gpro_t.fan_counter_error ++;
			  if(gpro_t.fan_counter_error  > 5){
			      gpro_t.fan_warning_flag=1;
				  gctl_t.ptc_prohibit_on_flag = 1;
				  gpro_t.rx_ptc_flag = 0;
				  PTC_SetLow();
			  }
		  

	   }
	   else{
	      gpro_t.fan_counter_error =0;

	   }

	   
    }
   if(gpro_t.fan_warning_flag==1){
      fan_warning_sound();

   	}
	

}

#if 0
// 启动ADC转换
static uint8_t ADC_StartConversion(void)
{
    if(LL_ADC_IsEnabled(ADC1) == 0) {
        return 0; // ADC未使能
    }
    
    if(LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_3)) {
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
    }
    
    // 配置DMA传输
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3,
                          LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
                          (uint32_t)adc_buffer,
                          LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, ADC_BUFFER_SIZE);
    
    adc_conversion_complete = 0;
    
    // 使能DMA通道
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
    
    // 启动ADC转换
    LL_ADC_REG_StartConversion(ADC1);
    
    return 1;
}
#endif 
// 获取ADC转换结果
void ADC_GetValues(void)
{

  fan_detect_voltage=	(adc_buffer[0] * 3300 )/4095;

}


/*****************************************************************
	*
	*Function Name: 
	*Function :ADC input channel be selected "which one channe"
	*Input Ref: which one ? AC_Channel_?, hexadecimal of average
	*Return Ref: No
	*
*****************************************************************/
//static uint16_t ADC_FAN_ReadVoltage(void)
//{
//    uint16_t raw_value;
//	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
//    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_COMMON_1);

//	LL_ADC_REG_StartConversion(ADC1);

//	while(!LL_ADC_IsActiveFlag_EOC(ADC1));

//	raw_value= LL_ADC_REG_ReadConversionData12(ADC1);
	
//	LL_ADC_ClearFlag_EOC(ADC1);

//	return compute_voltage(raw_value);

//}
/*****************************************************************
	*
	*Function Name: static uint16_t ADC_PTC_ReadVoltage(void)
	*Function :
	*Input Ref:
	*Return Ref: No
	*
*****************************************************************/
static uint16_t ADC_PTC_ReadVoltage(void)
{
    uint16_t raw_value;
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_1);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_COMMON_1);

	LL_ADC_REG_StartConversion(ADC1);

	while(!LL_ADC_IsActiveFlag_EOC(ADC1));

	raw_value= LL_ADC_REG_ReadConversionData12(ADC1);
	
	LL_ADC_ClearFlag_EOC(ADC1);

	return compute_voltage(raw_value);

}



void fan_warning_sound(void)
{
   if(gpro_t.fan_warning_flag == 1){
     

   
		   gctl_t.ptc_prohibit_on_flag = 1;
		   gpro_t.rx_ptc_flag = 0;//gctl_t.gDry =0;
		
		    PTC_SetLow();


           buzzer_sound();//Buzzer_KeySound();
		   tx_thread_sleep(100);//tx_thread_sleep(100);
		   buzzer_sound();//Buzzer_KeySound();
		   tx_thread_sleep(100);//tx_thread_sleep(100);
		   buzzer_sound();//Buzzer_KeySound();
			tx_thread_sleep(100);//tx_thread_sleep(100);
		   buzzer_sound();//Buzzer_KeySound();
		   tx_thread_sleep(100);//tx_thread_sleep(100);
		   buzzer_sound();//Buzzer_KeySound();
		   tx_thread_sleep(100);//tx_thread_sleep(100);
          
           SendWifiData_To_Cmd(0x09, 0x01);
           tx_thread_sleep(100);
	       if(wifi_link_net_state()==1){

              // MqttData_Publis_SetFan(0);
	           //tx_thread_sleep(pdMS_TO_TICKS(200));//HAL_Delay(350);

    		   Publish_Data_Warning(fan_warning,warning);
    	        tx_thread_sleep(200);//HAL_Delay(200);

           }

    }



}
/*****************************************************************
	*
	*Function Name: 
	*Function ADC input channel be selected "which one channe"
	*Input Ref: which one ? AC_Channel_?, hexadecimal of average
	*Return Ref: No
	*
*****************************************************************/
static uint16_t compute_voltage(uint16_t raw_value) 
{
    uint64_t temp ;
	const uint32_t multiplier = 3462835200U;
   temp = (uint64_t)raw_value * multiplier;
    return (uint16_t)(temp >> 32);  // ��λ��mV
}





