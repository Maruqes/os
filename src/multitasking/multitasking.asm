
global initialize_task
global change_task_asm

initialize_task:
    int 24
    ret

change_task_asm:
    int 25
    ret

