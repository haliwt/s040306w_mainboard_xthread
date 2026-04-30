#include "bsp.h"


_run_t gctl_t; 

 uint8_t first_open_set_temp_flag;
 uint8_t first_set_ptc_on ,set_temp_first;






uint8_t set_temp_first_closeptc  ;

/**********************************************************************
    *
    *Functin Name: void copy_cmd_notice_handler(void)
    *Function : 
    *Input Ref:  key of value
    *Return Ref: NO
    *
************************************************************************/
void ai_mode_display_fun(void)
{
	if(gctl_t.mode_ai_switch_flag == 1 && wifi_link_net_state()==1){
	gctl_t.mode_ai_switch_flag=0;
	if(gctl_t.gModel ==1){
	   MqttData_Publish_AitState(1);
	  tx_thread_sleep(20);

	}
	else if(gctl_t.gModel ==2){
	  MqttData_Publish_AitState(2);
	  tx_thread_sleep(20);
	
	}
	}

}





