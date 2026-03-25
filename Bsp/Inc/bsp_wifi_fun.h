#ifndef __BSP_WIFI_FUN_
#define __BSP_WIFI_FUN_
#include "main.h"


typedef enum{
  
   wifi_AI=0x08 ,wifi_notAI=0x18,wifi_kill=0x04,wifi_notkill=0x14,
   wifi_heat= 0x02,wifi_notheat=0x12


}wifi_mode_t;

typedef enum _wifi_state_t{  
	
    wifi_link_start =0x01,
	wifi_link_tencent_cloud, //2
	wifi_tencent_publish_init_data, //3
	wifi_tencent_subscription_data, //4
	wifi_publish_update_tencent_cloud_data,//5
	wifi_tencent_publish_dht11_data,
	wifi_get_beijing_time,
	wifi_disconnect,
	wifi_null


}wifi_state_t;


typedef struct _WIFI_FUN{
	
    uint8_t runCommand_order_lable;

    uint8_t soft_ap_config_flag;
    uint8_t get_rx_beijing_time_enable;
	
	

	
	uint8_t tencent_cloud_command_power_on;
    uint8_t once_rx_data_done;


    
	uint8_t restart_link_tencent_cloud ;
	uint8_t received_data_from_tencent_cloud ;

    uint8_t gTimer_auto_detected_net_state_times ;
  

    uint8_t gTimer_auto_link_net_time;


    
    uint8_t gTimer_get_beijing_time;
    
	
		
	
}WIFI_FUN;

extern WIFI_FUN   wifi_t;

extern void (*PowerOn)(void); //函数指针
extern void (*PowerOff)(void);
extern void (*Ai_Fun)(uint8_t sig);
extern void (*SetTimes)(void);
extern void (*SetTemperature)(void);


void PowerOn_Host(void (*poweronHandler)(void));
void PowerOff_Host(void (*poweroffHandler)(void));













#endif 

