/******************************************************************************
 *
 * File Name: HMI_ECU.h
 *
 * Description: the human machine interface micro-controller that give access
 * 				to the user
 *
 * Author: Youssef abdelaziz
 *
 *******************************************************************************/


#ifndef HMI_ECU_H_
#define HMI_ECU_H_


#include"std_types.h"
/*******************************************************************************
 *                     			 Definitions 									*
 *******************************************************************************/
#define PASS_LENGTH 5
#define PRINTED_CHARACTER '*'
/*shakehands to synchronize between UART in both micros*/
#define MC2_READY 0x01
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

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/* Function Name:  MC1_setPassword
 * Description: this function allow the user to change the password.
 * 				it takes the password from the user
 * 				two times and send both to the CONTROL_ECU through the UART
 * 				to check if both are matched or not
 */

void  MC1_setPassword(void);


/* Function Name:  MC1_mainOptions
 * Description: this function takes the chosen action from the user
 * 				and send it to the CONTROL_ECU through the UART
 */

void MC1_mainOptions(void);

/* Function Name:  MC1_enterPassword
 * Description:	takes password from user and send it to the UART to compare it
 * 				with one saved in the EEPROM
 */

void MC1_enterPassword(void);


#endif /* HMI_ECU_H_ */
