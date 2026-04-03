#include "bsp.h"

// 为协议中的魔术字节定义常量，提高可读性
#define FRAME_HEADER        0xA5        //receive display board header  
#define FRAME_NUM           0x02          //main deviece number is 0x10 
#define FRAME_OLD_NUM       0x01          //older version device NUM
#define FRAME_ACK_NUM       0x80          //new version from main answer singnal 0x80 new version . 
#define FRAME_END_BYTE              0xFE
#define DATA_FRAME_TYPE_INDICATOR   0x0F
#define FRAME_COPY_NUM              0xFF   //this is older version .

#define ACK_SUCCESS 0x00U
#define ACK_FAILURE 0x01U




volatile uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
volatile uint16_t uart1_rx_head ;
volatile uint16_t uart1_rx_tail ;


typedef void (*Usart1RxCallback)(uint8_t data);

static Usart1RxCallback usart1_rx_cb = NULL;  //定义一个全局静态函数指针

//static void usart1_invoke_callback(uint8_t data);


//static void usart1_isr_callback_handler(uint8_t data);
static void parse_recieve_copy_data(uint8_t *pddata);


uint8_t rx_inputBuf[12];
uint8_t check_bcc_code;
uint8_t rx1_data;
uint8_t counter_power_flag;
uint8_t ptc_onoff_default ;


//提供注册接口
void usart1_register_rx_callback(Usart1RxCallback cb)
{
   usart1_rx_cb = cb;

}

void usart1_invoke_callback(uint8_t data)
{
   if(usart1_rx_cb !=NULL){

       usart1_rx_cb(data);
   }


}


void callback_register_usart1_rx(void)
{

   usart1_register_rx_callback(usart1_isr_callback_handler);

}




volatile uint8_t rx_state;



typedef enum{

  open =1,
  close =2,
  no_change =0  

}atcion_state_e;



typedef enum {
    UART_STATE_WAIT_HEADER = 0,
    UART_STATE_NUM=1,
    UART_STATE_CMD_NOTICE=2,
    UART_STATE_EXEC_CMD_OR_LEN=3,
    UART_STATE_FRAME_END=4,
    UART_STATE_BCC_CHECK,
    UART_STATE_OLDER_BCC_CHECK,
    UART_STATE_DATA_LEN,
    UART_STATE_DATA,
    UART_STATE_DATA_END,
    UART_STATE_DATA_BCC
} uart_parse_state_t;

typedef enum{

    power_on_off=1,
    ptc_on_off=2,
    plasma_on_off=3,
    ultrasonic_on_off=4,
    wifi_link=5,
    buzzer_sound_s=6,
    ai_mode=7,
    temp_high_warning=8,
    fan_warning_s=9,
    fan_on_off = 0x0B,

     //notice no sound 
    ack_power_on_off = 0x10,
    ack_ptc_on_off = 0x12,
    ack_plasma_on_ff= 0x13,
    ack_ultrasonic_on_off = 0x14,
}signal_parase_t;


typedef struct Msg
{
    
	uint8_t   cmd_notice;
	uint8_t   execuite_cmd_notice;
	uint8_t   copy_cmd_flag;
	uint8_t   rx_data_flag;	
    uint8_t   bcc_check_code;
	uint8_t   check_code_hex;
    uint8_t   receive_data_length;
    uint8_t   data_length;
	uint8_t   rc_data_length;
	uint8_t   total_data_length;
    uint8_t   rx_end_code;
	uint8_t   rx_total_numbers;
	uint8_t   rx_data[4];
	uint8_t   usData[12];
	uint8_t   desData[12];

}MSG_T;

MSG_T   gl_tMsg; 

uint8_t inputBuf[1];
uint8_t wifi_rx_inputBuf[WIFI_RX_NUMBERS];


static void usart1_protocol_state_machine(uint8_t *pdata);

static void receive_cmd_or_notice_handler(void);

static void parse_recieve_copy_data_handler(void);



volatile uint8_t rx_data_counter=0;

uint8_t parse_exit_flag,parse_decoder_flag;
/********************************************************************************
	**
	*Function Name:void usart1_isr_callback_handler(void)
	*Function :  this is receive data from mainboard.
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
#if 1
void usart1_isr_callback_handler(uint8_t data)
{
   
	switch(rx_state){

	 case 0:
		if(data == FRAME_HEADER){
			rx_data_counter=0;
			gl_tMsg.usData[rx_data_counter]=data;
			
			rx_state =1;

		}
		else{
		   rx_state =0;

		}
	 break;

	 case 1:
			rx_data_counter++;
			gl_tMsg.usData[rx_data_counter]=data;

	        if(gl_tMsg.usData[rx_data_counter]==0x01 || gl_tMsg.usData[rx_data_counter]==0x02){
			 
				 rx_state = 2;
			 }
			 else{
				rx_state = 0;
				
             }

	 break;

	 case 2:
	    // if(gpro_t.decoder_success_flag==0){
		 	
		   rx_data_counter++;
           gl_tMsg.usData[rx_data_counter]=data;
		   
		  if(gl_tMsg.usData[rx_data_counter]==0xFE && rx_data_counter> 4){
		      rx_state = 3;
		  }
		 
     break;
			 
	 case 3:
		       rx_data_counter++;
	           gl_tMsg.usData[rx_data_counter]=data;
			 
	           rx_state = 0;
               gl_tMsg.rx_total_numbers = rx_data_counter;

			   gl_tMsg.rx_end_code = 0;
			   
		       gpro_t.decoder_success_flag=1;

			   gl_tMsg.bcc_check_code = data;

               display_board_xtask_notice();

	 break;

	 default:
	   rx_state =0;

	 break;

	 }

}
#else 
void usart1_isr_callback_handler(uint8_t data)
{
    switch(rx_state) {
        case 0: // 寻找帧头 0xA5
            if(data == FRAME_HEADER) {
                rx_data_counter = 0;
                gl_tMsg.usData[rx_data_counter] = data;
                rx_state = 1; 
                gpro_t.decoder_success_flag = 0; // 开始新的一帧，清除成功标志
            }
            break;

        case 1: // 接收数据体
            rx_data_counter++;
            if(rx_data_counter < 12) { // 防止数组越界
                gl_tMsg.usData[rx_data_counter] = data;
                
                // 关键点：只有当收到 0xFE 时，才认为数据接收完毕，准备接收最后的校验位
                if(data == FRAME_END_BYTE) {
                    rx_state = 2;
                }
            } else {
                // 超长错误，重置
                rx_state = 0;
            }
            break;

        case 2: // 接收最后的 BCC 校验位
            rx_data_counter++;
            if(rx_data_counter < 12) {
                gl_tMsg.usData[rx_data_counter] = data;
                gl_tMsg.bcc_check_code = data; // 存入校验码
                gl_tMsg.rx_total_numbers = rx_data_counter + 1;
                
                // --- 校验逻辑 ---
                // 这里可以写一个简单的循环计算 BCC，如果匹配再置标志位
                gpro_t.decoder_success_flag = 1; 
                
                // 只有一整帧完全接收且校验过，才唤醒任务
                //display_board_xtask_notice(); 
            }
            rx_state = 0; // 处理完无论成功失败，必须回到状态0等待新帧
            break;

        default:
            rx_state = 0;
            break;
    }
}



#endif 
/********************************************************************************
	**
	*Function Name:void usart1_protocol_state_machine(void)
	*Function :  in process bsp_freertos.c xTaskMsgPro
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
static void usart1_protocol_state_machine(uint8_t *pdata)
{

   static uint8_t ptc_set_wifi = 0xff;
   switch(pdata[2]){

   case 0:

   break;

   case power_on_off: 

         
        if(pdata[3] == 0x01){ //open
                
		        gpro_t.process_run_step=0;
				
		        buzzer_sound();//buzzer_sound_fun();
	            SendWifiData_Answer_Cmd(0x01,0x01);
	            tx_thread_sleep(100);
				if(gpro_t.soft_version > 2)gpro_t.soft_version =0;
				if(gpro_t.soft_version ==0 && gctl_t.app_timer_power_on_flag==0){
				   gpro_t.gpower_on = power_on;
				   fan_full_run();//WT.EDIT 2026.01.26
				   PLASMA_SetHigh();
                   ultrasonic_open();   //ultrasnoic ON 
                   PTC_SetHigh();

				}
	         
	           

		 }
        else if(pdata[3] == 0x0){ //close 

		     counter_power_flag ++;
			 buzzer_sound();
		     gpro_t.power_off_run_step=1;
             gpro_t.gpower_on = power_off;
			 PTC_SetLow();
              if(gpro_t.soft_version ==2){
                SendWifiData_Answer_Cmd(0x01,0x0); //power off .
                tx_thread_sleep(100); 
              }
              else{
			  	
			    SendWifiData_Answer_Cmd(0x01,0x02); //compatible older version 
	            tx_thread_sleep(100);
			     
              }
      
       }

     break;

	  case ptc_on_off: //PTC key of command .

       if(pdata[3] == 0x01 ){//phone_cmd_power

	     buzzer_sound();
	
			 gpro_t.rx_ptc_flag = 1;
			 gctl_t.ptc_prohibit_on_flag=0;
			 
			 if(gpro_t.stopTwoHours_flag==0){//two hours have a rest ten minutes .
	         if(gpro_t.ptc_warning ==0 && gpro_t.fan_warning_flag ==0){ //PTC warning flag
	             
	              PTC_SetHigh();
         }
             
           SendWifiData_Answer_Cmd(0x02,0x01); //
           tx_thread_sleep(100); 
		
           }

		 
	   }
       else if(pdata[3]== 0x0 ){
	   
		 
          buzzer_sound();
		
	          gpro_t.rx_ptc_flag = 0;
		      PTC_SetLow();
			  gctl_t.ptc_prohibit_on_flag =1;
		  
          SendWifiData_Answer_Cmd(0x02,0x0); //
          tx_thread_sleep(100); 
     
       }
      break;

	  
     case plasma_on_off: //PLASMA ACTIVE OPEN OR CLOSE
   
		  if(pdata[3]== 0x01){
			 
			buzzer_sound();
			
			 gctl_t.gPlasma = 1;
			 if(gpro_t.stopTwoHours_flag==0){
				 PLASMA_SetHigh() ;
   
			}


			SendWifiData_Answer_Cmd(0x03,0x01); //
			tx_thread_sleep(100); 
			 
		  }
		  else if(pdata[3]  == 0x0){
			 buzzer_sound();
			
			 
			 gctl_t.gPlasma = 0;
			 PLASMA_SetLow();

			SendWifiData_Answer_Cmd(0x03,0x0); //
			tx_thread_sleep(100); 
			  
		  
		  }
   
   
	break;

	   
   	case 0x04: //ultrasonic	ACTIVE OPEN OR CLOSE
          
		  if(pdata[3]  == 0x01){  //open 
			 buzzer_sound();
			gctl_t.gUlransonic =1;
   
			if(gpro_t.stopTwoHours_flag==0){
				 ultrasonic_open();
   
			}
			
			SendWifiData_Answer_Cmd(0x04,0x01); //
			tx_thread_sleep(100); 
   
		  }
		  else if(pdata[3] == 0x0){ //close 
              buzzer_sound();
			gctl_t.gUlransonic = 0;
   
			ultrasonic_close();
			
			SendWifiData_Answer_Cmd(0x04,0x0); //
			tx_thread_sleep(100); 
   
		  }
   
   
    break;

	
     case  wifi_link: // link wifi command

       if(pdata[3] == 0x01){  // link wifi 
        
          buzzer_sound();
          gpro_t.link_net_step =0;
	      net_t.wifi_link_net_success=0;
          gpro_t.wifi_led_fast_blink_flag =1;
          gctl_t.wifi_config_net_lable=wifi_set_restor;
		  wifi_t.runCommand_order_lable= wifi_link_tencent_cloud;//2  
		 
		  
          gctl_t.gTimer_linkTencentCounter=0; //total times is 120s
          SendWifiData_Answer_Cmd(0x05,0x01); //WT.EDIT 2024.12.28
          tx_thread_sleep(100);
         
      
        }


     break;

	  case buzzer_sound_s: //buzzer sound command 
          if(pdata[3] == 0x01)
            buzzer_sound();
		 
     break;

	 case 0x10: //power on or off don't sound .

	     if(gctl_t.app_timer_power_on_flag ==1) return ;
	     if(pdata[3] == 0x01){ //open
           
		   gpro_t.process_run_step=0;
	       gpro_t.gpower_on = power_on;
		   SendWifiData_Answer_Cmd(0x10,0x01);
	       tx_thread_sleep(100);
		    fan_full_run();//WT.EDIT 2026.01.26
		    if(gctl_t.app_timer_power_on_flag ==0){
			    PLASMA_SetHigh();
	             ultrasonic_open();   //ultrasnoic ON 
	             PTC_SetHigh();

		    }
	         
	    }
        else if(pdata[3] == 0x0){ //close 

			    PTC_SetLow(); //ptc off
				PLASMA_SetLow() ; //plasma turn off.
	            ultrasonic_close();
			
              SendWifiData_Answer_Cmd(0x10,0x0); //power off .
              tx_thread_sleep(100); 
      
             gpro_t.power_off_run_step=1;
             gpro_t.gpower_on = power_off;
			 
		     
        }


	 break;

      case 0x11:
		    gpro_t.second_disp_flag = pdata[3];
	  break; 

	  case 0x12: //powe off fan run one minute stop .
	  	 if(pdata[3]==1){ // recach 2 hours fan stop

            // gpro_t.power_off_run_step=1;
               gpro_t.gpower_on = power_off;
			 
               FAN_Stop();
			   PTC_SetLow(); //ptc off;
			   tx_thread_sleep(100);
			    PLASMA_SetLow() ; //plasma turn off.
	           ultrasonic_close();
         }
		 else{
            
		    Fan_RunSpeed_Fun();//WT.EDIT 2026.01.26
        }


	  break;
	  
	  case 0x16 : //buzzer sound command with answer .

        buzzer_sound();
        
         //SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.07.28

		 /// tx_thread_sleep(100);
		  
       break;
	  
      case 0x27: //AI command without buzzer sound
	  case 0x17: //AI notice
	  
	  if(pdata[3] == 0x02){
	 
		
          gctl_t.gModel=2;
          gctl_t.mode_ai_switch_flag =1;
        
		  
	      if(wifi_link_net_state()==1){
	          MqttData_Publish_AitState(2);
			   tx_thread_sleep(200);//tx_thread_sleep(200);//HAL_Delay(350);
	       }
        
          
       }
       else if(pdata[3] == 0x01){ //AI mode 
       
	     gctl_t.gModel=1;
	     gctl_t.mode_ai_switch_flag =1;
         if(wifi_link_net_state()==1){
	         MqttData_Publish_AitState(1);
			 tx_thread_sleep(200);//tx_thread_sleep(200);//HAL_Delay(350);
	      }
		 
       }


	 break;

	 case 0x18: //WT.EDIT 2026.03.02
         if(pdata[3]==1){ // recach 2 hours fan stop
               gpro_t.fan_rx_stop_flag =1 ;
			   gpro_t.stopTwoHours_flag=1;
		     
			   gctl_t.gTimer_senddata_panel=0;
               FAN_Stop();
			   PTC_SetLow(); //ptc off;
			   PLASMA_SetLow() ; //plasma turn off.
               ultrasonic_close();
         }
		 else{
            gpro_t.fan_rx_stop_flag = 0;
		    fan_full_run();//WT.EDIT 2026.01.26
			if(gpro_t.rx_ptc_flag ==1 && gctl_t.ptc_prohibit_on_flag==0){
			  	PTC_SetHigh();
				
             }
			 if(gctl_t.gPlasma==1)PLASMA_SetHigh();
			 if(gctl_t.gUlransonic==1) ultrasonic_open();
        }

	 break;

	  case 0x19: //works 2 hours ,then have a rest 10 minutes ->notice 

	    if(pdata[3]==1){ // recach 2 hours 

           gpro_t.stopTwoHours_flag=1;
	
	
		   gctl_t.gTimer_senddata_panel =0;
	
		    PTC_SetLow(); //ptc off;
			PLASMA_SetLow() ; //plasma turn off.
            ultrasonic_close();
			
		}
		else if(pdata[3]==0){
			  gpro_t.stopTwoHours_flag=0;//WT.EDIT 2026.01.26
			   gpro_t.fan_rx_stop_flag =0 ;
		       gctl_t.gTimer_senddata_panel=0;
        
              if(gpro_t.rx_ptc_flag >1)gpro_t.rx_ptc_flag=1;//2026.02.27 WT.EDIT
              if(gctl_t.gPlasma > 1) gctl_t.gPlasma =1;
			  if(gctl_t.gUlransonic > 1) gctl_t.gUlransonic =1;
              
              if(gpro_t.rx_ptc_flag ==1 && gctl_t.ptc_prohibit_on_flag==0){
			  	PTC_SetHigh();
				
              }
			  if(gctl_t.gPlasma==1)PLASMA_SetHigh();
			  if(gctl_t.gUlransonic==1) ultrasonic_open();
			 // Fan_RunSpeed_Fun();//WT.EDIT 2026.01.26
			  
		}
	   

	  break;

  
	  case 0x1B: //write set temperature value .data.2026.01.06
	  
        if(pdata[3]== 0x01){
		       gctl_t.ptc_prohibit_on_flag =0;
			   gpro_t.rx_ptc_flag = 1;//gctl_t.gDry = 1;

			   if(gpro_t.stopTwoHours_flag ==0){
			       PTC_SetHigh();
		        
				 SendWifiData_Answer_Cmd(0x22,0x01); //WT.EDIT 2025.07.28
		         tx_thread_sleep(10);
				 if(wifi_link_net_state()==1){ 
					  MqttData_Publish_SetPtc(0x01);
					  tx_thread_sleep(200);
					
				  }
			   	
		       } 
      }
      else if(pdata[3]== 0x0){
        
          gpro_t.rx_ptc_flag =0 ;//gctl_t.gDry =0;

	       PTC_SetLow();
        
		   SendWifiData_Answer_Cmd(0x22,0x0); //WT.EDIT 2025.07.28
           tx_thread_sleep(10);

		  
		  if(wifi_link_net_state()==1){ 
			MqttData_Publish_SetPtc(0x0);
			tx_thread_sleep(200);
		  }
         
	   }
			 
      break;
     
 
	  
    case 0x1C: // is time data: hours,minutes,sencodes.
		   
	break;


	case 0x22: //PTC ON OR OFF by compare temperature value .

	    if(gctl_t.ptc_prohibit_on_flag == 1)return ;
		
        if(pdata[3]== 0x01){
		  
		   if(gpro_t.stopTwoHours_flag >1 )gpro_t.stopTwoHours_flag=0; //This is be solved bug.
		   if(gctl_t.ptc_prohibit_on_flag >1) gctl_t.ptc_prohibit_on_flag=0;
		   
		   if(gpro_t.stopTwoHours_flag ==0 && gctl_t.ptc_prohibit_on_flag==0){
			  
			     gpro_t.rx_ptc_flag = 1;//gctl_t.gDry = 1;
			     ptc_onoff_default++;
                 PTC_SetHigh();
		   	
//		         if(gpro_t.soft_version == 0x02){
//					 SendWifiData_Answer_Cmd(0x22,0x01); //WT.EDIT 2025.07.28
//			         tx_thread_sleep(pdMS_TO_TICKS(100));
//		         }
				 if(ptc_set_wifi !=gpro_t.rx_ptc_flag){
				 	ptc_set_wifi =gpro_t.rx_ptc_flag;
					 if(wifi_link_net_state()==1){ 
						  MqttData_Publish_SetPtc(0x01);
						tx_thread_sleep(200);
						
					  }
				 }
		   	}   	
	   }
       else if(pdata[3]== 0x0){
	   	 if(gpro_t.stopTwoHours_flag >1 )gpro_t.stopTwoHours_flag=0; //This is be solved bug.
		 if(gctl_t.ptc_prohibit_on_flag >1) gctl_t.ptc_prohibit_on_flag=0;
		 
               gpro_t.rx_ptc_flag =0 ;//gctl_t.gDry =0;
               ptc_onoff_default++;
	    
	          PTC_SetLow();
         if(gpro_t.soft_version == 0x02){
//		   SendWifiData_Answer_Cmd(0x22,0x0); //WT.EDIT 2025.07.28
//           tx_thread_sleep(pdMS_TO_TICKS(100));
         	}
		  if(ptc_set_wifi !=gpro_t.rx_ptc_flag){
				 	ptc_set_wifi =gpro_t.rx_ptc_flag;
		  if(wifi_link_net_state()==1){ 
			MqttData_Publish_SetPtc(0x0);
			tx_thread_sleep(200);
		  }
		  }
         
	  }
	
   
     break;


	 
	 case 0x2A: //smart phone or display  board set temperature .receive.
	 
		   if(pdata[4]==0x01 && gpro_t.gpower_on == power_on){
			  
			   if(pdata[5] >19 && pdata[5] < 41){
			   	gctl_t.ptc_prohibit_on_flag = 0;
				gpro_t.set_temp_value_success=1;
			   	if(gpro_t.stopTwoHours_flag >1 )gpro_t.stopTwoHours_flag=0; //This is be solved bug.
			   gctl_t.set_temperature_value = pdata[5] ;
			   gctl_t.ptc_prohibit_on_flag =0;
			   if(gctl_t.set_temperature_value > gctl_t.gDht11_temperature && gpro_t.stopTwoHours_flag ==0){
			
			        
					  ptc_onoff_default++;
                      gpro_t.rx_ptc_flag=1;
				      PTC_SetHigh();
					  
			   }
			   else{
			   	   ptc_onoff_default++;
				   gpro_t.rx_ptc_flag =0 ;//gctl_t.gDry =0;

			       PTC_SetLow();
		       }

			   if(ptc_set_wifi !=gpro_t.rx_ptc_flag){
				 	ptc_set_wifi =gpro_t.rx_ptc_flag;
				   if(wifi_link_net_state()==1){
					   MqttData_Publis_SetTemp(gctl_t.set_temperature_value);
					   tx_thread_sleep(200);//tx_thread_sleep(200);//HAL_Delay(350);
					}
			   	}
			  }
		   
			}
		
		
	 break;

	 

	 case 0x6C: //Synchronize local time ->two display board 

	   if(pdata[4]==0x03){ 

		     if(pdata[5] < 24 && pdata[6] < 61 && pdata[7] < 61){
         
		      gpro_t.disp_works_hours= pdata[5];
			 
			  gpro_t.disp_works_minutes =pdata[6];
			

			  gpro_t.gTimer_works_time_seconds=pdata[7];
			
		     }
		 }

	 break;

	 case 0xF0: //software version difference older and new sotfware 
      
            gpro_t.soft_version = pdata[3];
	 
				
			#if DEBUG_FLAG

			 // printf("gpro_t.soft_version = %d\r\n",gpro_t.soft_version);

			#endif 
			 
		

    break;


	case 0xFF: //copy comand or notice or data.

	       parse_recieve_copy_data(pdata) ;

	break;

	}

}
/**********************************************************************
	*
	*Function Name:static void parse_recieve_copy_data_handler(void)
	*Function: display board send to mainboard answer signal
	*Input Ref:NO
	*Return Ref:NO
	*
**********************************************************************/
static void parse_recieve_copy_data(uint8_t *pddata)
{

    switch(pddata[3]){
    
       case 0:
    
    
       break;

	   case 0x01:

	     if(pddata[4] == 0x01){ //open

		    gpro_t.process_run_step=0;
	        gpro_t.gpower_on = power_on;

		 }
        else if(pddata[4] == 0x0){ //close 

		   gpro_t.power_off_run_step=1;
           gpro_t.gpower_on = power_off;
			 
		}
	   

	   break;

	   
    }
      
 }

/**
  * @brief This function handles USART1 global interrupt 
  * @param
  * @retrval 
**/
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
  volatile uint8_t data;
  // static uint8_t rx_flag;
   if (LL_USART_IsActiveFlag_ORE(USART1)) LL_USART_ClearFlag_ORE(USART1);

   if(LL_USART_IsActiveFlag_RXNE_RXFNE(USART1)){
   
     
      rx1_data = LL_USART_ReceiveData8(USART1);
       usart1_isr_callback_handler(rx1_data);
	  // gl_tMsg.usData[rx_data_counter]=rx1_data;
	  /// rx_data_counter++;
	  /// if(rx_data_counter > 8){
	   	 // rx_data_counter=0;
	   //	}
        // 写入环形缓冲区
      // uart1_rx_buf[uart1_rx_head] = rx1_data;
     //  uart1_rx_head = (uart1_rx_head + 1) % UART1_RX_BUF_SIZE;

        // 通知接收任务
        //tx_semaphore_put(&uart1_rx_semaphore);
      //display_board_xtask_notice(rx1_data);
      

   }
  /* USER CODE END USART1_IRQn 0 */

  
  /* USER CODE BEGIN USART1_IRQn 1 */
	 // 清除错误标志
  
    if (LL_USART_IsActiveFlag_FE(USART1))  LL_USART_ClearFlag_FE(USART1);
    if (LL_USART_IsActiveFlag_NE(USART1))  LL_USART_ClearFlag_NE(USART1);
  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt 
  * @param
  * @retrval 
**/
void decoder_handler(void)
{
   // gpro_t.decoder_success_flag=0;
	check_bcc_code = bcc_check(gl_tMsg.usData,gl_tMsg.rx_total_numbers);
	if(check_bcc_code == gl_tMsg.bcc_check_code){
		usart1_protocol_state_machine(gl_tMsg.usData);
    }
}

