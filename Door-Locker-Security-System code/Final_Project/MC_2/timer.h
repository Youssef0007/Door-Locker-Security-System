/******************************************************************************
 * Module: Timer
 *
 * File Name: Timer.h
 *
 * Description: Source file for the TIMER driver
 *
 * Author: Youssef Abdelaziz
 *******************************************************************************/

#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"


/*******************************************************************************
 *                      Types Declaration                                    *
 *******************************************************************************/
typedef enum
{
	NORMAL,CTC
}Timer_Mode;
typedef enum
{
	NOCLOCK,PRESCALER_1,PRESCALER_8,PRESCALER_64,PRESCALER_256,PRESCALER_1024,EXTERNAL_FALLING,EXTERNAL_RISING
}Timer0_1_Prescaler;

typedef enum
{
	NOCLOCK2,NO_PRESCALER2,PRESCALER2_8,PRESCALER2_32,PRESCALER2_64,PRESCALER2_128,PRESCALER2_256,PRESCALER2_1024
}Timer2_Prescaler;
typedef enum
{
	TIMER_0,TIMER_1,TIMER_2
}Timer_ID;
typedef enum
{
	NORMAL_OC_DISCONNECT,TOGGLE_OC,CLEAR_OC,SET_OC
}Timer_CompareMatchOutputMode;


typedef struct
{
	Timer_Mode     	    mode;
	Timer0_1_Prescaler 	prescaler;
	Timer2_Prescaler 	prescaler2;
	Timer_ID        	timer_id;
	uint32           	initialValue;
	uint32           	comparelValue;
	Timer_CompareMatchOutputMode CompareOutputMode;
}Timer_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/* Function Name:TIMER_init
 * Description: initialize the selected timer driver in the AVR
 * 				according to the conifig-type structure
 */
void TIMER_init(const Timer_ConfigType * Config_Ptr);

/* Function Name:TIMER0_setCallBack
 * Description:  Save the address of the Call back function in a global variable
 */

void TIMER0_setCallBack(void(*a_ptr0)(void));

/* Function Name:TIMER1_setCallBack
 * Description:  Save the address of the Call back function in a global variable
 */
void TIMER1_setCallBack(void(*a_ptr1)(void));

/* Function Name:TIMER2_setCallBack
 * Description:  Save the address of the Call back function in a global variable
 */
void TIMER2_setCallBack(void(*a_ptr2)(void));

/* Function Name:TIMER_deinit
 * Description:  closing the selected timer driver by clearing the clock
 * */
void TIMER_deinit();

#endif /* TIMER_H_ */
