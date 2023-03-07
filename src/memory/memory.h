#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#define HEAP_SECTOR_NUMBER 25600
#define HEAP_LENGTH 10485600
#define GAYOS_HEAP_ADDR 0x1000000
#define GAYOS_HEAP_TABLE_ADDR 0x00007E00

void *memset(void *ptr, int c, size_t size);
int memcmp(void *s1, void *s2, int count);
void *memcpy(void *dist, void *source, size_t size);
void *malloc(size_t size);
void *zalloc(int size);
void free(void *ptr);
void heap_init();

#endif