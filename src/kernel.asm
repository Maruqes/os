section .asm

extern test

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
    mov eax, mensagem
    int 20 ;CD 3C
    ret;fiquei a tarde inteira / noite do dia anterior ai das 3 e 30 as 5 e 30 sem saber o porque de o interrupt crashar o os... faltava retornar... triste mas é a PUTA DA VIDA

mensagem db 'Hello world', 0x00