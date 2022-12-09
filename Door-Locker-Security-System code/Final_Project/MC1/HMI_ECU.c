/******************************************************************************
 *
 * File Name: HMI_ECU.c
 *
 * Description: the human machine interface micro-controller that give access
 * 				to the user
 *
 * Author: Youssef abdelaziz
 *
 *******************************************************************************/


#include "HMI_ECU.h"
#include<util/delay.h>
/******************************************************************************
 *								DRIVERS MODULES                               *
 *******************************************************************************/
#include"lcd.h"
#include"uart.h"
#include"keypad.h"

/******************************************************************************
 *								GLOBAL VARIABLES                              *
 *******************************************************************************/

uint8 g_displayMessage;/*variable to monitor the current system state*/

uint8 g_Password[PASS_LENGTH+1];/*array to get password*/

/*creating a variable from defined structure with the desired configuration
 * Uart_characterSize= 8 bits
 * Uart_stopBit= 1 bit
 *  no parity
 *  baudRate=9600 bit per second*/
Uart_ConfigType g_UartMc1 ={_8_BITS,_1_BIT,DISABLED,9600};
/******************************************************************************
 *								Main                                          *
 *******************************************************************************/
int main()
{
	LCD_init();/*initiate the LCD driver*/
	UART_init(&g_UartMc1);/*initiate the UART driver*/
	UART_sendByte(MC2_READY);/*send byte from HMI_ECU to CONTROL_ECU to synchronize*/
	MC1_setPassword();/*set the pass for the first time*/


	/******************************************************************************
	 *								super loop                                    *
	 *******************************************************************************/

	while(1)
	{
		/*receive the current state from CONTROL_ECU*/
		g_displayMessage=UART_recieveByte();

		/*check on the system current state saved in g_displayMessage in a switch case
		 *
		 * in state MAIN_OPTIONS -->> call MC1_mainOptions function to get the chosen action
		 * from the user and send it to the CONTROL_ECU through the UART
		 *
		 * in state ENTER_YOUR_PASSWORD -->> call MC1_enterPassword function to allow
		 * the user to enter the password and send it to CONTROL_ECU to get checked
		 *
		 * in state MATCH -->> display "WELCOME" on screen for two seconds to confirm that
		 * the entered password was correct
		 *
		 * in state MISMATCH -->> display "TRY AGAIN" on screen for two seconds then
		 * ask the user to enter the password again
		 *
		 * in state OPENING -->> display "opening..." on screen while the motor
		 * is opening the door
		 *
		 * in state DOOR_IS_OPENED -->>  display "door opened" for 3 seconds
		 *
		 * in state CLOSING -->> display "closing..." on screen while the motor
		 * is closing the door
		 *
		 * in state ERROR_STATE -->> display "ERROR!!!" while the alarm is on for one minute
		 *
		 * in state ENTER_NEW_PASSWORD -->> call function MC1_setPassword to allow the user
		 * to set or change the password
		 * */
		switch (g_displayMessage)
		{
		case MAIN_OPTIONS:
			MC1_mainOptions();
			break;
		case ENTER_YOUR_PASSWORD:
			MC1_enterPassword();
			break;
		case MATCH:
			LCD_clearScreen();
			LCD_moveCursor(0,0);
			LCD_displayString("Welcome");
			_delay_ms(2000);
			break;
		case MISMATCH:
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,"TRY AGAIN");
			_delay_ms(2000);
			MC1_enterPassword();
			break;
		case OPENING:
			LCD_clearScreen();
			LCD_moveCursor(0,0);
			LCD_displayString("Opening...");
			break;
		case DOOR_IS_OPENED:
			LCD_clearScreen();
			LCD_moveCursor(0,0);
			LCD_displayString("Door opened");
			break;
		case CLOSING:
			LCD_clearScreen();
			LCD_moveCursor(0,0);
			LCD_displayString("Closing...");
			break;
		case ERROR_STATE:
			LCD_clearScreen();
			LCD_moveCursor(0,0);
			LCD_displayString("ERROR!!!");
			break;
		case ENTER_NEW_PASSWORD:
			MC1_setPassword();
			break;
		}
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/* Function Name:  MC1_setPassword
 * Description: this function allows the user to change the password.
 * 				it takes the password from the user
 * 				two times and send both to the CONTROL_ECU through the UART
 * 				to check if both are matched or not.
 */
void MC1_setPassword(void)
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter pass: ");
	LCD_moveCursor(1,0);
	/*this for loop takes the password from user for the first time
	 * and print asterisks on the screen then send the entered password
	 * to CONTROL_ECU */
	for(uint8 i =0;i<PASS_LENGTH+1;i++)
	{
		g_Password[i]=KEYPAD_getPressedKey();
		if(i<PASS_LENGTH)
		{
			/*display * for each entered character*/
			LCD_displayCharacter('*');
			/*send the first password to CONTROL_ECU*/
			UART_sendByte(g_Password[i]);
		}
		_delay_ms(3000); /* Press time delay */
	}
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Reenter pass: ");
	LCD_moveCursor(1,0);
	/*this for loop takes the password from user for the second time
	 * and print asterisks on the screen then send the entered password
	 * to CONTROL_ECU to match it with the second one*/
	for(uint8 i =0;i<PASS_LENGTH+1;i++)
	{
		g_Password[i]=KEYPAD_getPressedKey();
		/*user press each number and save it in g_password array*/
		if(i<PASS_LENGTH)
		{
			/*display * for each entered character*/
			LCD_displayCharacter('*');
			/*send the second password to CONTROL_ECU*/
			UART_sendByte(g_Password[i]);
		}
		_delay_ms(3000); /* Press time */
	}
	/*receive the current state from micro2 */
	g_displayMessage=UART_recieveByte();
	/*check if password is right or wrong*/
	if(MATCH==g_displayMessage)
	{
		/*if match terminate the function*/
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Password is set ");
		_delay_ms(3000);
	}
	else if(MISMATCH==g_displayMessage)
	{
		/*if mismatch call the same function to try again*/
		MC1_setPassword();
	}
}


/* Function Name:  MC1_mainOptions
 * Description: this function takes the chosen action from the user
 * 				and send it to the CONTROL_ECU through the UART
 */

void MC1_mainOptions(void)
{
	uint8 key;/*variable to save the user choice*/
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"+: Open Door");
	LCD_displayStringRowColumn(1,0,"-: Change Pass");
	key=KEYPAD_getPressedKey();/*user enter the desired choice*/
	_delay_ms(3000); /*delay for press time*/
	/*if condition to check what does the user entered
	 * and send it to CONTROL_ECU to take action*/
	if(!(key=='+'||key=='-'))
	{
		/*if the pressed key is not + or - then the user must try again*/
		MC1_mainOptions();
	}
	else
	{
		/*if the password is correct then send the character to CONTROL_ECU*/
		UART_sendByte(key);
	}
}

/* Function Name:  MC1_enterPassword
 * Description:	takes password from user and send it to the UART to compare it
 * 				with one saved in the EEPROM
 */

void MC1_enterPassword()
{
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"Enter password");
	LCD_moveCursor(1,0);
	/*get the password from the user and save it in an array*/
	for(uint8 i =0;i<PASS_LENGTH;i++)
	{
		g_Password[i]=KEYPAD_getPressedKey();
		if(i<PASS_LENGTH)
		{
			 /*display * for each entered character*/
			LCD_displayCharacter('*');
			/*send the first password to CONTROL_ECU*/
			UART_sendByte(g_Password[i]);
		}
		_delay_ms(3000); /*delay for Press time */
	}
}
