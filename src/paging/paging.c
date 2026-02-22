#include "paging.h"
#include "memory/memory.h"
#include <stdint.h>

#define PAGING_PAGE_DIRECTORY_ENTRIES 1024U
#define PAGING_4MB_PAGE_SIZE_SHIFT 22
#define PAGING_CR0_PAGING_FLAG 0x80000000U
#define PAGING_CR4_PSE_FLAG 0x00000010U
#define PAGING_4MB_ADDRESS_MASK 0xFFC00000U
#define PAGING_FLAGS_MASK 0x00000FFFU
#define PAGING_PROCESS_FRAME_POOL_START 0x02000000U
#define PAGING_PROCESS_FRAME_COUNT 24U

static uint32_t kernel_page_directory[PAGING_PAGE_DIRECTORY_ENTRIES] __attribute__((aligned(4096)));
static uint32_t *current_page_directory = kernel_page_directory;
static uint8_t process_frame_used[PAGING_PROCESS_FRAME_COUNT];
static int paging_initialized = 0;

static uint32_t paging_directory_index(uint32_t virtual_address)
{
    return virtual_address >> PAGING_4MB_PAGE_SIZE_SHIFT;
}

static int paging_is_4mb_aligned(uint32_t address)
{
    return (address % PAGING_PAGE_4MB_SIZE) == 0;
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

uint32_t paging_get_cr3(void)
{
    uint32_t cr3 = 0;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

void paging_load_cr3(uint32_t *directory)
{
    if (!directory)
    {
        return;
    }

    current_page_directory = directory;
    asm volatile("mov %0, %%cr3" : : "r"(directory) : "memory");
}

void paging_reload_cr3(void)
{
    if (!current_page_directory)
    {
        return;
    }

    asm volatile("mov %0, %%cr3" : : "r"(current_page_directory) : "memory");
}

uint32_t *paging_get_kernel_directory(void)
{
    return kernel_page_directory;
}

uint32_t *paging_get_current_directory(void)
{
    return current_page_directory;
}

void paging_clone_kernel_space(uint32_t *directory)
{
    if (!directory)
    {
        return;
    }

    for (uint32_t i = 0; i < PAGING_PAGE_DIRECTORY_ENTRIES; i++)
    {
        directory[i] = kernel_page_directory[i];
    }
}

void *paging_alloc_process_frame_4mb(void)
{
    for (uint32_t i = 0; i < PAGING_PROCESS_FRAME_COUNT; i++)
    {
        if (!process_frame_used[i])
        {
            process_frame_used[i] = 1;
            return (void *)(PAGING_PROCESS_FRAME_POOL_START + (i * PAGING_PAGE_4MB_SIZE));
        }
    }
    return 0;
}

void paging_free_process_frame_4mb(void *physical_address)
{
    uint32_t address = (uint32_t)physical_address;
    if (address < PAGING_PROCESS_FRAME_POOL_START)
    {
        return;
    }

    uint32_t delta = address - PAGING_PROCESS_FRAME_POOL_START;
    if ((delta % PAGING_PAGE_4MB_SIZE) != 0)
    {
        return;
    }

    uint32_t index = delta / PAGING_PAGE_4MB_SIZE;
    if (index >= PAGING_PROCESS_FRAME_COUNT)
    {
        return;
    }

    process_frame_used[index] = 0;
}

uint32_t *paging_create_page_directory(void)
{
    uint32_t *directory = (uint32_t *)zalloc(sizeof(uint32_t) * PAGING_PAGE_DIRECTORY_ENTRIES);
    if (!directory)
    {
        return 0;
    }

    paging_clone_kernel_space(directory);
    return directory;
}

void paging_free_page_directory(uint32_t *directory)
{
    if (!directory || directory == kernel_page_directory)
    {
        return;
    }
    free(directory);
}

int paging_map_4mb_page(uint32_t *directory, uint32_t virtual_address, uint32_t physical_address, uint32_t flags)
{
    if (!directory)
    {
        return -1;
    }

    if (!paging_is_4mb_aligned(virtual_address) || !paging_is_4mb_aligned(physical_address))
    {
        return -1;
    }

    uint32_t index = paging_directory_index(virtual_address);
    if (index >= PAGING_PAGE_DIRECTORY_ENTRIES)
    {
        return -1;
    }

    directory[index] = (physical_address & PAGING_4MB_ADDRESS_MASK) |
                       (flags & PAGING_FLAGS_MASK) |
                       PAGING_FLAG_PRESENT |
                       PAGING_FLAG_4MB;

    if (directory == current_page_directory)
    {
        paging_reload_cr3();
    }

    return 0;
}

void paging_unmap_4mb_page(uint32_t *directory, uint32_t virtual_address)
{
    if (!directory || !paging_is_4mb_aligned(virtual_address))
    {
        return;
    }

    uint32_t index = paging_directory_index(virtual_address);
    if (index >= PAGING_PAGE_DIRECTORY_ENTRIES)
    {
        return;
    }

    directory[index] = 0;
    if (directory == current_page_directory)
    {
        paging_reload_cr3();
    }
}

int paging_map_default_process_layout(uint32_t *directory, uint32_t program_physical_base, uint32_t stack_physical_base)
{
    if (!directory)
    {
        return -1;
    }

    if (paging_map_4mb_page(directory,
                            PAGING_LAYOUT_USER_PROGRAM_BASE,
                            program_physical_base,
                            PAGING_FLAG_WRITEABLE | PAGING_FLAG_USER) < 0)
    {
        return -1;
    }

    if (paging_map_4mb_page(directory,
                            PAGING_LAYOUT_USER_STACK_BASE,
                            stack_physical_base,
                            PAGING_FLAG_WRITEABLE | PAGING_FLAG_USER) < 0)
    {
        paging_unmap_4mb_page(directory, PAGING_LAYOUT_USER_PROGRAM_BASE);
        return -1;
    }

    return 0;
}

void paging_init(void)
{
    if (paging_initialized)
    {
        return;
    }

    for (uint32_t i = 0; i < PAGING_PROCESS_FRAME_COUNT; i++)
    {
        process_frame_used[i] = 0;
    }

    for (uint32_t i = 0; i < PAGING_PAGE_DIRECTORY_ENTRIES; i++)
    {
        kernel_page_directory[i] = (i << PAGING_4MB_PAGE_SIZE_SHIFT) |
                                   PAGING_FLAG_4MB |
                                   PAGING_FLAG_PRESENT |
                                   PAGING_FLAG_WRITEABLE;
    }

    paging_enable_4mb_pages();
    paging_load_cr3(kernel_page_directory);
    paging_enable();
    paging_initialized = 1;
}
