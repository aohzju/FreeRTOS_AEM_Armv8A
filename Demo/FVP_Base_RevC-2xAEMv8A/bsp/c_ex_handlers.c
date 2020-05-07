/*
 * c_ex_handers.c
 *
 *  Created on: Apr 29, 2020
 *      Author: oscarh
 */

#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "gicv3.h"
#include "basehw.h"

extern void FreeRTOS_Tick_Handler( void );

//The core FIQ handler in c, called assembly function fiq_handler.
//The surrounding logic is in assembly code: context save/restore,
//Interrupt acknowledge by reading ICC_IAR0_EL1
//EOI by writing ICC_EOIR0_EL1
void vApplicationFIQHandler(uint64_t intID)
{
	if(intID == GEN_TIMER_INTID)
		FreeRTOS_Tick_Handler();  //defined in port.c

	//To do: handers for other intIDs
}
