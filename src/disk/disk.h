#ifndef DISK_H
#define DISK_H
#include <stddef.h>
#include "config.h"
int disk_read_sector(int lba, void *buf);
int disk_write_sector(int lba, void *buf);
int save_on_disk();
int read_from_disk();
int createFile(char *name, int size, int start_Sector, int last);
void ls();
int delete_File(char *name);
int write_file_to_memory(char *name, void *buf, int size);
int rewrite_file(char *name, void *buf, int size);
struct File get_file(int n);
size_t round_to_512(size_t size);
int get_n_files();
int read_file(char *name, void *buf, int size);
void ATA_wait_BSY();
void ATA_wait_DRQ();
void disk_init();
int check_file_existance(char *name);
int read_block_file(char *name, void *buf, int size, int starting_byte);
struct File get_file_by_name(char *name);
int write_block_file(char *name, void *buf, int size, int starting_byte);
void dtest();
struct File
{
    int start_sector;
    int last_sector;
    int number_of_sectors;
    char name[MAX_FILENAME_LENGTH];
} __attribute__((packed));

struct disk_save_struct
{
    int number_of_files;
    int last_sector;
} __attribute__((packed));

extern struct disk_save_struct disk_save;
extern void *files_addr;

#endif
