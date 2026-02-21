#include "disk.h"
#include "io/io.h"
#include "memory/memory.h"
#include "idt/idt.h"
#include "string/string.h"
#include "kernel.h"
#include "disk/disk.h"
#include "terminal/terminal.h"
#define DISK_STRUCT_SECTOR 1
#define OS_SECTORS 20020
#define HEADER_SECTORS 512
struct disk_save_struct disk_save;
void *files_addr;

#define STATUS_BSY 0x80
#define STATUS_DRQ 0x08
#define STATUS_ERR 0x01
#define STATUS_DF 0x20
#define ATA_TIMEOUT 1000000

struct File errFile;

int ATA_wait_BSY() // Wait for bsy to be 0
{
	for (int i = 0; i < ATA_TIMEOUT; i++)
	{
		if (!(insb(0x1F7) & STATUS_BSY))
		{
			return 0;
		}
	}
	return -1;
}

int ATA_wait_DRQ() // Wait for drq to be 1
{
	for (int i = 0; i < ATA_TIMEOUT; i++)
	{
		unsigned char status = insb(0x1F7);
		if (status & (STATUS_ERR | STATUS_DF))
		{
			return -1;
		}

		if (!(status & STATUS_BSY) && (status & STATUS_DRQ))
		{
			return 0;
		}
	}
	return -1;
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
size_t round_to_512_down(size_t size)
{
	if ((size % 512) == 0)
	{
		return size;
	}

	size_t val = size % 512;
	size_t val2 = size;
	val2 = size - val;
	return val2;
}
struct File get_file(int n)
{
	struct File tfile;
	memcpy(&tfile, files_addr + (n * (sizeof(struct File))), sizeof(struct File));
	return tfile;
}

void set_file(struct File file, int n)
{
	memcpy(files_addr + (n * (sizeof(struct File))), &file, sizeof(struct File));
}

struct File get_file_by_name(char *name)
{
	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File tfile = get_file(i);
		if (cmpstring(tfile.name, name))
		{
			return get_file(i);
		}
	}
	return errFile;
}

void sort_files_for_sector_number()
{
	for (int i = 0; i < disk_save.number_of_files - 1; i++)
	{
		struct File tfile = get_file(i);
		struct File tfile2 = get_file(i + 1);
		if (tfile.start_sector > tfile2.start_sector)
		{
			set_file(tfile, i + 1);
			set_file(tfile2, i);
			sort_files_for_sector_number();
		}
	}
}

int disk_read_sector(int lba, void *buf)
{
	if (ATA_wait_BSY() < 0)
	{
		return -1;
	}

	lba = lba + OS_SECTORS + HEADER_SECTORS;
	outb(0x1F6, 0xE0 | ((lba >> 24) & 0xF));
	outb(0x1F2, 1);
	outb(0x1F3, (uint8_t)lba);
	outb(0x1F4, (uint8_t)(lba >> 8));
	outb(0x1F5, (uint8_t)(lba >> 16));
	outb(0x1F7, 0x20); // Send the read command

	uint16_t *ptr = (uint16_t *)buf;

	if (ATA_wait_BSY() < 0)
	{
		return -1;
	}

	if (ATA_wait_DRQ() < 0)
	{
		return -1;
	}

	for (int i = 0; i < 256; i++)
	{
		*ptr = insw(0x1F0);
		ptr++;
	}

	return 0;
}

int disk_write_sector(int lba, void *buf)
{
	if (ATA_wait_BSY() < 0)
	{
		return -1;
	}

	lba = lba + OS_SECTORS + HEADER_SECTORS;
	outb(0x1F6, (lba >> 24) | 0xE0);
	outb(0x1F2, 1);
	outb(0x1F3, (unsigned char)(lba & 0xff));
	outb(0x1F4, (unsigned char)(lba >> 8));
	outb(0x1F5, (unsigned char)(lba >> 16));
	outb(0x1F7, 0x30);

	if (ATA_wait_BSY() < 0)
	{
		return -1;
	}

	if (ATA_wait_DRQ() < 0)
	{
		return -1;
	}

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

	if (disk_write_sector(0 - HEADER_SECTORS, &disk_save) < 0)
	{
		return -1;
	}

	size_t files_size = disk_save.number_of_files * sizeof(struct File);
	files_size = round_to_512(files_size) / 512;

	for (int i = 0; i < files_size; i++)
	{
		void *t = files_addr + (i * 512);
		if (disk_write_sector(i + DISK_STRUCT_SECTOR - HEADER_SECTORS, t) < 0)
		{
			return -1;
		}
	}
	return 0;
}

int read_from_disk()
{
	char buf[512];
	if (disk_read_sector(0 - HEADER_SECTORS, &buf) < 0)
	{
		free(files_addr);
		disk_save.number_of_files = 0;
		disk_save.last_sector = 0;
		files_addr = 0;
		return -1;
	}
	memcpy(&disk_save, &buf, sizeof(struct disk_save_struct));

	if (disk_save.number_of_files < 0 || disk_save.number_of_files > OS_MAX_FILE)
	{
		free(files_addr);
		disk_save.number_of_files = 0;
		disk_save.last_sector = 0;
		files_addr = 0;
		return -1;
	}

	free(files_addr);
	files_addr = 0;

	if (disk_save.number_of_files == 0)
	{
		return 0;
	}

	files_addr = zalloc(sizeof(struct File) * disk_save.number_of_files);
	if (!files_addr || files_addr == (void *)-1)
	{
		disk_save.number_of_files = 0;
		disk_save.last_sector = 0;
		files_addr = 0;
		return -1;
	}

	size_t files_bytes = disk_save.number_of_files * sizeof(struct File);
	size_t files_size = round_to_512(files_bytes);

	for (int i = 0; i < files_size / 512; i++)
	{
		char buf2[512];
		if (disk_read_sector(DISK_STRUCT_SECTOR + i - HEADER_SECTORS, &buf2) < 0)
		{
			free(files_addr);
			files_addr = 0;
			disk_save.number_of_files = 0;
			disk_save.last_sector = 0;
			return -1;
		}

		int offset = (512 * i);
		size_t copy_size = 512;
		if ((size_t)offset + copy_size > files_bytes)
		{
			copy_size = files_bytes - (size_t)offset;
		}

		void *p = files_addr + offset;
		memcpy(p, &buf2, copy_size);
	}

	return 0;
}

int createFile(char *name, int size, int start_Sector, int last)
{
	if (size <= 0)
	{
		return -1;
	}

	struct File tfile;
	tfile.number_of_sectors = size;
	tfile.start_sector = start_Sector;
	tfile.last_sector = start_Sector + (size - 1);
	memset(tfile.name, 0x00, MAX_FILENAME_LENGTH);
	for (int i = 0; i < MAX_FILENAME_LENGTH - 1; i++)
	{
		if (name[i] == 0x00)
		{
			break;
		}
		tfile.name[i] = name[i];
	}

	void *t_files_addr = zalloc(sizeof(struct File) * (disk_save.number_of_files + 1));
	if (!t_files_addr || t_files_addr == (void *)-1)
	{
		return -1;
	}

	if (disk_save.number_of_files > 0)
	{
		memcpy(t_files_addr, files_addr, sizeof(struct File) * disk_save.number_of_files);
	}

	memcpy(t_files_addr + sizeof(struct File) * disk_save.number_of_files, &tfile, sizeof(struct File));

	free(files_addr);
	files_addr = t_files_addr;

	disk_save.number_of_files++;
	if (last)
		disk_save.last_sector += tfile.number_of_sectors;
	sort_files_for_sector_number();
	return 0;
}

int check_file_existance(char *name)
{
	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File tfile = get_file(i);
		if (cmpstring(tfile.name, name))
		{
			return 1;
		}
	}
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

	int new_file_count = disk_save.number_of_files - 1;
	void *t_files_addr = 0;
	if (new_file_count > 0)
	{
		t_files_addr = zalloc(sizeof(struct File) * new_file_count);
		if (!t_files_addr || t_files_addr == (void *)-1)
		{
			return -1;
		}
	}

	for (int i = 0; i < file_number; i++)
	{
		memcpy(t_files_addr + (i * sizeof(struct File)), files_addr + (i * sizeof(struct File)), sizeof(struct File));
	}

	for (int i = file_number + 1; i < disk_save.number_of_files; i++)
	{
		memcpy(t_files_addr + ((i - 1) * sizeof(struct File)), files_addr + (i * sizeof(struct File)), sizeof(struct File));
	}

	disk_save.number_of_files = new_file_count;
	free(files_addr);
	files_addr = t_files_addr;

	disk_save.last_sector = 0;
	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File tfile = get_file(i);
		int next_free_sector = tfile.last_sector + 1;
		if (next_free_sector > disk_save.last_sector)
		{
			disk_save.last_sector = next_free_sector;
		}
	}

	new_line();
	print(name);
	print(" is gone");
	new_line();
	return 0;
}

int write_block_file(char *name, void *buf, int size, int starting_byte)
{
	if (size <= 0 || starting_byte < 0)
	{
		return -1;
	}

	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File tfile = get_file(i);
		if (cmpstring(name, tfile.name))
		{
			int file_size = tfile.number_of_sectors * 512;
			if (starting_byte + size <= file_size)
			{
				int first_sector = starting_byte / 512;
				int last_sector = (starting_byte + size - 1) / 512;
				int n_sectors = (last_sector - first_sector) + 1;
				void *t_buf = zalloc(n_sectors * 512);
				if (!t_buf || t_buf == (void *)-1)
				{
					return -1;
				}

				for (int j = 0; j < n_sectors; j++)
				{
					if (disk_read_sector(tfile.start_sector + first_sector + j, t_buf + (j * 512)) < 0)
					{
						free(t_buf);
						return -1;
					}
				}

				memcpy(t_buf + (starting_byte - (first_sector * 512)), buf, size);

				for (int j = 0; j < n_sectors; j++)
				{
					if (disk_write_sector(tfile.start_sector + first_sector + j, t_buf + (j * 512)) < 0)
					{
						free(t_buf);
						return -1;
					}
				}

				free(t_buf);
				return 0;
			}
		}
	}
	print("problem writting");
	return -1;
}

int write_file_to_memory(char *name, void *buf, int size)
{
	if (size <= 0)
	{
		return -1;
	}

	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File f = get_file(i);
		if (cmpstring(f.name, name))
		{
			print("FILE EXISTS");
			return 1;
		}
	}
	int sizeR = round_to_512(size);
	int size_of_sectors = sizeR / 512;
	for (int i = 0; i < disk_save.number_of_files - 1; i++)
	{
		struct File f = get_file(i);
		struct File f2 = get_file(i + 1);

		if (size_of_sectors < f2.start_sector - f.last_sector)
		{
			// encontrou
			for (int k = 0; k < size_of_sectors; k++)
			{
				if (disk_write_sector(f.last_sector + k + 1, buf + (k * 512)) < 0)
				{
					return -1;
				}
			}
			if (createFile(name, size_of_sectors, f.last_sector + 1, 0) < 0)
			{
				return -1;
			}
			new_line();
			print(name);
			print(" has been createdd");
			new_line();
			return 0;
		}
	}

	if (disk_save.number_of_files == 1) // caso tenhamos 1 ficheiro e ele estiver tipo no setor 20
	{
		struct File f = get_file(0);
		if (f.start_sector > size_of_sectors)
		{
			for (int j = 0; j < size_of_sectors; j++)
			{
				if (disk_write_sector(0 + j, buf + (j * 512)) < 0)
				{
					return -1;
				}
			}
			if (createFile(name, size_of_sectors, 0, 0) < 0)
			{
				return -1;
			}
			new_line();
			print(name);
			print(" has been created");
			new_line();
			return 0;
		}
	}

	for (int j = 0; j < size_of_sectors; j++)
	{
		if (disk_write_sector(disk_save.last_sector + j, buf + (j * 512)) < 0)
		{
			return -1;
		}
	}
	if (createFile(name, size_of_sectors, disk_save.last_sector, 1) < 0)
	{
		return -1;
	}
	new_line();
	print(name);
	print(" has been created");
	new_line();
	return 0;
}

int rewrite_file(char *name, void *buf, int size)
{
	if (size < 0)
	{
		return -1;
	}
	if (size == 0)
	{
		return 0;
	}

	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File tfile = get_file(i);
		if (cmpstring(name, tfile.name))
		{
			if (size <= tfile.number_of_sectors * 512)
			{
				if (write_block_file(name, buf, size, 0) < 0)
				{
					print("problem rewritting");
					return -1;
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

int read_block_file(char *name, void *buf, int size, int starting_byte)
{
	if (size <= 0 || starting_byte < 0)
	{
		return -1;
	}

	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File tfile = get_file(i);
		if (cmpstring(name, tfile.name))
		{
			int file_size = tfile.number_of_sectors * 512;
			if (starting_byte + size <= file_size)
			{
				int first_sector = starting_byte / 512;
				int last_sector = (starting_byte + size - 1) / 512;
				int n_sectors = (last_sector - first_sector) + 1;
				void *t_buf = zalloc(n_sectors * 512);
				if (!t_buf || t_buf == (void *)-1)
				{
					return -1;
				}

				for (int j = 0; j < n_sectors; j++)
				{
					if (disk_read_sector(tfile.start_sector + first_sector + j, t_buf + (j * 512)) < 0)
					{
						free(t_buf);
						return -1;
					}
				}
				memcpy(buf, t_buf + (starting_byte - (first_sector * 512)), size);
				free(t_buf);
				return 0;
			}
		}
	}
	print("problem reading");
	return -1;
}

int read_file(char *name, void *buf, int size)
{
	if (size < 0)
	{
		return -1;
	}
	if (size == 0)
	{
		return 0;
	}

	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File tfile = get_file(i);
		if (cmpstring(name, tfile.name))
		{
			if (size <= tfile.number_of_sectors * 512)
			{
				return read_block_file(name, buf, size, 0);
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
		printN(tfile.last_sector);
		print(",");
		printN(tfile.number_of_sectors);
		print(", ");
	}
	new_line();
}

void dtest()
{
	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File f = get_file(i);
		print(digit_to_number(f.start_sector));
		print(", ");
	}
	new_line();

	for (int i = 0; i < disk_save.number_of_files; i++)
	{
		struct File f = get_file(i);
		print(digit_to_number(f.last_sector));
		print(", ");
	}
	new_line();
}

int get_n_files()
{
	return disk_save.number_of_files;
}

void disk_init()
{
	files_addr = 0;
	disk_save.last_sector = 0;
	disk_save.number_of_files = 0;
	errFile.last_sector = -1;
	errFile.number_of_sectors = -1;
	errFile.start_sector = -1;
}
// problema a realocar ficheiros com espaço antes do lastfile
