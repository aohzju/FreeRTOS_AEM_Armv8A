/*
*/

	/* Variables and functions. */
	.extern pxCurrentTCB
	.extern vTaskSwitchContext
	.extern vApplicationFIQHandler
	.extern ullPortYieldRequired


.macro portSAVE_CONTEXT

	/* Switch to use the EL0 stack pointer. */
	//Tasks use SP_EL0 pointer
	MSR 	SPSEL, #0

	/* Save the entire context. */
	STP 	X0, X1, [SP, #-0x10]!
	STP 	X2, X3, [SP, #-0x10]!
	STP 	X4, X5, [SP, #-0x10]!
	STP 	X6, X7, [SP, #-0x10]!
	STP 	X8, X9, [SP, #-0x10]!
	STP 	X10, X11, [SP, #-0x10]!
	STP 	X12, X13, [SP, #-0x10]!
	STP 	X14, X15, [SP, #-0x10]!
	STP 	X16, X17, [SP, #-0x10]!
	STP 	X18, X19, [SP, #-0x10]!
	STP 	X20, X21, [SP, #-0x10]!
	STP 	X22, X23, [SP, #-0x10]!
	STP 	X24, X25, [SP, #-0x10]!
	STP 	X26, X27, [SP, #-0x10]!
	STP 	X28, X29, [SP, #-0x10]!
	STP 	X30, XZR, [SP, #-0x10]!

	/* Save the SPSR. */
	MRS		X3, SPSR_EL3
	/* Save the ELR. */
	MRS		X2, ELR_EL3

	STP 	X2, X3, [SP, #-0x10]!

	/* Save the critical section nesting depth. */
	//We choose to define portCRITICAL_NESTING_IN_TCB.
	//LDR		X0, ullCriticalNestingConst
	//LDR		X3, [X0]

	//We don't support FPU for now
	//...
	/* Save the FPU context indicator. */

	/* Save the FPU context, if any (32 128-bit registers). */
	/* Store the critical nesting count and FPU context indicator. */

	LDR 	X0, =pxCurrentTCB
	LDR 	X1, [X0]
	MOV 	X0, SP   /* Move SP into X0 for saving. */
	STR 	X0, [X1] //Save SP to pxCurrentTCB->pxTopOfStack

	/* Switch to use the ELx stack pointer. */
	MSR 	SPSEL, #1

	.endm

; /**********************************************************************/

.macro portRESTORE_CONTEXT

	/* Switch to use the EL0 stack pointer. */
	MSR 	SPSEL, #0

	/* Set the SP to point to the stack of the task being restored. */
	LDR		X0, =pxCurrentTCB
	LDR		X1, [X0]
	LDR		X0, [X1]
	MOV		SP, X0

	LDP 	X2, X3, [SP], #0x10  /* SPSR and ELR. */
	/* Restore the SPSR. */
	MSR		SPSR_EL3, X3 /*Assumes started in EL3. */
	/* Restore the ELR. */
	MSR		ELR_EL3, X2

	LDP 	X30, XZR, [SP], #0x10
	LDP 	X28, X29, [SP], #0x10
	LDP 	X26, X27, [SP], #0x10
	LDP 	X24, X25, [SP], #0x10
	LDP 	X22, X23, [SP], #0x10
	LDP 	X20, X21, [SP], #0x10
	LDP 	X18, X19, [SP], #0x10
	LDP 	X16, X17, [SP], #0x10
	LDP 	X14, X15, [SP], #0x10
	LDP 	X12, X13, [SP], #0x10
	LDP 	X10, X11, [SP], #0x10
	LDP 	X8, X9, [SP], #0x10
	LDP 	X6, X7, [SP], #0x10
	LDP 	X4, X5, [SP], #0x10
	LDP 	X2, X3, [SP], #0x10
	LDP 	X0, X1, [SP], #0x10

	/* Switch to use the ELx stack pointer.  _RB_ Might not be required. */
	MSR 	SPSEL, #1

	ERET

	.endm


/******************************************************************************
 * FreeRTOS_SYNC_Handler handler is used to perform a context switch when
 * the sync exception is caused by SMC
 *****************************************************************************/
	.align 8
	.global FreeRTOS_SYNC_Handler
	.type FreeRTOS_SYNC_Handler, @function
FreeRTOS_SYNC_Handler:
	/* Save the context of the current task and select a new task to run. */
	portSAVE_CONTEXT
	MRS		X0, ESR_EL3
	LSR		X1, X0, #26
	CMP		X1, #0x17 	/* 0x17 = SMC instruction. */
	B.NE	FreeRTOS_Abort
	BL 		vTaskSwitchContext

	portRESTORE_CONTEXT

FreeRTOS_Abort:
	/* Full ESR is in X0, exception class code is in X1. */
	B		.

/******************************************************************************
 * vPortRestoreTaskContext is used to start the first task in xPortStartScheduler.
 *****************************************************************************/
	.align 8
	.global vPortRestoreTaskContext
	.type vPortRestoreTaskContext, @function
vPortRestoreTaskContext:
	/* Install the FreeRTOS interrupt handlers. */
	LDR		X1, =vector_table_el3
	MSR		VBAR_EL3, X1
	DSB		SY
	ISB		SY

	/* Start the first task. */
	portRESTORE_CONTEXT


/******************************************************************************
 * FreeRTOS_FIQ_Handler handles FIQ entry and exit.
 *****************************************************************************/
	.align 8
	.global FreeRTOS_FIQ_Handler
	.type FreeRTOS_FIQ_Handler, @function
FreeRTOS_FIQ_Handler:
	/* Save volatile registers. */
	STP		X0, X1, [SP, #-0x10]!
	STP		X2, X3, [SP, #-0x10]!
	STP		X4, X5, [SP, #-0x10]!
	STP		X6, X7, [SP, #-0x10]!
	STP		X8, X9, [SP, #-0x10]!
	STP		X10, X11, [SP, #-0x10]!
	STP		X12, X13, [SP, #-0x10]!
	STP		X14, X15, [SP, #-0x10]!
	STP		X16, X17, [SP, #-0x10]!
	STP		X18, X19, [SP, #-0x10]!
	STP		X29, X30, [SP, #-0x10]!

	/* Save the SPSR and ELR. */
	MRS		X3, SPSR_EL3
	MRS		X2, ELR_EL3
	STP 	X2, X3, [SP, #-0x10]!

	/* Read value from the interrupt acknowledge register, which is stored in W0
	for future parameter and interrupt clearing use. */
	MRS     X0, ICC_IAR0_EL1  //Read INTID, acknowledging the interrupt
    MOV     X19, X0

	/* Call the C handler. */
	BL vApplicationFIQHandler
	MSR   ICC_EOIR0_EL1, X19        //End of interrupt handling

	/* Is a context switch required? */
	LDR		X0, =ullPortYieldRequired
	LDR		X1, [X0]
	CMP		X1, #0
	B.EQ	Exit_FIQ_No_Context_Switch

	/* Reset ullPortYieldRequired to 0. */
	STR		XZR, [X0]

	/* Restore volatile registers. */
	LDP 	X4, X5, [SP], #0x10  /* SPSR and ELR. */
	MSR		SPSR_EL3, X5 
	MSR		ELR_EL3, X4
	DSB		SY
	ISB		SY

	LDP		X29, X30, [SP], #0x10
	LDP		X18, X19, [SP], #0x10
	LDP		X16, X17, [SP], #0x10
	LDP		X14, X15, [SP], #0x10
	LDP		X12, X13, [SP], #0x10
	LDP		X10, X11, [SP], #0x10
	LDP		X8, X9, [SP], #0x10
	LDP		X6, X7, [SP], #0x10
	LDP		X4, X5, [SP], #0x10
	LDP		X2, X3, [SP], #0x10
	LDP		X0, X1, [SP], #0x10

	/* Save the context of the current task and select a new task to run. */
	portSAVE_CONTEXT
	BL vTaskSwitchContext
	portRESTORE_CONTEXT

Exit_FIQ_No_Context_Switch:
	/* Restore volatile registers. */
	LDP 	X4, X5, [SP], #0x10  /* SPSR and ELR. */
	MSR		SPSR_EL3, X5
	MSR		ELR_EL3, X4
	DSB		SY
	ISB		SY

	LDP		X29, X30, [SP], #0x10
	LDP		X18, X19, [SP], #0x10
	LDP		X16, X17, [SP], #0x10
	LDP		X14, X15, [SP], #0x10
	LDP		X12, X13, [SP], #0x10
	LDP		X10, X11, [SP], #0x10
	LDP		X8, X9, [SP], #0x10
	LDP		X6, X7, [SP], #0x10
	LDP		X4, X5, [SP], #0x10
	LDP		X2, X3, [SP], #0x10
	LDP		X0, X1, [SP], #0x10

	ERET

	.end





