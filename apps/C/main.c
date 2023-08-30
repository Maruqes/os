#include <stddef.h>
#include <stdint.h>
#include "os.h"

extern void printASM();
extern void quit_app();
extern void inputASM();
extern void printADDRASM();
extern void get_screen_accessASM();
extern void test_screen_bufferASM();
extern void testesCRL();

int fodasse = 69;
unsigned fodasse2 = 1000;

char test[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
char test2[10][10] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

signed test3[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
signed char test4[10][10] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

unsigned test5[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
unsigned char test6[10][10] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

short test7 = 0x6969;
unsigned test8 = 0xFEFE;

long test9 = 0xFCFC;
unsigned test10 = 0x12345678;

float test11 = 0x1234;
double test12 = 0x87654321;

long double test13 = 0x1234567890;

int fds(int a)
{
    return a + 1;
}

int main()
{
    get_OS_FUNCTIONS_addr();

    enable_int();
    finish_int();

    char *a = "RUNNING...";

    print_2(a);

    new_line();
    int pls = fodasse;
    int pls2 = fodasse2;
    while (1)
    {
        if (pls == 69)
        {
            print("TEST1");
            new_line();
            pls = fds(fodasse);
            pls2 = fds(fodasse2);
            if (pls == 70 && pls2 == 1001)
            {
                print("TEST2");
            }
        }
    }

    quit_app();
    return 0;
}