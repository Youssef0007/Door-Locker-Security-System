/******************************************************************************
 *
 * Module: DC motor
 *
 * File Name: dc_motor.h
 *
 * Description: header file for a dc motor driver
 *
 * Date Creation : Oct 9, 2021
 *
 * Author: Youssef Abdel Aziz
 *
 *******************************************************************************/


#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/* DC_motor HW Ports and Pins Ids */
#define MOTOR1_IN_PORT_ID      PORTB_ID
#define MOTOR1_IN1_PIN_ID      PIN0_ID
#define MOTOR1_IN2_PIN_ID	   PIN1_ID
#define MOTOR1_ENABLE_PIN_ID

#define MOTOR2_IN_PORT_ID
#define MOTOR2_IN1_PIN_ID
#define MOTOR2_IN2_PIN_ID
#define MOTOR2_ENABLE_PIN_ID

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	STOP,CLOCKWISE,ANTI_CLOCKWISE
}DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/* Function Name:DcMotor_Init
 * Description: initialize the DC motor driver selected pins in the AVR
 * 				to output pins and keep the motor off at start
 */
void DcMotor_Init(void);
/* Function Name:DcMotor_Rotate
 * description: update the motor state and speed according to its selected configuration structure
 * Arguments:the members of the config_type structure state and speed
 *
 */

void DcMotor_Rotate(DcMotor_State a_state);

#endif /* DC_MOTOR_H_ */
