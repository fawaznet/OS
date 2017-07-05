.section .text

.extern _ZN16InterruptManager15handleInterruptEhj


.macro HandleInterruptRequest interruptNumber
.global _ZN16InterruptManager26handleInterruptRequest\num\()Ev



#jump into the function
int_bottom:
	pusha
	pushl %ds
	pushl %es
	pushl %fs
	pushl %gs


	pushl %esp
	push (interruptNumber)
	call _ZN16InterruptManager15handleInterruptEhj
	# addl $5, %esp
	movl %eax, %esp


	popl %gs
	popl %fs
	popl %es
	popl %ds
	popa

	iret

.data
	interruptNumber: .byte 0
