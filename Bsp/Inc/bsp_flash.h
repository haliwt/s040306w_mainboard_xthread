#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H
#include "main.h"


void EEPROM_Write(uint16_t virtAddr, uint32_t data);



uint32_t read_flash_value(void);





#endif 

