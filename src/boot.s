bits 32

section .multiboot
align 4

dd 0x1BADB002
dd 0x04
dd -(0x1BADB002 + 0x04)

dd 0 ; address fields (unused, because bit 16 is not set)
dd 0
dd 0
dd 0
dd 0

dd 0 ; graphics mode request
dd 800
dd 600
dd 32

section .text
global _start
global start
extern kernel_main

_start:
start:
    cli
    push ebx ; multiboot info pointer
    call kernel_main

.hang:
    hlt
    jmp .hang
