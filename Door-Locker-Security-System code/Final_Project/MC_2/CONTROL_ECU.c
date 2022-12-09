/******************************************************************************
 *
 * File Name: CONTROL_ECU.c
 *
 * Description: the micro-controller that give order according to the current state
 * 				given by user through the HMI_ECU
 *
 * Author: Youssef Abdelaziz
 *
 *******************************************************************************/

#include "CONTROL_ECU.h"
#include<util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
/******************************************************************************
 *								DRIVERS MODULES                               *
 *******************************************************************************/
#include"dc_motor.h"
#include"buzzer.h"
#include"twi.h"
#include"uart.h"
#include"external_eeprom.h"
#include"timer.h"
/******************************************************************************
 *								GLOBAL VARIABLES                              *
 *******************************************************************************/

uint8 g_receivedChoice;/*variable to get the received key from HMI_ECU*/
uint8 g_state;/*variable to monitor the current state in the CONTROL_ECU*/
uint8 g_recivedIntialPassword[PASS_LENGTH];/*array to save the first password*/
uint8 g_recivedReenterdPassword[PASS_LENGTH];/*array to save the second password*/
uint8 g_Password[PASS_LENGTH];/*array to save and read the password to and from the EEPROM*/
uint8 mismatchCounter=0;/*variable to count the times of false enters*/
uint8 g_flag_motor=1;/*flag to initiate and close the timer for the door function*/
uint8 g_flag_buzzer=1;/*flag to initiate and close the timer for the buzzer function*/

uint8 g_timer_counter=0;/*a variable to count the second in the timer callback function */


/*creating a variable from defined structure with the desired configuration
 * Timer_Mode  compare mode
 * Timer1 Pre-scaler  1024
 * Timer_ID timer1
 * initialValue 0
 * comparelValue 900
 * Timer_CompareMatchOutputMode  NORMAL_OC_DISCONNECT
 *  */
Timer_ConfigType g_timer={CTC,PRESCALER_1024,NOCLOCK2,TIMER_1,0,8350,NORMAL_OC_DISCONNECT};


/*creating a variable from defined structure with the desired configuration
 * Uart_characterSize= 8 bits
 * Uart_stopBit= 1 bit
 *  no parity
 *  baudRate=9600 bit per second*/
Uart_ConfigType g_UartMc2 ={_8_BITS,_1_BIT,DISABLED,9600};

/*creating a variable from defined structure with the desired configuration
 * Twi_bitRate 2 bits per seconds
 * Twi_prescaler pre-scaler = 1
 *  slaveAddress set from static configuration
 *  */
Twi_ConfigType  g_Twi={BITRATE_2,TWI_PRESCALER_1,TWI_SLAVE_ADDRESS};
/******************************************************************************
 *								Main                                          *
 *******************************************************************************/
int main()
{
	SREG|=(1<<7);/*enable the global interrupt bit*/
	TWI_init(&g_Twi);/*initiate the i2c driver*/
	UART_init(&g_UartMc2);/*initiate the UART driver of CONTROL_ECU*/
	BUZZER_init();/*initiate the buzzer driver*/
	DcMotor_Init();/*initiate the DC motor driver*/
	TIMER1_setCallBack(MC2_timer_Handler);/*set the call-back function to handle the interrupt*/
	while(UART_recieveByte() != MC2_READY){}/*receive the byte and insure the synchronization*/
	MC2_checkNewPassword();/*check the send password from HMI_ECU*/


	/******************************************************************************
	 *								super loop                                    *
	 *******************************************************************************/

	/*check on the system current state saved in g_state in a switch case
	 * in state MAIN_OPTIONS -->> call MC2_receiveOption function to save the sent command
	 * from the user and send it to the CONTROL_ECU through the UART
	 *
	 * in state ENTER_YOUR_PASSWORD -->> call MC2_checkPassword function to compare
	 * the entered password with the one saved in the EEPROM
	 *
	 * in state MATCH -->> call MC2_selectedOptions function to choose what is the next
	 *  task to execute according to the choice send from HMI_ECU UART
	 *
	 * in state MISMATCH -->> call MC2_checkPassword function again to check on
	 * the reentered password send from HMI_ECU
	 *
	 * in state OPENING -->> call MC2_DoorHandler function to open the door and close it again
	 *
	 * in state ENTER_NEW_PASSWORD -->> call function MC2_checkNewPassword to check the
	 * new password and confirm it then save it in the EEPROM instead of the old one
	 *
	 * in state ERROR_STATE -->>call MC2_alarm function to enable buzzer for 1 minute
	 * in case of three consecutive mismatches
	 * */
	while(1)
	{
		switch (g_state)
		{
		case MAIN_OPTIONS:
			MC2_receiveOption();
			break;
		case ENTER_YOUR_PASSWORD:
			MC2_checkPassword();
			break;
		case MATCH:
			MC2_selectedOptions();
			break;
		case MISMATCH:
			MC2_checkPassword();
			break;
		case OPENING:
			MC2_DoorHandler();
			break;
		case ENTER_NEW_PASSWORD:
			MC2_checkNewPassword();
			break;
		case ERROR_STATE:
			MC2_alarm();
			break;
		}
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/* Function Name: MC2_receiveOption
 * Description: receive the chosen key from HMI_ECU either '+' or '-'
 * 				and then change the current state flag and send it back
 * 				to HMI_ECU through the UART
 */

void    MC2_receiveOption()
{
	/*save the send choice from HMI_ECU*/
	g_receivedChoice=UART_recieveByte();
	/*change the current state to ENTER_YOUR_PASSWORD to get the password
	 * from user and check if it's correct to execute the desired option*/
	g_state=ENTER_YOUR_PASSWORD;
	/*send the current state to the HMI_ECU*/
	UART_sendByte(g_state);
}


/* Function Name:  MC2_selectedOptions
 * Description:switch case over the received key then change the current state
 * 				and send it back through the UART
 */

void  MC2_selectedOptions(void)
{
	/*in case the choice is '+'
	 * change the the current state to OPENING so to start
	 * then send the current state to HMI_ECU
	 *
	 * in case the choice is '-'
	 * change the the current state to ENTER_NEW_PASSWORD so allow the
	 * user to enter the new password and change the old one
	 * then send the current state to HMI_ECU
	 * */
	switch(g_receivedChoice)
	{
	case '+':
		/*change the the current state to OPENING*/
		g_state=OPENING;
		/*send the match flag to MC1_mainOptions*/
		UART_sendByte(g_state);
		break;
	case'-':
		/*change the the current state to ENTER_NEW_PASSWORD*/
		g_state=ENTER_NEW_PASSWORD;
		/*send the match flag to MC1_mainOptions*/
		UART_sendByte(g_state);
		break;
	default:
		/*if input is false try again change the current state to MAIN_OPTIONS*/
		g_state=MAIN_OPTIONS;
		/*send the match flag to MC1_mainOptions*/
		UART_sendByte(g_state);
	}
}


/* Function Name:  MC2_DoorHandler
 * Description: this function opens the door then wait for 3 sec then close
 * 				it again using timer driver,and on each action changes the current
 * 				state and send it to HMI_ECU
 */
void  MC2_DoorHandler(void)
{
	if(g_flag_motor==1)
	{
		/*initiate the Timer driver*/
		TIMER_init(&g_timer);
		/*clear the flag so that the timer INIT. function won't be
		 *called again in next iterations in the super loop
		 */
		g_flag_motor=0;
	}
	/* motor rotates clockwise (opens door)*/
	DcMotor_Rotate(CLOCKWISE);

	/*wait 15 seconds then change the state to door is opened*/
	while(g_timer_counter <=15);

	/*stop motor door is opened*/
	DcMotor_Rotate(STOP);

	/*change the current state to DOOR_IS_OPENED*/
	g_state = DOOR_IS_OPENED;

	/*send the current state to HMI_ECU*/
	UART_sendByte(g_state);

	/*wait 3 more seconds then change current state to CLOSING */
	while(g_timer_counter <=18);

	/*motor rotates anti-clockwise (closes door)*/
	DcMotor_Rotate(ANTI_CLOCKWISE);

	/*change the current state to CLOSING*/
	g_state = CLOSING;

	/*send the current state to HMI_ECU*/
	UART_sendByte(g_state);/*send the match flag to MC1_mainOptions*/

	/*wait 15 more seconds while closing the door*/
	while(g_timer_counter <=33);
	/*stop motor door is closed again*/
	DcMotor_Rotate(STOP);

	/*change the current state to MAIN_OPTIONS*/
	g_state = MAIN_OPTIONS;

	/*send the current state to HMI_ECU*/
	UART_sendByte(g_state);

	/*close the timer driver by disabling the clock bits*/
	TIMER_deinit(&g_timer);

	/*clear the seconds counter*/
	g_timer_counter=0;

	/*set the flag again to be able to execute the function again when called */
	g_flag_motor=1;
}

/* Function Name:  MC2_checkNewPassword
 * Description: this function takes the password from the user 2 times in 2 different
 * 				array and check if they are matched it will save it in the EEPROM
 * 				as the new password of the system.
 * 				this function is used to set the password for the first time
 * 				of using the system or to change an old password.
 *
 */
uint8  MC2_checkNewPassword()
{
	/*this for loop takes the password for the first time from HMI_ECU
	 * by UART and save it in array g_recivedIntialPassword*/
	for(uint8 i=0;i<5;i++)
	{
		g_recivedIntialPassword[i]=UART_recieveByte();
	}

	/*this for loop takes the password for the second time from HMI_ECU
	 * by UART and save it in array g_recivedReenterdPassword*/
	for(uint8 j =0;j<PASS_LENGTH;j++)
	{
		g_recivedReenterdPassword[j]=UART_recieveByte();
	}

	/*this for loop takes compare between the two arrays and check if MATCHED*/
	for(uint8 k=0;k<PASS_LENGTH;k++)
	{
		if(g_recivedIntialPassword[k]!=g_recivedReenterdPassword[k])
		{
			/*password mismatched change current state to MISMATCH */
			g_state=MISMATCH;

			/*send the current state to HMI_ECU*/
			UART_sendByte(g_state);

			/*call same function to try again*/
			MC2_checkNewPassword();

			/*terminate the function*/
			return 0;
		}
	}

	/*save the new pass in EEPROM*/
	MC2_savePassword((uint8 *)&g_recivedIntialPassword);
	/*change current state to MATCH */
	g_state=MATCH;

	/*send the current state to HMI_ECU to print PASWORD IS SET  on screen*/
	UART_sendByte(g_state);

	/*change current state to MAIN_OPTIONS */
	g_state=MAIN_OPTIONS;

	/*send the current state to HMI_ECU */
	UART_sendByte(g_state);
	return 0;
}

/* Function Name:  MC2_checkPassword
 * Description: this function check if the password entered by user is matching
 * 				the one saved in the EEPROM if matched the current flag will be
 * 				changed so the action chosen by user will be executed
 * 				if the user missed for 3 consecutive times then the flag will
 * 				be set to ERROR
 */
uint8  MC2_checkPassword(void)
{
	/*this for loop takes the password  from HMI_ECU
	 * by UART and save it in array g_recivedIntialPassword*/
	for(uint8 j =0;j<PASS_LENGTH;j++)
	{
		g_recivedIntialPassword[j]=UART_recieveByte();
	}

	/*read password from EEPROM to compare it with entered one*/
	MC2_readPassword((uint8 *)&g_Password);

	/*this for loop takes compare between the received password
	 * and the one in EEPROM and check if MATCHED*/
	for(uint8 i=0;i<PASS_LENGTH;i++)
	{
		if(g_recivedIntialPassword[i]==g_Password[i])
		{
			/*skip the current iteration of the for loop*/
			continue;
		}
		else if(g_recivedIntialPassword[i]!=g_Password[i])
		{
			/*if they are not the same increment the counter by 1*/
			if(mismatchCounter<2)
			{
				/*increment counter*/
				mismatchCounter++;

				/*change current state to MISMATCH */
				g_state=MISMATCH;

				/*send the current state to HMI_ECU */
				UART_sendByte(g_state);

				return 0;
			}
			else if(mismatchCounter==2)
			{
				/*if the password was entered wrong for 3 times*/
				/*change current state to ERROR_STATE */
				g_state=ERROR_STATE;

				/*send the current state to HMI_ECU */
				UART_sendByte(g_state);
				return 0;
			}
		}
	}
	/*change current state to MATCH */
	g_state=MATCH;

	/*send the current state to HMI_ECU */
	UART_sendByte(g_state);
	return 0;
}

/* Function Name:  MC2_savePassword
 * Description: this function saves the password in the EEPROM
 * Input:
 * 		a_passwordArr: pointer to array of characters
 */
void  MC2_savePassword(uint8 *a_passwordArr)
{
	/*this for loop write bytes equal to the number of PASS_LENGTH
	 * in the EEPROM to save the password*/
	for(uint8 counter=0 ; counter < PASS_LENGTH ; counter++)
	{
		EEPROM_writeByte(PASSWORD_ADDRESS+counter,a_passwordArr[counter]);
		_delay_ms(100);
	}
}

/* Function Name:  MC2_readPassword
 * Description: this function read the password saved in the EEPROM
 * Input:
 * 		a_passwordSavedInEeprom: pointer to constant data in array of characters
 */

void  MC2_readPassword(const uint8 *a_passwordSavedInEeprom)
{
	/*this for loop read bytes equal to the number of PASS_LENGTH
	 * from the EEPROM to read the current password*/
	for(uint8 counter=0 ; counter < PASS_LENGTH ; counter++)
	{
		EEPROM_readByte(PASSWORD_ADDRESS+counter,a_passwordSavedInEeprom+counter);
		_delay_ms(100);
	}
}

/* Function Name:  MC2_timer_Handler
 * Description: this is the callback function of the time
 * 				this function increment the counter of seconds
 */

void MC2_timer_Handler()
{
	/*increment the seconds counter each call*/
	g_timer_counter++;
}

/* Function Name:  MC2_alarm
 * Description: this function enable the buzzer for one second
 */

void MC2_alarm()
{
	if(g_flag_buzzer== 1 )
	{
		/*initiate the timer driver to count the 60 seconds*/
		TIMER_init(&g_timer);

		/*clear the flag so that the timer INIT won't be called
		 * again in next iterations of the super loop */
		g_flag_buzzer=0;
	}
	if(g_timer_counter <=60)
	{
		/*enable the buzzer for 60 seconds*/
		BUZZER_on();
	}
	else if (g_timer_counter>60)
	{
		/*after 1 minute close the buzzer
		 * close the timer driver by clearing the clock bits
		 * set the g_flag_buzzer again so that the function could be  executed again
		 * clear the timer counter
		 * */
		BUZZER_off();
		g_flag_buzzer=1;
		TIMER_deinit(&g_timer);
		g_timer_counter=0;

		/*change current state to MAIN_OPTIONS */
		g_state = MAIN_OPTIONS;

		/*send the current state to HMI_ECU */
		UART_sendByte(g_state);

		/*clear the mismatch counter */
		mismatchCounter=0;
	}
}
