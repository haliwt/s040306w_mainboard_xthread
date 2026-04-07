#ifndef __BSP_THREADX_H
#define __BSP_THREADX_H
#include "bsp.h"



void threadx_handler(void);



void display_board_xtask_notice(void);


extern volatile uint8_t tx_error_flag;



#endif 

