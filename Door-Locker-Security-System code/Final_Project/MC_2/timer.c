/******************************************************************************
 * Module: Timer
 *
 * File Name: Timer.c
 *
 * Description: Source file for the TIMER driver
 *
 * Author: Youssef Abdelaziz
 *******************************************************************************/
#include "timer.h"
#include "common_macros.h"
#include <avr/io.h>
#include <avr/interrupt.h>


/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr_timer0)(void) = NULL_PTR;
static volatile void (*g_callBackPtr_timer1)(void) = NULL_PTR;
static volatile void (*g_callBackPtr_timer2)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER0_OVF_vect)
{
	if(g_callBackPtr_timer0 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the interrupt is detected */
		(*g_callBackPtr_timer0)();
	}
}
ISR(TIMER0_COMP_vect)
{
	if(g_callBackPtr_timer0 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the interrupt is detected */
		(*g_callBackPtr_timer0)();
	}
}
ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr_timer1 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the interrupt is detected */
		(*g_callBackPtr_timer1)();
	}
}
ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr_timer1 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the interrupt is detected */
		(*g_callBackPtr_timer1)();
	}
}
ISR(TIMER2_OVF_vect)
{
	if(g_callBackPtr_timer2 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the interrupt is detected */
		(*g_callBackPtr_timer2)();
	}
}
ISR(TIMER2_COMP_vect)
{
	if(g_callBackPtr_timer2 != NULL_PTR)
	{
		/* Call the Call Back function in the application after the interrupt is detected */
		(*g_callBackPtr_timer2)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/* Function Name:TIMER_init
 * Description: initialize the selected timer driver in the AVR
 * 				according to the conifig-type structure
 */
void TIMER_init(const Timer_ConfigType * Config_Ptr)
{/*choosing timer 0*/
	if(Config_Ptr->timer_id==TIMER_0)
	{/*timer0 overflow mode*/
		if(Config_Ptr->mode==NORMAL)
		{
			TCNT0 = Config_Ptr->initialValue; /* Set Timer initial value */
			SET_BIT(TIMSK,TOIE0);// Enable Timer0 Overflow Interrupt
			/* Configure the timer control register
			 * 1. Non PWM mode FOC0=1
			 * 2. Normal Mode WGM01=0 & WGM00=0
			 * 3. Normal Mode COM00=0 & COM01=0
			 * 4. clock = F_CPU/selected_prescaler
			 */
			SET_BIT(TCCR0,FOC0);
			TCCR0 |=(TCCR0 & 0xF8)|(Config_Ptr->prescaler);
		}
		else if(Config_Ptr->mode==CTC)
		{/*timer0 compare mode*/
			TCNT0 = Config_Ptr->initialValue;  /* Set Initial Value */
			OCR0 = Config_Ptr->comparelValue; /* Set Compare Value*/
			SET_BIT(TIMSK,OCIE0); /* Enable Timer0 output compare match Interrupt */
			/* Configure timer control register
			 * 1. Non PWM mode FOC0=1
			 * 2. CTC Mode WGM01=1 & WGM00=0
			 * 3. selected compare output mode operation for  COM00 & COM01
			 * 4. clock = F_CPU/selected_prescaler
			 */
			SET_BIT(TCCR0,FOC0) ;
			SET_BIT(TCCR0,WGM01) ;
			TCCR0 |=(TCCR0 & 0xCF)|((Config_Ptr->CompareOutputMode)<<4);
			TCCR0 |=(TCCR0 & 0xF8)|(Config_Ptr->prescaler);
			if(Config_Ptr->CompareOutputMode!=NORMAL_OC_DISCONNECT)
			{
				SET_BIT(DDRB,PB3);/* Configure PB3/OC0 Pin as output pin*/
			}
		}
	}

	/*choosing timer1*/
	else if(Config_Ptr->timer_id==TIMER_1)
	{/*timer1 overflow mode*/
		if(Config_Ptr->mode==NORMAL)
		{
			TCNT1 = Config_Ptr->initialValue; /* Set Timer initial value */
			SET_BIT(TIMSK,TOIE1); /* Enable Timer1 Overflow Interrupt */
			/* Configure the timer control register
			 * 1. Non PWM mode FOC1A=1
			 * 2. Normal Mode WGM12=0 & WGM11=0 & WGM10=0
			 * 3. Normal Mode COM1A0=0 & COM1A1=0
			 * 4. clock = F_CPU/selected_prescaler using dynamic configuration
			 */
			SET_BIT(TCCR1A,FOC1A);
			TCCR1B |=(TCCR1B & 0xF8)|(Config_Ptr->prescaler);
		}
		else if(Config_Ptr->mode==CTC)
		{
			TCNT1 = Config_Ptr->initialValue;  /* Set Initial Value */
			OCR1A = Config_Ptr->comparelValue; /* Set Compare Value*/
			SET_BIT(TIMSK,OCIE1A); /* Enable Timer1 output compare match Interrupt */
			/* Configure timer control register
			 * 1. Non PWM mode FOC1A=1
			 * 2. CTC Mode WGM12=1 & WGM11=0 & WGM10=0
			 * 3. selected compare output mode operation for COM1A0 & COM1A1
			 * 4. clock = F_CPU/selected_prescaler using dynamic configuration
			 */
			SET_BIT(TCCR1A,FOC1A);
			SET_BIT(TCCR1B,WGM12);
			TCCR1A |=(TCCR1A & 0x3F)|((Config_Ptr->CompareOutputMode)<<6);
			TCCR1B |=(TCCR1B & 0xF8)|(Config_Ptr->prescaler);
			if(Config_Ptr->CompareOutputMode!=NORMAL_OC_DISCONNECT)
			{
				SET_BIT(DDRD,PD5);/* Configure PD5/OC1A Pin as output pin*/
			}
		}

	}
	/*choosing timer 2*/
	else if(Config_Ptr->timer_id==TIMER_2)
	{/*overflow mode */
		if(Config_Ptr->mode==NORMAL)
		{
			TCNT2 = Config_Ptr->initialValue; /* Set Timer initial value */
			SET_BIT(TIMSK,TOIE2); /* Enable Timer2 Overflow Interrupt */
			/* Configure the timer control register
			 * 1. Non PWM mode FOC2=1
			 * 2. Normal Mode WGM21=0 & WGM20=0
			 * 3. Normal Mode COM20=0 & COM21=0
			 * 4. clock = F_CPU/selected_prescaler
			 */
			SET_BIT(TCCR2,FOC2);
			TCCR2 |=(TCCR2 & 0xF8)|(Config_Ptr->prescaler2);
		}
		/*compare mode*/
		else if(Config_Ptr->mode==CTC)
		{
			SET_BIT(TIMSK,OCIE2); /* Enable Timer2 Overflow Interrupt */
			TCNT2 = Config_Ptr->initialValue;  /* Set Initial Value */
			OCR2 = Config_Ptr->comparelValue; /* Set Compare Value*/
			/* Configure timer control register
			 * 1. Non PWM mode FOC2=1
			 * 2. CTC Mode WGM21=1 & WGM20=0
			 * 3. selected compare output mode operation for  COM20 & COM21
			 * 4. clock = F_CPU/selected_prescaler
			 */
			SET_BIT(TCCR2,WGM21);
			SET_BIT(TCCR2,FOC2);
			TCCR2 |=(TCCR2 & 0xCF)|((Config_Ptr->CompareOutputMode)<<4);
			TCCR2 |=(TCCR2 & 0xF8)|(Config_Ptr->prescaler2);
			if(Config_Ptr->CompareOutputMode!=NORMAL_OC_DISCONNECT)
			{
				SET_BIT(DDRD,PD7);/* Configure PD7/OC2 Pin as output pin*/
			}
		}
	}
}
/* Function Name:TIMER0_setCallBack
 * Description:  Save the address of the Call back function in a global variable
 */
void TIMER0_setCallBack(void(*a_ptr0)(void))
{

	g_callBackPtr_timer0 = a_ptr0;
}

/* Function Name:TIMER1_setCallBack
 * Description:  Save the address of the Call back function in a global variable
 */
void TIMER1_setCallBack(void(*a_ptr1)(void))
{
	g_callBackPtr_timer1 = a_ptr1;
}
/* Function Name:TIMER2_setCallBack
 * Description:  Save the address of the Call back function in a global variable
 */
void TIMER2_setCallBack(void(*a_ptr2)(void))
{
		g_callBackPtr_timer2 = a_ptr2;
}

/* Function Name:TIMER_deinit
 * Description:  closing the selected timer driver by clearing the clock
 * */
void TIMER_deinit(const Timer_ConfigType * Config_Ptr)
{
	if(Config_Ptr->timer_id==TIMER_0)
	{/*closing timer0*/
		TCCR0 = (TCCR0 & 0xF8);
	}
	else if(Config_Ptr->timer_id==TIMER_1)
	{/*closing timer1*/
		TCCR1B = (TCCR1B & 0xF8);
	}
	else if(Config_Ptr->timer_id==TIMER_2)
	{/*closing timer2*/
		TCCR2 = (TCCR2 & 0xF8);
	}
}

