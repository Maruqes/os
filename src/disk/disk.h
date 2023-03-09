#ifndef DISK_H
#define DISK_H
#include <stddef.h>
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
struct File
{
    char name[6];
    int start_sector;
    int number_of_sectors;
};

struct disk_save_struct
{
    int number_of_files;
    int last_sector;
};

extern struct disk_save_struct disk_save;
extern void *files_addr;

extern char *startFiles;
extern char *endFiles;

#endif
