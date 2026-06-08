#include "bsp.h"

uint8_t  ptc_rx_counter ;

/************************************************************************************
*
*Function Name: void module_action_handler(void)
*Fucntion :
*Input Ref:NO
*Return Ref:NO
*
************************************************************************************/
void module_action_handler(void)
{

   static uint8_t ptc_default =0xff,plasma_default =0xff,ultrasonic_default =0xff;
   static uint8_t app_timer_power_counter=0;

   if(gpro_t.fan_warning_flag ==1 || gpro_t.ptc_warning ==1) return ; //WT.EDIT 2025.10.29
   
   if(gpro_t.rx_ptc_flag==1 && gctl_t.ptc_prohibit_on_flag ==0){//if( gctl_t.gDry==1 && gctl_t.ptc_prohibit_on_flag ==0){

       ptc_rx_counter ++ ;
      if(gpro_t.stopTwoHours_flag ==0) PTC_SetHigh();

	   if(gctl_t.app_timer_power_on_flag == 1){
	   	
                SendWifiData_To_Cmd(0x02,0x01);
				tx_thread_sleep(1);

	   }
       


		if(wifi_link_net_state()==1 && ptc_default != gpro_t.ptc_actiov_f){//ptc_actiov_f = 0++
           
			ptc_default = gpro_t.ptc_actiov_f;
	

			MqttData_Publish_SetPtc(0x01);
			tx_thread_sleep(20);//tx_thread_sleep(100);//HAL_Delay(350);
	     }
		
   	  
	}
	else if(gpro_t.rx_ptc_flag ==0){
		
	    ptc_rx_counter ++ ;
		PTC_SetLow();
		if(gctl_t.app_timer_power_on_flag == 1){
			    gctl_t.ptc_prohibit_on_flag =1;
                SendWifiData_To_Cmd(0x02,0);
				tx_thread_sleep(1);

		}
		


	   if(wifi_link_net_state()==1 && ptc_default != gpro_t.ptc_actiov_f){//if(ptc_default!= get_ptc_value() && wifi_link_net_state()==1){
		    ptc_default = gpro_t.ptc_actiov_f;
		    MqttData_Publish_SetPtc(0x0);
			tx_thread_sleep(20);//tx_thread_sleep(100);//HAL_Delay(350);
			
		}
		
   }
   

   //plasma
    if(gctl_t.gPlasma == 1){
		
	     PLASMA_SetHigh();

	     if(gctl_t.app_timer_power_on_flag == 1){
                SendWifiData_To_Cmd(0x03,0x01);
				tx_thread_sleep(1);

		 }
		 
		 if(plasma_default!=gpro_t.plasma_switch_flag && wifi_link_net_state()==1){
		 	gpro_t.plasma_switch_flag++;
			plasma_default = gpro_t.plasma_switch_flag;	
		   MqttData_Publish_SetPlasma(0x01);
		   tx_thread_sleep(20);
		 
		}
	}
	else if(gctl_t.gPlasma == 0){

		PLASMA_SetLow();
		if(gctl_t.app_timer_power_on_flag == 1){
                SendWifiData_To_Cmd(0x03,0);
				tx_thread_sleep(1);

		 }
		 if(plasma_default!=gpro_t.plasma_switch_flag && wifi_link_net_state()==1){
			plasma_default = gpro_t.plasma_switch_flag;
		
		   MqttData_Publish_SetPlasma(0);
		  tx_thread_sleep(20);
		 
		}
	}
	//driver bug
	if(gctl_t.gUlransonic ==1){
		
	     ultrasonic_open();
		 if(gctl_t.app_timer_power_on_flag == 1){
                SendWifiData_To_Cmd(0x04,0x01);
				tx_thread_sleep(1);

		 }
	
	 if(ultrasonic_default!=gpro_t.ultrasonic_switch_flag && wifi_link_net_state()==1){
	    ultrasonic_default = gpro_t.ultrasonic_switch_flag;
		 
		 MqttData_Publish_SetUltrasonic(0x01);
		tx_thread_sleep(20);
	 } 
		
	}
	else if(gctl_t.gUlransonic ==0){

	    ultrasonic_close();
		if(gctl_t.app_timer_power_on_flag == 1){
                SendWifiData_To_Cmd(0x04,0);
				tx_thread_sleep(1);

		 }
		
		if(ultrasonic_default!=gpro_t.ultrasonic_switch_flag && wifi_link_net_state()==1){
			
			ultrasonic_default = gpro_t.ultrasonic_switch_flag;	
		
			MqttData_Publish_SetUltrasonic(0);
		    tx_thread_sleep(20);
			
		}

	}

	 if(wifi_link_net_state()==1 && gpro_t.tx_wifi_temperature_f ==1){
	 	gpro_t.tx_wifi_temperature_f++;
		MqttData_Publis_SetTemp(gctl_t.set_temperature_value);
		tx_thread_sleep(20);//tx_thread_sleep(200);//HAL_Delay(350);
	 }
   
	 Fan_RunSpeed_Fun();
    
	
	if(app_timer_power_counter < 3 && gctl_t.app_timer_power_on_flag == 1){
	   app_timer_power_counter++;

    }
	else if(app_timer_power_counter > 2 && gctl_t.app_timer_power_on_flag == 1){
	   app_timer_power_counter=0;

	   gctl_t.app_timer_power_on_flag =0;

	}
 }


