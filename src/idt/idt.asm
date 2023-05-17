section .asm

extern int21h_handler
extern int32h_handler
extern finish_int

extern input_str
extern int_malloc_addr
extern framebuffer
extern sleep



global int21h
global int32h
global idt_load
global no_interrupt
global enable_int
global disable_int


extern idt_printINT
extern idt_quit_appINT
extern idt_inputINT
extern idt_printADDRINT
extern int_mallocINT
extern int_freeINT
extern int_screenINT
extern int_sleepINT

global printINT
global quit_appINT
global inputINT
global printADDRINT
global mallocINT
global freeINT
global screenINT
global sleepINT


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

int32h:
    cli
    pushad
    call int32h_handler
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


printINT:
    ;pushad
    push eax
    call idt_printINT
    pop eax
    ;popad
    iret

quit_appINT:
    call idt_quit_appINT
    iret

inputINT:
    push eax
    call idt_inputINT
    pop eax
    mov eax, [input_str]
    iret

printADDRINT:
    ;pushad
    push eax
    call idt_printADDRINT
    pop eax
    ;popad
    iret

mallocINT:
    push eax
    call int_mallocINT
    pop eax
    call return_malloc_value
    iret

return_malloc_value:
    mov eax, [int_malloc_addr]
    ret

freeINT:
    push eax
    call int_freeINT
    pop eax
    iret

screenINT:
    call int_screenINT
    mov eax, [framebuffer]
    iret

sleepINT:
    ;call int_screenINT
    mov eax, sleep
    iret

