section .asm

extern test_kernel_call
extern test
extern test2
extern test3
extern test4

global divide_zero
global setup


divide_zero:
    mov ax,0
    div ax
    ret

setup:
    
	in al, 0x92
    or al, 2
    out 0x92, al
    ret


mensagem db 'Hello world', 0x00