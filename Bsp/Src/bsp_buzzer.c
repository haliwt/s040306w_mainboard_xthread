#include "bsp.h"

static void beep_delay_us(uint32_t us) ;
static void beep_delay_ms(uint32_t ms) ;


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

  
  LL_TIM_OC_SetCompareCH4(TIM3,0);
  LL_TIM_DisableCounter(TIM3);
  LL_TIM_CC_DisableChannel(TIM3,LL_TIM_CHANNEL_CH4);
	

}




void buzzer_sound_once(void)
{
   
   tim3_buzzer_sound_config();
   //beep_delay_ms(20);
   delay_ms(20);
   tim3_stop_buzzer_sound_config();
   LL_TIM_DisableCounter(TIM3);

}





void buzzer_sound(void)
{
   
   tim3_buzzer_sound_config();
   tx_thread_sleep(20);
   tim3_stop_buzzer_sound_config();

}


/**
 * @brief  微秒级延时 (粗略)
 * @param  us: 延时微秒数
 * @note   64MHz 下, 1us 大约需要 64 个周期
 * 假设 while 循环 + NOP 占用 4 个周期, 则需要循环 16 次
 */
static void beep_delay_us(uint32_t us) 
{
    // 这里的 16 是基于 64MHz 的经验估算值，可能需要根据编译器优化等级微调
    uint32_t count = us * 16; 
    while (count--) {
        __NOP();
    }
}

/**
 * @brief  毫秒级延时
 * @param  ms: 延时毫秒数
 */
static void beep_delay_ms(uint32_t ms) 
 {
    while (ms--) {
        delay_us(1000);
    }
}

#if 0
void delay_us_nop(uint32_t us) 
{
    // 64MHz 频率下，1us 约执行 64 个时钟周期
    // 扣除循环跳转开销，这里大约取 12~16 次循环
    uint32_t delay = us * 12; 
    while (delay--) {
        __NOP(); 
		__NOP(); 
	    __NOP();
		__NOP();
    }
}

#endif 
