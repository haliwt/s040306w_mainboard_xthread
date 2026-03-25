#ifndef __BSP_USART_WIFI_H
#define __BSP_USART_WIFI_H
#include "main.h"

//void usart2_isr_callback_handler(uint8_t data);

void USART2_DMA_Send(uint8_t *txBuf,uint16_t txlen);

void usart2_rx_callback_invoke(uint8_t data);

//void callback_register_usart2_rx(void);




#endif 

