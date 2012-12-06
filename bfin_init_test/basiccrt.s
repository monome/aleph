#include <sys/platform.h>
#include <cplb.h>
#include <sys/anomaly_macros_rtl.h>
#include <defBF533.h>
#include <def_LPBlackfin.h>
 
	.text;
	.align 2;
	.global __start;
	.extern _main;
	.type __start, STT_FUNC;
__start:
 
	R7 = 0;
	LC0 = R7;
	LC1 = R7;
	L0 = R7;
	L1 = R7;
	L2 = R7;
	L3 = R7;
	I0.L = (ITEST_COMMAND & 0xFFFF);
	I0.H = (ITEST_COMMAND >> 16);
	I1.L = (DTEST_COMMAND & 0xFFFF);
	I1.H = (DTEST_COMMAND >> 16);
	R7 = 0;
	[I0] = R7;
	[I1] = R7;
 
	CSYNC;
 
	SP.L=__stack_end - 12;
	SP.H=__stack_end - 12;
	FP = SP;
	SP += -12;
 
 
	///////////////from PRM/////////////////////////////////////
	P0.L = (EVT15 & 0xFFFF) ; /* Point to IVG15 in Event Vector Table */
	P0.H = ((EVT15 >> 16) & 0xFFFF) ;
	P1.L = START; /* Point to start of User code */
	P1.H = START;
	[P0] = P1 ; /* Place the address of START in IVG15 of EVT */
	P0.L = (IMASK & 0xFFFF) ;
	R0 = [P0] ;
	R1.L = (EVT_IVG15 & 0xFFFF) ;
	R0 = R0 | R1 ;
	[P0] = R0 ; /* Set (enable) IVG15 bit in IMASK register */
	RAISE 15 ; /* Invoke IVG15 interrupt */
	P0.L = WAIT_HERE ;
	P0.H = WAIT_HERE ;
	RETI = P0 ; /* RETI loaded with return address */
	RTI ; /* Return from Reset Event */
	WAIT_HERE : /* Wait here till IVG15 interrupt is serviced */
	JUMP WAIT_HERE ;
	START: /* IVG15 vectors here */
	/* Enables interrupts and saves return address to stack */
	[--SP] = RETI ;
    ///////////////from PRM/////////////////////////////////////
 
	[--SP]=R0;
	[--SP]=R0;
	FP = SP;
	SP += -12;
 
	CALL.X _main;
    NOP;
__end:
	IDLE;
	JUMP __end;