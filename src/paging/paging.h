#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define PAGING_PAGE_4MB_SIZE 0x00400000U

#define PAGING_FLAG_PRESENT 0x001U
#define PAGING_FLAG_WRITEABLE 0x002U
#define PAGING_FLAG_USER 0x004U
#define PAGING_FLAG_4MB 0x080U

/*
 * Fixed virtual layout (4MB granularity):
 * - Kernel identity window: [0x00000000, 0x03FFFFFF]
 * - User program base:      0x40000000
 * - User stack:             [0x7FC00000, 0x80000000)
 */
#define PAGING_LAYOUT_KERNEL_IDENTITY_START 0x00000000U
#define PAGING_LAYOUT_KERNEL_IDENTITY_END 0x03FFFFFFU
#define PAGING_LAYOUT_USER_PROGRAM_BASE 0x40000000U
#define PAGING_LAYOUT_USER_STACK_TOP 0x80000000U
#define PAGING_LAYOUT_USER_STACK_SIZE PAGING_PAGE_4MB_SIZE
#define PAGING_LAYOUT_USER_STACK_BASE (PAGING_LAYOUT_USER_STACK_TOP - PAGING_LAYOUT_USER_STACK_SIZE)

void paging_init(void);
int paging_is_enabled(void);

uint32_t paging_get_cr3(void);
void paging_load_cr3(uint32_t *directory);
void paging_reload_cr3(void);

uint32_t *paging_get_kernel_directory(void);
uint32_t *paging_get_current_directory(void);
uint32_t *paging_create_page_directory(void);
void paging_free_page_directory(uint32_t *directory);
void paging_clone_kernel_space(uint32_t *directory);

void *paging_alloc_process_frame_4mb(void);
void paging_free_process_frame_4mb(void *physical_address);

int paging_map_4mb_page(uint32_t *directory, uint32_t virtual_address, uint32_t physical_address, uint32_t flags);
void paging_unmap_4mb_page(uint32_t *directory, uint32_t virtual_address);
int paging_map_default_process_layout(uint32_t *directory, uint32_t program_physical_base, uint32_t stack_physical_base);

#endif
