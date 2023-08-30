global printASM
global quit_app
global inputASM
global printADDRASM
global get_screen_accessASM
global get_addrASM
global test_screen_bufferASM


printASM:
        int 16 ;CD 3C
        ret

quit_app:
        int 17
        ret

inputASM:
        int 18 
        ret

printADDRASM:
        int 19
        ret

get_screen_accessASM:
        int 22
        ret

get_addrASM:
        int 23
        ret

test_screen_bufferASM:
        int 20
        ret





.data:
;teste times 60 db 0
mensagem db 'Hello world', 0x00