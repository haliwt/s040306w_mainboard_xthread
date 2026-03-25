#ifndef __BSP_TIME_H
#define  __BSP_TIME_H
#include "main.h"

//void works_times_handler(void);
#define  TEST_TWO_HOURS_UNIT     0

extern uint8_t counter_two_hours;
extern uint8_t twoHours_stop_flag;


void works_run_two_hours_state(void);



void getBeijingTime_cofirmLinkNetState_handler(void);




#endif 

