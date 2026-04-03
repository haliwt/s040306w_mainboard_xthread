#include "bsp.h"


static void power_off_stop_fun(void);
void every_power_on_run(void);
uint8_t fan_run_one_minute_flag;
static void app_timer_power_on_reference(void);


/**********************************************************************
	*
	*Functin Name: 
	*Function : be check key of value 
	*Input Ref:  key of value
	*Return Ref: NO
	*
**********************************************************************/
void power_on_handler(void)
{
    static uint8_t counter,sw_flag,counter_flag;
    switch(gpro_t.process_run_step){

	case 0: //1
	     
          gpro_t.power_off_run_step=0;
          /*power on initial reference---start */
         gctl_t.gTimer_senddata_panel=0; //main board function run action.
     
         gctl_t.set_temperature_value=40; //power on default set temperature value is 40 degree,don't compare
         gctl_t.gTimer_ptc_adc_times=0;
		 gctl_t.gTimer_fan_adc_times=0;
		
		 gctl_t.set_wind_speed_value= 100;
		
		 gctl_t.first_link_tencent_cloud_flag=1;
		 gctl_t.ptc_prohibit_on_flag =0; //WT.EDIT 2025.09.18
		 gctl_t.set_temperature_flag=0;
		  gctl_t.set_temp_first_closeptc =0;
		  gctl_t.rx_set_temp_flag =0;
		  gpro_t.fan_rx_stop_flag=0;

		 /*end*/
         
	     /*clear error detected flag --start*/
		
		 gctl_t.ptc_warning =0;
		 gctl_t.ptc_warning =0;
	     gpro_t.fan_warning_flag =0;
		 gpro_t.fan_counter_error =0;
	
		 gpro_t.gTimer_detect_fan_error=0;
		/*end*/
       
	
        /*POWER OFF REF-start */
        gpro_t.power_off_run_step = 1;
    
		gpro_t.gTimer_conter_twohours_minutes=0;
	    gpro_t.gTimer_twohours_seconds_counter=0;
		gpro_t.wifi_led_fast_blink_flag=0;
        /*end*/
		
       
        gpro_t.stopTwoHours_flag =0;
		gpro_t.set_temp_value_success=0;
	
         Fan_Full_Speed();//Fan_RunSpeed_Fun();//WT.EDIT 2026.01.26
     
         read_sensorData();//updateDht11_sensorData_toDisp();
         fan_run_one_minute_flag=1;
		 gpro_t.process_run_step= 1;
		
		 
	break; 


  case 1:
  	if(gctl_t.app_timer_power_on_flag > 1)gctl_t.app_timer_power_on_flag=0;
	
    if(gctl_t.app_timer_power_on_flag ==1){
     	smartphone_timer_power_on_and_normal_handler();
	 	//tx_thread_sleep(100);
    }
    every_power_on_run();
	 gpro_t.process_run_step= 2;

  break;

  case 2:
  
	
	 read_sensorData();
	
	 gpro_t.process_run_step= 3;
	   
    break;
	
		
    case 3:
	
         if(wifi_link_net_state() ==1 && gctl_t.app_timer_power_on_flag ==0){
    
		    
	         gctl_t.set_wind_speed_value =100;

		     MqttData_Publish_SetOpen(1);  
			 tx_thread_sleep(200);
		
		 }
		 else if(gctl_t.app_timer_power_on_flag ==1){
		     	app_timer_power_on_reference();
			 	
		  }
		 read_sensorData();
	     gpro_t.process_run_step= 4;
	break;

  case 4: 

     if(wifi_link_net_state() ==1 &&  gctl_t.app_timer_power_on_flag==0){
    
		  MqttData_Publish_Init();
		  tx_thread_sleep(200);
     }
	 else if(gctl_t.app_timer_power_on_flag == 1){

           	gctl_t.set_wind_speed_value=100;
            MqttData_Publis_SetFan(gctl_t.set_wind_speed_value);//WT.EDIT 2025.12.19
            gctl_t.set_temperature_value=40;
            MqttData_Publis_SetTemp(gctl_t.set_temperature_value);

	 }
     	
  
    gctl_t.gTimer_senddata_panel = 8;
    gpro_t.process_run_step= 5;

	 break;

	case 5:
        ActionEvent_Handler();
	    gpro_t.process_run_step= 6;

	break;
	     
		
    case 6: //repeat process 
		
	   if(gpro_t.gTimer_update_todisplay > 4){
				   gpro_t.gTimer_update_todisplay=0;
				   counter++;
				   updateDht11_sensorData_toDisp();
				  	tx_thread_sleep(100);
				   
				   if(net_t.wifi_link_net_success ==1 && counter > 1 && gpro_t.soft_version == 0){ //WT.EDIT 2026.02.27
					  counter =0;
					  sw_flag = sw_flag ^ 0x01;
					  if(sw_flag == 1){
						  SendWifiData_olderCmd(0x1F,0x01);//SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
						  tx_thread_sleep(100);
					  }
					  else{
					   SendWifiData_To_Data(0x1F,0x01);
					   tx_thread_sleep(100);
					  }
					  
				   }
				   else if(net_t.wifi_link_net_success ==0 && counter > 1 && gpro_t.soft_version ==0){ //WT.EDIT 2026.02.27
					  counter =0;
					   sw_flag = sw_flag ^ 0x01;
					  if(sw_flag == 1){
						  SendWifiData_olderCmd(0x1F,0x0);//SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
						  tx_thread_sleep(100);
					  }
					  else{
						SendWifiData_To_Data(0x1F,0x0);
					   tx_thread_sleep(100);
					  }
				   }
				   
			  }

       gpro_t.process_run_step=7 ;


 break;
        
  case 7: 

   if(gpro_t.wifi_led_fast_blink_flag==0 && net_t.wifi_link_net_success ==1){
      if(gctl_t.app_timer_power_on_flag==0 && gctl_t.first_link_tencent_cloud_flag ==1){
	
		  gctl_t.first_link_tencent_cloud_flag++;

               MqttData_Publish_Update_Data();
			  tx_thread_sleep(200);//HAL_Delay(200);
             SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
             tx_thread_sleep(100);
          
    	}
	    else if(gctl_t.first_link_tencent_cloud_flag < 4){
			 gctl_t.first_link_tencent_cloud_flag++;

            Subscriber_Data_FromCloud_Handler();
    	    tx_thread_sleep(100);
	    }
		
		   SendData_Set_Command(0x1F,0x01);//SendWifiData_To_Data(0x1F,0x01);
           tx_thread_sleep(100);
	}
    
      gpro_t.process_run_step=8 ;
 break; 


 case 8:
     if(gpro_t.fan_warning_flag > 1 || gpro_t.ptc_warning  > 1){
        if(gpro_t.fan_warning_flag > 1 ) gpro_t.fan_warning_flag = 0; //strictly forbid 
	    if(gpro_t.ptc_warning  > 1)gpro_t.ptc_warning = 0;
		
     }

	 works_run_two_hours_state();
   
	gpro_t.process_run_step= 9;
  break;


  case 9:
  	 
       adc_detected_hundler();
	   
        gpro_t.process_run_step= 10; 

  break;

  case 10:
    

	   if(wifi_link_net_state() ==1 && gpro_t.gTimer_update_tencet_dht11 >5){
				gpro_t.gTimer_update_tencet_dht11=0;
				Update_Dht11_Totencent_Value();
        }
	 
       gpro_t.process_run_step= 11; 


  break;

  case 11:

     if(gctl_t.set_temperature_flag > 1 || gctl_t.set_temperature_value > 40 || gctl_t.ptc_prohibit_on_flag > 1
	 	  ||gctl_t.app_timer_power_on_flag > 2 || gctl_t.set_temp_first_closeptc > 1 || gpro_t.soft_version > 2){
	 	if(gctl_t.set_temperature_flag > 1)gctl_t.set_temperature_flag =0;
		if(gctl_t.set_temperature_value > 40 && gctl_t.set_temperature_flag ==0)gctl_t.set_temperature_value =40;
        if(gctl_t.ptc_prohibit_on_flag > 1)gctl_t.ptc_prohibit_on_flag =0;
		if(gctl_t.app_timer_power_on_flag > 2)gctl_t.app_timer_power_on_flag=0;
		if( gctl_t.set_temp_first_closeptc > 1)  gctl_t.set_temp_first_closeptc =0;
		if(gpro_t.soft_version > 2)gpro_t.soft_version = 0 ;
	 }

    if(gpro_t.gTimer_read_dht11_to_disp >3){
		gpro_t.gTimer_read_dht11_to_disp=0;
	   read_sensorData();

    }

    if(gpro_t.rx_ptc_flag >1)gpro_t.rx_ptc_flag=1;//2026.02.27 WT.EDIT
    if(gctl_t.gPlasma > 1) gctl_t.gPlasma =1;
	if(gctl_t.gUlransonic > 1) gctl_t.gUlransonic =1;
	if(gpro_t.stopTwoHours_flag==0)gpro_t.fan_rx_stop_flag =0;

	gpro_t.process_run_step= 12;	

   break;

   case 12:
   	    counter ++ ;
   		if(gpro_t.stopTwoHours_flag ==0 && counter > 50){
			   counter =0;
			   Fan_RunSpeed_Fun();
		 }

		gpro_t.process_run_step=6;

   break;

   

     default:
	
	 break;
  }
}


/************************************************************************************
*
*Function Name: void ActionEvent_Handler(void)
*Fucntion :
*Input Ref:NO
*Return Ref:NO
*
************************************************************************************/
void ActionEvent_Handler(void)
{

   static uint8_t ptc_default =0xff,plasma_default =0xff,ultrasonic_default =0xff;
   static uint8_t app_timer_power_counter;

   if(gpro_t.stopTwoHours_flag ==1) return ; //WT.EDIT 2025.10.29
   
   if(gpro_t.rx_ptc_flag==1 && gctl_t.ptc_prohibit_on_flag ==0){//if( gctl_t.gDry==1 && gctl_t.ptc_prohibit_on_flag ==0){
	if(gpro_t.fan_warning_flag !=1 && gpro_t.ptc_warning !=1 ){ //PTC warning flag

      PTC_SetHigh();

	   if(gctl_t.app_timer_power_on_flag == 1){
	   	
                SendWifiData_To_Cmd(0x02,0x01);
				tx_thread_sleep(100);

	   }
       else if(wifi_link_net_state()==1){
              gpro_t.rx_ptc_flag =1 ;

			MqttData_Publish_SetPtc(0x01);
			tx_thread_sleep(200);//tx_thread_sleep(100);//HAL_Delay(350);
	     	}
		}
   	  
	}
	else if(gpro_t.rx_ptc_flag ==0){
		
	  
		PTC_SetLow();
		if(gctl_t.app_timer_power_on_flag == 1){
			    gctl_t.ptc_prohibit_on_flag =1;
                SendWifiData_To_Cmd(0x02,0);
				tx_thread_sleep(100);

		}
		else if(wifi_link_net_state()==1){//if(ptc_default!= get_ptc_value() && wifi_link_net_state()==1){
			gpro_t.rx_ptc_flag =0 ;
		    MqttData_Publish_SetPtc(0x0);
			tx_thread_sleep(200);//tx_thread_sleep(100);//HAL_Delay(350);
			
		}
		
   }
   

   //plasma
    if(gctl_t.gPlasma == 1){
		
	     PLASMA_SetHigh();

	     if(gctl_t.app_timer_power_on_flag == 1){
                SendWifiData_To_Cmd(0x03,0x01);
				tx_thread_sleep(100);

		 }
		 if(plasma_default!=gpro_t.plasma_switch_flag && wifi_link_net_state()==1){
		 	gpro_t.plasma_switch_flag++;
			plasma_default = gpro_t.plasma_switch_flag;	
		
		   MqttData_Publish_SetPlasma(0x01);
		   tx_thread_sleep(200);
		 
		}
	}
	else if(gctl_t.gPlasma == 0){

		PLASMA_SetLow();
		if(gctl_t.app_timer_power_on_flag == 1){
                SendWifiData_To_Cmd(0x03,0);
				tx_thread_sleep(100);

		 }
		 if(plasma_default!=gpro_t.plasma_switch_flag && wifi_link_net_state()==1){
		 	gpro_t.plasma_switch_flag++;
			plasma_default = gpro_t.plasma_switch_flag;
		
		   MqttData_Publish_SetPlasma(0);
		  tx_thread_sleep(200);
		 
		}
	}
	//driver bug
	if(gctl_t.gUlransonic ==1){
		
	     ultrasonic_open();
		 if(gctl_t.app_timer_power_on_flag == 1){
                SendWifiData_To_Cmd(0x04,0x01);
				tx_thread_sleep(100);

		 }
	
	 if(ultrasonic_default!=gpro_t.ultrasonic_switch_flag && wifi_link_net_state()==1){
	 	gpro_t.ultrasonic_switch_flag++;
	    ultrasonic_default = gpro_t.ultrasonic_switch_flag;
		 
		 MqttData_Publish_SetUltrasonic(0x01);
		tx_thread_sleep(200);
	 } 
		
	}
	else if(gctl_t.gUlransonic ==0){

	    ultrasonic_close();
		if(gctl_t.app_timer_power_on_flag == 1){
                SendWifiData_To_Cmd(0x04,0);
				tx_thread_sleep(100);

		 }
		
		if(ultrasonic_default!=gpro_t.ultrasonic_switch_flag && wifi_link_net_state()==1){
			gpro_t.ultrasonic_switch_flag++;
			ultrasonic_default = gpro_t.ultrasonic_switch_flag;	
			 
			MqttData_Publish_SetUltrasonic(0);
		    tx_thread_sleep(200);
			
		}

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

/************************************************************************************
*
*Function Name:void smartphone_timer_power_on_and_normal_handler(void)
*
*
*
************************************************************************************/
void smartphone_timer_power_on_and_normal_handler(void)
{
 

	if(gctl_t.app_timer_power_on_flag==1){
	       gctl_t.gModel =1;

          Parse_Json_Statement();
		  
           if( gctl_t.gPlasma==1){ //Anion
			
                
				SendWifiData_To_Cmd(0x03,0x01);
                tx_thread_sleep(100);
			
			}
			else{
				gctl_t.gPlasma =0;
				SendWifiData_To_Cmd(0x03,0x0);
				tx_thread_sleep(100);
			}


			if(gctl_t.gUlransonic==1){

					SendWifiData_To_Cmd(0x04,0x01);
					tx_thread_sleep(100);
			}
			else {
					gctl_t.gUlransonic=0;
					SendWifiData_To_Cmd(0x04,0x0);
					tx_thread_sleep(100);
			}



		   if(gpro_t.rx_ptc_flag==1){
              
				SendWifiData_To_Cmd(0x02,0x01);
				tx_thread_sleep(100);
			}
			else if(gpro_t.rx_ptc_flag  ==0){
					gctl_t.ptc_prohibit_on_flag =1;
                    PTC_SetLow();
					SendWifiData_To_Cmd(0x02,0x0);
					tx_thread_sleep(100);

			}

		     gctl_t.set_wind_speed_value =100;
	      
		     MqttData_Publish_Update_Data();
		    tx_thread_sleep(200);

			
	 
		}
			
}

void SetPowerOff_ForDoing(void)
{
   
    // gctl_t.set_wind_speed_value =10;
 
    gctl_t.gFan = 0;
    gpro_t.rx_ptc_flag = 0;//gctl_t.gDry = 0;
  
	gctl_t.gPlasma =0;       //"é„1¤7?é‘„1¤7?"
    gctl_t.gUlransonic = 0; // "æ¤¹è¾«æ«„1¤7"
	gctl_t.gModel =1;


    
	PLASMA_SetLow(); //
    ultrasonic_close();//ultrasnoic Off 
	PTC_SetLow();
	

}






/**********************************************************************
    *
    *Functin Name: void power_off_handler(void)
    *Function : 
    *Input Ref:  key of value
    *Return Ref: NO
    *
************************************************************************/
void power_off_handler(void)
{

   
    switch(gpro_t.power_off_run_step){

    case 1:
		  SendWifiData_Answer_Cmd(0x01,0x0); //power off .
          tx_thread_sleep(100); 
          gpro_t.gTimer_poweroff_fan=0;
         
	
       
        //timer timing 
        gctl_t.set_wind_speed_value=10;
		gctl_t.gModel =1;
		gctl_t.app_timer_power_on_flag =0;
		
	    gpro_t.gTimer_conter_twohours_minutes=0;
	    gpro_t.gTimer_twohours_seconds_counter=0;
	  
         //power off init two hours flag
	     gpro_t.stopTwoHours_flag=0;
		 gpro_t.process_run_step=0;
	     gpro_t.fan_counter_error =0;
		 gpro_t.fan_rx_stop_flag=0;

		  gctl_t.ptc_warning =0;
		  gctl_t.fan_warning =0;
       
		 gctl_t.gTimer_ptc_adc_times=0;
		 gctl_t.gTimer_fan_adc_times=0;
         gpro_t.process_run_step=0;//gpro_t.process_run_step
          gctl_t.rx_set_temp_flag=0; 
         gctl_t.set_temperature_flag = 0; 
		 fan_detect_voltage=100;
         fan_run_one_minute_flag=1;
		 gpro_t.gTimer_poweroff_fan =0;
		 
	      SetPowerOff_ForDoing();
		  gpro_t.power_off_run_step = 2;
       
     // break;

      case 2:
        
       if(wifi_link_net_state() == 1){

          MqttData_Publish_PowerOff_Ref(); 
          tx_thread_sleep(200); //WT.EDTI 2024.11.19 
       }
         gpro_t.power_off_run_step = 4;
       break;

       case 4:

          if(gctl_t.ptc_warning == 1){
		 	
		  	Publish_Data_Warning(ptc_temp_warning,0);
		  	tx_thread_sleep(200);
            
          }
           gpro_t.power_off_run_step = 5;
        break;

        case 5:
            if(gctl_t.ptc_warning == 1){
			Publish_Data_Warning(fan_warning,0);
			tx_thread_sleep(200);
			
          }
        gpro_t.power_off_run_step = 6;
      break;


      case 6:
	   

        gpro_t.stopTwoHours_flag =0;
       
        power_off_stop_fun();

        if(gpro_t.soft_version == 0){
			
			if(gpro_t.gTimer_poweroff_fan > 60 &&  fan_run_one_minute_flag==1){
                 fan_run_one_minute_flag++;
                 FAN_Stop();
			}
        }

	   if(gpro_t.gTimer_update_todisplay > 3){
			gpro_t.gTimer_update_todisplay=0;

			read_sensorData();
	
		}
		

     break;

     }

    
	  
}

/**********************************************************************
    *
    *Functin Name:void power_off_stop_fun(void)
    *Function : 
    *Input Ref:  key of value
    *Return Ref: NO
    *
************************************************************************/
static void power_off_stop_fun(void)
{
      
      PLASMA_SetLow(); //
      ultrasonic_close();// HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic Off 
      PTC_SetLow();
      

}

    
void power_off_action_fun(void)
{
    PLASMA_SetLow(); //
    ultrasonic_open();//ultrasnoic Off 
    PTC_SetLow();

   


}
/**
*@brief
*@notice
*@param
**/
void every_power_on_run(void)
{

   Fan_Full_Speed();//WT.EDIT 2025.01.03//Fan_RunSpeed_Fun();//FAN_CCW_RUN();
   if(gctl_t.app_timer_power_on_flag==0){
     
     // gctl_t.gModel=1;
      gctl_t.gFan = 1;
      gpro_t.rx_ptc_flag = 1;//gctl_t.gDry = 1;
  
  
	 
      //g_dry_open_flag =1;
      gctl_t.gPlasma =1;       //"é„1¤7?é‘„1¤7?"
      gctl_t.gUlransonic = 1; // "æ¤¹è¾«æ«„1¤7"
    
	
       gpro_t.process_run_step=0;
	

	
	  gpro_t.ultrasonic_switch_flag++;
	  gpro_t.plasma_switch_flag++;

	
      PLASMA_SetHigh();
      ultrasonic_open();   //ultrasnoic ON 
      PTC_SetHigh();
	  

    }
  
    gctl_t.gModel=1;
}
/**
*@brief
*@notice
*@param
**/
void app_timer_power_on_reference(void)
{

          if(get_ptc_value()==1){
              
				SendWifiData_To_Cmd(0x02,0x01);
				tx_thread_sleep(100);
			}
			else if(get_ptc_value() ==0){
					
                    
					SendWifiData_To_Cmd(0x02,0x0);
					tx_thread_sleep(100);

			}


			if(gctl_t.gUlransonic==1){

					SendWifiData_To_Cmd(0x04,0x01);
					tx_thread_sleep(100);
			}
			else {
					gctl_t.gUlransonic=0;
					SendWifiData_To_Cmd(0x04,0x0);
					tx_thread_sleep(100);
			}



		  
}
