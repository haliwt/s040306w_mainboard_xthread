#include "bsp.h"


void ptc_turn_off(void)
{
    PTC_SetLow(); //ptc off;
}
void ptc_turn_on(void)
{
    PTC_SetHigh(); //ptc on

}









