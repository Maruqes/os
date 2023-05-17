#ifndef EXEC_H
#define EXEC_H
#include <stdint.h>
typedef struct
{
    uint8_t e_ident[16];  /* Magic number and other info */
    uint16_t e_type;      /* Object file type */
    uint16_t e_machine;   /* Architecture */
    uint32_t e_version;   /* Object file version */
    uint32_t e_entry;     /* Entry point virtual address */
    uint32_t e_phoff;     /* Program header table file offset */
    uint32_t e_shoff;     /* Section header table file offset */
    uint32_t e_flags;     /* Processor-specific flags */
    uint16_t e_ehsize;    /* ELF header size in bytes */
    uint16_t e_phentsize; /* Program header table entry size */
    uint16_t e_phnum;     /* Program header table entry count */
    uint16_t e_shentsize; /* Section header table entry size */
    uint16_t e_shnum;     /* Section header table entry count */
    uint16_t e_shstrndx;  /* Section header string table index */
} Elf32Hdr;

void execute(char *fileName);
extern void *addr_program;
extern uint16_t p_offset;

#endif