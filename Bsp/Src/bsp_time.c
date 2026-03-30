#include "bsp.h"


// ????????
#define TWOH_RUN_DURATION_MIN   119 //(2U * 60U * 60U)  // 2 hours
#define TWOH_FAN_DURATION_SEC   (60U)             // fan run 1 minute
#define TWOH_PAUSE_DURATION_SEC 10       // rest 10 minutes

#define TEMP_ON_THRESHOLD   38   // 温度低于 38℃ 打开 PTC
#define TEMP_OFF_THRESHOLD  40   // 温度高于等于 40℃ 关闭 PTC
//#define TEMP_SAFE_LIMIT     60   // 安全保护温度
#define DEFAULT_TEMP        40   // 默认温度（未设置时）

typedef enum {
    PTC_STATE_OFF = 0,
    PTC_STATE_ON  = 1
} PTC_State;

static PTC_State ptc_state = PTC_STATE_OFF;

static void CompareSetAndActualTemperature(void);

uint8_t counter_two_hours;
uint8_t define_twohours_flag ;



typedef enum {
    TWOH_STATE_RUNNING,
    TWOH_STATE_FAN_COOLING,
    TWOH_STATE_PAUSED
} TwoHoursState_t;

uint8_t  beijing_step;

uint8_t real_hours,real_minutes,real_seconds;

uint8_t auto_link_net_flag;
uint8_t timer_fan_flag;//times_flag;
uint8_t twoHours_stop_flag;


/**********************************************************************
    *
    *Functin Name: void works_run_two_hours_state(void)
    *Function :  
    *Input Ref: NO
    *Return Ref: NO
    *
************************************************************************/
void works_run_two_hours_state(void)
{

  switch(gpro_t.soft_version ){

   case 0x02:
  
 
  if(gpro_t.stopTwoHours_flag ==1){//WT.EDIT 2025.11.19

   
          PLASMA_SetLow(); //
         PTC_SetLow();
         ultrasonic_close();

  }
  else if(gctl_t.gTimer_senddata_panel >6  && gpro_t.fan_rx_stop_flag ==0 && gpro_t.stopTwoHours_flag ==0){ //300ms
            gctl_t.gTimer_senddata_panel=0;
          
            ActionEvent_Handler();
   }

	if(gpro_t.fan_rx_stop_flag ==1){
               FAN_Stop();
			  PLASMA_SetLow(); //
            PTC_SetLow();
            ultrasonic_close();


	 }

  	break;

	case 0x0://older version 

	  if(gpro_t.gTimer_conter_twohours_minutes >119 && gpro_t.stopTwoHours_flag ==0){
			gpro_t.gTimer_conter_twohours_minutes=0;
	        gpro_t.gTimer_twohours_seconds_counter=0;
	        gpro_t.stopTwoHours_flag =1;
			define_twohours_flag =1;
			PLASMA_SetLow(); //
            PTC_SetLow();
            ultrasonic_close();
	  }
	  else if(gpro_t.stopTwoHours_flag ==1 &&  define_twohours_flag ==0){
	  
	  	    gpro_t.gTimer_conter_twohours_minutes=0;
	        gpro_t.gTimer_twohours_seconds_counter=0;
			define_twohours_flag =1;
			PLASMA_SetLow(); //
            PTC_SetLow();
            ultrasonic_close();
	  
	  }
	  
	  

	  if(define_twohours_flag==1)gpro_t.stopTwoHours_flag=1;
	  else if(define_twohours_flag==2)gpro_t.stopTwoHours_flag=1;
	  else if(define_twohours_flag==0)gpro_t.stopTwoHours_flag=0;
	  

	  if(gpro_t.stopTwoHours_flag ==1 && gpro_t.gTimer_conter_twohours_minutes > 0 && define_twohours_flag ==1){
			  define_twohours_flag++;
			   gpro_t.gTimer_conter_twohours_minutes=0;
			   gpro_t.gTimer_twohours_seconds_counter=0;
               gpro_t.fan_rx_stop_flag =1;
	           FAN_Stop();
      }
      else if(gpro_t.stopTwoHours_flag ==1 && gpro_t.gTimer_conter_twohours_minutes > 10 && define_twohours_flag ==2){
			  define_twohours_flag=0;
			  gpro_t.stopTwoHours_flag =0;
	          gpro_t.fan_rx_stop_flag =0;
	          gpro_t.gTimer_conter_twohours_minutes=0;
			  gpro_t.gTimer_twohours_seconds_counter=0;

	          
      }
      else if(gctl_t.gTimer_senddata_panel >6  && define_twohours_flag==0 && gpro_t.stopTwoHours_flag ==0){ //300ms
            gctl_t.gTimer_senddata_panel=0;
          
            ActionEvent_Handler();
      }

     if(gpro_t.gTimer_poweroff_fan > 5){
	 	gpro_t.gTimer_poweroff_fan=0;
	    CompareSetAndActualTemperature();
	  
     	}
	break;

  	}

}
/**
*@brief:
*@notice
*@param
*
*
**/
static void CompareSetAndActualTemperature(void)
{
    if(gpro_t.stopTwoHours_flag ==1 || gctl_t.ptc_prohibit_on_flag ==1)return ;
	// 控制 PTC 加热器开关（带滞后控制）
	uint8_t real_temp = gctl_t.gDht11_temperature;
	int8_t target_temp;
	
		// 确定目标温度：有设置用设置值，否则默认 40℃
		if (gpro_t.set_temp_value_success == 1) {
			target_temp = gctl_t.set_temperature_value;
		} else {
			target_temp = DEFAULT_TEMP;
		}
	
		// 安全保护：超过 60℃ 强制关闭
		if (real_temp >= DEFAULT_TEMP) {
			   gpro_t.rx_ptc_flag = 0;
				
			    PTC_SetLow();
		        ptc_state = PTC_STATE_OFF;
				SendData_Set_Command(0x22, 0x00); // close PTC
				tx_thread_sleep(10);
			    
			return;
		}
	

        if(real_temp < target_temp && gctl_t.ptc_prohibit_on_flag ==0){

           gpro_t.rx_ptc_flag= 1;
		   PTC_SetHigh();
		   ptc_state = PTC_STATE_ON;
		   SendData_Set_Command(0x22, 0x01); // open PTC

		   tx_thread_sleep(10);


		}
        else if (ptc_state == PTC_STATE_OFF) {// 滞后控制逻辑
			// 当前关闭状态 → 低于 (目标温度 - 2℃) 才打开
			if (real_temp <= (target_temp - 2)) {
				
				gpro_t.rx_ptc_flag  = 1;
			     PTC_SetHigh();
				ptc_state = PTC_STATE_ON;
				SendData_Set_Command(0x22, 0x01); // open PTC

				tx_thread_sleep(10);
			}
		} 
		else {
			// 当前开启状态 → 高于等于目标温度才关闭
			if (real_temp >= target_temp) {
				
				 gpro_t.rx_ptc_flag = 0;
				
			    PTC_SetLow();
				ptc_state = PTC_STATE_OFF;
				SendData_Set_Command(0x22, 0x00); // close PTC
				tx_thread_sleep(10);
			}
		}
	


}

/********************************************************************************
	*
	*Functin Name:void void getBeijingTime_cofirmLinkNetState_handler(void)
	*Functin :
	*Input Ref: NO
	*Return Ref:NO
	*
********************************************************************************/
void getBeijingTime_cofirmLinkNetState_handler(void)
{
   static uint8_t flag_switch;
    switch(gpro_t.get_beijing_flag){

     case 0: //WT.EDIT .2024.08.10

      if(net_t.wifi_link_net_success ==1 && gpro_t.gTimer_get_data_from_tencent_data > 9){ ////9
       
          gpro_t.gTimer_get_data_from_tencent_data =0;
           flag_switch++;

       

        if(flag_switch > 4 && gpro_t.gpower_on == power_on){
             flag_switch=0;
            wifi_t.get_rx_beijing_time_enable=0;
            Subscriber_Data_FromCloud_Handler();
           // tx_thread_sleep(200);//HAL_Delay(200)
            gpro_t.get_beijing_flag = 1;

            
        }
        else if(gpro_t.gpower_on == power_off && flag_switch > 4){
            flag_switch=0;
            wifi_t.get_rx_beijing_time_enable=0;
            Update_Dht11_Totencent_Value();
			//tx_thread_sleep(200);
            gpro_t.get_beijing_flag = 1;

         }
         else{ //WT.EDIT 2024.08.10 ADD ITEM
              if(flag_switch > 5){
                 flag_switch=0;
              }
              wifi_t.get_rx_beijing_time_enable=0; 
              gpro_t.get_beijing_flag = 1;
             

         }
       
    }
    else{
	 if(net_t.wifi_link_net_success ==0){
	    SendData_Set_Command(0x1F,0);//SendWifiData_To_Data(0x1F,0x0); //WT.EDIT 2025.04.02 0x1F: wifi link net is succes 
	    tx_thread_sleep(10);
	 }
     gpro_t.get_beijing_flag = 1;

    }
   

   break;


   case 1:

        if(wifi_t.gTimer_get_beijing_time > 65){//WT.EDIT 2025.01.06 //100

         wifi_t.gTimer_get_beijing_time=0;
        
          if(wifi_link_net_state()==1){

     
    		    gpro_t.get_beijing_flag = 2;
           
      
                net_t.linking_tencent_cloud_doing  =0; //receive from tencent command state .
                SendData_Set_Command(0x1F,0x01);//SendWifiData_To_Data(0x1F,0x01);
                tx_thread_sleep(10);

            }
            else{
              
               SendData_Set_Command(0x1F,0);//SendWifiData_To_Data(0x1F,0x0); //0x1F: 0x1=wifi link net is succes ,0x0 = wifi link net is fail
               tx_thread_sleep(10);
               gpro_t.get_beijing_flag = 10;
               net_t.linking_tencent_cloud_doing  =1; //receive from tencent command state .
               gpro_t.send_ack_cmd = 1; //ack_wifi_on;
             
             }
	
       }
       else{

            gpro_t.get_beijing_flag = 0;
       }

 
      break;


      case 2:


	    if(gpro_t.get_beijing_time_success == 0){
		

         gpro_t.get_beijing_flag = 3;
         beijing_step =0; //WT.EDIT 2024.08.10
	      
		wifi_t.gTimer_auto_detected_net_state_times =0;
	
		wifi_t.get_rx_beijing_time_enable=0;//disenable get beijing timing
		
	
       }
	   else{

          wifi_t.get_rx_beijing_time_enable=0;//disenable get beijing timing
		
		    gpro_t.get_beijing_flag = 3;
			beijing_step =0; //WT.EDIT 2024.08.10

        }



    break;


     case 3:

         switch(beijing_step){

         case 0:
            //disable publish data to tencent cloud.
            gpro_t.gTimer_get_data_from_tencent_data=0;

    		
    		Get_BeiJing_Time_Cmd();
    	    tx_thread_sleep(100);//tx_thread_sleep(100);//HAL_Delay(20); //WT.EDIT .2024.08.10//HAL_Delay(20);
    	    beijing_step =1;
         
         break;

         case 1:
                //disable publish data to tencent cloud.
                gpro_t.gTimer_get_data_from_tencent_data=0;
                wifi_t.get_rx_beijing_time_enable=1;
        		gpro_t.wifi_rx_data_counter =0;
        		Get_Beijing_Time();
              
        	    tx_thread_sleep(100);//tx_thread_sleep(100);//HAL_Delay(20); //WT.EDIT .2024.08.10
                
        	
                beijing_step =2;
          
        

          break; 


          case 2:

            if(gpro_t.wifi_rx_data_array[50] > 0x31 ){
           
                real_hours = (gpro_t.wifi_rx_data_array[41]-0x30)*10 + gpro_t.wifi_rx_data_array[42]-0x30;
                real_minutes =(gpro_t.wifi_rx_data_array[44]-0x30)*10 + gpro_t.wifi_rx_data_array[45]-0x30;
                real_seconds = (gpro_t.wifi_rx_data_array[47]-0x30)*10 + gpro_t.wifi_rx_data_array[48]-0x30;

                //memcpy(time_str,gpro_t.wifi_rx_data_array,sizeof(gpro_t.wifi_rx_data_array));
                ///net_parse_beijint_time();

                wifi_t.get_rx_beijing_time_enable=0; //enable beijing times
                gpro_t.disp_works_hours = real_hours;    
                    gpro_t.disp_works_minutes = real_minutes;

                    gpro_t.gTimer_works_time_seconds = real_seconds;
                    gpro_t.get_beijing_time_success = 1;

                    SendWifiData_To_PanelTime(gpro_t.disp_works_hours,gpro_t.disp_works_minutes,gpro_t.gTimer_works_time_seconds);
                    tx_thread_sleep(10);//tx_thread_sleep(50);

                   gpro_t.get_beijing_flag = 6; //WT.EDIT 2025.01.06
                    
                    
                
                }
                else if(gpro_t.wifi_rx_data_array[50] == 0x31){  //"0x31" ASCII = '1'

                   wifi_t.get_rx_beijing_time_enable=0; //enable beijing times
                   if(wifi_link_net_state()==1){
                       
                       gpro_t.gTimer_get_data_from_tencent_data=0;
                       wifi_t.gTimer_get_beijing_time = 50;
                       gpro_t.get_beijing_flag = 6;//WT.EDIT 2025.01.06
                      

                    }
                    else 
                       gpro_t.get_beijing_flag = 6;//WT.EDIT 2025.01.06
            }
            else{

              gpro_t.get_beijing_flag = 6;//WT.EDIT 2025.01.06
              

            }
            break;
         }

		 
    break;


    case 6:

  
       //confirm_wifi_link_net_state();
    

       wifi_t.gTimer_auto_detected_net_state_times=0;  

       gpro_t.get_beijing_flag = 0;
       
    break;

    //auto link net 

    case 10:

     if(wifi_t.gTimer_auto_detected_net_state_times > 100){

		      wifi_t.gTimer_auto_detected_net_state_times=0;

    

         if(net_t.wifi_link_net_success ==0){
            gpro_t.get_beijing_flag = 11;
            net_t.linking_tencent_cloud_doing  =1; //receive from tencent command state .
            gpro_t.wifi_rx_data_counter=0;
            SendData_Set_Command(0x1F,0);//SendWifiData_To_Data(0x1F,0x0); //WT.EDIT 2025.04.02 0x1F: wifi link net is succes 
            tx_thread_sleep(10);
           
          }
          else{
             wifi_t.soft_ap_config_flag =1; //WE.EIDT 
             net_t.linking_tencent_cloud_doing  =0; //receive from tencent command state .
             gpro_t.get_beijing_flag = 0;
		     SendData_Set_Command(0x1F,0x01);//SendWifiData_To_Data(0x1F,0x01); //WT.EDIT 2025.04.02 0x1F: wifi link net is succes 
			 tx_thread_sleep(10);
          }
        
       }

     
     if(wifi_link_net_state()==1){
          gpro_t.get_beijing_flag = 0;

     }
       
     break;


     case 11:
         if(wifi_link_net_state()==0 && gpro_t.wifi_led_fast_blink_flag==0){

           net_t.linking_tencent_cloud_doing =1;
        

          //  WIFI_IC_ENABLE();
       
    		at_send_data("AT+RST\r\n", strlen("AT+RST\r\n"));
            tx_thread_sleep(1000);//HAL_Delay(1000);
            wifi_t.gTimer_auto_link_net_time =0;
            auto_link_net_flag=1;

            gpro_t.get_beijing_flag = 12;

         }
         else {

             gpro_t.get_beijing_flag = 10;


         }
        
               


     break;


     case 12:

        if(wifi_t.gTimer_auto_link_net_time > 2 && auto_link_net_flag==1){

            wifi_t.gTimer_auto_link_net_time=0;
            gpro_t.wifi_rx_data_counter=0;
	        wifi_t.soft_ap_config_flag =1;
             auto_link_net_flag =2;
   
//	        HAL_UART_Transmit(&huart2, "AT+TCMQTTCONN=1,5000,240,0,1\r\n", strlen("AT+TCMQTTCONN=1,5000,240,0,1\r\n"), 0xffff);//瀵�1�?7?婵绻涢幒?
            at_send_data("AT+TCMQTTCONN=1,5000,240,0,1\r\n", strlen("AT+TCMQTTCONN=1,5000,240,0,1\r\n"));
            tx_thread_sleep(1000);//HAL_Delay(1000);
         
           
		
	    }

        if(wifi_t.gTimer_auto_link_net_time > 4 && auto_link_net_flag==2){

            auto_link_net_flag=0 ;
            gpro_t.get_beijing_flag = 13;
           
        }



     break;


     case 13:
       if(wifi_link_net_state()==1){
       
         

        net_t.linking_tencent_cloud_doing  =0; //receive from tencent command state .
	    gpro_t.wifi_rx_data_counter=0; //clear USART2 counter is zero
		wifi_t.soft_ap_config_flag =0; 
	

          if(gpro_t.gpower_on == power_on){
                MqttData_Publish_Update_Data();//Publish_Data_ToTencent_Initial_Data();
               // tx_thread_sleep(200);//tx_thread_sleep(200);//HAL_Delay(200);

            }
            else if(gpro_t.gpower_on == power_off){

               MqttData_Publish_PowerOff_Ref();
               //tx_thread_sleep(200);//tx_thread_sleep(200);//HAL_Delay(200);


            }
            Subscriber_Data_FromCloud_Handler();
            //tx_thread_sleep(200);//HAL_Delay(200);

            SendData_Set_Command(0x1F,0x01);//SendWifiData_To_Data(0x1F,0x01); //0x1F: wifi link net is succes 
			tx_thread_sleep(10);
             gpro_t.get_beijing_flag = 0;
		
         }
         else{
			 SendData_Set_Command(0x1F,0);//SendWifiData_To_Data(0x1F,0x0); //WT.EDIT 2025.04.02 0x1F: wifi link net is succes 
			 tx_thread_sleep(10);
             gpro_t.get_beijing_flag = 10;
         }
	   

       break;
	

	
        }

}




