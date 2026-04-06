#ifndef __BSP_THREADX_H
#define __BSP_THREADX_H
#include "bsp.h"



void threadx_handler(void);



void display_board_xtask_notice(void);


void tx_wifi_flag_get_hander(void);
void tx_wifi_flag_put_hander(void);


void tx_disp_flag_put_hander(void);
void tx_disp_flag_get_hander(void);



#endif 

