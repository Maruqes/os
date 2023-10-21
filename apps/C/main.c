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

char *digit_to_number()
{
    int res = 123;
    char *t;

    int count = 3;

    t = malloc(count);
    res = res / 10;
    res = res * 10;
    res = res / 10;
    res = res / 10;
    res = res * 10;
    res = res / 10;

    res = res * 10;
    res = res / 10;
    res = res * 10;
    res = res * 10;
    res = res / 10;
    res = res * 10;
    return t;
}

void put_pixel(int x, int y, int color, unsigned int *buf)
{
    buf[y * 800 + x] = color;
}

void draw_square(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int *buf, unsigned int color)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            put_pixel(x + j, y + i, color, buf);
        }
    }
}

void draw_square_exce(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int *buf, unsigned int color)
{
    for (int j = 0; j < width; j++)
    {
        put_pixel(x + j, y, color, buf);
        put_pixel(x + j, y + height, color, buf);
    }
    for (int j = 0; j < height; j++)
    {
        put_pixel(x, y + j, color, buf);
        put_pixel(x + width, y + j, color, buf);
    }
}

int main()
{
    get_OS_FUNCTIONS_addr();

    enable_int();
    finish_int();

    char *runn = "RUNNING...";

    print(runn);

    new_line();
    // if (test_dot_data() == 0)
    // {
    //     // dot_data_crash();
    //     return 0;
    // }

    // if (test_ro_data() == 0)
    // {
    //     // dot_rodata_crash();
    //     return 0;
    // }
    print(digit_to_number());
    sleep(5000);

    unsigned int *buf = set_buffer();
    draw_square_exce(0, 0, 20, 20, buf, 0xff0000);

    while (1)
    {
    }

    quit_app();
    return 0;
}