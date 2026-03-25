#include "bsp.h"

#if 0

char time_str[60];


char week_day[4], month[4];


int8_t day, hour, minute, second, year;

void net_parse_beijint_time(void)
{

  char *newline_pos = strstr(time_str, "\n");

  if (newline_pos) { 
    // 将字符串分割为两部分 
    *newline_pos = '\0'; 
    newline_pos += 2; 
    // 跳过 "\r\n" 
    //sscanf();//parse_time(newline_pos); 
    sscanf(newline_pos,"%13s %3s %3s %d %d:%d:%d %d", week_day, month, &day, &hour, &minute, &second, &year);
    } 
    else 
    {



    }

 // sscanf(time_str,"%29s %3s %3s %d %d:%d:%d %d", week_day, month, &day, &hour, &minute, &second, &year);
 

}
#endif 


#if 0


#define BUFFER_SIZE 256

uint8_t rxBuffer[BUFFER_SIZE];
uint8_t dataBuffer[BUFFER_SIZE];
uint8_t rxData;
int bufferIndex = 0;
int paramsFound = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        // 将接收的数据存入缓冲区
        rxBuffer[bufferIndex++] = rxData;
        
        // 检查缓冲区是否包含 "params"
        if (!paramsFound && bufferIndex >= 6 && strncmp((char *)&rxBuffer[bufferIndex - 6], "params", 6) == 0) {
            paramsFound = 1;
            bufferIndex = 0;
        }

        // 如果找到 "params"，将后续数据存入 dataBuffer
        if (paramsFound) {
            dataBuffer[bufferIndex++] = rxData;
        }

        // 再次启用接收中断
        HAL_UART_Receive_IT(&huart1, &rxData, 1);
    }
}





#endif 
