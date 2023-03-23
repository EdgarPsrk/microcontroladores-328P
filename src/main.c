#include <avr/io.h>
#include "ports.h"
#include "pulse.h"
#include "timer.h"

int main(void)
{
    initPorts();
    pulse();
    //timer0_NormalMode_NoPrescaler_16Mhz();
    while (1)
    {
        
    }
}