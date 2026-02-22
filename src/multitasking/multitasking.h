#ifndef MULTITASKING_H
#define MULTITASKING_H

#include <stdint.h>

#define TASK_MAX_MAPPED_PAGES 16

typedef struct
{
    void *addr_program;
    uint32_t entry_virtual_address;
    uint32_t *page_directory;
    uint32_t user_stack_physical_base;
    uint32_t mapped_page_count;
    uint32_t mapped_virtual_pages[TASK_MAX_MAPPED_PAGES];
    uint32_t mapped_physical_pages[TASK_MAX_MAPPED_PAGES];
    uint32_t *start_stack_pointer;
    uint32_t *stack_pointer;
    uint32_t *stack_base_pointer;
    uint32_t pid;
    int end_task;
    char *task_name;
} Task;

void change_tasks();
void multitasking_init();
void create_task(void *addr_program, uint32_t entry_virtual_address, char *filename, int program_length);
void set_old_ptr();
void quit_app(uint32_t pid);
void printPID();
void quit_curApp();
Task get_task(int n);
void *get_prgm_offset();
extern unsigned int tasks_n;
extern int cur_task;
int return_pid(int p);

extern uint32_t *new_stack_pointer;
extern uint32_t *new_stack_base_pointer;
extern uint32_t *new_page_directory;

extern uint32_t *save_current_task_esp;
extern uint32_t *save_current_task_ebp;

extern void (*application_adress)();

#endif
