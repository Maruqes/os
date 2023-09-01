#include "exec.h"
#include "kernel.h"
#include "disk/disk.h"
#include "memory/memory.h"
#include "terminal/terminal.h"
#include "multitasking/multitasking.h"
#include "idt/idt.h"

void execute(char *fileName)
{
    disable_int();
    void *addr_program;
    uint16_t p_offset;

    struct File tfile = get_file_by_name(fileName);
    free(input_str);
    print("EXECUTING: ");
    print(fileName);
    if (tfile.last_sector == -1)
    {
        print("FILE does not exist");
        return;
    }
    int program_length = tfile.number_of_sectors * 512;
    addr_program = zalloc(program_length);
    int a = read_file(tfile.name, addr_program, program_length);
    if (a == -1)
    {
        print("cannot execute file");
        return;
    }
    Elf32Hdr elf_struct;
    memcpy(&elf_struct, addr_program, sizeof(elf_struct));
    if (elf_struct.e_ident[0] != 0x7F || elf_struct.e_ident[1] != 'E' || elf_struct.e_ident[2] != 'L' || elf_struct.e_ident[3] != 'F')
    {
        print(fileName);
        print(" is not an executable");
        return;
    }
    memcpy(&p_offset, addr_program + elf_struct.e_phoff + 4, 4);
    if (p_offset != 0x1000)
    {
        new_line();
        print("PROGRAM OFFSET IS NOT 0X1000");
        while (1)
        {
        }
    }
    create_task(addr_program, p_offset, tfile.name, (program_length));
}

// program crashes

void dot_data()
{
    clear_screen();
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDHT; j++)
        {
            put_pixel(j, i, 0xff0000);
        }
    }
    print(".DATA ERROR");
    while (1)
    {
    }
}

void dot_rodata()
{
    clear_screen();
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDHT; j++)
        {
            put_pixel(j, i, 0xff0000);
        }
    }
    print(".RODATA ERROR");
    while (1)
    {
    }
}