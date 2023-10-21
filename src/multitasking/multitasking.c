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

int number_of_changes = 0;
int number_of_changes2 = 0;

int rodata_addr = -1;
int rodata_size = -1;

void change_dot_data_addresses(char bytes[16], void *addr_program, int current_byte, int program_length)
{
    memcpy(bytes, (addr_program + current_byte), 16);
    if (bytes[8] == 0x11 && bytes[10] == 0x3) // addrs do .data (falta o read only data .rodata)
    {
        int change_address = 0;
        memcpy(&change_address, bytes, 4);

        int result_addr = (int)addr_program + change_address + 0x1000;
        for (int l = 0; l < program_length; l++)
        {
            int test_address;
            memcpy(&test_address, (addr_program + l), 4);
            if (test_address == change_address)
            {
                memcpy((addr_program + l), &result_addr, 4);
                number_of_changes++;
            }
        }
    }
}

void change_dot_rodata_addresses(char bytes[16], void *addr_program, int current_byte, int program_length)
{
    if (bytes[8] == 0x03 && bytes[10] == 0x02) //.rodata
    {

        int rodata_addr_test = -1;
        int rodata_addr_confirmation = -1;
        memcpy(&rodata_addr, bytes, 4);
        for (int l = 0; l < program_length; l++)
        {
            memcpy(&rodata_addr_test, (addr_program + current_byte + l), 4);

            if (rodata_addr == rodata_addr_test)
            {
                memcpy(&rodata_addr_confirmation, (addr_program + current_byte + l + 4), 4);
                if ((rodata_addr_confirmation - 0x1000) == rodata_addr)
                {
                    memcpy(&rodata_size, (addr_program + current_byte + l + 8), 4);
                    new_line();
                    print("RODATA FOUND");
                    break;
                }
            }
        }

        if (rodata_size == -1)
        {
            print("RODATA ERROR");
        }
    }
}

void apply_rodata_fixes(void *addr_program, int program_length)
{
    int rodata_next_addr;
    clear_screen();

    int is_count = 0;

    for (int n = 0; n < program_length; n++)
    {
        memcpy(&rodata_next_addr, (addr_program + n), 4);

        if (rodata_next_addr == rodata_addr)
        {
            rodata_next_addr = rodata_next_addr + (int)addr_program + 0x1000;
            memcpy((addr_program + n), &rodata_next_addr, 4);
            memset(&rodata_next_addr, 0x00, 4);
            number_of_changes2++;
        }
    }

    for (int l = 0; l < rodata_size - 1; l++)
    {
        if (*(char *)(addr_program + rodata_addr + l) == 0x00)
        {
            int addr_found = (int)(rodata_addr + l + 1);

            for (int n = 0; n < program_length; n++)
            {
                memcpy(&rodata_next_addr, (addr_program + n), 4);

                if (rodata_next_addr == addr_found)
                {
                    is_count++;
                    rodata_next_addr = rodata_next_addr + (int)addr_program + 0x1000;
                    memcpy((addr_program + n), &rodata_next_addr, 4);
                    memset(&rodata_next_addr, 0x00, 4);
                    number_of_changes2++;
                }
            }
            if (is_count == 0)
                continue;
            printN(addr_found);
            print("--");
            printN(is_count);
            print("    ");
            is_count = 0;
        }
    }
}

void fix_program_addresses(void *addr_program, int program_length)
{
    for (int i = 0; i < program_length; i++)
    {
        // check for "gcc version 1.13.0"
        if (*(char *)(addr_program + i) == 0x28 &&
            *(char *)(addr_program + i + 1) == 0x47 &&
            *(char *)(addr_program + i + 2) == 0x4E &&
            *(char *)(addr_program + i + 3) == 0x55 &&
            *(char *)(addr_program + i + 4) == 0x29 &&
            *(char *)(addr_program + i + 5) == 0x20 &&
            *(char *)(addr_program + i + 6) == 0x31 &&
            *(char *)(addr_program + i + 7) == 0x33 &&
            *(char *)(addr_program + i + 8) == 0x2E &&
            *(char *)(addr_program + i + 9) == 0x31 &&
            *(char *)(addr_program + i + 10) == 0x2E &&
            *(char *)(addr_program + i + 11) == 0x30)
        {
            new_line();
            print("VERION 1.13.0 FOUND");
            for (int j = 0; j < program_length - j; j++) // check for00 00 00 00   00 00 00 00   03 00 01 00   00   00   00 00
            {
                if (*(char *)(addr_program + i + j) == 0x00 &&
                    *(char *)(addr_program + i + j + 1) == 0x00 &&
                    *(char *)(addr_program + i + j + 2) == 0x00 &&
                    *(char *)(addr_program + i + j + 3) == 0x00 &&
                    *(char *)(addr_program + i + j + 4) == 0x00 &&
                    *(char *)(addr_program + i + j + 5) == 0x00 &&
                    *(char *)(addr_program + i + j + 6) == 0x00 &&
                    *(char *)(addr_program + i + j + 7) == 0x00 &&
                    *(char *)(addr_program + i + j + 8) == 0x03 &&
                    *(char *)(addr_program + i + j + 9) == 0x00 &&
                    *(char *)(addr_program + i + j + 10) == 0x01 &&
                    *(char *)(addr_program + i + j + 11) == 0x00 &&
                    *(char *)(addr_program + i + j + 12) == 0x00 &&
                    *(char *)(addr_program + i + j + 13) == 0x00 &&
                    *(char *)(addr_program + i + j + 14) == 0x00 &&
                    *(char *)(addr_program + i + j + 15) == 0x00)
                {
                    new_line();
                    print("RANDOM CODE  03 00 01  FOUND");
                    int k = 0;

                    new_line();
                    print("SEARCHING FOR ADDRESSES");
                    while (true)
                    {
                        if (*(char *)(addr_program + i + j + k) == 0x00 && // search for addresses
                            *(char *)(addr_program + i + j + k + 1) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 2) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 3) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 4) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 5) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 6) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 7) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 8) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 9) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 10) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 11) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 12) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 13) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 14) == 0x00 &&
                            *(char *)(addr_program + i + j + k + 15) == 0x00)
                        {
                            break;
                        }
                        else
                        {

                            // updatezada por causa das versoes do gcc, tens o add do "GCC: (GNU)" rumores dizem q 0x12 bytes dps começa a symbol table
                            char bytes[16];

                            change_dot_data_addresses(bytes, addr_program, i + j + (k * 16), program_length);

                            change_dot_rodata_addresses(bytes, addr_program, i + j + (k * 16), program_length);
                        }
                        k++;
                    }
                }
            }
        }
    }
    apply_rodata_fixes(addr_program, program_length);
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

    fix_program_addresses(addr_program, program_length);

    new_line();
    printN(number_of_changes);
    new_line();
    printN(number_of_changes2);

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