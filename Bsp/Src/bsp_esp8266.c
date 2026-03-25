#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bsp.h"



ESP8266DATATypedef net_t;

 uint8_t dma_tx_done;


 uint8_t *sub_buf;

char *CloudInfo="+TCMQTTCONN:OK";
char *usart2_tx;

uint8_t usart2_flag;

/**
 *pdata: pointer of data for send
 *len:  len of data to be sent
 *return: the len of data send success
 * @brief hal api for at data send
 */
uint8_t at_send_data(const uint8_t* pdata, uint16_t len)
{
   #if 0
    uint8_t i ;
    //USART2_DAM_Send(pdata,len);
	//LL_USART_TransmitData8(USART2,&pdata);
	for ( i = 0; i < len; i++)
    {
        // 等待发送缓冲区为空
        while (!LL_USART_IsActiveFlag_TXE(USART2));
        
        // 发送一个字节
        LL_USART_TransmitData8(USART2, pdata[i]);
    }

    // 等待发送完成
    while (!LL_USART_IsActiveFlag_TC(USART2));
	return len;
   #else 
	if (pdata == NULL || len == 0)
        return 0;

    dma_tx_done = dma_false;
    USART2_DMA_Send((uint8_t *)pdata, len);

    // 等待 DMA 发送完成（可加超时机制）
    uint32_t timeout = 100000;
    while (!dma_tx_done && --timeout);
    return (timeout == 0) ? 0 : len;

   #endif 
}


/****************************************************************************************************
**
*Function Name:static void initBtleModule(void)
*Function: 
*Input Ref: 
*Return Ref:NO
*
****************************************************************************************************/
void InitWifiModule(void)
{
	
	if(gctl_t.wifi_config_net_lable==0){
		 gctl_t.wifi_config_net_lable++;
			
			//WIFI_IC_ENABLE();
	
	
			//at_send_data("AT+RST\r\n", strlen("AT+RST\r\n"));

	     USART2_DMA_Send((uint8_t*)"AT+RST\r\n", strlen("AT+RST\r\n"));
		 tx_thread_sleep(1000);//HAL_Delay(1000);
	}
		
}

void InitWifiModule_Hardware(void)
{
	//WIFI_IC_DISABLE();
	//HAL_Delay(1000);
	//WIFI_IC_ENABLE();
	at_send_data((const uint8_t *)"AT+RESTORE\r\n", strlen("AT+RESTORE\r\n"));
	tx_thread_sleep(1000);//HAL_Delay(1000);
	tx_thread_sleep(1000);
	tx_thread_sleep(1000);
			//at_send_data("AT+RESTORE\r\n", strlen("AT+RESTORE\r\n"));
		
}

void ReConnect_Wifi_Net_ATReset_Hardware(void)
{
	   // WIFI_IC_DISABLE();
        //tx_thread_sleep(1000);
        //tx_thread_sleep(1000);
		///HAL_Delay(1000);
		//HAL_Delay(1000);
		//HAL_Delay(1000);
		//WIFI_IC_ENABLE();
		//at_send_data("AT+RESTORE\r\n", strlen("AT+RESTORE\r\n"));
		at_send_data((const uint8_t *)"AT+RST\r\n", strlen("AT+RST\r\n"));
		 tx_thread_sleep(3000);//HAL_Delay(1000);
       // HAL_Delay(1000);
		//tx_thread_sleep(1000);
        ///tx_thread_sleep(1000);



}



/****************************************************************************************************
	**
	*Function Name:void Wifi_SoftAP_Config_Handler(void)
	*Function: tensent cloud configuration softAP to connector WIFI
	*Input Ref: 
	*Return Ref:NO
	*
****************************************************************************************************/
void Wifi_SoftAP_Config_Handler(void)
{
   #if 0
    // uint8_t *device_massage;
    
    static uint8_t device_massage=[128];
   // device_massage = (uint8_t *)malloc(128);


   switch (gctl_t.wifi_config_net_lable)
  {

    case wifi_set_restor:
           //InitWifiModule();
           ReConnect_Wifi_Net_ATReset_Hardware();//InitWifiModule_Hardware();
		    tx_thread_sleep(pdMS_TO_TICKS(1000));
           gctl_t.wifi_config_net_lable =wifi_set_cwmode;
	break;


	 case wifi_set_cwmode:
    	   // WIFI_IC_ENABLE();
         	//HAL_UART_Transmit(&huart2, "AT+CWMODE=3\r\n", strlen("AT+CWMODE=3\r\n"), 5000);
         	//at_send_data("AT+CWMODE=3\r\n", strlen("AT+CWMODE=3\r\n"));
		   USART2_DMA_Send((uint8_t*)"AT+CWMODE=3\r\n", strlen("AT+CWMODE=3\r\n"));
        	 tx_thread_sleep(pdMS_TO_TICKS(1000));//HAL_Delay(1000);
           
			gctl_t.wifi_config_net_lable =wifi_set_softap;
			gctl_t.randomName[0]=HAL_GetUIDw0();
		

	 break;

	  case wifi_set_softap:
           // WIFI_IC_ENABLE();
			
            sprintf((char *)device_massage, "AT+TCPRDINFOSET=1,\"%s\",\"%s\",\"UYIJIA01-%d\"\r\n", PRODUCT_ID, DEVICE_SECRET,gctl_t.randomName[0]);
			at_send_data(device_massage, strlen((const char *)device_massage));
	  		 tx_thread_sleep(pdMS_TO_TICKS(1000));//HAL_Delay(1000);
        
           
			tx_thread_sleep(1000);
            tx_thread_sleep(1000);
            tx_thread_sleep(1000);
            tx_thread_sleep(1000);
         
	        
			gctl_t.wifi_config_net_lable=wifi_set_tcdevreg;
		
	


	 case wifi_set_tcdevreg://dynamic register
		// HAL_UART_Transmit(&huart2, "AT+TCDEVREG\r\n", strlen("AT+TCDEVREG\r\n"), 0xffff); //动�?�注�? 
		  at_send_data("AT+TCDEVREG\r\n", strlen("AT+TCDEVREG\r\n"));
	       tx_thread_sleep(pdMS_TO_TICKS(1000));//HAL_Delay(1000);
       tx_thread_sleep(1000);
        tx_thread_sleep(1000);

	  
	     gctl_t.wifi_config_net_lable=wifi_set_tcsap;

	 break;


	 case wifi_set_tcsap:
	 
 
			tx_thread_sleep(1000);
			tx_thread_sleep(1000);
			tx_thread_sleep(1000);
			tx_thread_sleep(1000);
			tx_thread_sleep(1000);
			tx_thread_sleep(1000);
		  
         
            net_t.linking_tencent_cloud_doing =1;
            wifi_t.soft_ap_config_flag =1; //WE.EIDT 
	        sprintf((char *)device_massage, "AT+TCSAP=\"UYIJIA01-%d\"\r\n",gctl_t.randomName[0]);
            at_send_data(device_massage, strlen((const char *)device_massage));
		
            tx_thread_sleep(pdMS_TO_TICKS(2000));

			tx_thread_sleep(pdMS_TO_TICKS(2000));
         
         
			gctl_t.wifi_config_net_lable=0xff;//
		
	break;
          
	case wifi_inquire_register_codes: //0x06//WT.EDIT 2024.07.22
       wifi_t.gTimer_get_beijing_time=0;

         tx_thread_sleep(1000);
         tx_thread_sleep(1000);
           tx_thread_sleep(1000);
       
	   if(net_t.soft_ap_config_success==0){
         
		// net_t.wifi_uart_counter=0;
  //      HAL_UART_Transmit(&huart2, "AT+TCPRDINFOSET?\r\n", strlen("AT+TCPRDINFOSET?\r\n"), 0xffff); //�?
        at_send_data("AT+TCPRDINFOSET?\r\n", strlen("AT+TCPRDINFOSET?\r\n"));
		tx_thread_sleep(pdMS_TO_TICKS(1000));//HAL_Delay(1000);
        gctl_t.wifi_config_net_lable=0xff;//

        }

	 }

  free(device_massage);
   #endif 

   
	static uint8_t device_massage[128];  // 静态分配，避免 malloc/free
	
		switch (gctl_t.wifi_config_net_lable)
		{
			case wifi_set_restor:
				ReConnect_Wifi_Net_ATReset_Hardware();
				tx_thread_sleep(1000);
				gctl_t.wifi_config_net_lable = wifi_set_cwmode;
				break;
	
			case wifi_set_cwmode:
				USART2_DMA_Send((uint8_t *)"AT+CWMODE=3\r\n", strlen("AT+CWMODE=3\r\n"));
				tx_thread_sleep(1000);
				gctl_t.wifi_config_net_lable = wifi_set_softap;
				gctl_t.randomName[0] = HAL_GetUIDw0();
				break;
	
			case wifi_set_softap:
				sprintf((char *)device_massage,
						"AT+TCPRDINFOSET=1,\"%s\",\"%s\",\"UYIJIA01-%d\"\r\n",
						PRODUCT_ID, DEVICE_SECRET, gctl_t.randomName[0]);
				USART2_DMA_Send(device_massage, strlen((const char *)device_massage));
				tx_thread_sleep(1000);
				tx_thread_sleep(4000);
				gctl_t.wifi_config_net_lable = wifi_set_tcdevreg;
				break;
	
			case wifi_set_tcdevreg:
				USART2_DMA_Send((uint8_t *)"AT+TCDEVREG\r\n", strlen("AT+TCDEVREG\r\n"));
				tx_thread_sleep(1000);
				tx_thread_sleep(2000);
				gctl_t.wifi_config_net_lable = wifi_set_tcsap;
				break;
	
			case wifi_set_tcsap:
				tx_thread_sleep(6000);
				net_t.linking_tencent_cloud_doing = 1;
				wifi_t.soft_ap_config_flag = 1;
				sprintf((char *)device_massage, "AT+TCSAP=\"UYIJIA01-%d\"\r\n", gctl_t.randomName[0]);
				USART2_DMA_Send(device_massage, strlen((const char *)device_massage));
				tx_thread_sleep(4000);
				gctl_t.wifi_config_net_lable = 0xff;
				break;
	
			case wifi_inquire_register_codes:
				wifi_t.gTimer_get_beijing_time = 0;
				tx_thread_sleep(3000);
				if (net_t.soft_ap_config_success == 0)
				{
					USART2_DMA_Send((uint8_t *)"AT+TCPRDINFOSET?\r\n", strlen("AT+TCPRDINFOSET?\r\n"));
					tx_thread_sleep(1000);
					gctl_t.wifi_config_net_lable = 0xff;
				}
				break;
		
	}
}

/****************************************************************************************************
**
*Function Name:void Wifi_Link_SmartPhone_Fun(void)
*Function: dy
*Input Ref: 
*Return Ref:NO
*
****************************************************************************************************/
void SmartPhone_LinkTencent_Cloud(void)
{
   
    if(net_t.soft_ap_config_success==1){

       net_t.soft_ap_config_success=0;
	 //  HAL_UART_Transmit(&huart2, "AT+TCMQTTCONN=1,5000,240,0,1\r\n", strlen("AT+TCMQTTCONN=1,5000,240,0,1\r\n"), 5000);//�?始连�?
         at_send_data((const uint8_t *)"AT+TCMQTTCONN=1,5000,240,0,1\r\n", strlen("AT+TCMQTTCONN=1,5000,240,0,1\r\n"));
	     tx_thread_sleep(1000);//HAL_Delay(1000);
         tx_thread_sleep(1000);//HAL_Delay(1000);
       //HAL_Delay(1000);
       ///HAL_Delay(1000);
       
	 
     }
	
 	// free(device_submassage);

}



void wifi_Disconnect_Fun(void)
{

//  HAL_UART_Transmit(&huart2, "AT+TCMQTTDISCONN\r\n", strlen("AT+TCMQTTDISCONN\r\n"), 5000);//�?始连�?
    at_send_data((const uint8_t *)"AT+TCMQTTDISCONN\r\n", strlen("AT+TCMQTTDISCONN\r\n"));
}

void Get_BeiJing_Time_Cmd(void)
{

 // HAL_UART_Transmit(&huart2, "AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"\r\n", strlen("AT+CIPSNTPCFG=1,800,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"\r\n"), 5000);//�?始连�?
 // at_send_data("AT+CIPSNTPCFG=1,8,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"\r\n", strlen("AT+CIPSNTPCFG=1,800,\"cn.ntp.org.cn\",\"ntp.sjtu.edu.cn\"\r\n"));
//   HAL_UART_Transmit(&huart2, "AT+CIPSNTPCFG=1,8\r\n", strlen("AT+CIPSNTPCFG=1,8\r\n"), 0xffff);//�?
	at_send_data((const uint8_t *)"AT+CIPSNTPCFG=1,8\r\n", strlen("AT+CIPSNTPCFG=1,8\r\n"));

}

void Get_Beijing_Time(void)
{

//   HAL_UART_Transmit(&huart2, "AT+CIPSNTPTIME?\r\n", strlen("AT+CIPSNTPTIME?\r\n"), 0xffff);//�?始连�?
     at_send_data((const uint8_t *)"AT+CIPSNTPTIME?\r\n", strlen("AT+CIPSNTPTIME?\r\n"));


}

void PowerOn_Self_Auto_Link_Tencent_Cloud(void)
{

	 uint8_t *device_massage,auto_link_cloud_flag=0;
    

    device_massage = (uint8_t *)malloc(128);


   switch (auto_link_cloud_flag)
  {

    case 0:
           InitWifiModule_Hardware();//InitWifiModule();
           
		    tx_thread_sleep(1000);//HAL_Delay(1000);
		   
           auto_link_cloud_flag =wifi_set_cwmode;
	break;


	 case wifi_set_cwmode:
    	   // WIFI_IC_ENABLE();
//         	HAL_UART_Transmit(&huart2, "AT+CWMODE=3\r\n", strlen("AT+CWMODE=3\r\n"), 5000);
           at_send_data((const uint8_t *)"AT+CWMODE=3\r\n", strlen("AT+CWMODE=3\r\n"));
           tx_thread_sleep(1000);// HAL_Delay(1000);
           tx_thread_sleep(1000);
		
			gctl_t.randomName[0]=HAL_GetUIDw0();
		

	 break;
	 default:
        
	 break;

}
  free(device_massage);

}

void SmartPhone_TryToLink_TencentCloud(void)
{
    net_t.linking_tencent_cloud_doing =1; //enable usart2 receive wifi  data
    gpro_t.wifi_rx_data_counter=0;
	wifi_t.soft_ap_config_flag =0;
//    HAL_UART_Transmit(&huart2, "AT+TCMQTTCONN=1,5000,240,0,1\r\n", strlen("AT+TCMQTTCONN=1,5000,240,0,1\r\n"), 0xffff);//�?始连�?
    at_send_data((const uint8_t *)"AT+TCMQTTCONN=1,5000,240,0,1\r\n", strlen("AT+TCMQTTCONN=1,5000,240,0,1\r\n"));
	
	tx_thread_sleep(1000);
	tx_thread_sleep(1000);
	tx_thread_sleep(1000);
	   
}


void Reconnection_Wifi_Order(void)
{
//	HAL_UART_Transmit(&huart2, "AT+TCMQTTSTATE?\r\n", strlen("AT+TCMQTTSTATE?\r\n"), 0xffff);
    at_send_data((const uint8_t *)"AT+TCMQTTSTATE?\r\n", strlen("AT+TCMQTTSTATE?\r\n"));
    tx_thread_sleep(2000);//HAL_Delay(200);
    
  
}









