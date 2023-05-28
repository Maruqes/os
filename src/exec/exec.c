#include "exec.h"
#include "kernel.h"
#include "disk/disk.h"
#include "memory/memory.h"
#include "terminal/terminal.h"
#include "multitasking/multitasking.h"
#include "idt/idt.h"

void execute(char *fileName)
{
    void *addr_program;
    uint16_t p_offset;

    struct File tfile = get_file_by_name(fileName);
    print("EXECUTING: ");
    print(fileName);
    if (tfile.last_sector == -1)
    {
        print("FILE does not exist");
        return;
    }
    addr_program = zalloc(tfile.number_of_sectors * 512);
    int a = read_file(tfile.name, addr_program, tfile.number_of_sectors * 512);
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

    create_task(addr_program, p_offset);
}