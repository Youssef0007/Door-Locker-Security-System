 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Youssef Abdelaiz
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

/*******************************************************************************
 *                      Types declarations                                   *
 *******************************************************************************/
typedef enum
{
	_5_BITS,_6_BITS,_7_BITS,_8_BITS,RESERVED_1,RESERVED_2,RESERVED_3,_9_BITS
}Uart_characterSize;

typedef enum
{
	_1_BIT,_2_BIT
}Uart_stopBit;


typedef enum
{
	DISABLED,RESERVED,EVEN_PARITY,ODD_PARITY
}Uart_parityMode;


typedef struct
{
	Uart_characterSize characterSize;
	Uart_stopBit stopBit;
	Uart_parityMode parityMode;
	uint64 baudRate;
}Uart_ConfigType;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const Uart_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
