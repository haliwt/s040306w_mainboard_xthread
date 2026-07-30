#include "bsp.h"
#include "tx_api.h"


#define DEBUG_ENABLE      0

/***********************************************************************************************************
											函数声明
***********************************************************************************************************/
// 使用数组对齐，确保 8 字节对齐
#define ALIGN_8BYTE(size) (((size) + 7) & ~7)

// 建议将堆栈大小适当调大一点点，或者使用预留在 RAM 最后的区域
//static uint64_t stack_msg_pro_ptr[ALIGN_8BYTE(STACK_SIZE_ONE) / 8];
//static uint64_t stack_start_pro_ptr[ALIGN_8BYTE(STATC_SIZE_TWO) / 8];



#define STACK_SIZE_UI    1664//1536//1280//640//1024//1920//1792//1536
#define STACK_SIZE_DEC   1280//1024//512//1024//512//256
//#define STACK_SIZE_WIFI  640


__attribute__((aligned(8))) static UCHAR stack_msg_pro[STACK_SIZE_UI];
__attribute__((aligned(8))) static UCHAR stack_dec_pro[STACK_SIZE_DEC];
//__attribute__((aligned(8))) static UCHAR stack_wifi_pro[STACK_SIZE_WIFI];



/*在 ThreadX 里，优先级数字越小，优先级越高：*/

static TX_THREAD thread_msg;
static TX_THREAD thread_decoder;
//static TX_THREAD thread_wifi;
/* 定义信号量 */
TX_SEMAPHORE decoder_semaphore;

//TX_TIMER  buzzer_timer;

/*队列*/



static void main_thread_entry(ULONG thread_input);
static void decoder_thread_entry(ULONG thread_input);
//static void wifi_thread_entry(ULONG thread_input);

//static void buzzer_timer_callback(ULONG input);





static void wifi_run_handler(void);
static void power_run_handler(void);


volatile uint8_t tx_error_flag;


static void tx_thread_stack_error_handler(TX_THREAD *thread_ptr);
#if DEBUG_ENABLE
static void debug_stack_ui_check(void);
static void debug_stack_decoder_check(void);
static void debug_stack_wifi_check(void);

ULONG ui_unused =0,dec_unused,wifi_unused;

#endif 
uint16_t ui_counter,wifi_counter,dec_cnt;

/**
 * @brief  :  
 * @note    
 * @param   None
 * @retval  None
 */

void tx_application_define(void *first_unused_memory)
{
   #if DEBUG_ENABLE
     // --- 关键点：在创建任务之前填充魔术字 ---
     memset(stack_msg_pro, 0xEF, sizeof(stack_msg_pro));
     memset(stack_dec_pro, 0xEF, sizeof(stack_dec_pro));
	// memset(stack_wifi_pro, 0xEF, sizeof(stack_wifi_pro));
   #endif 
    tx_thread_stack_error_notify(tx_thread_stack_error_handler);

    // 创建线程、信号量、事件组、队列
     threadx_handler();
}
/**
 * @brief  : 
 * @note    
 * @param   None
 * @retval  None
 */
 static void main_thread_entry(ULONG thread_input)
{
   (void)thread_input;  /* 消除未使用的参数警告 */
   static uint8_t power_on_sound_flag ;
	while(1)
    {
         power_run_handler();
         wifi_run_handler();
         
         #if DEBUG_ENABLE
		 
		   debug_stack_ui_check();

		 #endif 
         //ui_counter++;
         LL_IWDG_ReloadCounter(IWDG);
		 tx_thread_sleep(20);//10ms*20 =200ms
		
    }
}
/**
  * @brief	:  
  * @note	 
  * @param	 None
  * @retval  None
  */
 static void decoder_thread_entry(ULONG thread_input)
 {
   (void)thread_input;  /* 消除未使用的参数警告 */
  
  
   while(1){
		
  
      // 阻塞等待 ISR 投递
      if(tx_semaphore_get(&decoder_semaphore, TX_WAIT_FOREVER) == TX_SUCCESS)
      {
            
              decoder_handler();

         
            #if DEBUG_ENABLE
		     debug_stack_decoder_check();

		     #endif 
      }
	  
	  
    } 
}
 

 /**
 * @brief  : 
 * @note    
 * @param   None
 * @retval  None
 */
void threadx_handler(void)
{

      /* 创建信号量 */
   tx_semaphore_create(&decoder_semaphore, "DecoderSemaphore", 0);
   
  
	tx_thread_create(&thread_msg,                  /* 任务控制块地址 */ 
 	                 "MsgPro",                     /* 任务名 */
                     main_thread_entry,                  /* 启动任务函数地址 */
                     0,                            /* 传递给任务的参数 */
                     stack_msg_pro,                /* 堆栈基地址 */
                     STACK_SIZE_UI,               /* 堆栈空间大小 */ 
                     2,							   /* 任务优先级*/
                     2,							   /* 任务抢占阀值 , 允许它不被优先级 1-0 之间的任务抢占，除非是中断 */
                     TX_NO_TIME_SLICE,             /* 不开启时间片 TX_NO_TIME_SLICE*/
                     TX_AUTO_START);               /* 创建后立即启动 */


    tx_thread_create(&thread_decoder,                /* 任务控制块地址 */    
    				 "Decoder",                      /* 任务名 */
                     decoder_thread_entry,                   /* 启动任务函数地址 */
                     0,                            /* 传递给任务的参数 */
                     stack_dec_pro,              /* 堆栈基地址 */
                     STACK_SIZE_DEC,			   /* 堆栈空间大小 */  
                     1, 						   /* 任务优先级*/
                     1, 						   /* 任务抢占阀值 */
                     TX_NO_TIME_SLICE, 			            /* 不开启时间片 TX_NO_TIME_SLICE*/
                     TX_AUTO_START);               /* 创建后立即启动 */


 
}
/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
static void power_run_handler(void)
{
    switch(gpro_t.gpower_on){ 

            case power_on:
		 
			
            power_on_handler();
        
		  break;

		  case power_off:
		  
          
		      power_off_handler();
             break;
          }


}

/********************************************************************************
	**
	*Function Name:static void wifi_run_handler(void)
	*Function :
	*Input Ref: 
	*Return Ref:NO
	*
*******************************************************************************/
static void wifi_run_handler(void)
{
	static uint8_t time_slot =0;

	if(gpro_t.wifi_led_fast_blink_flag==0 ){

	switch(time_slot){

	case 0:

		wifi_communication_tnecent_handler();//

	break;

	case 1:

		getBeijingTime_cofirmLinkNetState_handler();

	break;

	case 2:

		wifi_auto_detected_link_state();

	break;

	default:
	break;

    }

	}

	time_slot ++;
	if(time_slot > 2) time_slot = 0;

}


void display_board_xtask_notice(void)
{

  //tx_semaphore_put(&decoder_semaphore);
    // 投递到队列
   // tx_queue_send(&uart1_rx_queue, &data, TX_NO_WAIT);
   tx_semaphore_ceiling_put(&decoder_semaphore,1);

}

static void tx_thread_stack_error_handler(TX_THREAD *thread_ptr)
{
    // 这里可以打印日志、点灯、复位等
    //printf("Stack overflow detected in thread: %s\n", thread_ptr->tx_thread_name);
     tx_error_flag ++;
    // 或者进入安全模式
}
#if 0
void buzzer_timer_callback(ULONG input)
{
   (void)input;
   buzzer_sound_close();
}

void buzzer_sound_open(void)
{
  tx_timer_activate(&buzzer_timer);
}

void tx_thread_set_sound_once(void)
{
 tx_timer_change(&buzzer_timer,2,0);
}
#endif 
#if DEBUG_ENABLE
static void debug_stack_ui_check(void)
{
    ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计
    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_UI; i++)
    {
        if (stack_msg_pro[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
	ui_unused = temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！
}

static void debug_stack_decoder_check(void)
{
    ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计
    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_DEC; i++)
    {
        if (stack_dec_pro[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
	dec_unused = temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！
}
#if 0
static void debug_stack_wifi_check(void)
{
    ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计
    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_WIFI; i++)
    {
        if (stack_wifi_pro[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
	wifi_unused = temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！
}
#endif 
#endif 
