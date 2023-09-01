#include <stddef.h>
#include <stdint.h>
#include "os.h"
#define OS_EXTERN_FUNCTIONS 17

void print(char *a)
{
    asm volatile("mov eax, %0"
                 :
                 : "r"(a)
                 :);
    asm volatile("call printASM");
}

char *input(int size)
{
    char *return_Addr;

    asm volatile("mov eax, %0"
                 :
                 : "r"(size)
                 :);

    asm volatile("call inputASM");

    asm volatile("mov %0, eax"
                 : "=r"(return_Addr)
                 :
                 :);

    return return_Addr;
}

void print_addr(char *a)
{
    // unsigned int b = 7;
    asm volatile("mov eax, %0"
                 :
                 : "r"(a)
                 :);
    asm volatile("call printADDRASM");
}

void *get_screen_access()
{
    char *return_Addr;

    asm volatile("call get_screen_accessASM");

    asm volatile("mov %0, eax"
                 : "=r"(return_Addr)
                 :
                 :);

    return return_Addr;
}

void (*OS_FUNCTIONS[OS_EXTERN_FUNCTIONS])();
void get_OS_FUNCTIONS_addr()
{
    for (int i = 0; i < OS_EXTERN_FUNCTIONS; i++)
    {
        void *return_Addr;

        asm volatile("call get_addrASM");

        asm volatile("mov %0, eax"
                     : "=r"(return_Addr)
                     :
                     :);

        OS_FUNCTIONS[i] = return_Addr;
    }
}

void sleep(unsigned int mills)
{
    (*OS_FUNCTIONS[0])(mills);
}

void *malloc(unsigned long size)
{
    void *return_Addr;
    (*OS_FUNCTIONS[2])(size);
    asm volatile("mov %0, eax"
                 : "=r"(return_Addr)
                 :
                 :);

    return return_Addr;
}

void *zalloc(unsigned long size)
{
    void *return_Addr;
    (*OS_FUNCTIONS[14])(size);
    asm volatile("mov %0, eax"
                 : "=r"(return_Addr)
                 :
                 :);

    return return_Addr;
}

void free(void *ptr)
{
    (*OS_FUNCTIONS[3])(ptr);
}

void print_2(char *str)
{
    (*OS_FUNCTIONS[1])(str);
}

int test_screen_buffer()
{
    int res;

    asm volatile("call test_screen_bufferASM");

    asm volatile("mov %0, eax"
                 : "=r"(res)
                 :
                 :);

    return res;
}

void *memset(void *ptr, int c, size_t size)
{
    void *return_Addr;
    (*OS_FUNCTIONS[4])(ptr, c, size);
    asm volatile("mov %0, eax"
                 : "=r"(return_Addr)
                 :
                 :);

    return return_Addr;
}

void *memcpy(void *dist, void *source, size_t size)
{
    void *return_Addr;
    (*OS_FUNCTIONS[5])(dist, source, size);
    asm volatile("mov %0, eax"
                 : "=r"(return_Addr)
                 :
                 :);

    return return_Addr;
}

void *memcpy_prgm(void *dist, void *source, size_t size)
{
    void *return_Addr;
    (*OS_FUNCTIONS[15])(dist, source, size);
    asm volatile("mov %0, eax"
                 : "=r"(return_Addr)
                 :
                 :);

    return return_Addr;
}

int memcmp(void *s1, void *s2, int count)
{
    int return_Addr;
    (*OS_FUNCTIONS[6])(s1, s2, count);
    asm volatile("mov %0, eax"
                 : "=r"(return_Addr)
                 :
                 :);

    return return_Addr;
}

void new_line()
{
    (*OS_FUNCTIONS[7])();
}

void clear_screen()
{
    (*OS_FUNCTIONS[8])();
}

void finish_int()
{
    (*OS_FUNCTIONS[10])();
}

void enable_int()
{
    (*OS_FUNCTIONS[11])();
}

Mouse_struct *get_mouse_info()
{
    Mouse_struct *return_Addr;
    (*OS_FUNCTIONS[9])();
    asm volatile("mov %0, eax"
                 : "=r"(return_Addr)
                 :
                 :);

    return return_Addr;
}

Clock *get_clock()
{
    Clock *return_Addr;
    (*OS_FUNCTIONS[12])();
    asm volatile("mov %0, eax"
                 : "=r"(return_Addr)
                 :
                 :);

    return return_Addr;
}

unsigned int *set_buffer()
{
    void *return_Addr;
    (*OS_FUNCTIONS[13])();
    asm volatile("mov %0, eax"
                 : "=r"(return_Addr)
                 :
                 :);

    return return_Addr;
}

void sleep_this(int mills)
{
    Clock *clock = get_clock();
    Clock *sum_clock = malloc(sizeof(Clock));

    memcpy(sum_clock, clock, sizeof(Clock));

    int sec = mills / 1000;

    int left_mills = mills - (sec * 1000);

    if (sum_clock->milliseconds + left_mills >= 1000)
    {
        sum_clock->milliseconds = sum_clock->milliseconds + left_mills - 1000;
        sum_clock->seconds++;
    }
    else
    {
        sum_clock->milliseconds += left_mills;
    }

    if (sum_clock->seconds + sec >= 60)
    {
        sum_clock->seconds = sum_clock->seconds + sec - 60;
        sum_clock->minutes++;
    }
    else
    {
        sum_clock->seconds += sec;
    }

    int finish_line = (sum_clock->milliseconds) + (sum_clock->seconds * 1000) + (sum_clock->minutes * 60 * 1000) + (sum_clock->hours * 60 * 60 * 1000);

    while (finish_line > (clock->milliseconds) + (clock->seconds * 1000) + (clock->minutes * 60 * 1000) + (clock->hours * 60 * 60 * 1000))
    {
    }

    free(sum_clock);
}

void program_error(int error)
{
    (*OS_FUNCTIONS[16])((int)error);
}

void test_prgm()
{
    // input
    // char *str = input(6);
    // print_addr(str);

    // malloc
    void *t1 = malloc(1024);
    void *t2 = malloc(1024);

    if (t1 != t2)
    {
        print("TEST 1 MALLOC");
        new_line();
    }

    if ((t1 + 4096) == t2)
    {
        print("TEST 2 MALLOC");
        new_line();
    }

    // free
    void *t3 = malloc(1024);
    free(t3);
    void *t4 = malloc(1024);

    if (t3 == t4)
    {
        print("TEST 1 FREE");
        new_line();
    }

    free(t1);
    free(t2);
    free(t4);

    // unsigned int *screenbuffer_save = get_screen_access();
    // for (int i = 0; i < 600; i++)
    // {
    //     for (int j = 0; j < 800; j++)
    //     {
    //         screenbuffer_save[i * 800 + j] = 0x0000ff;
    //     }
    // }
    // if (test_screen_buffer() == 1)
    // {
    //     sleep(1000);
    // }

    char *memset_test = malloc(4096);
    memset(memset_test, 'A', 4096);
    for (int i = 0; i < 4096; i++)
    {
        if ((memset_test[i]) != 'A')
        {
            print("MEMSET PROBLEM");
        }
    }
    print("MEMSET COMPLETE");
    new_line();

    char *memcpy_test = malloc(4096);
    memcpy(memcpy_test, memset_test, 4096);
    for (int i = 0; i < 4096; i++)
    {
        if ((memcpy_test[i]) != 'A')
        {
            print("MEMCPY PROBLEM");
        }
    }
    print("MEMCPY COMPLETE");
    new_line();

    if (memcmp(memcpy_test, memset_test, 4096) != 0)
    {
        print("MEMCMP PROBLEM");
    }

    print("MEMCMP COMPLETE");
    new_line();

    free(memcpy_test);
    free(memset_test);

    // unsigned int *screenbuffer_save = get_screen_access();
    // while (1)
    // {
    //     Mouse_struct *ms = get_mouse_info();
    //     screenbuffer_save[(ms->y) * 800 + (ms->x)] = 0x0000ff;
    // }
}