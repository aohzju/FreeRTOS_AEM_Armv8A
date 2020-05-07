//Exception handler table:

//For this port, we run on EL3 only

	.global vector_table_el3

	.balign 0x800
vector_table_el3:

//Group 1: Current EL with SP0
curr_el_sp0_sync:
	b	FreeRTOS_SYNC_Handler

	.balign 0x80
curr_el_sp0_irq:
	b	.

	.balign 0x80
curr_el_sp0_fiq:
	b	FreeRTOS_FIQ_Handler	//defined in Source/portable/ARMClang/Armv8-A_aarch64/portASM.S

	.balign 0x80
curr_el_sp0_serr:
	b	.

//Group 2: Current EL with SPx:
	.balign 0x80
curr_el_spx_sync:
	b	.

	.balign 0x80
curr_el_spx_irq:
	b	.

	.balign 0x80
curr_el_spx_fiq:
	b	.

	.balign 0x80
curr_el_spx_serr:
	b	.

//Group 3: Lower EL using AArch64:
	.balign 0x80
lower_el_a64_sync:
	b	.

	.balign 0x80
lower_el_a64_irq:
	b	.

	.balign 0x80
lower_el_a64_fiq:
	b	.

	.balign 0x80
lower_el_a64_serr:
	b	.

//Group4: Lower EL using AArch32:
	.balign 0x80
lower_el_a32_sync:
	b	.

	.balign 0x80
lower_el_a32_irq:
	b	.

	.balign 0x80
lower_el_a32_fiq:
	b	.

	.balign 0x80
lower_el_a32_serr:
	b	.
