#include "bsp.h"

// --- 1. 定义任务的时间周期（单位：毫秒，假设基础Tick为1ms） ---
#define PERIOD_WIFI_STATE      3500    // 10ms*300 =  3s
#define PERIOD_WIFI_UPDATE     1500    // 10ms*150 = 1.5s
#define PERIOD_WORKS_HOURS     4300    //  10ms*150 = 1500ms = 1.5s
#define PERIOD_FAN_ADC         5000    //  10ms*250 = 2500ms = 2.5s
#define PERIOD_WIFI_TEMP       7000    //   10ms * 500 = 50000ms = 5s 
#define PERIOD_READ_DHT11      2800    //   10ms * 300 = 3s
#define PERIOD_FAN_SPEED       2000    //   10ms * 200 = 2s
#define PERIOD_PERIPHERAL      500     //   10ms* 50 = 500ms
#define PERIOD_LINK_WIFI       30
#define PERIOD_DISP_AI_WIF     2300
#define PERIOD_WIFI_REPORT     1000

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
static void handler_hardware_module_action(void);
static void handler_link_wifi(void);
static void handler_send_ai_wif(void);
static void handler_wifi_report(void);

volatile uint8_t time_slot ;

#if 1
// --- 4. 初始化分时任务表 ---
TimeSharingTask_t g_tasks[] = {
    {0, PERIOD_WIFI_STATE,       handler_wifi_state},
    {0, PERIOD_WIFI_UPDATE,      handler_wifi_update_data},
    {0, PERIOD_WORKS_HOURS,      handler_works_hours},
    {0, PERIOD_FAN_ADC,          handler_fan_adc},
    {0, PERIOD_WIFI_TEMP,        handler_wifi_update_temp_humidity},
    {0, PERIOD_READ_DHT11,       handler_read_dht11},
    {0, PERIOD_FAN_SPEED,        handler_fan_speed_state},
    {0,PERIOD_PERIPHERAL,        handler_hardware_module_action},
    {0,PERIOD_LINK_WIFI,         handler_link_wifi},
    {0,PERIOD_DISP_AI_WIF,       handler_send_ai_wif},
    {0,PERIOD_WIFI_REPORT,       handler_wifi_report}
   
    
	
};

#define TASK_NUM (sizeof(g_tasks) / sizeof(TimeSharingTask_t))

// 定义一个 32 位的全局 Tick 计数器
// 使用 volatile 修饰，防止编译器将其优化，确保每次读取都从内存中获取最新值
static __IO uint32_t g_system_ticks = 0;

/**
  * @brief  获取当前系统的绝对时间戳 (单位: 毫秒 ms)
  * @retval 当前的 tick 值
  */
uint32_t get_system_tick(void)
{
    return g_system_ticks ;
}

/**
  * @brief  SysTick 递增函数（由中断服务函数调用）
  */
void inc_system_tick(void)
{
    g_system_ticks ++;
}

#else 

void static task_time_slot_scheduler(void)
{
   static uint16_t wifi_counter_1,wifi_counter_2,counter_1,counter_2;
   static uint16_t counter_3,counter_4,counter_5,counter_6;
   switch(time_slot){

    case 0:
	   wifi_counter_1 ++;
       if(wifi_counter_1 >300 ){//200
        wifi_counter_1=0;
	   handler_wifi_state();

       }

    break;

    case 1:
		wifi_counter_2++;
		if(wifi_counter_2 > 150){
			wifi_counter_2=0;
		handler_wifi_update_data();

	    }

	break;

	case 2:
		counter_1++;
		if(counter_1 > 400){
			counter_1 =0;
		handler_works_hours();

	    }

	break;

	case 3:

	counter_2++;
		if(counter_2 > 500){
			counter_2 =0;
		 handler_fan_adc();
		}

	break;

	case 4:
	counter_3++;
		if(counter_3 > 600){
				counter_3 =0;
			handler_wifi_update_temp_humidity();
		}

	break;

	case 5:
	counter_4++;
			if(counter_4 > 100){
				counter_4 =0;
				handler_read_dht11();
				}

	break;

	case 6:
	counter_5++;
			if(counter_5 > 270){
				counter_5 =0;
				handler_fan_speed_state();
				}

	break;

	case 7:
	counter_6++;
			if(counter_6 > 60){
				counter_6 =0;
				handler_module_action();

			}

	break;

	case 8:
	   link_wifi_to_tencent_handler(); //detected ADC of value 
        

	break;

	case 9:
	 ai_mode_display_fun();

	break;


   }

   time_slot ++;
   if(time_slot > 9)time_slot = 0;//10ms * 10 = 100ms
}



#endif 




static void power_off_stop_fun(void);
void donot_smart_app_power_on_init(void);
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
    else{
	   #if 0
       task_scheduler();
	   #else
	   
	   power_on_cycle_handler();
	   #endif 

	}
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

	static uint8_t counter,sw_flag,counter_flag,i;
	uint32_t boot_tick;
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
	
	
         Fan_Full_Speed();//Fan_RunSpeed_Fun();//WT.EDIT 2026.01.26
     
         read_sensorData();//updateDht11_sensorData_toDisp();
         fan_run_one_minute_flag=1;
		 gpro_t.process_run_step= 1;
		
		 
	break; 


  case 1:
  	
  	if(gctl_t.app_timer_power_on_flag > 1)gctl_t.app_timer_power_on_flag=0;
	
    if(gctl_t.app_timer_power_on_flag ==1){
		Fan_Full_Speed();
		gctl_t.gModel=1;
     	smartphone_timer_power_on_and_normal_handler();
	 
    }
    else{
		donot_smart_app_power_on_init();

    }

	
	 gpro_t.process_run_step= 2;

  break;


   case 2:
	
         if(wifi_link_net_state() ==1 && gctl_t.app_timer_power_on_flag ==0){
    
		    
	         gctl_t.set_wind_speed_value =100;
			 Fan_Full_Speed();

		     MqttData_Publish_SetOpen(1);  
			
		
		 }
		 else if(gctl_t.app_timer_power_on_flag ==1){
               
				app_timer_power_on_reference();
			 	
		  }
		// read_sensorData();
	     gpro_t.process_run_step= 4;
	break;


  case 3:
  
	 module_hardware_control();
	 read_sensorData();
	
	 gpro_t.process_run_step= 3;
	   
    break;


  case 4: 

     if(wifi_link_net_state() ==1 &&  gctl_t.app_timer_power_on_flag==0){
    
		  MqttData_Publish_Init();
		 
     } 
	 else if(gctl_t.app_timer_power_on_flag == 1){

           	gctl_t.set_wind_speed_value=100;
            MqttData_Publis_SetFan(gctl_t.set_wind_speed_value);//WT.EDIT 2025.12.19
           

	 }
	gpro_t.process_run_step= 5;

	 break;

	case 5:
		
        if(gctl_t.app_timer_power_on_flag == 1){

           
            gctl_t.set_temperature_value=40;
            MqttData_Publis_SetTemp(gctl_t.set_temperature_value);

	     }
		read_sensorData();

#if 1	
        boot_tick = get_system_tick();
		for(i=0;i < TASK_NUM;i ++){

		     g_tasks[i].last_tick = boot_tick;
		}
#endif 
	    gpro_t.process_run_step= 0xfe;//0xff ->? -1

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
#if 1
static void power_on_cycle_handler(void)
{

      // 获取当前系统的绝对时间戳
      uint32_t current_tick = get_system_tick();//tx_time_get();
	
        // 通过时间片轮询核心算法，分时调用各个功能模块
    for (uint8_t i = 0; i < TASK_NUM; i++) 
    {
        if ((current_tick - g_tasks[i].last_tick) >= g_tasks[i].period) 
        {
            // 【工业级进化：防轰炸饱和截断】
            // 如果卡顿/被高优先级抢占的时间超过了 2 个周期，直接对齐当前时间，放弃追赶
            if ((current_tick - g_tasks[i].last_tick) > (g_tasks[i].period * 2)) 
            {
                g_tasks[i].last_tick = current_tick;
            }
            else 
            {
                // 如果只是正常范围内的轻微抖动，滚动累加周期，死锁锁相，消除长期长跑漂移
                g_tasks[i].last_tick += g_tasks[i].period;
            }
            
            // 触发对应周期的执行函数（确保不为 NULL，防止空指针崩溃）
            if (g_tasks[i].task_handler != NULL)
            {
                g_tasks[i].task_handler(); 
            }
        }
    }
	  

}
#endif 
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
   
    static uint8_t sw_flag = 0;
	
   if( gpro_t.soft_version == 0){ //WT.EDIT 2026.02.27
		
		sw_flag = sw_flag ^ 0x01;
	    // 关键优化：用三元运算符直接提取状态值，消灭大面积重复的 if-else 块
        uint8_t wifi_status = (net_t.wifi_link_net_success == 1) ? 0x01 : 0x00;
		if(sw_flag == 1){
			SendWifiData_olderCmd(0x1F,wifi_status);//SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
			//LL_mDelay(10);
		}
		else{
			SendWifiData_To_Data(0x1F,wifi_status);
			//LL_mDelay(10);
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
			
             SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
             //LL_mDelay(10);
      }
	  else if(gctl_t.first_link_tencent_cloud_flag < 3){
			 gctl_t.first_link_tencent_cloud_flag++;

            Subscriber_Data_FromCloud_Handler();
    	
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
static void handler_works_hours(void)
{
 
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
static void handler_hardware_module_action(void)
{
   
   if((gpro_t.fan_rx_stop_flag ==0 && gpro_t.stopTwoHours_flag ==0)){//(gctl_t.app_timer_power_on_flag == 1)
		 
			 
		module_hardware_control();//module_action_handler();
	 }
	

}
/**
*
*@brief dispatch module_hardware_control task 
*@notice
*@param
*@retval
*
**/
static void handler_wifi_report(void)
{

  module_wifi_report_handler();

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

     if(gctl_t.set_temperature_flag > 1 || gctl_t.set_temperature_value > 40 || 
	 	  gctl_t.app_timer_power_on_flag > 2 || gctl_t.set_temp_first_closeptc > 1 || gpro_t.soft_version > 2){
	 	if(gctl_t.set_temperature_flag > 1)gctl_t.set_temperature_flag =0;
		if(gctl_t.set_temperature_value > 40 && gctl_t.set_temperature_flag ==0)gctl_t.set_temperature_value =40;
   
		if(gctl_t.app_timer_power_on_flag > 2)gctl_t.app_timer_power_on_flag=0;
		if( gctl_t.set_temp_first_closeptc > 1)  gctl_t.set_temp_first_closeptc =0;
		if(gpro_t.soft_version > 2)gpro_t.soft_version = 0 ;
	 }

 
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
/**
	*
	*@brief 
	*@notice
	*@param
	*@retval
	*
**/


static void handler_link_wifi(void)
{
 link_wifi_to_tencent_handler();
}
/**
	*
	*@brief 
	*@notice
	*@param
	*@retval
	*
**/

static void handler_send_ai_wif(void)
{
	ai_mode_display_fun();


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
		    gctl_t.set_wind_speed_value =100;
	      
		    MqttData_Publish_Update_Data();
		     

	}
			
}

void SetPowerOff_ForDoing(void)
{
   
    // gctl_t.set_wind_speed_value =10;
 
 
    gpro_t.gPtc = 0;//gctl_t.gDry = 0;
  
	gctl_t.gPlasma =0;       //"é„1¤7?é‘„1¤7?"
    gctl_t.gUltrasonic = 0; // "æ¤¹è¾«æ«„1¤7"
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
    static uint8_t dc_power = 0,power_on_sound_flag=0;
   
    switch(gpro_t.power_off_run_step){

	case 0:

	
		gpro_t.process_run_step=0;
         //timer timing 
        gctl_t.set_wind_speed_value=10;
		gctl_t.gModel =1;
		gctl_t.app_timer_power_on_flag =0;
		
	    gpro_t.gTimer_conter_twohours_minutes=0;
	    gpro_t.gTimer_twohours_seconds_counter=0;
	  
         //power off init two hours flag
	     gpro_t.stopTwoHours_flag=0;
	
		     
	
		 gpro_t.fan_rx_stop_flag=0;

		  gctl_t.ptc_warning =0;
		  gctl_t.fan_warning =0;
       

          gctl_t.rx_set_temp_flag=0; 
         gctl_t.set_temperature_flag = 0; 
		 gpro_t.first_ptc_on=0;
		  gpro_t.wifi_led_fast_blink_flag=0;
		 fan_detect_voltage=100;
		 if(dc_power ==0){
		 	dc_power ++;
		    fan_run_one_minute_flag=0;

		 }
		 else{ 
		 	dc_power = 2;
            fan_run_one_minute_flag=1;
		    gpro_t.gTimer_poweroff_fan =0;
		 }

		
		 
		 
	      SetPowerOff_ForDoing();
		  gpro_t.power_off_run_step = 1;
       
      break;

      case 1:

	  if(dc_power == 2){
	     fan_run_one_minute_flag=1;
		 gpro_t.gTimer_poweroff_fan =0;

       }
        
       if(wifi_link_net_state() == 1){

          MqttData_Publish_PowerOff_Ref(); 
          LL_mDelay(200); //WT.EDTI 2024.11.19 
       }
         gpro_t.power_off_run_step = 2;
       break;

       case 2:

          if(gctl_t.ptc_warning == 1){
		 	
		  	Publish_Data_Warning(ptc_temp_warning,0);
		  	LL_mDelay(200);
            
          }
           gpro_t.power_off_run_step = 3;
        break;

        case 3:
          
       
       if(gpro_t.gTimer_poweroff_fan > 60 &&  fan_run_one_minute_flag==1){
                 fan_run_one_minute_flag++;
                 FAN_Stop();
		}
	    else if(fan_run_one_minute_flag==2){
		     FAN_Stop();

        }
        gpro_t.power_off_run_step = 4;

     break;

	 case 4:

	    power_off_stop_fun();

	   if(gpro_t.gTimer_update_tencet_dht11  > 10 && dc_power != 1){
				gpro_t.gTimer_update_tencet_dht11=0;
	 
				read_sensorData();
		}
	   gpro_t.power_off_run_step = 3;

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
void donot_smart_app_power_on_init(void)
{

   Fan_Full_Speed();//WT.EDIT 2025.01.03//Fan_RunSpeed_Fun();//FAN_CCW_RUN();
   if(gctl_t.app_timer_power_on_flag==0){
     
      gctl_t.gModel=1;
     
      gpro_t.gPtc = 1;//gctl_t.gDry = 1;
  
 
      gctl_t.gPlasma =1;       //"é„1¤7?é‘„1¤7?"
      gctl_t.gUltrasonic = 1; // "æ¤¹è¾«æ«„1¤7"
    
	  gpro_t.ultrasonic_switch_flag++;
	  gpro_t.plasma_switch_flag++;

	
      PLASMA_SetHigh();
      ultrasonic_open();   //ultrasnoic ON 
      PTC_SetHigh();
	  

    }
  
    
}
/**
*@brief
*@notice
*@param
**/
void app_timer_power_on_reference(void)
{
   
     // 1. 局部变量缓存状态，避免多次调用函数或中途状态被其它时片修改
   // 1. 局部变量初始化时，直接一步到位完成数据清洗（非1即0）
    uint8_t ptc_val      = (get_ptc_value() == 1) ? 1 : 0;
    uint8_t ultrasonic   = (gctl_t.gUltrasonic == 1) ? 1 : 0;
    uint8_t plasma       = (gctl_t.gPlasma == 1) ? 1 : 0;
    gctl_t.gModel=1;
    // 2. 一次性打包发送，大幅降低串口与 WiFi 模块的通信开销
    SendWifiData_To_three_Cmd(0x15, ptc_val,plasma,ultrasonic);

    // 3. 仅需一次 Sleep 释放 CPU 控制权
    LL_mDelay(15);

 }
