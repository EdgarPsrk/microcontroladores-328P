#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "pulse.h"

void pulse(void){
    int contador = 0;
    int contador1 = 0;

	
	// Define Timer sin prescaler (ver tabla 34 en pag.72)
	TCCR0B |= (1 << CS00);
	
	// Inicia contador
	TCNT0 = 0;
	
	while(1)
	{

		if (TCNT0 >= 200) 
		{
			contador ++;
			if (contador >= 95)	
			{
				PORTB ^= 1<<PB5;
				contador = 0;
			}
			TCNT0 = 0;
		}

        if (TCNT0 >= 200) 
		{
			contador ++;
			if (contador >= 10)	
			{
				PORTB ^= 0<<PB5;
				contador = 0;
			}
			TCNT0 = 0;
		}
	}
}	
