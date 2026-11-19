#include "bsp.h"


/**
 * @brief 1. 硬件控制与安全防护函数
 * @note  建议将其挂在 50ms 或 100ms 的高频轮询周期中，确保安全响应灵敏
 */
void module_hardware_control(void)
{

    if(gpro_t.twoHoursStop_flag == true) return ;
	
    if (gpro_t.fan_warning_flag == 1 || gpro_t.ptc_warning == 1) {
        PTC_SetLow();
        PLASMA_SetLow();
       // ultrasonic_close();
        return; 
    }

        // ==========================================
	   // 业务 1：定时开机——本地 WiFi 模块串口命令下发
	   // ==========================================
	   if (gctl_t.app_timer_power_on_flag == 1) {

	     // 1. 准备参数（高效率，减少逻辑嵌套）
    uint8_t ptc_val   = (gpro_t.gPtc == 1 ) ? 0x01 : 0x00;
    uint8_t plasma_val = (gctl_t.gPlasma == 1) ? 0x01 : 0x00;
    uint8_t ultra_val  = (gctl_t.gUltrasonic == 1) ? 0x01 : 0x00; // 修正拼写
    

		   if (gpro_t.gPtc == 1 ){
		   	    gctl_t.ptc_prohibit_on_flag =0;
			   SendWifiData_To_Cmd(0x02, 0x01);
		   } 
           else {
			   gctl_t.ptc_prohibit_on_flag = 1;
			   SendWifiData_To_Cmd(0x02, 0x00);
		   }
		   tx_thread_sleep(2); 
	
		   SendWifiData_To_Cmd(0x03, plasma_val);
		   tx_thread_sleep(2);
	
		   SendWifiData_To_Cmd(0x04, ultra_val);
		   tx_thread_sleep(2);
	
		   gctl_t.app_timer_power_on_flag = 0; // 发送完立即清零
	   }

    // --- PTC 硬件驱动 ---

    if (gpro_t.gPtc == 1 && gctl_t.ptc_prohibit_on_flag == 0) {
        if (gpro_t.twoHoursStop_flag == false) {
            PTC_SetHigh();
        } else {
            PTC_SetLow();
        }
    } else {
        PTC_SetLow();
    }

    // --- 等离子硬件驱动 ---
    if (gctl_t.gPlasma == 1 ) {
        PLASMA_SetHigh();
    } else {
        PLASMA_SetLow();
    }

    // --- 超声波硬件驱动 ---
    if (gctl_t.gUltrasonic == 1) { // 修正拼写
        ultrasonic_open();
    } else {
        ultrasonic_close();
    }

   
}


/**
 * @brief 2. WiFi 本地串口与 MQTT 云端数据上报函数
 * @note  挂在 3s 一次的时间戳慢周期中执行
 */
void module_wifi_report_handler(void)
{

   static uint8_t temporary_variable =0xff;
   // 如果系统已经报警，清空业务标志，不进行正常上报（或者在此处增加上报报警错误码的逻辑）
    if (gpro_t.fan_warning_flag == 1 || gpro_t.ptc_warning == 1) {
        gctl_t.app_timer_power_on_flag = 0;
        return;
    }

    // 状态保持变量移到这里，它们只为“数据上报”服务
    static uint8_t ptc_default = 0xff;
    static uint8_t plasma_default = 0xff;
    static uint8_t ultrasonic_default = 0xff;
#if 0
    // ==========================================
    // 业务 1：定时开机——本地 WiFi 模块串口命令下发
    // ==========================================
    if (gctl_t.app_timer_power_on_flag == 1) {
        if (gpro_t.gPtc == 1 && gctl_t.ptc_prohibit_on_flag == 0) {
            SendWifiData_To_Cmd(0x02, 0x01);
        } else {
            gctl_t.ptc_prohibit_on_flag = 1;
            SendWifiData_To_Cmd(0x02, 0x00);
        }
        tx_thread_sleep(1); 

        SendWifiData_To_Cmd(0x03, gctl_t.gPlasma ? 0x01 : 0x00);
        tx_thread_sleep(1);

        SendWifiData_To_Cmd(0x04, gctl_t.gUltrasonic ? 0x01 : 0x00);
        tx_thread_sleep(1);

        gctl_t.app_timer_power_on_flag = 0; // 发送完立即清零
    }
#endif 
    // ==========================================
    // 业务 2：云端状态同步——MQTT 属性上报
    // ==========================================
    if (wifi_link_net_state() == 1) {
        
        // PTC 状态有变动则上报
        if (ptc_default != gpro_t.ptc_active_f) {
            ptc_default = gpro_t.ptc_active_f;
            MqttData_Publish_SetPtc(gpro_t.gPtc ? 0x01 : 0x00);
            //tx_thread_sleep(20); 
        }

        // 等离子状态有变动则上报
        if (plasma_default != gpro_t.plasma_switch_flag) {
            plasma_default = gpro_t.plasma_switch_flag;
            MqttData_Publish_SetPlasma(gctl_t.gPlasma ? 0x01 : 0x00);
            //tx_thread_sleep(20);
        }

        // 超声波状态有变动则上报
        if (ultrasonic_default != gpro_t.ultrasonic_switch_flag) {
            ultrasonic_default = gpro_t.ultrasonic_switch_flag;
            MqttData_Publish_SetUltrasonic(gctl_t.gUltrasonic ? 0x01 : 0x00);
           // tx_thread_sleep(20);
        }

        // 温度定时上报
        if (gpro_t.tx_wifi_temperature_f == 1) {
            gpro_t.tx_wifi_temperature_f++;

		    if(temporary_variable != gctl_t.set_temperature_value){
			   temporary_variable = gctl_t.set_temperature_value;
		       MqttData_Publis_SetTemp(gctl_t.set_temperature_value);
            }
           
        }
    }
}


