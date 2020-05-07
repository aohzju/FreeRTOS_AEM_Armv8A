/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "basehw.h"
#include "gicv3.h"


#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1
	/* Check the configuration. */
	#if( configMAX_PRIORITIES > 64 )
		#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 64.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
	#endif
#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */



#define portSP_EL0					( ( StackType_t ) 0x00 )
#define portEL3						( ( StackType_t ) 0x0c )
#define portINITIAL_PSTATE			( portEL3 | portSP_EL0 )

/* Some vendor specific files default configCLEAR_TICK_INTERRUPT() in
portmacro.h. */
#ifndef configCLEAR_TICK_INTERRUPT
	#define configCLEAR_TICK_INTERRUPT()
#endif

/* A critical section is exited when the critical section nesting count reaches
this value. */
#define portNO_CRITICAL_NESTING			( ( size_t ) 0 )

/* In all GICs 255 can be written to the priority mask register to unmask all
(but the lowest) interrupt priority. */
#define portUNMASK_VALUE				( 0xFFUL )

/*-----------------------------------------------------------*/

/*
 * Starts the first task executing.  This function is necessarily written in
 * assembly code so is implemented in portASM.s.
 */
extern void vPortRestoreTaskContext( void );

/* Set to 1 to pend a context switch from an ISR. */
uint64_t ullPortYieldRequired = pdFALSE;

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Setup the initial stack of the task.  The stack is set exactly as
	expected by the portRESTORE_CONTEXT() macro. */

	/* First all the general purpose registers. */
	pxTopOfStack--;
	*pxTopOfStack = 0x0101010101010101ULL;	/* R1 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) pvParameters; /* R0 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0303030303030303ULL;	/* R3 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0202020202020202ULL;	/* R2 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0505050505050505ULL;	/* R5 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0404040404040404ULL;	/* R4 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0707070707070707ULL;	/* R7 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0606060606060606ULL;	/* R6 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0909090909090909ULL;	/* R9 */
	pxTopOfStack--;
	*pxTopOfStack = 0x0808080808080808ULL;	/* R8 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1111111111111111ULL;	/* R11 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1010101010101010ULL;	/* R10 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1313131313131313ULL;	/* R13 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1212121212121212ULL;	/* R12 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1515151515151515ULL;	/* R15 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1414141414141414ULL;	/* R14 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1717171717171717ULL;	/* R17 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1616161616161616ULL;	/* R16 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1919191919191919ULL;	/* R19 */
	pxTopOfStack--;
	*pxTopOfStack = 0x1818181818181818ULL;	/* R18 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2121212121212121ULL;	/* R21 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2020202020202020ULL;	/* R20 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2323232323232323ULL;	/* R23 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2222222222222222ULL;	/* R22 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2525252525252525ULL;	/* R25 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2424242424242424ULL;	/* R24 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2727272727272727ULL;	/* R27 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2626262626262626ULL;	/* R26 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2929292929292929ULL;	/* R29 */
	pxTopOfStack--;
	*pxTopOfStack = 0x2828282828282828ULL;	/* R28 */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00;	/* XZR - has no effect, used so there are an even number of registers. */
	pxTopOfStack--;
	*pxTopOfStack = ( StackType_t ) 0x00;	/* R30 - procedure call link register. */
	pxTopOfStack--;

	*pxTopOfStack = portINITIAL_PSTATE;
	pxTopOfStack--;

	*pxTopOfStack = ( StackType_t ) pxCode; /* Exception return address. */


	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void vConfigureTickInterrupt()  //config generic timer for FreeRTOS tick interrupt
{
	uint32_t regVal;
	int x;

	setGenTimerFreq(GenCounterFreq);
	setEL3PhyTimerCV(getPhyCount()+TICK_CYCLES);
	setEL3PhyTimerCtrl(0x01);

	//timer interrupt is level sensitive per ARM document:
	regVal = gicrReadReg(GICR_ICFGR1, 0);
	x = GEN_TIMER_INTID - 16;
	x = x*2 + 1;
	regVal &= ~(0x1<<x); //clear to zero
	gicrWriteReg(GICR_ICFGR1, 0, regVal);

	//Set priority:
	setSGIPPIPriority(0, GEN_TIMER_INTID, 0);

	regVal = gicrReadReg(GICR_ISENABLER0, 0);
	regVal |= (1<<GEN_TIMER_INTID); //enable generic timer interrupt
	gicrWriteReg(GICR_ISENABLER0, 0, regVal);
}

BaseType_t xPortStartScheduler( void )
{

	/* Interrupts are turned off in the CPU itself to ensure a tick does
	not execute	while the scheduler is being started.  Interrupts are
	automatically turned back on in the CPU when the first task starts
	executing. */
	//portDISABLE_INTERRUPTS(); ///already disabled on reset...

	/* Start the timer that generates the tick ISR. */
	vConfigureTickInterrupt();

	/* Start the first task executing. */
	vPortRestoreTaskContext();

	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Not implemented in ports where there is nothing to return to.
	Artificially force an assert. */
	configASSERT( 0 );
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

void FreeRTOS_Tick_Handler( void )
{

	setEL3PhyTimerCV(getPhyCount()+TICK_CYCLES);  //Reload generic timer compare value for next tick



	/* Increment the RTOS tick. */
	if( xTaskIncrementTick() != pdFALSE )
	{
		ullPortYieldRequired = pdTRUE;
	}

}


/*-----------------------------------------------------------*/

