#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

#define GAYOS_TOTAL_INTERRUPTS 256

#define GAYOS_ALL_OK 0
#define EIO 1
#define EINVARG 2
#define ENOMEM 3
#define EBADPATH 4
#define EFSNOTUS 5
#define ERDONLY 6

#define GAYOS_SECTOR_SIZE 512
#define GAYOS_MAX_FILE 10

#define OS_EXTERN_FUNCTIONS 12
#endif