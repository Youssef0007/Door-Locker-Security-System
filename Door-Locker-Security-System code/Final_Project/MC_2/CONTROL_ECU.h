/******************************************************************************
 *
 * File Name: mc2.h
 *
 * Description: door locker security system micro2
 *
 * Date Creation: 31/10/2021
 *
 * Author: Youssef Abdel Aziz
 *
 *******************************************************************************/

#ifndef CONTROL_ECU_H_
#define CONTROL_ECU_H_

#include"std_types.h"
/*******************************************************************************
 *                      Definitions & static configuration                     *
 *******************************************************************************/
/*shakehands to synchronize between UART in both micros*/
#define MC2_READY 0x01

#define PASS_LENGTH 5
#define PASSWORD_ADDRESS 0x005
#define TWI_SLAVE_ADDRESS 0b0000011

/*some flags to save and monitor the system current state*/
#define MISMATCH	0xEE
#define MATCH		0xCC
#define ENTER_NEW_PASSWORD 0xFE
#define MAIN_OPTIONS 0xFD
#define ENTER_YOUR_PASSWORD 0xFB
#define OPENING 0xFA
#define DOOR_IS_OPENED 0xF9
#define CLOSING 0xF8
#define ERROR_STATE 0xF7
#define FLAG_MOTOR_INIT 0xA1
#define FLAG_BUZZER_INIT 0xA2

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/* Function Name: MC2_receiveOption
 * Description: receive the chosen key from HMI_ECU either '+' or '-'
 * 				and then change the current state flag and send it back
 * 				to HMI_ECU through the UART
 */

void    MC2_receiveOption();
/* Function Name:  MC2_selectedOptions
 * Description:switch case over the received key then change the current state
 * 				and send it back through the Uart
 */

void    MC2_selectedOptions(void);

/* Function Name:  MC2_DoorHandler
 * Description: this function opens the door then wait for 3 sec then close
 * 				it again using timer driver,and on each action changes the current
 * 				state and send it to HMI_ECU
 */

void    MC2_DoorHandler(void);

/* Function Name:  MC2_checkNewPassword
 * Description: this function takes the password from the user 2 times in 2 different
 * 				array and check if they are matched it will save it in the EEPROM
 * 				as the new password of the system.
 * 				this function is used to set the password for the first time
 * 				of using the system or to change an old password.
 *
 */
uint8   MC2_checkNewPassword(void);

/* Function Name:  MC2_checkPassword
 * Description: this function check if the password entered by user is matching
 * 				the one saved in the EEPROM if matched the current flag will be
 * 				changed so the action chosen by user will be executed
 * 				if the user missed for 3 consecutive times then the flag will
 * 				be set to ERROR
 */
uint8   MC2_checkPassword(void);


/* Function Name:  MC2_savePassword
 * Description: this function saves the password in the EEPROM
 * Input:
 * 		a_passwordArr: pointer to array of characters
 */
void  	MC2_savePassword(uint8 *a_passwordArr);
/* Function Name:  MC2_readPassword
 * Description: this function read the password saved in the EEPROM
 * Input:
 * 		a_passwordSavedInEeprom: pointer to constant data in array of characters
 */

void    MC2_readPassword(const uint8 *a_passwordSavedInEeprom);
/* Function Name:  MC2_timer_Handler
 * Description: this is the callback function of the time
 * 				this function increment the counter of seconds
 */

void    MC2_timer_Handler();
/* Function Name:  MC2_alarm
 * Description: this function enable the buzzer for one second
 */

void  	MC2_alarm(void);


#endif /* CONTROL_ECU_H_ */
