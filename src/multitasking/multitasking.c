#include <stdint.h>
#include "multitasking.h"
#include "kernel.h"
#include "terminal/terminal.h"
#include "memory/memory.h"
#include "idt/idt.h"
#include "string/string.h"
#include "window_management/window_management.h"
#include "paging/paging.h"
#include "exec/exec.h"

#define ELF_PROGRAM_HEADER_TYPE_LOAD 1U
#define PAGE_4MB_MASK (PAGING_PAGE_4MB_SIZE - 1U)

extern void initialize_task();
extern void change_task_asm();

unsigned int tasks_n;
uint32_t tasks_pid;
void *tasks_save;
int cur_task;

uint32_t *new_stack_pointer;
uint32_t *new_stack_base_pointer;
uint32_t *new_page_directory;

uint32_t *old_p;
uint32_t *old_ebp;

uint32_t *save_current_task_esp;
uint32_t *save_current_task_ebp;

void (*application_adress)();

int is_going_to_create_task;

static uint32_t align_down_4mb(uint32_t value)
{
    return value & ~PAGE_4MB_MASK;
}

static uint32_t align_up_4mb(uint32_t value)
{
    if ((value & PAGE_4MB_MASK) == 0)
    {
        return value;
    }
    return (value + PAGE_4MB_MASK) & ~PAGE_4MB_MASK;
}

static int task_find_page_index(Task *task, uint32_t virtual_page_base)
{
    for (uint32_t i = 0; i < task->mapped_page_count; i++)
    {
        if (task->mapped_virtual_pages[i] == virtual_page_base)
        {
            return (int)i;
        }
    }
    return -1;
}

static int task_add_page_mapping(Task *task, uint32_t virtual_page_base, uint32_t physical_page_base)
{
    if (task->mapped_page_count >= TASK_MAX_MAPPED_PAGES)
    {
        return -1;
    }

    task->mapped_virtual_pages[task->mapped_page_count] = virtual_page_base;
    task->mapped_physical_pages[task->mapped_page_count] = physical_page_base;
    task->mapped_page_count++;
    return 0;
}

static int task_map_4mb_page(Task *task, uint32_t virtual_page_base)
{
    if (task_find_page_index(task, virtual_page_base) >= 0)
    {
        return 0;
    }

    void *physical_page = paging_alloc_process_frame_4mb();
    if (!physical_page)
    {
        return -1;
    }

    memset(physical_page, 0x00, PAGING_PAGE_4MB_SIZE);
    if (paging_map_4mb_page(task->page_directory,
                            virtual_page_base,
                            (uint32_t)physical_page,
                            PAGING_FLAG_WRITEABLE | PAGING_FLAG_USER) < 0)
    {
        paging_free_process_frame_4mb(physical_page);
        return -1;
    }

    if (task_add_page_mapping(task, virtual_page_base, (uint32_t)physical_page) < 0)
    {
        paging_unmap_4mb_page(task->page_directory, virtual_page_base);
        paging_free_process_frame_4mb(physical_page);
        return -1;
    }

    return 0;
}

static uint32_t task_virtual_to_physical(Task *task, uint32_t virtual_address)
{
    uint32_t virtual_page_base = align_down_4mb(virtual_address);
    int page_index = task_find_page_index(task, virtual_page_base);
    if (page_index < 0)
    {
        return 0;
    }

    uint32_t offset = virtual_address - virtual_page_base;
    return task->mapped_physical_pages[page_index] + offset;
}

static int task_copy_segment(Task *task, void *program_data, int program_length, Elf32Phdr *phdr)
{
    if (!task || !program_data || !phdr)
    {
        return -1;
    }

    if (phdr->p_filesz == 0)
    {
        return 0;
    }

    if (phdr->p_offset > (uint32_t)program_length)
    {
        return -1;
    }

    if (phdr->p_filesz > ((uint32_t)program_length - phdr->p_offset))
    {
        return -1;
    }

    uint32_t copied = 0;
    while (copied < phdr->p_filesz)
    {
        uint32_t virtual_address = phdr->p_vaddr + copied;
        uint32_t physical_address = task_virtual_to_physical(task, virtual_address);
        if (physical_address == 0)
        {
            return -1;
        }

        uint32_t offset_in_page = virtual_address & PAGE_4MB_MASK;
        uint32_t left_in_page = PAGING_PAGE_4MB_SIZE - offset_in_page;
        uint32_t left_in_segment = phdr->p_filesz - copied;
        uint32_t chunk = left_in_page < left_in_segment ? left_in_page : left_in_segment;

        memcpy((void *)physical_address, (char *)program_data + phdr->p_offset + copied, chunk);
        copied += chunk;
    }

    return 0;
}

static int task_map_elf_segments(Task *task, void *program_data, int program_length)
{
    if (!task || !program_data || program_length < (int)sizeof(Elf32Hdr))
    {
        return -1;
    }

    Elf32Hdr elf_header;
    memcpy(&elf_header, program_data, sizeof(Elf32Hdr));

    if (elf_header.e_phnum == 0)
    {
        return -1;
    }

    if (elf_header.e_phoff > (uint32_t)program_length)
    {
        return -1;
    }

    if (elf_header.e_phentsize < sizeof(Elf32Phdr))
    {
        return -1;
    }

    int load_segments_mapped = 0;

    for (uint32_t i = 0; i < elf_header.e_phnum; i++)
    {
        uint32_t phdr_offset = elf_header.e_phoff + (i * elf_header.e_phentsize);
        if (phdr_offset > (uint32_t)program_length ||
            phdr_offset + sizeof(Elf32Phdr) > (uint32_t)program_length)
        {
            return -1;
        }

        Elf32Phdr phdr;
        memcpy(&phdr, (char *)program_data + phdr_offset, sizeof(Elf32Phdr));

        if (phdr.p_type != ELF_PROGRAM_HEADER_TYPE_LOAD || phdr.p_memsz == 0)
        {
            continue;
        }

        uint32_t segment_start = phdr.p_vaddr;
        uint32_t segment_end = phdr.p_vaddr + phdr.p_memsz;

        if (segment_end < segment_start)
        {
            return -1;
        }

        if (segment_start < PAGING_LAYOUT_USER_PROGRAM_BASE ||
            segment_end > PAGING_LAYOUT_USER_STACK_BASE)
        {
            return -1;
        }

        uint32_t first_page = align_down_4mb(segment_start);
        uint32_t end_page = align_up_4mb(segment_end);

        for (uint32_t page = first_page; page < end_page; page += PAGING_PAGE_4MB_SIZE)
        {
            if (task_map_4mb_page(task, page) < 0)
            {
                return -1;
            }
        }

        if (task_copy_segment(task, program_data, program_length, &phdr) < 0)
        {
            return -1;
        }

        load_segments_mapped = 1;
    }

    return load_segments_mapped ? 0 : -1;
}

static int task_map_virtual_stack(Task *task)
{
    if (!task)
    {
        return -1;
    }

    void *stack_physical = paging_alloc_process_frame_4mb();
    if (!stack_physical)
    {
        return -1;
    }

    memset(stack_physical, 0x00, PAGING_PAGE_4MB_SIZE);

    if (paging_map_4mb_page(task->page_directory,
                            PAGING_LAYOUT_USER_STACK_BASE,
                            (uint32_t)stack_physical,
                            PAGING_FLAG_WRITEABLE | PAGING_FLAG_USER) < 0)
    {
        paging_free_process_frame_4mb(stack_physical);
        return -1;
    }

    task->user_stack_physical_base = (uint32_t)stack_physical;
    task->start_stack_pointer = (uint32_t *)PAGING_LAYOUT_USER_STACK_BASE;
    task->stack_pointer = (uint32_t *)PAGING_LAYOUT_USER_STACK_TOP;
    task->stack_base_pointer = (uint32_t *)PAGING_LAYOUT_USER_STACK_TOP;
    return 0;
}

static void task_release_address_space(Task *task)
{
    if (!task)
    {
        return;
    }

    for (uint32_t i = 0; i < task->mapped_page_count; i++)
    {
        if (task->page_directory && task->mapped_virtual_pages[i] != 0)
        {
            paging_unmap_4mb_page(task->page_directory, task->mapped_virtual_pages[i]);
        }

        if (task->mapped_physical_pages[i] != 0)
        {
            paging_free_process_frame_4mb((void *)task->mapped_physical_pages[i]);
        }
    }

    if (task->page_directory && task->user_stack_physical_base != 0)
    {
        paging_unmap_4mb_page(task->page_directory, PAGING_LAYOUT_USER_STACK_BASE);
    }

    if (task->user_stack_physical_base != 0)
    {
        paging_free_process_frame_4mb((void *)task->user_stack_physical_base);
    }

    if (task->page_directory)
    {
        paging_free_page_directory(task->page_directory);
    }
}

void multitasking_init()
{
    cur_task = 0;
    tasks_n = 0;
    tasks_pid = 0;
    is_going_to_create_task = 0;
    tasks_save = zalloc(sizeof(Task) * 256);
    new_page_directory = paging_get_kernel_directory();
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

    if (task.task_name)
    {
        free(task.task_name);
    }

    if (task.addr_program)
    {
        free(task.addr_program);
    }

    task_release_address_space(&task);
    delete_task(cur_task);
}

void create_task(void *addr_program, uint32_t entry_virtual_address, char *filename, int program_length)
{
    Task task;
    memset(&task, 0x00, sizeof(Task));
    task.addr_program = addr_program;
    task.entry_virtual_address = entry_virtual_address;

    if (task.entry_virtual_address < PAGING_LAYOUT_USER_PROGRAM_BASE ||
        task.entry_virtual_address >= PAGING_LAYOUT_USER_STACK_BASE)
    {
        print("INVALID ELF ENTRY VADDR");
        free(task.addr_program);
        return;
    }

    task.page_directory = paging_create_page_directory();
    if (!task.page_directory)
    {
        print("FAILED TO CREATE PAGE DIRECTORY");
        free(task.addr_program);
        return;
    }

    if (task_map_elf_segments(&task, task.addr_program, program_length) < 0)
    {
        print("FAILED TO MAP ELF SEGMENTS");
        task_release_address_space(&task);
        free(task.addr_program);
        return;
    }

    if (task_map_virtual_stack(&task) < 0)
    {
        print("FAILED TO MAP TASK STACK");
        task_release_address_space(&task);
        free(task.addr_program);
        return;
    }

    free(task.addr_program);
    task.addr_program = 0;

    task.pid = tasks_pid;
    tasks_pid += 1;
    task.end_task = 0;

    task.task_name = zalloc(MAX_FILENAME_LENGTH);
    if (!task.task_name)
    {
        print("FAILED TO ALLOC TASK NAME");
        task_release_address_space(&task);
        return;
    }
    memcpy(task.task_name, filename, MAX_FILENAME_LENGTH);

    set_task(task, tasks_n);

    is_going_to_create_task = cur_task;
    cur_task = tasks_n;
    tasks_n++;

    new_stack_pointer = task.stack_pointer;
    new_stack_base_pointer = task.stack_base_pointer;
    new_page_directory = task.page_directory;
    application_adress = (void *)task.entry_virtual_address;
    cur_addr_program = (void *)task.entry_virtual_address;

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

    if (t_change.end_task == 1)
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
    new_page_directory = t_change.page_directory;
    cur_addr_program = (void *)t_change.entry_virtual_address;
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
    for (int i = 0; i < tasks_n; i++)
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
    if (tasks_n == 0)
    {
        return 0;
    }

    Task task = get_task(cur_task);
    return (void *)task.entry_virtual_address;
}
