#include "bsp.h"
#include "tx_api.h"



/***********************************************************************************************************
											函数声明
***********************************************************************************************************/
// 使用数组对齐，确保 8 字节对齐
#define ALIGN_8BYTE(size) (((size) + 7) & ~7)

// 建议将堆栈大小适当调大一点点，或者使用预留在 RAM 最后的区域
//static uint64_t stack_msg_pro_ptr[ALIGN_8BYTE(STACK_SIZE_ONE) / 8];
//static uint64_t stack_start_pro_ptr[ALIGN_8BYTE(STATC_SIZE_TWO) / 8];



#define STACK_SIZE_ONE  1536//1792//3072//2048//1024//896//768
#define STATC_SIZE_TWO  512//256

/*在 ThreadX 里，优先级数字越小，优先级越高：*/

static TX_THREAD thread_msg;
static TX_THREAD thread_start;
/* 定义信号量 */
TX_SEMAPHORE decoder_semaphore;
/*队列*/
//static TX_QUEUE uart1_rx_queue;
//static uint8_t uart1_rx_queue_buffer[UART1_RX_BUF_SIZE * sizeof(uint8_t)];


static UCHAR stack_msg_pro[STACK_SIZE_ONE];
static UCHAR stack_start_pro[STATC_SIZE_TWO];

static void vTaskMsgPro(ULONG thread_input);
static void vTaskStart(ULONG thread_input);


/* 创建任务通信机制 */
//static void AppObjCreate(void);

static void wifi_run_handler(void);
static void power_run_handler(void);


volatile uint8_t tx_error_flag,counter_flag;


static void tx_thread_stack_error_handler(TX_THREAD *thread_ptr);

static void debug_stack_check(void);
ULONG unused =0;
/**
 * @brief  :  static void vTaskStart(void *pvParameters
 * @note    
 * @param   None
 * @retval  None
 */

void tx_application_define(void *first_unused_memory)
{
   tx_thread_stack_error_notify(tx_thread_stack_error_handler);

    // 创建线程、信号量、事件组、队列
     threadx_handler();
}



/**
 * @brief  :  static void vTaskStart(void *pvParameters
 * @note    
 * @param   None
 * @retval  None
 */
 static void vTaskMsgPro(ULONG thread_input)
{
   (void)thread_input;  /* 消除未使用的参数警告 */
   static uint8_t power_on_sound_flag ;
	while(1)
    {

		if(power_on_sound_flag==0){
            power_on_sound_flag ++;
            FAN_Stop();  //WT.EDIT.2025.01.03
            buzzer_sound_once();//buzzer_sound();//buzzer_sound();
            read_sensorData();
		

        }
         counter_flag ++;
	     power_run_handler();
       
         wifi_run_handler();
         #if DEBUG_ENABLE
		   debug_stack_check();

		 #endif 

         LL_IWDG_ReloadCounter(IWDG);
		 tx_thread_sleep(20);//100
		
	}
      
 }

 /**
  * @brief	:  static void vTaskStart(void *pvParameters
  * @note	 
  * @param	 None
  * @retval  None
  */
 static void vTaskStart(ULONG thread_input)
 {
   (void)thread_input;  /* 消除未使用的参数警告 */
  
  
   while(1){
		
	// 阻塞等待 ISR 投递
      if(tx_semaphore_get(&decoder_semaphore, TX_WAIT_FOREVER) == TX_SUCCESS)
      {
              // 或者直接调用解码器
             // if(gpro_t.decoder_success_flag==1){
			  	 /// gpro_t.decoder_success_flag =0;
                 decoder_handler();

              //}
                
       }
    } 
}
 

 /**
 * @brief  :  void AppTaskCreate (void)�����ݴ����������ȼ�Ϊ�е�
 * @note    �����ڲ�ʹ�ö��н������ݣ����ȳ�ʼ������
 * @param   None
 * @retval  None
 */
void threadx_handler(void)
{

      /* 创建信号量 */
   tx_semaphore_create(&decoder_semaphore, "DecoderSemaphore", 0);
   
//	tx_queue_create(&uart1_rx_queue,
//					"Uart1RxQueue",
//					TX_1_ULONG,   // 每个消息大小，这里用 1 字节
//					uart1_rx_queue_buffer,
//					sizeof(uart1_rx_queue_buffer));
  
	tx_thread_create(&thread_msg,                  /* 任务控制块地址 */ 
 	                 "MsgPro",                     /* 任务名 */
                     vTaskMsgPro,                  /* 启动任务函数地址 */
                     0,                            /* 传递给任务的参数 */
                     stack_msg_pro,                /* 堆栈基地址 */
                     STACK_SIZE_ONE,               /* 堆栈空间大小 */ 
                     1,							   /* 任务优先级*/
                     1,							   /* 任务抢占阀值 , 允许它不被优先级 1-0 之间的任务抢占，除非是中断 */
                     TX_NO_TIME_SLICE,             /* 不开启时间片 */
                     TX_AUTO_START);               /* 创建后立即启动 */
 #if 1

    tx_thread_create(&thread_start,                /* 任务控制块地址 */    
    				 "Start",                      /* 任务名 */
                     vTaskStart,                   /* 启动任务函数地址 */
                     0,                            /* 传递给任务的参数 */
                     stack_start_pro,              /* 堆栈基地址 */
                     STATC_SIZE_TWO,			   /* 堆栈空间大小 */  
                     0, 						   /* 任务优先级*/
                     0, 						   /* 任务抢占阀值 */
                     TX_NO_TIME_SLICE, 			   /* 不开启时间片 */
                     TX_AUTO_START);               /* 创建后立即启动 */
  #endif 


 
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
            link_wifi_to_tencent_handler(); //detected ADC of value 
            ai_mode_display_fun();
		

			//ack_handler();
            if(gpro_t.process_run_step > 15 || gpro_t.stopTwoHours_flag > 1){
				 if(gpro_t.process_run_step > 15 )gpro_t.process_run_step=6; //WT.EDIT 2025.10.07
				 if(gpro_t.stopTwoHours_flag > 1 )gpro_t.stopTwoHours_flag =0;
            }
		   
			
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
      if(gpro_t.process_run_step > 10){

		      gpro_t.process_run_step=6;
			  

		  }
          else if(gpro_t.wifi_led_fast_blink_flag > 1){
		  	 gpro_t.wifi_led_fast_blink_flag=0;
			
		  }
		  
		  if(gpro_t.wifi_led_fast_blink_flag==0 ){
             wifi_communication_tnecent_handler();//
        
             getBeijingTime_cofirmLinkNetState_handler();
	
             wifi_auto_detected_link_state();
		
           }

}


void display_board_xtask_notice(void)
{

  tx_semaphore_put(&decoder_semaphore);
    // 投递到队列
   // tx_queue_send(&uart1_rx_queue, &data, TX_NO_WAIT);

}

static void tx_thread_stack_error_handler(TX_THREAD *thread_ptr)
{
    // 这里可以打印日志、点灯、复位等
    //printf("Stack overflow detected in thread: %s\n", thread_ptr->tx_thread_name);
     tx_error_flag ++;
    // 或者进入安全模式
}
#if DEBUG_ENABLE
static void debug_stack_check(void)
{
    ULONG i;
   // ULONG unused = 0;
   ULONG temp_unused = 0; // 使用局部变量进行统计
    // 从数组起始位置（栈底/低地址）开始数连续的 0xEF
    for (i = 0; i < STACK_SIZE_ONE; i++)
    {
        if (stack_msg_pro[i] == 0xEF)
            temp_unused++;
        else
            break; 
    }
	unused = temp_unused;  // 统计完后再赋值给全局变量，方便 Watch 窗口查看
    // 剩下的 unused 就是你安全的“护城河”
    // 如果 unused < 100 字节，你的 G030 就危险了！
}

#endif 
