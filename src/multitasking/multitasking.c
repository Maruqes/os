#include <stdint.h>
#include "multitasking.h"
#include "kernel.h"
#include "terminal/terminal.h"
#include "memory/memory.h"
#include "idt/idt.h"
#include "string/string.h"

extern void initialize_task();
extern void change_task_asm();

unsigned int tasks_n;
uint32_t tasks_pid;
void *tasks_save;
int cur_task;

uint32_t *new_stack_pointer;
uint32_t *new_stack_base_pointer;

uint32_t *old_p;
uint32_t *old_ebp;

uint32_t *save_current_task_esp;
uint32_t *save_current_task_ebp;

void (*application_adress)();

void multitasking_init()
{
    cur_task = 0;
    tasks_n = 0;
    tasks_pid = 0;
    tasks_save = zalloc(sizeof(Task) * 256);
}

Task get_task(int n)
{
    Task t;
    memcpy(&t, tasks_save + (n * sizeof(Task)), sizeof(Task));
    return t;
}

void set_task(Task task, int n)
{
    memcpy(tasks_save + (n * sizeof(Task)), &task, sizeof(Task));
}

void delete_task(int n)
{
    void *task_pos = tasks_save + (n * sizeof(Task));
    void *next_task_pos = tasks_save + ((n + 1) * sizeof(Task));

    memcpy(task_pos, next_task_pos, sizeof(Task) * (tasks_n - (n + 2)));

    memset(tasks_save + ((tasks_n - 1) * sizeof(Task)), 0, sizeof(Task));

    tasks_n -= 1;
}

void create_task(void *addr_program, uint16_t p_offset)
{
    Task task;
    task.addr_program = addr_program;
    task.p_offset = p_offset;

    task.start_stack_pointer = (uint32_t *)zalloc(2048);

    task.stack_pointer = task.start_stack_pointer;
    task.stack_base_pointer = task.stack_pointer + 2048;

    task.pid = tasks_pid;
    tasks_pid += 1;

    set_task(task, tasks_n);

    cur_task = tasks_n;
    tasks_n++;

    new_stack_pointer = task.stack_pointer;
    new_stack_base_pointer = task.stack_base_pointer;
    application_adress = (addr_program + p_offset);
    cur_addr_program = application_adress;

    new_line();

    initialize_task();
}

void change_tasks()
{
    if (tasks_n < 2)
        return;

    if (cur_task == tasks_n - 1)
    {
        cur_task = 0;
    }
    else
    {
        cur_task = cur_task + 1;
    }

    Task t_change = get_task(cur_task);
    new_stack_pointer = t_change.stack_pointer;
    new_stack_base_pointer = t_change.stack_base_pointer;
    cur_addr_program = t_change.addr_program + t_change.p_offset;

    change_task_asm();
}

void set_old_ptr()
{
    if (tasks_n < 2)
        return;

    int save_task_n;
    if (cur_task == 0)
    {
        save_task_n = tasks_n - 1;
    }
    else
    {
        save_task_n = cur_task - 1;
    }

    Task task = get_task(save_task_n);
    task.stack_pointer = save_current_task_esp;
    task.stack_base_pointer = save_current_task_ebp;
    set_task(task, save_task_n);
}

void quit_app(uint32_t pid) // does now work
{
    disable_int();
    Task task;
    int i = 0;
    for (i = 0; i < tasks_n; i++) // get correct task
    {
        task = get_task(i);
        if (task.pid == pid)
        {
            break;
        }
        if (i == tasks_n - 1)
        {
            print("COULD NOT FIND TASK");
            return;
        }
    }
    free(task.addr_program);
    free(task.start_stack_pointer);
    delete_task(i);
    enable_int();
}
