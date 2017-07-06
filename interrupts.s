.set IRQ_BASE, 0x20

.section .text

.extern _ZN16InterruptManager15handleInterruptEhj

# exception
.macro HandleException num
.global _ZN16InterruptManager16handleException\num\()Ev #puplish outside
	movb $/num, (interruptNumber)
	jmp int_bottom
.endm	#end macro


# these create an implmintation for functons in interrupts.h
.macro HandleInterruptRequest num
.global _ZN16InterruptManager26handleInterruptRequest\num\()Ev #puplish outside
	movb $/num + IRQ_BASE, (interruptNumber)
	jmp int_bottom
.endm	#end macro

#
HandleInterruptRequest 0x00
HandleInterruptRequest 0x01



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
