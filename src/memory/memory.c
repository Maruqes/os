#include "memory.h"
#include "idt/idt.h"
#include <stddef.h>
#include <stdint.h>

void *heap_start_address;
void *heap_table_address;

void *memset(void *ptr, int c, size_t size)
{
    char *c_ptr = (char *)ptr;
    for (int i = 0; i < size; i++)
    {
        c_ptr[i] = (char)c;
    }
    return ptr;
}

void *memcpy(void *dist, void *source, size_t size)
{
    char *c_ptr = (char *)dist;
    char *c_ptr2 = (char *)source;
    for (int i = 0; i < size; i++)
    {
        c_ptr[i] = c_ptr2[i];
    }
    return dist;
}

void *memcpy_prgm(void *dist, int source, size_t size)
{
    char *c_ptr = (char *)dist;
    char *c_ptr2 = (char *)(cur_addr_program + source);
    for (int i = 0; i < size; i++)
    {
        c_ptr[i] = c_ptr2[i];
    }
    return dist;
}

int memcmp(void *s1, void *s2, int count)
{
    char *c1 = s1;
    char *c2 = s2;
    while (count-- > 0)
    {
        if (*c1++ != *c2++)
        {
            return c1[-1] < c2[-1] ? -1 : 1;
        }
    }

    return 0;
}

void heap_init()
{
    heap_start_address = (void *)(GAYOS_HEAP_ADDR);
    heap_table_address = (void *)(GAYOS_HEAP_TABLE_ADDR);
    memset(heap_start_address, 0, HEAP_LENGTH);
    memset(heap_table_address, 0, HEAP_SECTOR_NUMBER);
}

size_t round_to_4096(size_t size)
{
    if ((size % 4096) == 0)
    {
        return size;
    }

    size_t val = size % 4096;
    size_t val2 = size;
    val2 = size - val;
    val2 = val2 + 4096;
    return val2;
}

void *malloc(size_t size)
{
    size = (round_to_4096(size));
    uint32_t sector_number = size / 4096;

    char *table_ptr = (char *)heap_table_address;
    for (int i = 0; i < HEAP_SECTOR_NUMBER; i++)
    {
        if (table_ptr[i] == 0x0)
        {
            int found = 0;
            for (int j = 0; j < sector_number; j++)
            {
                if (table_ptr[i + j] != 0x0)
                {
                    found = 1;
                }
            }

            if (found == 0)
            {
                for (int j = 0; j < sector_number; j++)
                {
                    table_ptr[i + j] = 0x01;
                }
                table_ptr[i] = 0x03;
                table_ptr[i + sector_number - 1] = 0x02;

                return heap_start_address + (4096 * i);
            }
        }
    }

    return (void *)-1;
}

void *zalloc(int size)
{
    void *ptr = malloc(size);
    if (!ptr)
        return 0;
    memset(ptr, 0x00, size);
    return ptr;
}

void free(void *ptr)
{
    char *table_ptr = (char *)heap_table_address;

    for (int i = 0; i < HEAP_SECTOR_NUMBER; i++)
    {
        if (heap_start_address + (4096 * i) == ptr)
        {

            for (int j = 0; j < HEAP_SECTOR_NUMBER; j++)
            {
                if (table_ptr[i + j] == 0x03)
                {
                    table_ptr[i + j] = 0x0;
                }
                else if (table_ptr[i + j] == 0x01)
                {
                    table_ptr[i + j] = 0x0;
                }
                else if (table_ptr[i + j] == 0x02)
                {
                    table_ptr[i + j] = 0x0;
                    return;
                }
            }
        }
    }
}
