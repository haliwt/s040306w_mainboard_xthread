#ifndef __BSP_CTL_H__
#define __BSP_CTL_H__
#include "main.h"

#define WIFI_TUYA      1  //
#define AI_ENABLE      1


#define  TEST_UNIT     0


typedef enum {                   /* enumeration for CProcess signals */
    kill =1,notkill,dry,notdry,ai,notai
}Signal;

typedef enum
{
  AIENABLE = 0,
  AIDISABLE = !AIENABLE
} AiStatus;

typedef enum
{
  WIFIENABLE =1,
  WIFIDISABLE = !WIFIENABLE

}WifiStatus;

typedef enum{

  // POWER_ON =0x01,
  // POWER_OFF,
   RUN_COMMAND,
   POWER_CONNECTOR_WIFI,
   UPDATE_TO_PANEL_DATA,
   WIFI_RESTART_INIT,
   FAN_CONTINUCE_RUN_ONE_MINUTE,
   POWER_ON_FAN_CONTINUCE_RUN_ONE_MINUTE,
  
   POWER_NULL, //9
   KEY_NULL

 }run_state_t;

typedef enum{

	 DRY_ON = 0x40,
	 DRY_OFF = 0x41,

	 DRY_ON_NO_BUZZER = 0x90,
	 DRY_OFF_NO_BUZZER = 0x91,

	 PLASMA_ON = 0x20,
	 PLASMA_OFF = 0x21,

	 FAN_ON =0x10,
	 FAN_OFF = 0x11,

	 MODE_AI = 0x04,
	 MODE_TIMER = 0x14,
	 MODE_AI_NO_BUZZER= 0x44,

	 WIFI_CONNECT_FAIL = 0x55,
     DISPLAY_PANNEL_CONNECT_WIFI_SUCCESS= 0xAA,





}works_t;



typedef struct _RUN_T{
    
	
    uint8_t gDht11_humidity ;
 uint8_t gDht11_temperature;
    uint8_t set_temperature_flag;
   
   uint8_t app_timer_power_on_flag ;

   uint8_t response_wifi_signal_label;
  uint8_t ptc_prohibit_on_flag ;
   uint8_t set_temp_first_closeptc ;

 //fan:
    
	 uint8_t  gFan;
    //ptc:
   
    // uint8_t  gDry;

	  uint8_t  gUlransonic;
      uint8_t  gModel;
	  uint8_t gPlasma;

  uint8_t  mode_ai_switch_flag;
    

	 
	  //wifi link ref
  uint8_t first_link_tencent_cloud_flag ;
	

	  
	   uint8_t  set_temperature_value;


	  uint8_t  set_wind_speed_value;
	  uint8_t auto_link_cloud_flag;
	  uint8_t reconnect_tencent_cloud_flag;
	  uint8_t rx_set_temp_flag;
	
 uint8_t  wind_speed_decade;
  uint8_t  wind_speed_unit;
     uint8_t wifi_config_net_lable;

    
	  //ADC
	  uint8_t ptc_warning;
	  uint8_t fan_warning;


	  
      //timer ref 
	  uint8_t gTimer_fan_adc_times;
	  uint8_t gTimer_ptc_adc_times;
      
	

	 uint8_t gTimer_senddata_panel;
	

   
	 uint8_t gTimer_linkTencentCounter;
	 
	
	
	
	 uint32_t randomName[1];
	

}_run_t;

extern _run_t gctl_t; 


//#define CProcessInit(me_) ((me_)->cmdCtr__ =0,(me_)->state__ = IDLE)
extern uint8_t ReceiveBuffer[1];



void ai_mode_display_fun(void);


#endif 


