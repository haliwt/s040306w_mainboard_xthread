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




/****************************************************************************************************
**
*Function Name:void Wifi_Link_SmartPhone_Fun(void)
*Function: dy
*Input Ref: 
*Return Ref:NO
*
****************************************************************************************************/

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









