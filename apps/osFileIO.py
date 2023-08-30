from ctypes import *
import os

DISK_STRUCT_SECTOR = 1
OS_SECTORS = 20020
HEADER_SECTORS = 512
file_name = (r"\\wsl.localhost\Debian\home\marques\os\myos.iso")
sector_n = OS_SECTORS + HEADER_SECTORS

STARING_FILE_SECTOR = DISK_STRUCT_SECTOR - HEADER_SECTORS + sector_n


class File_struct(Structure):
    _fields_ = [
        ('start_sector', c_int),
        ('last_sector', c_int),
        ('number_of_sectors', c_int),
        ('name0', c_char),
        ('name1', c_char),
        ('name2', c_char),
        ('name3', c_char),
        ('name4', c_char),
        ('name5', c_char),
    ]


class disk_save_struct(Structure):
    _fields_ = [('number_of_files', c_int),
                ('last_sector', c_int)]


os_file_read = open(file_name, 'rb')

disk_save = disk_save_struct()


def set_disk_save_struct():
    os_file_read.seek((sector_n - HEADER_SECTORS - DISK_STRUCT_SECTOR+1)*512)
    os_file_read.readinto(disk_save)
    print("number_of_files :" + str(disk_save.number_of_files) + "\n")
    print("last_sector :" + str(disk_save.last_sector) + "\n")


def read_file_struct(file_n):
    if (file_n > disk_save.number_of_files-1):
        print("file number invalid")
        return
    file = File_struct()
    os_file_read.seek((STARING_FILE_SECTOR*512)+(file_n * (sizeof(file)-2)))
    os_file_read.readinto(file)
    return file


def rewrite_file(file_n, buf, starting_byte):
    file = File_struct()
    file = read_file_struct(file_n)
    print("number of sectors " + str(file.number_of_sectors))
    file_size = file.number_of_sectors * 512
    if (file_size - starting_byte < len(buf)):
        print("does not fit into file")
        return

    os_file_write = open(file_name, 'wb')
    os_file_write.write(os_save)
    os_file_write.seek((OS_SECTORS+HEADER_SECTORS + file.start_sector)*512)
    os_file_write.write(buf)
    os_file_write.close()


set_disk_save_struct()
read_file_struct(0)


os_file_read.seek(0)
os_save = os_file_read.read(os.stat(file_name).st_size)


# str = "FUNFA PQP SOU BUE INTELIGENTE OMFG"
# buff = bytes(str, 'ascii')
# rewrite_file(0,buff,0)

print("Number of file to rewrite (starts in 0): ")
number_of_file_to_rewrite = int(input())
print("Path to file: ")
path_of_file_to_rewrite = input()

file_rewrite = open(path_of_file_to_rewrite, 'rb')
data = file_rewrite.read()
file_rewrite.close()

rewrite_file(number_of_file_to_rewrite, data, 0)

os_file_read.close()


# print(sizeof(file))

# print(f.tell())
# print(buf)
# for i in range(0,510):
#     if(buf[i] == 0x50 and buf[i+1] == 0x51 and buf[i+2] == 0x50):
#         print("pqp")

# f.seek((sector_n - HEADER_SECTORS+1)*512)

# dsk = File_struct()
# f.readinto(dsk)

# dsk2 = File_struct()
# f.readinto(dsk2)

# print(dsk2.start_sector)


# 4F 49 55 oiu
