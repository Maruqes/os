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

test:
    int 21
    ret

test2:
    int 24
    ret

test3:
    int 25
    ret

test4:
    int 23
    ret

mensagem db 'Hello world', 0x00