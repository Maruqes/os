
global initialize_task
global change_task_asm

extern cur_addr_program

initialize_task:
    int 24
    ret

change_task_asm:
    int 25
    ret

