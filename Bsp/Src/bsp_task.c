#include "bsp.h"



#define DEBUG_ENABLE      0

static void power_run_handler(void);
static void wifi_run_handler(void);


/**
 * @brief  :  static void vTaskStart(void *pvParameters
 * @note    
 * @param   None
 * @retval  None
 */

void task_handler(void)	
{

//		 // 或者直接调用解码器
//	  if(gpro_t.decoder_success_flag==1){
//		  gpro_t.decoder_success_flag =0;
//		  decoder_handler();
		 
//	  }
//	  else{

	  

	     power_run_handler();
         if(gpro_t.time_20ms_f ==1){
		 	gpro_t.time_20ms_f=0;
            wifi_run_handler();
          }
        
       
        // LL_IWDG_ReloadCounter(IWDG);
		 
  }

 
 
/**********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
**********************************************************************************************************/
uint8_t power_on_sound_flag=0;

static void power_run_handler(void)
{
 
     if(power_on_sound_flag==0){
            power_on_sound_flag ++;
            FAN_Stop();  //WT.EDIT.2025.01.03
            buzzer_sound_once();//buzzer_sound();//buzzer_sound();
            read_sensorData();
		

     }
	 else{


	switch(gpro_t.gpower_on){ 

            case power_on:
		 
			
            power_on_handler();
          

		
			
		  break;

		  

          case power_off:
		  
          
		      power_off_handler();
             break;
          }

	 	}
}

/********************************************************************************
	**
	*Function Name:static void wifi_run_handler(void)
	*Function :
	*Input Ref: 
	*Return Ref:NO
	*
*******************************************************************************/
uint8_t wifi_time_slot;
static void wifi_run_handler(void)
{
      // static uint8_t time_slot =0;
		  
		  if(gpro_t.wifi_led_fast_blink_flag==0 ){

		     switch(wifi_time_slot){

			   case 0:

		         wifi_communication_tnecent_handler();//

		      break;

			  case 1:
        
             getBeijingTime_cofirmLinkNetState_handler();

			  break;

			  case 2:
	
             wifi_auto_detected_link_state();

			  break;

			  default:
			  break;



		     }
		
           }

		  wifi_time_slot ++;
		  if(wifi_time_slot > 2) wifi_time_slot = 0;

}




