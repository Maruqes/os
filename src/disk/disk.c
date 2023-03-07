#include "disk.h"
#include "io/io.h"
#include "memory/memory.h"
#include "idt/idt.h"
#include "string/string.h"
#include "kernel.h"
#include "disk/disk.h"
#include "terminal/terminal.h"
#define DISK_STRUCT_SECTOR 1
#define OS_SECTORS 20000
#define HEADER_SECTORS 512
struct disk_save_struct disk_save;
void *files_addr;

char *startFiles;
char *endFiles;

#define STATUS_BSY 0x80
#define STATUS_RDY 0x40

void ATA_wait_BSY() // Wait for bsy to be 0
{
	while (insb(0x1F7) & STATUS_BSY)
		;
}

void ATA_wait_DRQ() // Wait fot drq to be 1
{
	unsigned char a = 1;
	a = insb(0x1F7);
	while (!(a & STATUS_RDY))
	{
		a = insb(0x1F7);
	}
}

size_t round_to_512(size_t size)
{
	if ((size % 512) == 0)
	{
		return size;
	}

	size_t val = size % 512;
	size_t val2 = size;
	val2 = size - val;
	val2 = val2 + 512;
	return val2;
}

struct File get_file(int n)
{
	struct File tfile;
	memcpy(&tfile, files_addr + (n * (sizeof(struct File))), sizeof(struct File));
	return tfile;
}

int disk_read_sector(int lba, void *buf)
{
	ATA_wait_BSY();

	lba = lba + OS_SECTORS + HEADER_SECTORS;
	outb(0x1F6, 0xE0 | ((lba >> 24) & 0xF));
	outb(0x1F2, 1);
	outb(0x1F3, (uint8_t)lba);
	outb(0x1F4, (uint8_t)(lba >> 8));
	outb(0x1F5, (uint8_t)(lba >> 16));
	outb(0x1F7, 0x20); // Send the read command

	uint16_t *ptr = (uint16_t *)buf;

	ATA_wait_BSY();
	ATA_wait_DRQ();
	for (int i = 0; i < 256; i++)
	{
		*ptr = insw(0x1F0);
		ptr++;
	}

	return 0;
}

int disk_write_sector(int lba, void *buf)
{
	ATA_wait_BSY();

	lba = lba + OS_SECTORS + HEADER_SECTORS;
	outb(0x1F6, (lba >> 24) | 0xE0);
	outb(0x1F2, 1);
	outb(0x1F3, (unsigned char)(lba & 0xff));
	outb(0x1F4, (unsigned char)(lba >> 8));
	outb(0x1F5, (unsigned char)(lba >> 16));
	outb(0x1F7, 0x30);

	ATA_wait_BSY();
	ATA_wait_DRQ();

	unsigned short *ptr = (unsigned short *)buf;

	for (int i = 0; i < 256; i++)
	{
		outw(0x1F0, ptr[i]);
	}

	return 0;
}

int save_on_disk()
{
	size_t size = round_to_512(sizeof(disk_save));
	if (size != 512)
	{
		print("DISK_SAVE BIGGER THEN 512");
		return -1;
	}

	disk_write_sector(0 - HEADER_SECTORS, &disk_save);

	size_t files_size = disk_save.number_of_files * sizeof(struct File);
	files_size = round_to_512(files_size) / 512;

	for (int i = 0; i < files_size; i++)
	{
		void *t = files_addr + (i * 512);
		disk_write_sector(i + DISK_STRUCT_SECTOR - HEADER_SECTORS, t);
	}
	return 0;
}

int read_from_disk()
{
	char buf[512];
	disk_read_sector(0 - HEADER_SECTORS, &buf);
	memcpy(&disk_save, &buf, sizeof(struct disk_save_struct));

	free(files_addr);
	files_addr = zalloc(sizeof(struct File) * disk_save.number_of_files);

	size_t files_size = disk_save.number_of_files * sizeof(struct File);
	files_size = round_to_512(files_size);

	for (int i = 0; i < files_size / 512; i++)
	{
		char buf2[512];
		disk_read_sector(DISK_STRUCT_SECTOR + i - HEADER_SECTORS, &buf2);

		int offset = (512 * i);
		void *p = files_addr + offset;

		memcpy(p, &buf2, 512);
	}

	struct File file3;

	memcpy(&file3, files_addr + (0 * sizeof(struct File)), sizeof(struct File));

	return 0;
}

int createFile(char *name, int size, int start_Sector, int last)
{
	struct File tfile;
	tfile.number_of_sectors = size;
	tfile.start_sector = start_Sector;
	for (int i = 0; i < 5; i++)
	{
		tfile.name[i] = name[i];
	}

	tfile.name[5] = 0x00;

	void *t_files_addr = zalloc(sizeof(struct File) * (disk_save.number_of_files + 1));

	memcpy(t_files_addr, files_addr, sizeof(struct File) * disk_save.number_of_files);

	memcpy(t_files_addr + sizeof(struct File) * disk_save.number_of_files, &tfile, sizeof(struct File));

	free(files_addr);
	files_addr = t_files_addr;

	disk_save.number_of_files++;
	if (last)
		disk_save.last_sector += tfile.number_of_sectors;
	return 0;
}

int delete_File(char *name)
{
	int file_number = -1;
	int deleted = 0;
	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File tfile = get_file(i);
		if (cmpstring(tfile.name, name))
		{
			file_number = i;
			deleted = 1;
		}
	}

	if (!deleted)
	{
		new_line();
		print(name);
		print(" doesnt exist");
		new_line();
		return -1;
	}
	if (file_number == -1)
		return -1;

	void *t_files_addr = zalloc(sizeof(struct File) * (disk_save.number_of_files - 1));
	for (int i = 0; i < file_number; i++)
	{
		memcpy(t_files_addr + (i * sizeof(struct File)), files_addr + (i * sizeof(struct File)), sizeof(struct File));
	}

	for (int i = file_number + 1; i < disk_save.number_of_files; i++)
	{
		memcpy(t_files_addr + ((i - 1) * sizeof(struct File)), files_addr + (i * sizeof(struct File)), sizeof(struct File));
	}

	struct File tfile = get_file(file_number);
	struct File tfile2 = get_file(file_number - 1);
	if (tfile.start_sector + tfile.number_of_sectors - 1 == disk_save.last_sector)
	{
		disk_save.last_sector = tfile2.start_sector + tfile2.number_of_sectors;
	}

	disk_save.number_of_files--;
	free(files_addr);
	files_addr = t_files_addr;
	new_line();
	print(name);
	print(" is gone");
	new_line();
	return 0;
}

void sort_numbers_ascending(char number[], int count)
{
	int temp, j, k;
	for (j = 0; j < count; ++j)
	{
		for (k = j + 1; k < count; ++k)
		{
			if (number[j] > number[k])
			{
				temp = number[j];
				number[j] = number[k];
				number[k] = temp;
			}
		}
	}
}

int update_memory_map()
{
	free(startFiles);
	free(endFiles);
	startFiles = malloc(sizeof(int) * disk_save.number_of_files);
	endFiles = malloc(sizeof(int) * disk_save.number_of_files);
	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File tfile = get_file(i);

		startFiles[i] = tfile.start_sector;
		endFiles[i] = tfile.start_sector + tfile.number_of_sectors;
	}
	sort_numbers_ascending(startFiles, disk_save.number_of_files);
	sort_numbers_ascending(endFiles, disk_save.number_of_files);
	return -1;
}

int write_file_to_memory(char *name, void *buf, int size)
{
	update_memory_map();
	int sizeR = round_to_512(size);
	int size_of_sectors = sizeR / 512;
	for (int i = 0; i < disk_save.number_of_files - 1; i++)
	{
		if (size_of_sectors <= startFiles[i + 1] - endFiles[i])
		{
			// encontrou
			for (int k = 0; k < size_of_sectors; k++)
			{
				disk_write_sector(endFiles[i] + k, buf + (k * 512));
			}
			createFile(name, size_of_sectors, endFiles[i], 0);
			new_line();
			print(name);
			print(" has been createdd");
			new_line();
			return 0;
		}
	}

	for (int k = 0; k < size_of_sectors; k++)
	{
		buf = buf + (512 * k);
		for (int j = 0; j < size_of_sectors; j++)
		{
			disk_write_sector(disk_save.last_sector + j + 1, buf + (j * 512));
		}
		createFile(name, size_of_sectors, disk_save.last_sector + 1, 1); // o .last_sector é ocupado
		new_line();
		print(name);
		print(" has been created");
		new_line();
		return 0;
	}

	return -1;
}

int rewrite_file(char *name, void *buf, int size)
{
	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File tfile = get_file(i);
		if (cmpstring(name, tfile.name))
		{
			if (size <= tfile.number_of_sectors * 512)
			{
				for (int j = 0; j < tfile.number_of_sectors; j++)
				{
					disk_write_sector(tfile.start_sector + j, buf + (j * 512));
				}
				new_line();
				print(tfile.name);
				print(" was rewritten");
				new_line();
				return 0;
			}
			else
			{
				print("rewrite bigger than file");
				return -1;
			}
		}
	}
	print("problem rewritting");
	return -1;
}

int read_file(char *name, void *buf, int size)
{
	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File tfile = get_file(i);
		if (cmpstring(name, tfile.name))
		{
			if (size <= tfile.number_of_sectors * 512)
			{
				for (int j = 0; j < tfile.number_of_sectors; j++)
				{
					disk_read_sector(tfile.start_sector + j, buf + (j * 512));
				}

				return 0;
			}
			else
			{
				print("read bigger than file");
				return -1;
			}
		}
	}
	print("problem reading");
	return -1;
}

void ls()
{
	new_line();
	print("number of files: ");
	printN(disk_save.number_of_files);
	new_line();
	print("last sector: ");
	printN(disk_save.last_sector);
	new_line();
	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File tfile = get_file(i);
		print(tfile.name);
		print(",");
		printN(tfile.start_sector);
		print(",");
		printN(tfile.number_of_sectors);
		print(", ");
	}
	new_line();
}

int get_n_files()
{
	return disk_save.number_of_files;
}