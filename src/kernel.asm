section .asm

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

	mov al, 00010001b
    out 0x20, al

	mov al, 0x20
    out 0x21, al

	mov al, 00000001b
    out 0x21, al
    ret