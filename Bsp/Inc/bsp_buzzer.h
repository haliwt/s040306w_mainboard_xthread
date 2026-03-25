#ifndef __BSP_BUZZER_H_
#define __BSP_BUZZER_H_
#include "main.h"

#define BUZZER_PIN      GPIO_PIN_1
#define BUZZER_GPIO     GPIOB


void buzzer_sound(void);

void tim3_buzzer_sound_config(void);


#endif 
