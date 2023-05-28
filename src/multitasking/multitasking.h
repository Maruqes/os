#ifndef MULTITASKING_H
#define MULTITASKING_H

#include <stdint.h>
typedef struct
{
    void *addr_program;
    uint32_t p_offset;
    uint32_t *stack_pointer;
    uint32_t *stack_base_pointer;
} Task;

void change_tasks();
void multitasking_init();
void create_task(void *addr_program, uint16_t p_offset);
void set_old_ptr();
extern unsigned int tasks_n;

extern uint32_t *new_stack_pointer;
extern uint32_t *new_stack_base_pointer;

extern uint32_t *save_current_task_esp;
extern uint32_t *save_current_task_ebp;

extern void (*application_adress)();

#endif