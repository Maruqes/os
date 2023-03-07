bits    32
section .text
align 4

dd 0x1BADB002
dd 0x04
dd -(0x1BADB002 + 0x04)

dd 0 ; skip some flags
dd 0
dd 0
dd 0
dd 0

dd 0 ; sets it to graphical mode
dd 800 ; sets the width
dd 600 ; sets the height
dd 32 ; sets the bits per pixel

push ebx
global start
extern kernel_main
start:
    cli
    
    call kernel_main
    hlt