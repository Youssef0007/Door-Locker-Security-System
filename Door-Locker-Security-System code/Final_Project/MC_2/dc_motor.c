/******************************************************************************
 *
 * Module: DC motor
 *
 * File Name: dc_motor.c
 *
 * Description: source file for a dc motor driver
 *
 * Date Creation : Oct 9, 2021
 *
 * Author: Youssef Abdel Aziz
 *
 *******************************************************************************/
#include "dc_motor.h"
#include "gpio.h"
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/* Function Name:DcMotor_Init
 * Description: initialize the DC motor driver selected pins in the AVR
 * 				to output pins and keep the motor off at start
 */
void DcMotor_Init(void)
{
	GPIO_setupPinDirection(MOTOR1_IN_PORT_ID, MOTOR1_IN1_PIN_ID, PIN_OUTPUT);/*SET THE MOTOR PINS AS OUTPUT*/
	GPIO_setupPinDirection(MOTOR1_IN_PORT_ID, MOTOR1_IN2_PIN_ID, PIN_OUTPUT);
	GPIO_writePin(MOTOR1_IN_PORT_ID, MOTOR1_IN1_PIN_ID, LOGIC_LOW);/*MOTOR  OFF AT THE BEGINNG*/
	GPIO_writePin(MOTOR1_IN_PORT_ID, MOTOR1_IN2_PIN_ID, LOGIC_LOW);

}
/* Function Name:DcMotor_Rotate
 * description: update the motor state and speed according to its selected configuration structure
 * Arguments:the member of the config_type structure state
 *
 */
void DcMotor_Rotate(DcMotor_State a_state)
{
	switch (a_state)
	{/* switch case on the new state value to update the motor state*/
	case STOP:
		GPIO_writePin(MOTOR1_IN_PORT_ID, MOTOR1_IN1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(MOTOR1_IN_PORT_ID, MOTOR1_IN2_PIN_ID, LOGIC_LOW);/*both pins are zeros motor stop*/
		break;
	case CLOCKWISE:
		GPIO_writePin(MOTOR1_IN_PORT_ID, MOTOR1_IN1_PIN_ID, LOGIC_LOW);/*set motor pins in CW direction*/
		GPIO_writePin(MOTOR1_IN_PORT_ID, MOTOR1_IN2_PIN_ID, LOGIC_HIGH);
		break;
	case ANTI_CLOCKWISE:
		GPIO_writePin(MOTOR1_IN_PORT_ID, MOTOR1_IN1_PIN_ID, LOGIC_HIGH);/*set motor pins in ACW direction*/
		GPIO_writePin(MOTOR1_IN_PORT_ID, MOTOR1_IN2_PIN_ID, LOGIC_LOW);
		break;
	default:
		GPIO_writePin(MOTOR1_IN_PORT_ID, MOTOR1_IN1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(MOTOR1_IN_PORT_ID, MOTOR1_IN2_PIN_ID, LOGIC_LOW);/*both pins are zeros motor stop*/

	}
}
