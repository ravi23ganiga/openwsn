;*
;* File: os_cpu_a.s
;*
;*            (c) Copyright ARM Limited 1999.  All rights reserved. 
;*
;*                               ARM Specific code
;*
;*
; 
;	Functions defined in this module:
;
;	void ARMDisableInt(void)	/* disable interrupts when in SVC */
;	void ARMEnableInt(void)		/* enable interrupts when in SVC */
;	void OS_TASK_SWAP(void)		/* context switch */
;	void OSStartHighRdy(void)	/* start highest priority task */

SwiV		EQU	0x08
IrqV		EQU	0x18
FiqV		EQU	0x1C
NoInt		EQU	0x80

SVC32Mode	EQU	0x13
IRQ32Mode	EQU	0x12
FIQ32Mode	EQU	0x11

OSEnterSWI	EQU	0x00


BIT_TIMER0	EQU		(0x1<<13)
BIT_GLOBAL  EQU     (0x1<<26)
I_ISPC		EQU		0x1e00024
INTMSK		EQU		0x1e0000c
EXTINTPND   EQU     0x1d20054
BIT_EINT4567		EQU     (0x1<<21);#define BIT_EINT4567	(0x1<<21)



	AREA	|subr|, CODE, READONLY;|subr|

	; External symbols we need the addresses of
			IMPORT	OSTCBCur      ;Current TCB pointer
addr_OSTCBCur		DCD	OSTCBCur  ;define 1 word for OSTCBCur's address
			IMPORT	OSTCBHighRdy  ;The TCB pointer of the Task which has the highest priority in the Ready Table
addr_OSTCBHighRdy	DCD	OSTCBHighRdy
			IMPORT	OSPrioCur
addr_OSPrioCur		DCD	OSPrioCur
			IMPORT	OSPrioHighRdy
addr_OSPrioHighRdy	DCD	OSPrioHighRdy

	IMPORT	need_to_swap_context
	IMPORT	IrqStart              ;IRQ
	IMPORT	OSTimeTick
	IMPORT	IrqFinish
	IMPORT  EXINT4isr

	IMPORT	OutDebug
	IMPORT	BreakPoint
	
	EXPORT IRQContextSwap

IRQContextSwap
        LDMIA    sp!,{a1-v1, lr} ;the context in sp-->r0-r4,lr(r0-r4 maybe the para. for the routine)
        SUBS	 pc, lr, #4      ;pc=lr-4; it's a jump instruction,used to return from IRQ


        SUB             lr, lr, #4;lr=lr-4;
        MOV             r12, lr   ;r12=lr;

        MRS             lr, SPSR  ;lr=SPSR
		AND				lr, lr, #0xFFFFFFE0;clr the mode
		ORR				lr, lr, #0xD3      ;disable the interrupt and enter SVC mode
        MSR             CPSR_cxsf, lr      ;CPSR_cxsf=lr



	EXPORT	__Tick
__Tick
	STMDB    sp!,{r0-r11,lr}      ;=push  lr, r0-r11-->sp

	;interrupt disable(not nessary)
	mrs		r0, CPSR            ; r0=CPSR
	orr		r0, r0, #0x80		; and set IRQ disable flag
	msr		CPSR_cxsf, R0       ; CPSR_cxsf=R0
	
	;End of interrupt
	;(Clear pending bit of INTPEND that don't accessed it.)
	;	rI_ISPC= BIT_TIMER0;
	LDR	r0, =I_ISPC
	LDR	r1, =BIT_TIMER0;clr INT_TIMER0 pending bit
	STR	r1, [r0]       ;r1-->[r0]

	BL	IrqStart
	
	BL	OSTimeTick

	BL	IrqFinish

	
	LDR		r0, =need_to_swap_context
	LDR		r2, [r0]   ;ldr: memory to register,r2=need_to_swap_context
	CMP		r2, #1
	LDREQ	pc, =_CON_SW;if equal,then switch context
	LDR     pc, =_NOT_CON_SW

    EXPORT  _NOT_CON_SW
_NOT_CON_SW
	;not context switching
	LDMIA    sp!,{r0-r11, lr}
	SUBS	pc, lr, #4

    EXPORT  _CON_SW
_CON_SW
	;set need_to_swap_context is '0'
	MOV		r1, #0
	STR		r1, [r0]


	;now context switching
	LDMIA    sp!,{r0-r11,lr}  ;pop
	SUB		lr, lr, #4

	STR				lr, [pc, #SAVED_LR-.-8];???

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;Change Supervisor mode
	;!!!r12 register don't preserved. (r12 that PC of task)

    MRS             lr, SPSR            ;lr=SPSR
	AND				lr, lr, #0xFFFFFFE0 ;enable all IRQ FIQ
	ORR				lr, lr, #0x13
	;ORR             lr, lr, #0x93
    MSR             CPSR_cxsf, lr       ;CPSR_cxsf

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;Now  Supervisor mode
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	STR		r12, [sp, #-8]	; saved r12->sp-8
	LDR		r12, [pc, #SAVED_LR-.-8]
	STMFD	sp!, {r12}		; push r12 that PC of task under SVC mode
	SUB		sp, sp, #4		; inclease stack point
	LDMIA	sp!, {r12}		; restore r12
	
	STMFD	sp!, {lr}		; save lr (CPSR?)
	STMFD	sp!, {r0-r12}	; save register file and ret address
	MRS		r4, CPSR        ; r4=CPSR
	STMFD	sp!, {r4}		; save current PSR
	MRS		r4, SPSR		; YYY+
	STMFD	sp!, {r4}		; YYY+ save SPSR


	; OSPrioCur = OSPrioHighRdy
	LDR	r4, addr_OSPrioCur
	LDR	r5, addr_OSPrioHighRdy
	LDRB	r6, [r5]        ;[r5]->r6
	STRB	r6, [r4]        ;r6->[r4]
	
	; Get current task TCB address
	LDR	r4, addr_OSTCBCur   ;sp->OSTCBCur.OSTCBStkPtr; 
	LDR	r5, [r4]
	STR	sp, [r5]		    ; store sp in preempted tasks's TCB;sp->[r5]

	; Get highest priority task TCB address
	LDR	r6, addr_OSTCBHighRdy
	LDR	r6, [r6]
	LDR	sp, [r6]		    ; get new task's stack pointer;sp<-[r6]

	; OSTCBCur = OSTCBHighRdy
	STR	r6, [r4]		    ; set new current task TCB address

	LDMFD	sp!, {r4}		; YYY+                        ;pop r4
;	AND		r4, r4, #0xFFFFFF20
;	ORR		r4, r4, #0x13
	MSR	SPSR_cxsf, r4		; YYY+                        ;PSR=r4
	LDMFD	sp!, {r4}		; YYY+                        ;pop r4
;	AND		r4, r4, #0xFFFFFF20
;	ORR		r4, r4, #0x13
	MSR	CPSR_cxsf, r4		; YYY+                        ;CPSR_cxsf=r4
    LDMFD	sp!, {r0-r12, lr, pc}	; YYY+                ;pop to r0-r12,lr,pc
;	LDMFD sp!, {r0-r12, lr} ; YYY+
;    LDMFD sp!, {pc}^ ; 符号^的意思是从堆栈掸出一个数给PC,并且
                     ; 将SPSR拷贝到CPSR再返回.


	


SAVED_LR		DCD		0


;	void DisableInt(void)
;	void EnableInt(void)
;
;	Disable and enable IRQ and FIQ preserving current CPU mode.
;
	EXPORT	ARMDisableInt
ARMDisableInt
	STMDB	sp!, {r0}
	MRS		r0, CPSR
	ORR		r0, r0, #NoInt
	MSR		CPSR_cxsf, r0
	LDMIA	sp!, {r0}
	MOV	pc, lr
    

	EXPORT	ARMEnableInt
ARMEnableInt
	STMDB	sp!, {r0}
	MRS	r0, CPSR
	BIC	r0, r0, #NoInt
	MSR	CPSR_cxsf, r0
	LDMIA	sp!, {r0}
	MOV	pc, lr
    


;	void OS_TASK_SW(void)  called by OSSched()
;	
;	Perform a context switch.
;
;	On entry, OSTCBCur and OSPrioCur hold the current TCB and priority
;	and OSTCBHighRdy and OSPrioHighRdy contain the same for the task
;	to be switched to.
;
;	The following code assumes that the virtual memory is directly
;	mapped into  physical memory. If this is not true, the cache must 
;	be flushed at context switch to avoid address aliasing.
	EXPORT	OS_TASK_SW
OS_TASK_SW
	STMFD	sp!, {lr}		; save pc
	STMFD	sp!, {lr}		; save lr
	STMFD	sp!, {r0-r12}	; save register file and ret address
	MRS	r4, CPSR
	STMFD	sp!, {r4}		; save current PSR
	MRS	r4, SPSR		; YYY+
	STMFD	sp!, {r4}		; YYY+ save SPSR

	; OSPrioCur = OSPrioHighRdy
	LDR	r4, addr_OSPrioCur
	LDR	r5, addr_OSPrioHighRdy
	LDRB	r6, [r5]
	STRB	r6, [r4]
	
	; Get current task TCB address
	LDR	r4, addr_OSTCBCur
	LDR	r5, [r4]
	STR	sp, [r5]		; store sp in preempted tasks's TCB

	; Get highest priority task TCB address
	LDR	r6, addr_OSTCBHighRdy
	LDR	r6, [r6]
	LDR	sp, [r6]		; get new task's stack pointer

	; OSTCBCur = OSTCBHighRdy
	STR	r6, [r4]		; set new current task TCB address

	LDMFD	sp!, {r4}		; YYY+
	MSR	SPSR_cxsf, r4		; YYY+
	LDMFD	sp!, {r4}		; YYY+
	MSR	CPSR_cxsf, r4		; YYY+
	LDMFD	sp!, {r0-r12, lr, pc}	; YYY+



;	void OSStartHighRdy(void)
;	
;	Start the task with the highest priority;
;
	EXPORT	OSStartHighRdy
OSStartHighRdy
	LDR	r4, addr_OSTCBCur	; Get current task TCB address
	LDR	r5, addr_OSTCBHighRdy	; Get highest priority task TCB address

	LDR	r5, [r5]		; get stack pointer
	LDR	sp, [r5]		; switch to the new stack

	STR	r5, [r4]		; set new current task TCB address

	LDMFD	sp!, {r4}		; YYY
	MSR	SPSR_cxsf, r4
	LDMFD	sp!, {r4}		; get new state from top of the stack
	MSR	CPSR_cxsf, r4		; CPSR should be SVC32Mode
	LDMFD	sp!, {r0-r12, lr, pc }	; start the new task



;    EXPORT	EXINT4_ISR
;    
;EXINT4_ISR;
;	STMDB    sp!,{r0-r11,lr}      ;=push  lr, r0-r11-->sp
;
;	;interrupt disable(not nessary)
;	mrs		r0, CPSR            ; r0=CPSR
;	orr		r0, r0, #0x80		; and set IRQ disable flag
;	msr		CPSR_cxsf, R0       ; CPSR_cxsf=R0
;	
;	;End of interrupt
;	;(Clear pending bit of INTPEND that don't accessed it.)
;	;rEXTINTPND=0xf;		//clear EXTINTPND reg.
;	LDR R0, =EXTINTPND
;	MOV R1, #0x0f
;	STR R1, [R0]
;	
;	LDR	r0, =I_ISPC
;	LDR	r1, =BIT_EINT4567;clr INT_TIMER0 pending bit
;	STR	r1, [r0] 
;	
;	
;	LDR R0, =INTMSK
;	LDR R2, [R0]
;	LDR R1, =BIT_EINT4567
;	ORR R2, R1, R2
;	LDR R1, =BIT_TIMER0
;	ORR R2, R1, R2
;	STR R2, [R0]
;	
;	BL	IrqStart
;	
;	BL	EXINT4isr
;
;	BL	IrqFinish
;	
;	
;	
;	LDR		r0, =need_to_swap_context
;	LDR		r2, [r0]   ;ldr: memory to register,r2=need_to_swap_context
;	CMP		r2, #1
;	LDREQ	pc, =_CON_SW;if equal,then switch context
;	LDR     pc, =_NOT_CON_SW
;   
;   END
