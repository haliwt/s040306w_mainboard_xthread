#include "bsp.h"



/***********************************************************************************************************
											函数声明
***********************************************************************************************************/
#define STACK_SIZE_ONE  1792//3072//2048//1024//896//768
#define STATC_SIZE_TWO  512//256

#define EVT_UART_TX_DONE   (1u << 0)



static TX_THREAD thread_msg;
static TX_THREAD thread_start;
/* 定义信号量 */
TX_SEMAPHORE decoder_semaphore;
TX_SEMAPHORE wifi_tx_semaphore;
TX_SEMAPHORE disp_tx_semaphore;
/*队列*/
//static TX_QUEUE uart1_rx_queue;
//static uint8_t uart1_rx_queue_buffer[UART1_RX_BUF_SIZE * sizeof(uint8_t)];
//TX_EVENT_FLAGS_GROUP      evt_flags;

static UCHAR stack_msg_pro[STACK_SIZE_ONE];
static UCHAR stack_start_pro[STATC_SIZE_TWO];

static void vTaskMsgPro(ULONG thread_input);
static void vTaskStart(ULONG thread_input);


/* 创建任务通信机制 */
//static void AppObjCreate(void);

static void wifi_run_handler(void);
static void power_run_handler(void);
uint8_t power_on_sound_flag ;
uint8_t counter, success_flag ;



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
         counter++;
	     power_run_handler();
       
         wifi_run_handler();

        
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

  UINT status ;
  /* 创建信号量 */
   tx_semaphore_create(&decoder_semaphore, "DecoderSemaphore", 0);
   tx_semaphore_create(&wifi_tx_semaphore, "wifi_tx_semaphore",0);
   status = tx_semaphore_create(&disp_tx_semaphore,"disp_tx_semaphore",0);
   if(status ==TX_SUCCESS){
        success_flag =1;
   }
   else{
        success_flag =2;

   }
  // status  = tx_event_flags_create(&evt_flags,"evt_flags");
   
//	tx_queue_create(&uart1_rx_queue,
//					"Uart1RxQueue",
//					TX_1_ULONG,   // 每个消息大小，这里用 1 字节
//					uart1_rx_queue_buffer,
//					sizeof(uart1_rx_queue_buffer));
  
	tx_thread_create(&thread_msg,                    /* 任务控制块地址 */ 
 	                 "MsgPro",                    /* 任务名 */
                     vTaskMsgPro,                 /* 启动任务函数地址 */
                     0,                           /* 传递给任务的参数 */
                     stack_msg_pro,                /* 堆栈基地址 */
                     STACK_SIZE_ONE,               /* 堆栈空间大小 */ 
                     1,								/* 任务优先级*/
                     1,								/* 任务抢占阀值 */
                     TX_NO_TIME_SLICE,               /* 不开启时间片 */
                     TX_AUTO_START);                /* 创建后立即启动 */
 #if 1

    tx_thread_create(&thread_start,           /* 任务控制块地址 */    
    				 "Start",                 /* 任务名 */
                     vTaskStart,               /* 启动任务函数地址 */
                     0,                       /* 传递给任务的参数 */
                     stack_start_pro,         /* 堆栈基地址 */
                     STATC_SIZE_TWO,			/* 堆栈空间大小 */  
                     0, 						/* 任务优先级*/
                     0, 						/* 任务抢占阀值 */
                     TX_NO_TIME_SLICE, 			/* 不开启时间片 */
                     TX_AUTO_START);             /* 创建后立即启动 */
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

/******************************************************************************
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
/******************************************************************************
	**
	*Function Name:
	*Function :
	*Input Ref: 
	*Return Ref:NO
	*
*******************************************************************************/
void display_board_xtask_notice(void)
{

  tx_semaphore_put(&decoder_semaphore);
    // 投递到队列
   // tx_queue_send(&uart1_rx_queue, &data, TX_NO_WAIT);

}

void tx_wifi_flag_put_hander(void)
{

  //tx_event_flags_set(&evt_flags, EVT_UART_TX_DONE, TX_OR);
  tx_semaphore_put(&wifi_tx_semaphore);  // 通知发送完成

}

void tx_wifi_flag_get_hander(void)
{
  tx_semaphore_get(&wifi_tx_semaphore, TX_WAIT_FOREVER);
}

void tx_disp_flag_put_hander(void)
{

  //tx_event_flags_set(&evt_flags, EVT_UART_TX_DONE, TX_OR);
  tx_semaphore_put(&disp_tx_semaphore);  // 通知发送完成

}

void tx_disp_flag_get_hander(void)
{
//   ULONG actual;
//	tx_event_flags_get(&evt_flags,
//	                   EVT_UART_TX_DONE,
//	                   TX_OR_CLEAR,
//	                   &actual,
//	                   TX_WAIT_FOREVER);

  
   tx_semaphore_get(&disp_tx_semaphore, TX_WAIT_FOREVER);


}


