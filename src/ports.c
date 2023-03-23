#include "ports.h"
#include <avr/io.h>

void initPorts (void)
{
    // LED power on
    DDRB |= 1 << PB5;
}
//modificMOS EL TCCR0B CON LA PALABRA 