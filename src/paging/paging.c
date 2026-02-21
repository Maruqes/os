#include "paging.h"
#include <stdint.h>

#define PAGING_PAGE_DIRECTORY_ENTRIES 1024
#define PAGING_4MB_PAGE_FLAG 0x80
#define PAGING_PRESENT_FLAG 0x01
#define PAGING_WRITEABLE_FLAG 0x02
#define PAGING_4MB_PAGE_SIZE_SHIFT 22
#define PAGING_CR0_PAGING_FLAG 0x80000000
#define PAGING_CR4_PSE_FLAG 0x00000010

static uint32_t page_directory[PAGING_PAGE_DIRECTORY_ENTRIES] __attribute__((aligned(4096)));
static int paging_initialized = 0;

static void paging_load_directory(uint32_t *directory)
{
    asm volatile("mov %0, %%cr3" : : "r"(directory) : "memory");
}

static void paging_enable_4mb_pages(void)
{
    uint32_t cr4 = 0;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= PAGING_CR4_PSE_FLAG;
    asm volatile("mov %0, %%cr4" : : "r"(cr4));
}

static void paging_enable(void)
{
    uint32_t cr0 = 0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= PAGING_CR0_PAGING_FLAG;
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}

int paging_is_enabled(void)
{
    uint32_t cr0 = 0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    return (cr0 & PAGING_CR0_PAGING_FLAG) != 0;
}

void paging_init(void)
{
    if (paging_initialized)
    {
        return;
    }

    for (uint32_t i = 0; i < PAGING_PAGE_DIRECTORY_ENTRIES; i++)
    {
        page_directory[i] = (i << PAGING_4MB_PAGE_SIZE_SHIFT) |
                            PAGING_4MB_PAGE_FLAG |
                            PAGING_PRESENT_FLAG |
                            PAGING_WRITEABLE_FLAG;
    }

    paging_enable_4mb_pages();
    paging_load_directory(page_directory);
    paging_enable();
    paging_initialized = 1;
}
