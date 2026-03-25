#include "bsp.h"

#define FAN_PWM_100     40

#define FAN_PWM_90      36

#define FAN_PWM_80      32



uint8_t fan_switch_gears_flag;

static void tim16_stop_fan_pmw_config(void);
//static void SetLevel_Fan_PWMA(uint8_t levelval);


/******************************************************************************
*
*Fan adjut speed frequency is : 18KHz~25KHz 
*
*
*******************************************************************************/
void fan_run_fun(void)
{
    FAN_COM_SetLow();
	FAN_RUN_SetHigh();
	#if NEWPCB_FAN
	  SetLevel_Fan_PWMA(FAN_PWM_100);
	#else



	#endif 
	

}
void FAN_Stop(void)
{
   FAN_COM_SetLow(); //brake
   FAN_RUN_SetLow();//SetLevel_Fan_PWMA(0);//SetLevel_Fan_PWMA(16);
   tim16_stop_fan_pmw_config();
  
}

void Fan_One_Power_Off_Speed(void)
{
	
	 fan_run_fun();//SetLevel_Fan_PWMA(10);
}


void Fan_One_Speed(void)
{
   static uint8_t one_speed=0xff;
	 FAN_COM_SetLow();
     FAN_RUN_SetHigh();
     if(one_speed != fan_switch_gears_flag){
        fan_switch_gears_flag++;
        one_speed = fan_switch_gears_flag ;  //one_speed =2,5,8
        #if NEWPCB_FAN
	       SetLevel_Fan_PWMA(FAN_PWM_100);
		#endif 

     }


}

void Fan_Two_Speed(void)
{
     static uint8_t two_speed=0xff;
      FAN_COM_SetLow();
     FAN_RUN_SetHigh();
      if(two_speed != fan_switch_gears_flag){
         fan_switch_gears_flag++;
         two_speed = fan_switch_gears_flag;  //two_speed = 3;6,9

         #if NEWPCB_FAN
	        SetLevel_Fan_PWMA(FAN_PWM_90);
		 #endif 
       }

}

 void Fan_Full_Speed(void)
{
   static uint8_t full_speed = 0xff;
     FAN_COM_SetLow();
     FAN_RUN_SetHigh();
    if(full_speed != fan_switch_gears_flag){
        fan_switch_gears_flag++;
         full_speed = fan_switch_gears_flag;  //full_speed =1,4,7,10
         #if  NEWPCB_FAN
             SetLevel_Fan_PWMA(FAN_PWM_80);
		 #endif 
    }

}









void Fan_RunSpeed_Fun(void)
{

    if(gctl_t.set_wind_speed_value < 34 ){
        Fan_One_Speed();
    }
    else if(gctl_t.set_wind_speed_value > 33  && gctl_t.set_wind_speed_value < 67 ){

        Fan_Two_Speed();

    }
    else if(gctl_t.set_wind_speed_value > 66){

         Fan_Full_Speed();

    }

      

}

void fan_full_run(void)
{

  Fan_Full_Speed();

}


/********************************************************
*
*Function Name:void SetLevel_Fan_PWMA(uint8_t levelval)
*Function: 
*
*
********************************************************/
void SetLevel_Fan_PWMA(uint8_t levelval)
{
    
 //  MX_TIM16_Init();
 //  HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
    LL_TIM_DisableCounter(TIM16);
    LL_TIM_OC_SetMode(TIM16, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_SetAutoReload(TIM16,39);//PWM = 1/(39+1)MHZ = 0.025MHZ = 25KHZ.
	LL_TIM_OC_SetCompareCH1(TIM16,levelval); //pwm duty = 20/40 =50%
	LL_TIM_OC_EnablePreload(TIM16, LL_TIM_CHANNEL_CH1);
	
	LL_TIM_EnableARRPreload(TIM16);   // 建议加上，避免更新不同步
	  // 4. ✅【关键添加】使能高级定时器的主输出
    LL_TIM_EnableAllOutputs(TIM16); // 此函数会设置BDTR寄存器的MOE位
	LL_TIM_EnableCounter(TIM16);
	LL_TIM_CC_EnableChannel(TIM16,LL_TIM_CHANNEL_CH1);
	
	
}

void tim16_stop_fan_pmw_config(void)
{
  LL_TIM_DisableCounter(TIM16);
  LL_TIM_CC_DisableChannel(TIM16,LL_TIM_CHANNEL_CH1);


}

//void fan_run(void)
//{
//	if(gpro_t.stopTwoHours_flag ==0 && gpro_t.gpower_on==power_on){
//				 Fan_RunSpeed_Fun();
//	}


//}




