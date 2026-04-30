#include "bsp.h"

process_t gpro_t;

uint8_t power_on_login_tencent_cloud_flag  ;
uint8_t link_counter_times;

static void Auto_InitWifiModule_Hardware(void);

static void Auto_SmartPhone_TryToLink_TencentCloud(void);


uint32_t readFlash_value;

/**********************************************************************
    *
    *Function Name:uint8_t bcc_check(const unsigned char *data, int len) 
    *Function: BCC????
    *Input Ref:NO
    *Return Ref:NO
    *
**********************************************************************/
uint8_t bcc_check(const unsigned char *data, int len) 
{
    unsigned char bcc = 0;
    for (int i = 0; i < len; i++) {
        bcc ^= data[i];
    }
    return bcc;
}

/**************************************************************************************
*
*Function Name:  void bsp_init(void)
*Function: initial of base reference 
*
*
**************************************************************************************/
  void bsp_init(void)
  {
   

   DHT11_Init();//dht11_init();//sensor_dht11_init();//updateDht11_sensorData_toDisp();//dht11_init();
   wifi_init();
   // 注册回调函数
   callback_register_fun();//tim17_register_callback(tim17_isr_callback_handler);
   callback_register_usart1_rx();

	
#if Enable_EventRecorder == 1  
	EventRecorderInitialize(EventRecordAll, 1U);
	EventRecorderStart();
#endif
	
  
  }
  /********************************************************************************
	*
	*Functin Name:void wifi_communication_tnecent_handler(void)
	*Functin :
	*Input Ref: NO
	*Return Ref:NO
	*
********************************************************************************/
void wifi_communication_tnecent_handler(void)
{

   if(wifi_t.get_rx_beijing_time_enable==0){
    
         Tencent_Cloud_Rx_Handler();
		
    	 Json_Parse_Command_Fun();
		 
  
   }
}


 /**********************************************************************
    *
    *Function Name:void link_wifi_to_tencent_handler(uint8_t data)
    *Function: 
    *Input Ref:NO
    *Return Ref:NO
    *
**********************************************************************/
void link_wifi_to_tencent_handler(void)
{
    if(gpro_t.wifi_led_fast_blink_flag == 1){//if(gpro_t.wifi_led_fast_blink_flag==1){
        if(gctl_t.gTimer_linkTencentCounter  > 170){//119

           gpro_t.wifi_led_fast_blink_flag =0;//gpro_t.wifi_led_fast_blink_flag =0;
           if(wifi_link_net_state()==0){

              gpro_t.get_beijing_flag = 10;
              wifi_t.gTimer_auto_detected_net_state_times = 120;
               net_t.linking_tencent_cloud_doing =1;
               wifi_t.soft_ap_config_flag =1; //WE.EIDT 

           }

    }
    else{

       link_wifi_net_handler();
    }
        
 
    }

   

}

/**********************************************************************
	*
	*Functin Name: void wifi_auto_detected_link_state(void)
	*Function :
	*Input Ref:  NO
	*Return Ref: NO
	*
**********************************************************************/
void wifi_auto_detected_link_state(void)
{
    static uint8_t dc_power_on;
	if(power_on_login_tencent_cloud_flag  < 5 && wifi_link_net_state()==0 && link_counter_times < 3){
		
      
	  net_t.linking_tencent_cloud_doing = 1;
      gpro_t.gTimer_dc_power_on_auto_link_net = 0;

      Auto_InitWifiModule_Hardware();//InitWifiModule();
      Auto_SmartPhone_TryToLink_TencentCloud();
	 
    }
    if(net_t.wifi_link_net_success==1 && gpro_t.gTimer_dc_power_on_auto_link_net > 1 && dc_power_on ==0 ){
              
             dc_power_on ++ ;
			 link_counter_times =5;
           //wifi_t.linking_tencent_cloud_doing = 0;
           net_t.linking_tencent_cloud_doing  =0;
           gpro_t.process_run_step=0;
        
          if(gpro_t.gpower_on == power_off){
		     MqttData_Publish_PowerOff_Ref();
               tx_thread_sleep(20);//HAL_Delay(200);

          }
          
          Subscriber_Data_FromCloud_Handler();
          tx_thread_sleep(20);//HAL_Delay(200);
         

          SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
          tx_thread_sleep(10);
   }
   
   if(gpro_t.gTimer_power_on_auto_link  > 6 && link_counter_times < 3){
	  gpro_t.gTimer_power_on_auto_link =0;

      link_counter_times =5;
      if(net_t.wifi_link_net_success==0){
         SendData_Set_Command(0x1F,0);//SendWifiData_To_Data(0x1F,0x0); //WT.EDIT 2025.04.02 0x1F: wifi link net is succes 
         tx_thread_sleep(10);

	  }

   }
   
   
}
/****************************************************************************************************
**
*Function Name:static void initBtleModule(void)
*Function: power on auto link net fun
*Input Ref: 
*Return Ref:NO
*
****************************************************************************************************/
static void Auto_InitWifiModule_Hardware(void)
{
  
	//WIFI_IC_ENABLE();
	if(power_on_login_tencent_cloud_flag ==0){
	   power_on_login_tencent_cloud_flag=1;
	   gpro_t.gTimer_power_on_first_link_tencent=0;
       gpro_t.wifi_rx_data_counter=0;
       net_t.linking_tencent_cloud_doing = 1;
	   //at_send_data("AT+RESTORE\r\n", strlen("AT+RESTORE\r\n")); //
	   at_send_data((const uint8_t *)"AT+RST\r\n", strlen("AT+RST\r\n"));
	   tx_thread_sleep(100);//HAL_Delay(1000);

	}
	if(gpro_t.gTimer_power_on_first_link_tencent > 2 &&  power_on_login_tencent_cloud_flag==1 ){
	   gpro_t.gTimer_power_on_first_link_tencent=0;
	   power_on_login_tencent_cloud_flag=2;
	   
	  }
}

static void Auto_SmartPhone_TryToLink_TencentCloud(void)
{
    
  if(power_on_login_tencent_cloud_flag==2 && gpro_t.gTimer_power_on_first_link_tencent >3){
   	power_on_login_tencent_cloud_flag++;
	net_t.linking_tencent_cloud_doing  = 1;
    gpro_t.wifi_rx_data_counter=0;
	wifi_t.soft_ap_config_flag =0;
   }
  
   if(power_on_login_tencent_cloud_flag==3 ){
		
	   gpro_t.gTimer_power_on_first_link_tencent=0;
       power_on_login_tencent_cloud_flag++;

	  // readFlash_value = read_flash_value();
	  // if(readFlash_value == 0x00000001){
	      // HAL_UART_Transmit(&huart2, "AT+TCMQTTCONN=1,5000,240,0,1\r\n", strlen("AT+TCMQTTCONN=1,5000,240,0,1\r\n"), 0xffff);//??
	       at_send_data((const uint8_t *)"AT+TCMQTTCONN=1,5000,240,0,1\r\n", strlen("AT+TCMQTTCONN=1,5000,240,0,1\r\n"));
		   tx_thread_sleep(20);//HAL_Delay(1000);
	   
	  
	 }
   
   if(wifi_link_net_state()==1 && power_on_login_tencent_cloud_flag ==4){
			//wifi_t.linking_tencent_cloud_doing =0;
			net_t.linking_tencent_cloud_doing= 0;
            power_on_login_tencent_cloud_flag++;
            SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
            tx_thread_sleep(10);
	}
    else if(wifi_link_net_state()==0 && power_on_login_tencent_cloud_flag ==4){
       power_on_login_tencent_cloud_flag++;
        SendWifiData_To_Cmd(0x1F,0x00);
	    tx_thread_sleep(10);
    }
}


uint8_t get_ptc_value(void)
{
    return gpro_t.rx_ptc_flag;
}

