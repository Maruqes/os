#ifndef OS_H
#define OS_H

typedef struct Mouse_struct
{
    int x;
    int y;
    char mouse_info;
} Mouse_struct;

typedef struct
{
    int ticks;
    int milliseconds;
    int seconds;
    int minutes;
    int hours;

    char c_seconds[2];
    char c_minutes[2];
    char c_hours[2];

} Clock;

char *input(int size);
void print_addr(char *a);
void *get_screen_access();
void get_OS_FUNCTIONS_addr();
void sleep(unsigned int mills);
void *malloc(unsigned long size);
void free(void *ptr);
void print_2(char *str);
int test_screen_buffer();
void *memset(void *ptr, int c, size_t size);
void *memcpy(void *dist, void *source, size_t size);
int memcmp(void *s1, void *s2, int count);
void new_line();
void clear_screen();
void finish_int();
void enable_int();
Mouse_struct *get_mouse_info();
Clock *get_clock();
void sleep_this(int mills);
void print(char *a);
void test_prgm();
void *zalloc(unsigned long size);
void program_error(int error);

// syscalls for program crashes
void dot_data_crash();
void dot_rodata_crash();
#endif