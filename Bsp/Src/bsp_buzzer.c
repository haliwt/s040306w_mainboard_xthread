#include "bsp.h"


void tim3_buzzer_sound_config(void)
{
    LL_TIM_DisableCounter(TIM3);
    LL_TIM_GenerateEvent_UPDATE(TIM3);
   LL_TIM_SetAutoReload(TIM3, 249); //1/(249+1) =0.004MHZ = 4KHZ buzzer sound 
   LL_TIM_OC_SetCompareCH4(TIM3, 125);
   LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH4);
   LL_TIM_EnableCounter(TIM3);
}

static void tim3_stop_buzzer_sound_config(void)
{
  //LL_TIM_DisableCounter(TIM3);
 // LL_TIM_CC_DisableChannel(TIM3,LL_TIM_CHANNEL_CH4);
	LL_TIM_OC_SetCompareCH4(TIM3,0);


}








void buzzer_sound(void)
{
   
   tim3_buzzer_sound_config();
   tx_thread_sleep(20);
   tim3_stop_buzzer_sound_config();




}


