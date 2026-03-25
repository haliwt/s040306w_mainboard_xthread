#ifndef __BSP_H
#define __BSP_H
#include "main.h"

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "app_threadx.h"


#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"





#include "main.h"


#include "bsp_threadx.h"
#include "bsp_fan.h"

#include "bsp_ctl.h"
#include "bsp_ptc.h"
#include "bsp_delay.h"

#include "bsp_cmd_link.h"

#include "bsp_poweron.h"
#include "bsp_time.h"
#include "bsp_plasma.h"
#include "bsp_netparse.h"
#include "bsp_usart.h"
#include "bsp_usart_wifi.h"
#include "bsp_dma_usart.h"
#include "bsp_flash.h"


#include "interrupt_manager.h"


#include "bsp_wifi.h"
#include "bsp_buzzer.h"
#include "bsp_adc.h"



#include "bsp_wifi_fun.h"

#ifndef DHT22_ENABL
#include "bsp_dht11.h"
#else
#include "bsp_dht22.h"
#endif
#include "bsp_fan.h"
#include "bsp_ultrasonic.h"

//wifi files
#include "bsp_esp8266.h"
#include "bsp_mqtt_iot.h"
#include "bsp_publish.h"
#include "bsp_wifi_fun.h"
#include "bsp_wifi.h"
#include "bsp_subscription.h"


#define Enable_EventRecorder  0

#define DEBUG_FLAG        Enable_EventRecorder //(Enable_EventRecorder? 1:0)


#define WIFI_RX_NUMBERS         1



#define  USE_FreeRTOS      0


#if USE_FreeRTOS == 1

	#define DISABLE_INT()    taskENTER_CRITICAL()
	#define ENABLE_INT()     taskEXIT_CRITICAL()
#else
	/* ??????????l? */
	#define ENABLE_INT()	__set_PRIMASK(0)	/* '???????? */
	#define DISABLE_INT()	__set_PRIMASK(1)	/* ????????? */
#endif

#if Enable_EventRecorder == 1
	#include "EventRecorder.h"
#endif


extern uint8_t wifi_rx_inputBuf[WIFI_RX_NUMBERS];
extern uint8_t inputBuf[1];






typedef enum{


  power_off,
  power_on

}power_state_t;


typedef struct PROCESS_T{


   uint8_t gpower_on;
   uint8_t disp_rx_cmd_done_flag;
   uint8_t  wifi_rx_data_array[150];
   uint8_t wifi_rx_data_counter;
   uint8_t power_off_run_step;
  
   uint8_t copy_cmd_notice_buff[10];
   //copy command 

 
 


   uint8_t get_beijing_flag;
   uint8_t stopTwoHours_flag;

   uint8_t receive_copy_cmd ;
   uint8_t send_ack_cmd;
   uint8_t phone_power_on_flag;
   //uint8_t gFan_pwm_duty_level;
   uint8_t answer_buzzer_flag ;
 
   uint8_t rx_ptc_flag;
   uint8_t second_disp_flag;
	
    
	 uint8_t ultrasonic_switch_flag;
	 uint8_t plasma_switch_flag;
    volatile uint8_t process_run_step;
  
   uint8_t wifi_led_fast_blink_flag;
   uint8_t get_beijing_time_success;
   uint8_t link_net_step;
   uint8_t ptc_warning ;
   uint8_t fan_warning_flag;
   uint8_t soft_version;
   volatile uint8_t decoder_success_flag; //interrupt be used to flag.
   uint8_t fan_counter_error ;
   uint8_t fan_rx_stop_flag;

   
   uint8_t disp_works_hours ;    
   uint8_t disp_works_minutes ;
   uint8_t gTimer_works_time_seconds ;
   //timer time
   uint8_t gTimer_power_on_first_link_tencent;
   uint8_t gTimer_get_data_from_tencent_data;
   uint8_t gTimer_link_net_timer_time;
   uint8_t gTimer_dc_power_on_auto_link_net;

   uint8_t gTimer_detect_fan_error;

   uint8_t gTimer_power_on_auto_link;
   uint8_t gTimer_update_todisplay;
   uint8_t gTimer_update_tencet_dht11;
   uint8_t gTimer_poweroff_fan;
   uint8_t gTimer_read_dth11_sensor ;
   uint8_t gTimer_read_dht11_to_disp;
   uint8_t gTimer_twohours_seconds_counter;	
   uint8_t gTimer_conter_twohours_minutes;
  

   
  
  
}process_t;

extern process_t gpro_t;

void bsp_init(void);

uint8_t bcc_check(const unsigned char *data, int len) ;

void link_wifi_to_tencent_handler(void);

void wifi_communication_tnecent_handler(void);

void wifi_auto_detected_link_state(void);




 uint8_t get_ptc_value(void);



#endif 
