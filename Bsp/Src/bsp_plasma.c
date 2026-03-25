#include "bsp.h"



void plasma_fun(uint8_t sel)
{
     if(sel ==0){
        PLASMA_SetHigh();

     }
     else{

        PLASMA_SetLow();
     }

}


