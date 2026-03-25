#include "bsp.h"




static void tim17_isr_callback_handler(void);


// 定义 TIM17 回调函数类型
typedef void (*Tim17Callback)(void);

// 保存回调函数指针的全局变量
static Tim17Callback tim17_cb = NULL;   // 保存回调函数指针

// 注册回调函数
void tim17_register_callback(Tim17Callback cb) 
{
    tim17_cb = cb;  // 把传进来的函数指针保存到全局变量
}

// ISR 调用时触发
void tim17_invoke_callback(void) 
{
    if (tim17_cb) {
        tim17_cb();   // 调用用户注册的回调
    }
}

// 注册回调函数
void callback_register_fun(void)
{
  tim17_register_callback(tim17_isr_callback_handler);

}

/********************************************************************************
	**
	*Function Name:void tim17_isr_callback_handler(void)
	*Function : // 业务逻辑函数（回调实现）
	*Input Ref: 
	*Return Ref:NO
	*
*******************************************************************************/
static void tim17_isr_callback_handler(void)
{
   static  uint16_t tm0;

 

    tm0 ++ ;
      
     
	 if(tm0 > 999){//10ms *100 = 1000ms =1s
        tm0 =0;
     
	   gpro_t.gTimer_twohours_seconds_counter++;

        gpro_t.gTimer_link_net_timer_time++;
	    gctl_t.gTimer_senddata_panel++;
       

	   gctl_t.gTimer_fan_adc_times++;
	  // gctl_t.gTimer_ptc_adc_times++;
	  

	
	  gctl_t.gTimer_linkTencentCounter++;
	
	
	  //wifi function 
       wifi_t.gTimer_auto_detected_net_state_times ++;
       wifi_t.gTimer_auto_link_net_time++ ;

	   wifi_t.gTimer_get_beijing_time++;

     //gprocess 
       gpro_t.gTimer_power_on_first_link_tencent++;
       gpro_t.gTimer_get_data_from_tencent_data++;
       gpro_t.gTimer_dc_power_on_auto_link_net++;
       


       gpro_t.gTimer_detect_fan_error++;
     
	   gpro_t.gTimer_power_on_auto_link++;
	   gpro_t.gTimer_update_todisplay++;
	   gpro_t.gTimer_update_tencet_dht11++;

        gpro_t.gTimer_poweroff_fan++;
		gpro_t.gTimer_read_dth11_sensor ++;
		gpro_t.gTimer_read_dht11_to_disp++;
        if(gpro_t.gTimer_twohours_seconds_counter > 59){
			gpro_t.gTimer_twohours_seconds_counter=0;	
			gpro_t.gTimer_conter_twohours_minutes++;
        }
	
	
      }
   
}	
	
		    

        
		
	
 





 
