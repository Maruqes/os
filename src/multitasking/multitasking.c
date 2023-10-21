#include <stdint.h>
#include "multitasking.h"
#include "kernel.h"
#include "terminal/terminal.h"
#include "memory/memory.h"
#include "idt/idt.h"
#include "string/string.h"
#include "window_management/window_management.h"

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

int is_going_to_create_task;

void multitasking_init()
{
    cur_task = 0;
    tasks_n = 0;
    tasks_pid = 0;
    is_going_to_create_task = 0;
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

    memcpy(task_pos, next_task_pos, sizeof(Task) * (tasks_n - (n + 1)));

    memset(tasks_save + ((tasks_n - 1) * sizeof(Task)), 0, sizeof(Task));

    tasks_n -= 1;
}

void end_task()
{
    Task task = get_task(cur_task);
    if (task.end_task == 0)
    {
        print("ERROR ENDING TASK");
        return;
    }
    free(task.addr_program);
    free(task.start_stack_pointer);
    delete_task(cur_task);
}

void create_task(void *addr_program, uint16_t p_offset, char *filename, int program_length)
{
    Task task;
    task.addr_program = addr_program;
    task.p_offset = p_offset;

    task.start_stack_pointer = (uint32_t *)zalloc(4098);

    task.stack_pointer = task.start_stack_pointer;
    task.stack_base_pointer = task.stack_pointer + 4098;

    task.pid = tasks_pid;
    tasks_pid += 1;

    task.end_task = 0;

    task.task_name = zalloc(MAX_FILENAME_LENGTH);
    memcpy(task.task_name, filename, MAX_FILENAME_LENGTH);

    set_task(task, tasks_n);

    is_going_to_create_task = cur_task;
    cur_task = tasks_n;
    tasks_n++;

    new_stack_pointer = task.stack_pointer;
    new_stack_base_pointer = task.stack_base_pointer;
    application_adress = (addr_program + p_offset);
    cur_addr_program = application_adress;

    new_line();
    initialize_task();
}

int return_pid(int p)
{
    return get_task(p).pid;
}

void change_tasks()
{

    if (tasks_n < 2)
        return;

    // can change task... continue

    finish_int_without_tasks();
    disable_int();
    is_going_to_create_task = -1;

    if (cur_task == tasks_n - 1)
    {
        cur_task = 0;
    }
    else
    {
        cur_task = cur_task + 1;
    }

    Task t_change = get_task(cur_task);

    if (t_change.end_task == 1) // quit tasks
    {
        new_line();
        print("DELETING: ");
        print(digit_to_number(t_change.pid));
        new_line();
        end_task();
        if (tasks_n < 2)
            return;
        if (cur_task >= tasks_n - 1)
        {
            cur_task = 0;
        }
        t_change = get_task(cur_task);
    }

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

    if (is_going_to_create_task != -1)
    {
        save_task_n = is_going_to_create_task;
    }

    Task task = get_task(save_task_n);
    task.stack_pointer = save_current_task_esp;
    task.stack_base_pointer = save_current_task_ebp;
    set_task(task, save_task_n);
}

void quit_app(uint32_t pid)
{
    disable_int();
    for (int i = 0; i < tasks_n; i++) // get correct task
    {
        Task task = get_task(i);
        if (task.pid == pid)
        {
            task.end_task = 1;
            set_task(task, i);
            break;
        }
        if (i == tasks_n - 1)
        {
            print("COULD NOT FIND TASK");
            return;
        }
    }
    enable_int();
}

void quit_curApp()
{
    Task t = get_task(cur_task);
    quit_app(t.pid);
}

void printPID()
{
    new_line();
    for (int i = 0; i < tasks_n; i++)
    {
        Task task = get_task(i);
        print("PID: ");
        print(digit_to_number(task.pid));
        print("   PN: ");
        print(task.task_name);
        new_line();
    }
    new_line();
    print("NTASK: ");
    print(digit_to_number(tasks_n));
}

void *get_prgm_offset()
{
    return cur_addr_program;
}