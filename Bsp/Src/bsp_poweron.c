#include "bsp.h"



// --- 1. 定义任务的时间周期（单位：毫秒，假设基础Tick为1ms） ---
#define PERIOD_WIFI_STATE      300    // 10ms*300 = 3000ms = 3s
#define PERIOD_WIFI_UPDATE     200    // 10ms*200 = 2000ms = 2s
#define PERIOD_WORKS_HOURS     150    //  10ms*150 = 1500ms = 1.5s
#define PERIOD_FAN_ADC         250    //  10ms*250 = 2500ms = 2.5s
#define PERIOD_WIFI_TEMP       500    //   10ms * 500 = 50000ms = 5s 
#define PERIOD_READ_DHT11      100    //   10ms * 100 = 1000ms = 1s
#define PERIOD_FAN_SPEED       130    //   10ms * 130 = 1300ms = 1.3s


// --- 2. 定义分时任务控制结构体 ---
typedef struct {
    uint32_t last_tick;        // 记录上一次真正运行时的系统绝对时间戳
    //uint32_t counter;       // 时间计数器
    uint32_t period;        // 任务运行周期
    void (*task_handler)(void); // 任务函数指针
} TimeSharingTask_t;

static void handler_wifi_state(void);
static void handler_wifi_update_data(void);
static void handler_works_hours(void);
static void handler_fan_adc(void);
static void handler_wifi_update_temp_humidity(void);
static void handler_read_dht11(void);
static void handler_fan_speed_state(void);





// --- 4. 初始化分时任务表 ---
TimeSharingTask_t g_tasks[] = {
    {0, PERIOD_WIFI_STATE,       handler_wifi_state},
    {0, PERIOD_WIFI_UPDATE,      handler_wifi_update_data},
    {0, PERIOD_WORKS_HOURS,      handler_works_hours},
    {0, PERIOD_FAN_ADC,          handler_fan_adc},
    {0, PERIOD_WIFI_TEMP,        handler_wifi_update_temp_humidity},
    {0, PERIOD_READ_DHT11,       handler_read_dht11},
    {0, PERIOD_FAN_SPEED,        handler_fan_speed_state}
    
	
};

#define TASK_NUM (sizeof(g_tasks) / sizeof(TimeSharingTask_t))


static void power_off_stop_fun(void);
void every_power_on_run(void);
uint8_t fan_run_one_minute_flag;
static void app_timer_power_on_reference(void);
static void power_on_init_handler(void);
static void power_on_cycle_handler(void);



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
	if( gpro_t.process_run_step < 20){
	  power_on_init_handler();
	}
    else
       power_on_cycle_handler();

}

/************************************************************************************
*
*Function Name: static void power_on_init_handler(void)
*Fucntion :
*Input Ref:NO
*Return Ref:NO
*
************************************************************************************/
static void power_on_init_handler(void)
{

	static uint8_t counter,sw_flag,counter_flag;
    switch(gpro_t.process_run_step){

	case 0: //1
	     
          gpro_t.power_off_run_step=0;
          /*power on initial reference---start */
      
     
         gctl_t.set_temperature_value=40; //power on default set temperature value is 40 degree,don't compare
 
		
		
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
	 	//tx_thread_sleep(1);
    }
    every_power_on_run();
	//read_sensorData();
	
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
			 tx_thread_sleep(20);
		
		 }
		 else if(gctl_t.app_timer_power_on_flag ==1){
		     	app_timer_power_on_reference();
			 	
		  }
		// read_sensorData();
	     gpro_t.process_run_step= 4;
	break;

  case 4: 

     if(wifi_link_net_state() ==1 &&  gctl_t.app_timer_power_on_flag==0){
    
		  MqttData_Publish_Init();
		  tx_thread_sleep(20);
     } else if(gctl_t.app_timer_power_on_flag == 1){

           	gctl_t.set_wind_speed_value=100;
            MqttData_Publis_SetFan(gctl_t.set_wind_speed_value);//WT.EDIT 2025.12.19
            gctl_t.set_temperature_value=40;
            MqttData_Publis_SetTemp(gctl_t.set_temperature_value);

	 }
	
     	
   // read_sensorData();

    gpro_t.process_run_step= 5;

	 break;

	case 5:
		
        ActionEvent_Handler();
		read_sensorData();
       
	    gpro_t.process_run_step= 0xff;

	break;

	default:

	break;

    	}
}
/************************************************************************************
*
*Function Name: static void power_on_cycle_handler(void)
*Fucntion :
*Input Ref:NO
*Return Ref:NO
*
************************************************************************************/
static void power_on_cycle_handler(void)
{

      // 获取当前系统的绝对时间戳
      uint32_t current_tick = tx_time_get();

      // 第二步：通过时间片轮询核心算法，分时调用各个功能模块
	   for (uint8_t i = 0; i < TASK_NUM; i++) {
		   //g_tasks[i].counter++; // 基础 Tick 自增
		   if ((current_tick - g_tasks[i].last_tick) >= g_tasks[i].period) {
		   
		        // 滚动更新该任务的历史时间戳基准
               //g_tasks[i].last_tick = current_tick;
               // 改进：滚动累加周期，消除长跑下的时间漂移
               g_tasks[i].last_tick += g_tasks[i].period;
			 
			   g_tasks[i].task_handler(); // 触发对应周期的执行函数
		   
	   }

	   }

}
/************************************************************************************
*
*Function Name: static void power_on_cycle_handler(void)
*Fucntion :
*Input Ref:NO
*Return Ref:NO
*
************************************************************************************/
static void handler_wifi_state(void)
{
    // 如果这些变量之前是全局的，保持原样；如果是局部的，必须加 static 保持状态
    static uint8_t counter = 0;
    static uint8_t sw_flag = 0;
	

	counter++;


	if(net_t.wifi_link_net_success ==1 && counter > 1 && gpro_t.soft_version == 0){ //WT.EDIT 2026.02.27
		counter =0;
		sw_flag = sw_flag ^ 0x01;
		if(sw_flag == 1){
			SendWifiData_olderCmd(0x1F,0x01);//SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
			tx_thread_sleep(1);
		}
		else{
			SendWifiData_To_Data(0x1F,0x01);
			tx_thread_sleep(1);
		}

	}
	else if(net_t.wifi_link_net_success ==0 && counter > 1 && gpro_t.soft_version ==0){ //WT.EDIT 2026.02.27
		counter =0;
		sw_flag = sw_flag ^ 0x01;
		if(sw_flag == 1){
			SendWifiData_olderCmd(0x1F,0x0);//SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
			tx_thread_sleep(1);
		}
		else{
			SendWifiData_To_Data(0x1F,0x0);
			tx_thread_sleep(1);
		}
	}

			  
}

/**
*
*@brief 
*@notice
*@param
*@retval
*
**/
static void handler_wifi_update_data(void)
{

   if(gpro_t.wifi_led_fast_blink_flag==0 && net_t.wifi_link_net_success ==1){
      if(gctl_t.app_timer_power_on_flag==0 && gctl_t.first_link_tencent_cloud_flag ==1){
	
		  gctl_t.first_link_tencent_cloud_flag++;

               MqttData_Publish_Update_Data();
			  tx_thread_sleep(20);//HAL_Delay(200);
             SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
             tx_thread_sleep(1);
          
    	}
	    else if(gctl_t.first_link_tencent_cloud_flag < 4){
			 gctl_t.first_link_tencent_cloud_flag++;

            Subscriber_Data_FromCloud_Handler();
    	    tx_thread_sleep(20);
	    }
		
		   SendData_Set_Command(0x1F,0x01);//SendWifiData_To_Data(0x1F,0x01);
           tx_thread_sleep(1);
	}
    
}

 /**
 *
 *@brief 
 *@notice
 *@param
 *@retval
 *
 **/
static void handler_works_hours(void)
{
 
     if(gpro_t.fan_warning_flag > 1 || gpro_t.ptc_warning  > 1){
        if(gpro_t.fan_warning_flag > 1 ) gpro_t.fan_warning_flag = 0; //strictly forbid 
	    if(gpro_t.ptc_warning  > 1)gpro_t.ptc_warning = 0;
		
     }

	 works_run_two_hours_state();
   
	
}

  /**
  *
  *@brief 
  *@notice
  *@param
  *@retval
  *
  **/
static void handler_fan_adc(void)
{
  	 
     adc_detected_hundler();
	   
}


  /**
*
*@brief 
*@notice
*@param
*@retval
*
**/

static void handler_wifi_update_temp_humidity(void)
{
    
    if(wifi_link_net_state() ==1){
			
		Update_Dht11_Totencent_Value();
     }
	 
 }
  /**
*
*@brief 
*@notice
*@param
*@retval
*
**/

static void handler_read_dht11(void)
 {

     if(gctl_t.set_temperature_flag > 1 || gctl_t.set_temperature_value > 40 || gctl_t.ptc_prohibit_on_flag > 1
	 	  ||gctl_t.app_timer_power_on_flag > 2 || gctl_t.set_temp_first_closeptc > 1 || gpro_t.soft_version > 2){
	 	if(gctl_t.set_temperature_flag > 1)gctl_t.set_temperature_flag =0;
		if(gctl_t.set_temperature_value > 40 && gctl_t.set_temperature_flag ==0)gctl_t.set_temperature_value =40;
        if(gctl_t.ptc_prohibit_on_flag > 1)gctl_t.ptc_prohibit_on_flag =0;
		if(gctl_t.app_timer_power_on_flag > 2)gctl_t.app_timer_power_on_flag=0;
		if( gctl_t.set_temp_first_closeptc > 1)  gctl_t.set_temp_first_closeptc =0;
		if(gpro_t.soft_version > 2)gpro_t.soft_version = 0 ;
	 }

  

    if(gpro_t.rx_ptc_flag >1)gpro_t.rx_ptc_flag=1;//2026.02.27 WT.EDIT
    if(gpro_t.stopTwoHours_flag==0)gpro_t.fan_rx_stop_flag =0;

	read_sensorData();

}

   /**
   *
   *@brief 
   *@notice
   *@param
   *@retval
   *
   **/

static void handler_fan_speed_state(void)
{
   
   	 if(gpro_t.stopTwoHours_flag ==0 ){
	   
		Fan_RunSpeed_Fun();
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
				tx_thread_sleep(1);

	   }
       else if(wifi_link_net_state()==1 && ptc_default != gpro_t.ptc_actiov_f){//ptc_actiov_f = 0++
             gpro_t.rx_ptc_flag=1 ;
			ptc_default = gpro_t.ptc_actiov_f;

			MqttData_Publish_SetPtc(0x01);
			tx_thread_sleep(20);//tx_thread_sleep(100);//HAL_Delay(350);
	     	}
		}
   	  
	}
	else if(gpro_t.rx_ptc_flag ==0){
		
	  
		PTC_SetLow();
		if(gctl_t.app_timer_power_on_flag == 1){
			    gctl_t.ptc_prohibit_on_flag =1;
                SendWifiData_To_Cmd(0x02,0);
				tx_thread_sleep(1);

		}
		else if(wifi_link_net_state()==1 && ptc_default != gpro_t.ptc_actiov_f){//if(ptc_default!= get_ptc_value() && wifi_link_net_state()==1){
			gpro_t.rx_ptc_flag =0 ;

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
		 	gpro_t.plasma_switch_flag++;
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
	 	gpro_t.ultrasonic_switch_flag++;
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
			gpro_t.ultrasonic_switch_flag++;
			ultrasonic_default = gpro_t.ultrasonic_switch_flag;	
			 
			MqttData_Publish_SetUltrasonic(0);
		    tx_thread_sleep(20);
			
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
                tx_thread_sleep(1);
			
			}
			else{
				gctl_t.gPlasma =0;
				SendWifiData_To_Cmd(0x03,0x0);
				tx_thread_sleep(1);
			}


			if(gctl_t.gUlransonic==1){

					SendWifiData_To_Cmd(0x04,0x01);
					tx_thread_sleep(1);
			}
			else {
					gctl_t.gUlransonic=0;
					SendWifiData_To_Cmd(0x04,0x0);
					tx_thread_sleep(1);
			}



		   if(gpro_t.rx_ptc_flag==1){
              
				SendWifiData_To_Cmd(0x02,0x01);
				tx_thread_sleep(1);
			}
			else if(gpro_t.rx_ptc_flag  ==0){
					gctl_t.ptc_prohibit_on_flag =1;
                    PTC_SetLow();
					SendWifiData_To_Cmd(0x02,0x0);
					tx_thread_sleep(1);

			}

		     gctl_t.set_wind_speed_value =100;
	      
		     MqttData_Publish_Update_Data();
		     tx_thread_sleep(20);

			
	 
		}
			
}

void SetPowerOff_ForDoing(void)
{
   
    // gctl_t.set_wind_speed_value =10;
 
 
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
          tx_thread_sleep(1); 
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
	
		 gpro_t.fan_rx_stop_flag=0;

		  gctl_t.ptc_warning =0;
		  gctl_t.fan_warning =0;
       

         gpro_t.process_run_step=0;//gpro_t.process_run_step
          gctl_t.rx_set_temp_flag=0; 
         gctl_t.set_temperature_flag = 0; 
		 gpro_t.first_ptc_on=0;
		 fan_detect_voltage=100;
         fan_run_one_minute_flag=1;
		 gpro_t.gTimer_poweroff_fan =0;
		 
	      SetPowerOff_ForDoing();
		  gpro_t.power_off_run_step = 2;
       
      break;

      case 2:
        
       if(wifi_link_net_state() == 1){

          MqttData_Publish_PowerOff_Ref(); 
          tx_thread_sleep(20); //WT.EDTI 2024.11.19 
       }
         gpro_t.power_off_run_step = 4;
       break;

       case 4:

          if(gctl_t.ptc_warning == 1){
		 	
		  	Publish_Data_Warning(ptc_temp_warning,0);
		  	tx_thread_sleep(20);
            
          }
           gpro_t.power_off_run_step = 5;
        break;

        case 5:
            if(gctl_t.fan_warning == 1){
			Publish_Data_Warning(fan_warning,0);
			tx_thread_sleep(20);
			
            }
			if(gctl_t.ptc_warning == 1){
			   Publish_Data_Warning(ptc_temp_warning,0);
			  tx_thread_sleep(20);
			
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

	   if(gpro_t.gTimer_update_tencet_dht11  > 3){
			gpro_t.gTimer_update_tencet_dht11=0;

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
    else{


	  if(gpro_t.rx_ptc_flag==1){
              
				SendWifiData_To_Cmd(0x02,0x01);
				tx_thread_sleep(1);
			}
			else if(gpro_t.rx_ptc_flag  ==0){
					gctl_t.ptc_prohibit_on_flag =1;
                    PTC_SetLow();
					SendWifiData_To_Cmd(0x02,0x0);
					tx_thread_sleep(1);

			}

			if(gctl_t.gUlransonic==1){

					SendWifiData_To_Cmd(0x04,0x01);
					tx_thread_sleep(1);
			}
			else {
					gctl_t.gUlransonic=0;
					SendWifiData_To_Cmd(0x04,0x0);
					tx_thread_sleep(1);
			}

	      if( gctl_t.gPlasma==1){ //Anion
			
                
				SendWifiData_To_Cmd(0x03,0x01);
                tx_thread_sleep(1);
			
			}
			else{
				gctl_t.gPlasma =0;
				SendWifiData_To_Cmd(0x03,0x0);
				tx_thread_sleep(1);
			}
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
				tx_thread_sleep(1);
			}
			else if(get_ptc_value() ==0){
					
                    
					SendWifiData_To_Cmd(0x02,0x0);
					tx_thread_sleep(1);

			}


			if(gctl_t.gUlransonic==1){

					SendWifiData_To_Cmd(0x04,0x01);
					tx_thread_sleep(1);
			}
			else {
					gctl_t.gUlransonic=0;
					SendWifiData_To_Cmd(0x04,0x0);
					tx_thread_sleep(1);
			}
}
