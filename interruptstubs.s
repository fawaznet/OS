.set IRQ_BASE, 0x20

.section .text

.extern _ZN16InterruptManager15HandleInterruptEhj

.global _ZN16InterruptManager22IgnoreInterruptRequestEv

# exception
.macro HandleException num
.global _ZN16InterruptManager19HandleException\num\()Ev #puplish outside
_ZN16InterruptManager19HandleException\num\()Ev:
	movb $\num, (interruptNumber)
	jmp int_bottom
.endm	#end macro


# these create an implmintation for functons in interrupts.h
.macro HandleInterruptRequest num
#declare
.global _ZN16InterruptManager26HandleInterruptRequest\num\()Ev #puplish outside
_ZN16InterruptManager26HandleInterruptRequest\num\()Ev:
	movb $\num + IRQ_BASE, (interruptNumber)
	jmp int_bottom
.endm	#end macro

HandleException 0x00
HandleException 0x01

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
	call _ZN16InterruptManager15HandleInterruptEhj
	# addl $5, %esp
	movl %eax, %esp


	popl %gs
	popl %fs
	popl %es
	popl %ds
	popa

_ZN16InterruptManager22IgnoreInterruptRequestEv:
	iret

.data
	interruptNumber: .byte 0
