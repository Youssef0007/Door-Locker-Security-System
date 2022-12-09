/******************************************************************************
 * Module: Buzzer
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the BUZZER  driver
 *
 * Author: Youssef Abdelaziz
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"
/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

#define BUZZER_PORT_ID  PORTA_ID
#define BUZZER_PIN_ID	PIN0_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*Function name:BUZZER_init
 * Description :
 * initiating the buzzer driver according to the connection port and pin
 */
void BUZZER_init();

/*Function name:BUZZER_off
 * Description :
 * disabling the buzzer
 *
 */
void BUZZER_off();

/*Function name:BUZZER_on
 * Description :
 * enabling the buzzer
 *
 */
void BUZZER_on();

#endif /* BUZZER_H_ */
