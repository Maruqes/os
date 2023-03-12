#include "kernel.h"
#include "io/io.h"
#include <stdint.h>
#include "memory/memory.h"
#include "terminal/terminal.h"
#include "string/string.h"
#include "idt/idt.h"
#include "keyboardHandler/keyboardHandler.h"
#include "gdt/gdt.h"
#include "disk/disk.h"
#include "nano/nano.h"
#include "math/math.h"

unsigned int *framebuffer;
void divide_zero();
void setup();
int terminal_mode;
void (*console_controler)(char);

void put_pixel(int x, int y, int color)
{
    framebuffer[y * SCREEN_WIDHT + x] = color;
}

void timer_phase(float hz)
{
    uint16_t divisor = 1193180 / hz;
    // Init, Square Wave Mode, non-BCD, first transfer LSB then MSB
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void get_keyboard_input(void *func)
{
    console_controler = func;
}
void clear_pixels_screen()
{
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDHT; j++)
        {
            put_pixel(j, i, 0);
        }
    }
}
void kernel_main(unsigned int *MultiBootHeaderStruct)
{
    terminal_mode = 1;
    i686_GDT_Initialize();
    setup();
    heap_init();
    idt_init();
    startKeyboardHandler();
    enable_int();
    disk_init();
    read_from_disk();
    get_hz(5000);
    timer_phase(5000);

    framebuffer = (unsigned int *)MultiBootHeaderStruct[22];

    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDHT; j++)
        {
            put_pixel(j, i, 0x0000FF);
        }
    }

    start_terminal_mode();

    char buf[10000];
    memset(buf, 'O', 10000);
    memset(buf, 'B', 512);
    buf[508] = 'P';
    buf[509] = 'U';
    buf[510] = 'T';
    buf[511] = 'A';
    memset(buf + 512, 'C', 512);
    memset(buf + 1024, 'L', 512);
    memset(buf + 10000 - 512, 'H', 512);
    for (int i = 0; i < 10000 / 512; i++)
    {
        buf[i * 512 - 1] = 'K';
    }
    write_file_to_memory("TEST", buf, 10000);

    char *buff = zalloc(4);

    memset(buff, 'T', 4);

    write_block_file("TEST", buff, 4, 511);
    free(buff);

    // divide_zero();
}