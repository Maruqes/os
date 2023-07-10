section .asm

extern int21h_handler
extern int32h_handler
extern finish_int

extern input_str
extern int_malloc_addr
extern framebuffer
extern CURRENT_FUNCTION
extern result_screen_test




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
extern int_screenINT
extern int_give_OS_FUNCTIONS
extern int_test_screent_INT
extern int_debbugINT
extern int_debbugINT2

extern change_tasks
extern main_stack


global printINT
global quit_appINT
global inputINT
global printADDRINT
global screenINT
global get_addrINT
global test_screent_INT
global debbugINT
global create_task_int
global change_task_int


extern interrupts_enabled


enable_int:
    mov eax, 1
    mov [interrupts_enabled], eax
	sti
    ret

disable_int:
    mov eax, 0
    mov [interrupts_enabled], eax
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
    call disable_int
    pushad
    call int21h_handler
    popad
    call enable_int
    iret

int32h:
    call disable_int
    pushad
    call int32h_handler
    popad
    call enable_int
    iret

no_interrupt:
    call disable_int
    pushad
    call finish_int
    popad
    call enable_int
    iret


printINT:
    ;pushad
    push eax
    call idt_printINT
    pop eax
    ;popad
    iret

quit_appINT:
    call disable_int
    call idt_quit_appINT
    call enable_int
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



screenINT:
    call int_screenINT
    mov eax, [framebuffer]
    iret

extern application_adress

debbugINT:
    iret


;MULTI TASKING SYSTEM
extern new_stack_pointer
extern new_stack_base_pointer

extern save_current_task_esp
extern save_current_task_ebp


extern set_old_ptr

create_task_int:
    mov [save_current_task_esp], esp
    mov [save_current_task_ebp], ebp
    call set_old_ptr
    
    mov eax, [esp];copy old stack to create task
    mov ebx, [esp+4]
    mov ecx, [esp+8]
    
    mov esp, [new_stack_pointer]
    mov ebp, [new_stack_base_pointer]
    push ecx
    push ebx
    push eax

    mov eax, [application_adress]
    mov [esp], eax
    iret

change_task_int:
    mov [save_current_task_esp], esp
    mov [save_current_task_ebp], ebp
    call set_old_ptr

    mov eax, [new_stack_pointer]
    mov ebx, [new_stack_base_pointer]
    mov esp, eax
    mov ebp, ebx
    iret

;MULTI TASKING SYSTEM OVER


get_addrINT:
    call int_give_OS_FUNCTIONS
    mov eax, [CURRENT_FUNCTION]
    iret

test_screent_INT:
    call int_test_screent_INT
    mov eax, [result_screen_test]
    iret
