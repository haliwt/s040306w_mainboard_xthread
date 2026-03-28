#include "bsp.h"


uint8_t (*wifi_link_net_state)(void);

static uint8_t wifi_link_net_fun(void);


void wifi_init(void)
{

    wifi_link_net_handler(wifi_link_net_fun);


}



void wifi_link_net_handler(uint8_t (wifi_link_net_state_handler)(void))
{

   wifi_link_net_state = wifi_link_net_state_handler;

}

static uint8_t wifi_link_net_fun(void)
{
     if(net_t.wifi_link_net_success ==1) return 1;
     else 
        return 0;


}


void confirm_wifi_link_net_state(void)
{

//   HAL_UART_Transmit(&huart2, "AT+TCMQTTSTATE?\r\n", strlen("AT+TCMQTTSTATE?\r\n"), 5000);


}

/*****************************************************************************
    *
    *Function Name: static void AutoReconnect_Wifi_Neware_Function(void)
    *Function: 
    *Input Ref: NO
    *Return Ref:NO
    *
*****************************************************************************/	 	
void link_wifi_net_handler(void)
{
  

    uint8_t  device_massage[100];
  // device_massage = (uint8_t *)malloc(128);

      

        switch(gpro_t.link_net_step){

            case 0: //one step

          
        		at_send_data((const uint8_t *)"AT+RST\r\n", strlen("AT+RST\r\n"));
        
        		tx_thread_sleep(1000);//tx_thread_sleep(1000);
        		
              gpro_t.link_net_step = 1;

            break;

            case 1:
               // WIFI_IC_ENABLE();
                tx_thread_sleep(1000);
               at_send_data((const uint8_t *)"AT+CWMODE=3\r\n", strlen("AT+CWMODE=3\r\n"));
                tx_thread_sleep(1000);
                gctl_t.randomName[0]=HAL_GetUIDw0();
			          tx_thread_sleep(1000);
                gpro_t.gTimer_link_net_timer_time = 0;
		
                
                gpro_t.link_net_step = 2;

            break;

            case 2:
				 // tx_thread_sleep(1000);
                 if(gpro_t.gTimer_link_net_timer_time  > 2){
                     gpro_t.gTimer_link_net_timer_time = 0;

                        // WIFI_IC_ENABLE();
            			
                        sprintf((char *)device_massage, "AT+TCPRDINFOSET=1,\"%s\",\"%s\",\"UYIJIA01-%d\"\r\n", PRODUCT_ID, DEVICE_SECRET,gctl_t.randomName[0]);
            			at_send_data(device_massage, strlen((const char *)device_massage));
            	  		tx_thread_sleep(1000);
                         // tx_thread_sleep(1000);
                       gpro_t.link_net_step = 3;

                 }
		

            break;


            case 3:
                 //tx_thread_sleep(1000); 
            if(gpro_t.gTimer_link_net_timer_time  > 4){
                      gpro_t.gTimer_link_net_timer_time = 0;
                  
                at_send_data((const uint8_t *)"AT+TCDEVREG\r\n", strlen("AT+TCDEVREG\r\n"));

			    tx_thread_sleep(1000);
				  tx_thread_sleep(1000);
               
                  gpro_t.link_net_step = 4;
            }
	

            break;


            case 4:
				 // tx_thread_sleep(1000);
				 //   tx_thread_sleep(1000);
			  //tx_thread_sleep(1000);
                 if(gpro_t.gTimer_link_net_timer_time  > 4){
                  gpro_t.gTimer_link_net_timer_time = 0;

                   net_t.linking_tencent_cloud_doing =1;
                  wifi_t.soft_ap_config_flag =1; //WE.EIDT 
                  gpro_t.link_net_step = 5;
                 }
            
            break;

            case 5:

	            sprintf((char *)device_massage, "AT+TCSAP=\"UYIJIA01-%d\"\r\n",gctl_t.randomName[0]);
                at_send_data(device_massage, strlen((const char *)device_massage));
	           tx_thread_sleep(1000);
			
             //  tx_thread_sleep(1000);
       
              gpro_t.link_net_step = 6;


                    

            break;


            case 6:
                

            if(net_t.soft_ap_config_success==1){

            net_t.soft_ap_config_success=0;
 //           HAL_UART_Transmit(&huart2, "AT+TCMQTTCONN=1,5000,240,0,1\r\n", strlen("AT+TCMQTTCONN=1,5000,240,0,1\r\n"), 5000);//å¼?å§‹è¿žæŽ?
            at_send_data((const uint8_t *)"AT+TCMQTTCONN=1,5000,240,0,1\r\n", strlen("AT+TCMQTTCONN=1,5000,240,0,1\r\n"));
			tx_thread_sleep(1000);
	
	         gpro_t.link_net_step = 7;
            gpro_t.gTimer_link_net_timer_time = 0;
            }
            
                   
            break;

            case 7:
				//  tx_thread_sleep(1000);
				//    tx_thread_sleep(1000);

            if( gpro_t.gTimer_link_net_timer_time  > 3){

             if(net_t.wifi_link_net_success==1){
			
				gctl_t.first_link_tencent_cloud_flag =1;
				wifi_t.get_rx_beijing_time_enable=0;
			 
                
               SendData_Set_Command(0x1F,0x01);//SendWifiData_To_Data(0x1F,0x01); //link wifi order 1 --link wifi net is success.
               tx_thread_sleep(100);
			
			    gpro_t.link_net_step = 8;
               
				
		     }
		     else{
                
                  gpro_t.wifi_led_fast_blink_flag=0;
                  gpro_t.link_net_step = 11;
                  SendData_Set_Command(0x1F,0);//SendWifiData_To_Data(0x1F,0x00) ;	 //Link wifi net is fail .WT.EDTI .2024.08.31
                  tx_thread_sleep(100);
                  
                }
                
               }

            break;

            case 8:

              gpro_t.wifi_led_fast_blink_flag=0;
              gpro_t.gTimer_get_data_from_tencent_data=0;
			 
				MqttData_Publish_SetOpen(0x01);
		      
		       tx_thread_sleep(100);
		        
				
			  gpro_t.link_net_step = 9; // this is flag: link wifi times 119s is over.
		    break;
				 

			 case 9: 
			 
			    gpro_t.link_net_step = 10;


			break;

			case 10:

				Subscriber_Data_FromCloud_Handler();
		
	             tx_thread_sleep(100);
        
			 gpro_t.link_net_step = 0xfe;

                   
            break;


            case 11:

	

              gpro_t.get_beijing_flag= 10;
              wifi_t.gTimer_auto_detected_net_state_times = 120;
              gpro_t.link_net_step = 0xff;

            break;


            default:


            break;


        }

}




