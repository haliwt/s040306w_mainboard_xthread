#include "bsp.h"

// 假设已定义全局变量 HCLK_MHz (例如 64)
#define HCLK_MHZ 64
static uint32_t g_fac_us = 0;       /* us延时倍乘数 */

#define SYS_SUPPORT_OS        0

/* 如果SYS_SUPPORT_OS定义了,说明要支持OS了(不限于UCOS) */
#if SYS_SUPPORT_OS     

/* 添加公共头文件 ( ucos需要用到) */
#include "os.h"

/* 定义g_fac_ms变量, 表示ms延时的倍乘数, 代表每个节拍的ms数, (仅在使能os的时候,需要用到) */
static uint16_t g_fac_ms = 0;

/*
 *  当delay_us/delay_ms需要支持OS的时候需要三个与OS相关的宏定义和函数来支持
 *  首先是3个宏定义:
 *      delay_osrunning    :用于表示OS当前是否正在运行,以决定是否可以使用相关函数
 *      delay_ostickspersec:用于表示OS设定的时钟节拍,delay_init将根据这个参数来初始化systick
 *      delay_osintnesting :用于表示OS中断嵌套级别,因为中断里面不可以调度,delay_ms使用该参数来决定如何运行
 *  然后是3个函数:
 *      delay_osschedlock  :用于锁定OS任务调度,禁止调度
 *      delay_osschedunlock:用于解锁OS任务调度,重新开启调度
 *      delay_ostimedly    :用于OS延时,可以引起任务调度.
 *
 *  本例程仅作UCOSII的支持,其他OS,请自行参考着移植
 */

#if 0
/* 支持UCOSII */
#define delay_osrunning     OSRunning           /* OS是否运行标记,0,不运行;1,在运行 */
#define delay_ostickspersec OS_TICKS_PER_SEC    /* OS时钟节拍,即每秒调度次数 */
#define delay_osintnesting  OSIntNesting        /* 中断嵌套级别,即中断嵌套次数 */


/**
 * @brief     us级延时时,关闭任务调度(防止打断us级延迟)
 * @param     无
 * @retval    无
 */
void delay_osschedlock(void)
{
    OSSchedLock();                      /* UCOSII的方式,禁止调度，防止打断us延时 */
}

/**
 * @brief     us级延时时,恢复任务调度
 * @param     无
 * @retval    无
 */
void delay_osschedunlock(void)
{
    OSSchedUnlock();                    /* UCOSII的方式,恢复调度 */
}

/**
 * @brief     us级延时时,恢复任务调度
 * @param     ticks: 延时的节拍数
 * @retval    无
 */
void delay_ostimedly(uint32_t ticks)
{
    OSTimeDly(ticks);                               /* UCOSII延时 */
}
#endif
#if 0
/**
 * @brief     systick中断服务函数,使用OS时用到
 * @param     ticks : 延时的节拍数  
 * @retval    无
 */  
void SysTick_Handler(void)
{
    /* OS 开始跑了,才执行正常的调度处理 */
    if (delay_osrunning == OS_TRUE)
    {
        /* 调用 uC/OS-II 的 SysTick 中断服务函数 */
        OS_CPU_SysTickHandler();
    }
    HAL_IncTick();
}
#endif
#endif
/**
 * @brief     初始化延迟函数
 * @param     sysclk: 系统时钟频率, 即CPU频率(rcc_c_ck), 64MHz
 * @retval    无
 */  

/**
 * @brief     延时nus
 * @note      无论是否使用OS, 都是用时钟摘取法来做us延时
 * @param     nus: 要延时的us数
 * @note      nus取值范围: 0 ~ (2^32 / fac_us) (fac_us一般等于系统主频, 自行套入计算)
 * @retval    无
 */
void delay_us(uint32_t us)
{
  #if 0
   uint32_t ticks = us * (SystemCoreClock / 1000000); // 目标时钟周期数
    uint32_t told = SysTick->VAL;
    uint32_t tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;

    while (tcnt < ticks)
    {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            } else {
                tcnt += reload - tnow + told;
            }
            told = tnow;
        }
    }
	#else 
	
	uint32_t start = SysTick->VAL;
	uint32_t ticks = us * (SystemCoreClock / 1000000);	 // 64 ticks = 1us
	uint32_t reload = SysTick->LOAD;
	uint32_t now;
	uint32_t elapsed = 0;
	 SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	while (elapsed < ticks)
	{
		now = SysTick->VAL;

		if (now <= start)
			elapsed += start - now; 		 // 正常递减
		else
			elapsed += start + (reload - now); // SysTick 重装载

		start = now;
	}


	#endif 

}

/**
 * @brief     延时nms
 * @param     nms: 要延时的ms数 (0< nms <= (2^32 / fac_us / 1000))(fac_us一般等于系统主频, 自行套入计算)
 * @retval    无
 */
void delay_ms(uint16_t nms)
{
       delay_us((uint32_t)(nms * 1000));                   /* 普通方式延时 */
}

// 获取当前毫秒数
uint32_t millis(void) 
{
    return HAL_GetTick();   // 或者用 LL 的全局 tick 变量
}

// 非阻塞延时检查
bool delayExpired(uint32_t start, uint32_t delay_ms) 
{
    return (millis() - start) >= delay_ms;
}




#if 0
/**
 * @brief       LL 使用TIM2 定时器作为us级延时.
 * @note        LL库的,TIM2 是32位定时器
 * @param       Delay : 要延时的毫秒数
 * @retval      None
 */
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_bus.h"

void TIM2_Init(void)
{
    // 使能 TIM2 时钟
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

    // 假设系统时钟 64 MHz
    // 预分频器设为 64-1 = 63 → 定时器计数频率 = 1 MHz (1 tick = 1 us)
    LL_TIM_SetPrescaler(TIM2, 64 - 1);

    // 自动重装载寄存器设为最大值
    LL_TIM_SetAutoReload(TIM2, 0xFFFFFFFF);

    // 启动计数器
    LL_TIM_EnableCounter(TIM2);
}

void delay_us(uint32_t us)
{
    uint32_t start = LL_TIM_GetCounter(TIM2);
    while ((LL_TIM_GetCounter(TIM2) - start) < us);
}


#endif

