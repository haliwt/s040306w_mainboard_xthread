#include "bsp.h"

#define DECODER_BIT_0        (1<< 0)

#define BIT_1                (1<<1)

#define BIT_2                (1<<2)

#define LOWEST_PRIORITY   1  // ???????
#define HIGHEST_PRIORITY  3


/***********************************************************************************************************
											函数声明
***********************************************************************************************************/
#define STACK_SIZE_ONE  1024//3072//2048//1024//896//768
#define STATC_SIZE_TWO  512//512//256


static TX_THREAD thread_msg;
static TX_THREAD thread_start;
/* 定义信号量 */
TX_SEMAPHORE decoder_semaphore;


static UCHAR stack_msg_pro[STACK_SIZE_ONE];
static UCHAR stack_start_pro[STATC_SIZE_TWO];

static void vTaskMsgPro(ULONG thread_input);
static void vTaskStart(ULONG thread_input);


/* 创建任务通信机制 */
//static void AppObjCreate(void);
static void power_run_handler(void);
static void wifi_run_handler(void);






uint8_t power_on_sound_flag ;


/**
 * @brief  :  static void vTaskStart(void *pvParameters
 * @note    
 * @param   None
 * @retval  None
 */

static void vTaskMsgPro(ULONG thread_input)
{
   (void)thread_input;  /* 消除未使用的参数警告 */
	while(1)
    {

		if(power_on_sound_flag==0){
            power_on_sound_flag ++;
            FAN_Stop();  //WT.EDIT.2025.01.03
            buzzer_sound();//buzzer_sound();
		

        }
      
	     power_run_handler();
       
         wifi_run_handler();

        
		 tx_thread_sleep(100);//100
		
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
			
	tx_semaphore_get(&decoder_semaphore, TX_NO_WAIT) ;
	 
			
	  // 从环形缓冲区取数据
        while(uart1_rx_tail != uart1_rx_head)
        {
            rx1_data = uart1_rx_buf[uart1_rx_tail];
            uart1_rx_tail = (uart1_rx_tail + 1) % UART1_RX_BUF_SIZE;

            // 调用协议解析函数
            usart1_isr_callback_handler(rx1_data);
        }
       if(gpro_t.decoder_success_flag==1){
	      decoder_handler();
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
  
	tx_thread_create(&thread_msg,                    /* 任务控制块地址 */ 
 	                 "MsgPro",                    /* 任务名 */
                     vTaskMsgPro,                 /* 启动任务函数地址 */
                     0,                           /* 传递给任务的参数 */
                     stack_msg_pro,                /* 堆栈基地址 */
                     STACK_SIZE_ONE,               /* 堆栈空间大小 */ 
                     2,								/* 任务优先级*/
                     2,								/* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,               /* 不开启时间片 */
                     TX_AUTO_START);                /* 创建后立即启动 */
 #if 1

    tx_thread_create(&thread_start,           /* 任务控制块地址 */    
    				 "Start",                 /* 任务名 */
                     vTaskStart,               /* 启动任务函数地址 */
                     0,                       /* 传递给任务的参数 */
                     stack_start_pro,         /* 堆栈基地址 */
                     STATC_SIZE_TWO,			/* 堆栈空间大小 */  
                     1, 						/* 任务优先级*/
                     1, 						/* 任务抢占阀值 */
                     TX_NO_TIME_SLICE, 			/* 不开启时间片 */
                     TX_AUTO_START);             /* 创建后立即启动 */
  #endif 

   /* 创建信号量 */
   tx_semaphore_create(&decoder_semaphore, "DecoderSemaphore", 0);
 
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
			if(gpro_t.stopTwoHours_flag ==0){
			   Fan_RunSpeed_Fun();
		    }

			//ack_handler();
            if(gpro_t.process_run_step > 10 || gpro_t.stopTwoHours_flag > 1){
				 if(gpro_t.process_run_step > 10 )gpro_t.process_run_step=6; //WT.EDIT 2025.10.07
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

}


