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
unsigned int fodasse2 = 1000;

char test[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
char test2[10][10] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

signed char test3[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
signed char test4[10][10] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

unsigned char test5[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
unsigned char test6[10][10] = {{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};

short test7 = 0x6969;
unsigned test8 = 0xFEFE;

long test9 = 0xFCFC;
unsigned test10 = 0x12345678;

float test11 = 0x1234;
double test12 = 0x87654321;

long double test13 = 0x1234567890;

char test14[] = "BOAS";

int fds(int a)
{
    return a + 1;
}

int test_dot_data()
{
    print("DATA TYPE CHECKS: ");
    int pls = fodasse;
    int pls2 = fodasse2;

    if (pls2 == 1000)
    {
        fodasse2 = fds(pls2);
        if (fodasse2 == 1001)
        {
            print("TEST :D");
            new_line();
        }
        else
        {
            print("ERROR TEST");
            return 0;
        }
    }
    if (pls == 69)
    {
        print("TEST1");
        new_line();
        pls = fds(fodasse);
        if (pls == 70)
        {
            print("TEST2");
        }
        else
        {
            print("ERROR TEST2");
            return 0;
        }
    }

    new_line();
    for (int i = 0; i < 10; i++)
    {
        if (test[i] != (char)i)
        {
            print("ERROR TEST3");
            return 0;
        }
    }
    print("TEST3");

    new_line();
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (test2[i][j] != (char)j)
            {
                print("ERROR TEST4");
                return 0;
            }
        }
    }
    print("TEST4");

    new_line();
    for (int i = 0; i < 10; i++)
    {
        if (test3[i] != (char)i)
        {
            print("ERROR TEST5");
            return 0;
        }
    }
    print("TEST5");

    new_line();
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (test4[i][j] != (char)j)
            {
                print("ERROR TEST6");
                return 0;
            }
        }
    }
    print("TEST6");

    new_line();
    for (int i = 0; i < 10; i++)
    {
        if (test5[i] != (char)i)
        {
            print("ERROR TEST7");
            return 0;
        }
    }
    print("TEST7");

    new_line();
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (test6[i][j] != (char)j)
            {
                print("ERROR TEST8");
                return 0;
            }
        }
    }
    print("TEST8");

    new_line();
    if (test14[0] != 'B' && test14[1] != 'O' && test14[2] != 'A' && test14[3] != 'S')
    {
        print("ERROR TEST9");
        return 0;
    }
    print("TEST9");
    return 1;
}

int test_ro_data()
{
    char *test = "TEST";
    char *test2 = "TEST2";
    if ((char)test[0] == 'T' && (char)test[1] == 'E' && (char)test[2] == 'S' && (char)test[3] == 'T' && (char)test2[0] == 'T' && (char)test2[1] == 'E' && (char)test2[2] == 'S' && (char)test2[3] == 'T' && (char)test2[4] == '2')
    {
        new_line();
        print("RODATA TEST 1");
        return 1;
    }
    return 0;
}

int main()
{
    get_OS_FUNCTIONS_addr();

    enable_int();
    finish_int();

    char *runn = "RUNNING...";

    print(runn);

    new_line();
    if (test_dot_data() == 0)
    {
        dot_data_crash();
        return 0;
    }

    if (test_ro_data() == 0)
    {
        dot_rodata_crash();
        return 0;
    }
    while (1)
    {
    }

    quit_app();
    return 0;
}