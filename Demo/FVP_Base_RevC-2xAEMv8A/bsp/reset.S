
//Reset handler:

	.global	 reset_handler
	.section .reset,"ax"

//To guarantee no semihosting library functions are called:
 	.global __use_no_semihosting

//Override ARM library's __rt_entry:
	.global __rt_entry
	.type	__rt_entry @function
__rt_entry:		//called by __main
	B  appmain

reset_handler:
	// Initialize the register bank:
	.irp	reg, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
	MOV		X\reg, XZR
	.endr
	.irp	reg, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20
	MOV		X\reg, XZR
	.endr
	.irp	reg, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30
	MOV		X\reg, XZR
	.endr

	//Initialize stack pointers:
	//Uncomment this line if you'd like to use SP_EL0 as stack pointer
	//MSR		SPSel, #0		//Use SP_EL0 as SP
	//Use SP_EL0 and SP_EL3 only as we work in EL3 only
	//SP_EL0 is to be used by FreeRTOS tasks, which will be set to individual tasks' stack when context switches.
	//SP_EL3 is to be used by other cases, e.g. interrupts, system init
	//stage before the first FreeRTOS task is activated.
	//AARCH64 stack is full descending, so set the initial value of SP to Image$$ARM_LIB_STACK$$ZI$$Limit
	LDR		X0, =Image$$ARM_LIB_STACK$$ZI$$Limit  //ARM_LIB_STACK: to be defined in the scatter loading file
	MOV		SP, X0  //SP is SP_EL3 on reset

	//Initialize system control registers:

	//SCTLR_EL3

	//SCR_EL3
	MRS		X0, SCR_EL3
	ORR		X0, X0, #(1<<3)		//EA
	ORR		X0, X0, #(1<<2)		//FIQ
	ORR		X0, X0, #(1<<1)		//IRQ
	ORR		X0, X0, #(1<<8)		//HCE
	AND		X0, X0, #~(1<<7)	//Enable SMC instruction, which is used for task yield
	MSR		SCR_EL3, X0


	B	__main //do startup initialization (copy ZW regions, zero ZI regions, etc. And then call __rt_entry, which calls application's main function

	.end
