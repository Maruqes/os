section .asm

extern int21h_handler
extern finish_int

global int21h
global idt_load
global no_interrupt
global enable_int
global disable_int

enable_int:
	sti
    ret

disable_int:
    cli
    ret

idt_load:
    push ebp
    mov ebp, esp

    mov ebx, [ebp+8]
    lidt [ebx]
    pop ebp
    ret

int21h:
    cli
    pushad
    call int21h_handler
    popad
    sti
    iret

no_interrupt:
    cli
    pushad
    call finish_int
    popad
    sti
    iret