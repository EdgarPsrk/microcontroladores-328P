#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "timer.h"

/************************************************************************
* USO DEL TIMER EN MODO NORMAL   con frecuencia de 1MHz
* Ajustar valor en: Project -> Properties -> AVR/GNU C compiler Symbols
* a 1Mhz. Para simular en Proteus, seguir el proceso descrito en el
* tutorial disponible en https://fueradelaulafesc.com/                                                                
************************************************************************/

void timer0_NormalMode_NoPrescaler_16Mhz(void)
{
	int contador = 0;
	//Usar PB0 para observar la salida (PB0 --> salida en IO_PORTs.c)
	
	// Define Timer sin prescaler (ver tabla 34 en pag.72)
	TCCR0B |= (1 << CS00);
	
	// Inicia contador
	TCNT0 = 0;
	
	while(1)
	{

		if (TCNT0 >= 200) 
		{
			contador ++;
			if (contador >= 10)	
			{
				PORTB ^= 1<<PB5;
				contador = 0;
			}
			TCNT0 = 0;
		}
	}
}	

// void Timer0_InterruptMode(void)
// {
// 	//Usar PB0 para observar la salida (PB0 --> salida en IO_PORTs.c)
// 	// Define Timer sin prescaler (ver tabla 34 en pag.72)
// 	TCCR0B |= (1 << CS00);
// 	// Inicia contador
// 	TCNT0 =0;
// 	//Habilita interrupciones
// 	TIMSK0 |= 1<<TOIE0;
// 	// Habilita interrupciones globales
// 	sei();
//     //SIEMPRE DEBEMOS DE HABILITAR LAS INTERRUPCIONES GENERALES PARA ES SEI
// }