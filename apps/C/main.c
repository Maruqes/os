#include <stddef.h>
#include <stdint.h>
#include "os.h"

extern void printCharASM();
extern void printASM();
extern void quit_app();
extern void inputASM();
extern void printADDRASM();
extern void get_screen_accessASM();
extern void test_screen_bufferASM();
extern void testesCRL();

#define APP_SCREEN_WIDTH 800
#define APP_SCREEN_HEIGHT 600

char *digit_to_number()
{
    int res = 123;
    char *t = malloc(12);
    if (!t)
    {
        return 0;
    }

    if (res == 0)
    {
        t[0] = '0';
        t[1] = 0x00;
        return t;
    }

    int idx = 0;
    int value = res;
    while (value > 0 && idx < 11)
    {
        t[idx] = (char)('0' + (value % 10));
        value /= 10;
        idx++;
    }
    t[idx] = 0x00;

    for (int i = 0, j = idx - 1; i < j; i++, j--)
    {
        char tmp = t[i];
        t[i] = t[j];
        t[j] = tmp;
    }

    return t;
}

void put_pixel(int x, int y, int color, unsigned int *buf)
{
    if (!buf || x < 0 || y < 0 || x >= APP_SCREEN_WIDTH || y >= APP_SCREEN_HEIGHT)
    {
        return;
    }
    buf[(y * APP_SCREEN_WIDTH) + x] = color;
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
    if (width == 0 || height == 0)
    {
        return;
    }

    for (int j = 0; j < width; j++)
    {
        put_pixel(x + j, y, color, buf);
        put_pixel(x + j, y + height - 1, color, buf);
    }
    for (int j = 0; j < height; j++)
    {
        put_pixel(x, y + j, color, buf);
        put_pixel(x + width - 1, y + j, color, buf);
    }
}

int main()
{
    get_OS_FUNCTIONS_addr();

    enable_int();
    finish_int();
    
    printCharASM();
    char *runn = "RUNNING...2";

    for (int i = 0; i < 10; i++)
    {
        print(runn);
        sleep_this(5000);
    }

    // new_line();
    // // if (test_dot_data() == 0)
    // // {
    // //     // dot_data_crash();
    // //     return 0;
    // // }

    // // if (test_ro_data() == 0)
    // // {
    // //     // dot_rodata_crash();
    // //     return 0;
    // // }
    // print(digit_to_number());
    // sleep(5000);

    // unsigned int *buf = set_buffer();
    // draw_square_exce(0, 0, 20, 20, buf, 0xff0000);

    // while (1)
    // {
    // }

    quit_app();
    return 0;
}
