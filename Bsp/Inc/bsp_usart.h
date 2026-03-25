#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "main.h"

#define UART1_RX_BUF_SIZE 64

//static TX_SEMAPHORE uart1_rx_semaphore;
volatile extern uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
volatile extern uint16_t uart1_rx_head ;
volatile extern uint16_t uart1_rx_tail ;

extern uint8_t rx1_data;


typedef enum{

   COPY_OK,
   COPY_NG,

}copy_cmd_t;

void parse_recieve_data_handler(void);







void callback_register_usart1_rx(void);

void decoder_handler(void);

void usart1_isr_callback_handler(uint8_t data);


#endif 

