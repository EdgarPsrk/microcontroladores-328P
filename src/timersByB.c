/*
 * Timers.c
 *
 * Created: 17/03/2016 08:16:05 a. m.
 *  Author: JLB
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/****************************************************************
* timer normal mode freq=16MHz. Set compiler symbol:			*
* Project -> Properties -> AVR/GNU C compiler Symbols to 16Mhz.	*                     
*****************************************************************/
void timer0_normalmode_noprescaler(void)
{
	int contador = 0; //counting variable
	TCCR0B |= (1 << CS00); //timer no prescaler
	TCNT0 = 0; // start count from 0
	while(1)
	{
		/****************************************************************
		* fosc = 1/16,000,000 = 0.0625 us								*
		* counter_value = (delay/pulse_width)-1									*
		* example: count = (2ms/0.0625us)-1 = 31,999					*
		* because TCNT0 is 8 bits, divide again:31,999/200 = 159.995	*
		* ~ 160, means we obtain 2ms with a delay of (200 x 160) times.	*
		*****************************************************************/
		if (TCNT0 >= 200) 
		{
			contador ++;
			if (contador >= 160)	
			{
				PORTB ^= 1<<PB5; // PB5 --> output (ports.c)			
				contador = 0;
			}
			TCNT0 = 0;
		}
	}
}
void timer0_normalmode_prescaler(void)
{	
	TCCR0B |= (1 << CS00) | (1 << CS02); //prescaler = 1024
	TCNT0 = 0; // start count from 0
	
	while(1)
	{
		/************************************************************************
		* with prescaler: clock_cycle = ( 0.0625 us/1024) = 0.06103515625 ms
		* counter_value = (delay/pulse_width)-1
		* example: counter_value = (2ms/0.06103515625 ms)-1 = 31.768 ~ 31
		* means we obtain 2ms with a delay of (31) times.                                                             
		*************************************************************************/
		if (TCNT0 >= 31)
		{
				PORTB ^= 1<<PB5; // PB5 --> output (ports.c)	
				TCNT0 = 0;
		}
	}
}	
void timer1_normalmode_noprescaler(void) //*Note: timer1 is 16 bits
{
	TCCR1B |= 1<<CS20; //timer1 no prescaler
	TCNT1 = 0; //start count from 0
	while (1)
	{
		/************************************************************************
		* De acuerdo con la formula:
		* Valor_del_contador = (Retardo_requerido/Periodo_del_reloj)-1
		* Ejemplo: Valor_del_contador= (2ms/0.0625us)-1 = 31999                                                                
		*************************************************************************/	
		if (TCNT1 >= 31999)	
		{
				PORTB ^= 1<<PB5; //PB5 --> output (ports.c)	
				TCNT1 = 0;
		}
	}
}	
/* Interrupt Service Routine */
volatile uint8_t tot_overflow; // global variable to store count (must be volatile)
ISR(TIMER0_OVF_vect) // goes in when timer0 overflows (TCNT0=255)
{
	/*********************************************************************************
	* T = 1 / 16,000,000 Hz = 0.0625 us, then it takes = 255 x 0.0625 us = 15.9375 us*
	* to reach its maximum count.													 *
	* Example: for pulse width 2ms => tot_ovf_time = 2ms / 15.9375 us = 125.49 ~ 125 *
	* timer must count 125 times                                                     *
	**********************************************************************************/
	tot_overflow++;
	if (tot_overflow >=125)
	{
		PORTB ^= (1<<PB5); //output on PB5 (see ports.c)
		tot_overflow = 0;
	}
}
/* end of ISR */
void timer0_overflow_interrupt(void)
{
	TCCR0B |= (1 << CS00); //Start timer without pre-scaler
	TCNT0 =0; // Start count=0
	TIMSK0 |= 1<<TOIE0; //Overflow interrupt enable
	sei(); // Global interrupts enable
}

/************************
* timer in CTC mode		*
*************************/
void timer2_CTCmode(void)
{
	TCCR2A |= 1 << WGM21; // timer2 in CTC mode (table 18.8)
	/************************************************************************
	* example: fosc = 16 Mhz, T = 2ms -> focr = 1/(2x2ms) = 250 Hz			*
	* (precaler) N=(fosc/(1/T)x256) = (16,000,000)/(500x256) = 125 ~128		*
	* from (pag.156) formula: OCR2A = (fosc/(2xNxfocr))-1					*
	* OCR2A = (16,000,000/2x(128)x(250))-1 = 249							*
	* with this value pulse witdth = 2 ms									*
	************************************************************************/
	OCR2A = 249;								
	TCCR2B |= (1 << CS20) | (1 << CS22); //set prescaler = 128
	while (1)
	{
		if (TIFR2 & (1 << OCF2A)) //if OCF2A = 1 (timercounter2 = OCR2A)
		{
			PORTB ^= 1 << PB5; //PB5 --> output (ports.c)
			TIFR2 =  (1 << OCF2A); //TIFR2 flag back to 1
		}
	}
}
/* Interrupt Service Routine */
ISR(TIMER1_COMPA_vect)
{
	PORTB ^= 1<<PB5; //PB5 --> output (ports.c)
}
/* end of ISR */
void timer1_CTCmode_interrupt(void)
{
	TCCR1B |= 1<<WGM12; // timer2 in CTC mode (table 18.8)
	/************************************************************************************
	* example: fosc = 16 Mhz, T = 0.5ms -> f = 1/(2x0.5ms) = 1 KHz						*
	* (precaler factor) N=(fosc/(1/T)x65,536) = (16,000,000)/(2000x65,536) = 0.122125 ~1*
	* from (pag.132) formula: OCR1A = (fosc/(2xNxfocr))-1								*
	* OCR1A = (16,000,000/2x(1)x(1000))-1 = 7999											*
	* with this value pulse witdth = 0.5 ms												*
	************************************************************************************/
	OCR1A |= 7999;
	TCCR1B |= 1<<CS10;		//set prescaler = 1 	
	TCNT1 = 0;				//Set counting to 0
	TIMSK1 |= 1<<OCIE1A;	//Enable interrupts on compare
	sei();					//Enable  global interrupts
}

void timer1_CTCmode_nonPWM(void)
{
	TCCR1B |= 1<<WGM12; // timer2 in CTC mode (table 18.8)
	/************************************************************************************
	* example: fosc = 16 Mhz, T = 0.125ms -> f = 1/(2x0.125ms) = 4 KHz					*
	* (precaler factor) N=(fosc/(1/T)x65,536) = (16,000,000)/(8000x65,536) = 0.030517 ~1*
	* from (pag.132) formula: OCR2A = (fosc/(2xNxfocr))-1								*
	* OCR2A = (16,000,000/2x(1)x(4000))-1 = 1,999										*
	* with this value pulse witdth = 0.125 ms												*
	************************************************************************************/
	OCR1A |= 1999;
	TCCR1B |= 1<<CS10;		//set prescaler = 1
	TCCR1A |= 1 << COM1A0;	//OC1A toggle (table 16.1)
	TCNT1 = 0;				//Set counting to 0
}

