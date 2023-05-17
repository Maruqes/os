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
#include "raycaster/raycaster.h"
#include "mouse/mouse.h"

unsigned int *framebuffer;
void divide_zero();
void setup();
int terminal_mode;
void (*console_controler)(char);
void (*app)();
unsigned int sleep_time;

extern void test();

void null_f()
{
}

void quit_app()
{
    app = &null_f;
}

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
    mouse_start();
    idt_init();
    startKeyboardHandler();
    enable_int();
    disk_init();
    read_from_disk();
    get_hz(5000);
    timer_phase(5000);
    app = &null_f;
    sleep_time = 0;

    framebuffer = (unsigned int *)MultiBootHeaderStruct[22];

    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDHT; j++)
        {
            put_pixel(j, i, 0x0000FF);
        }
    }

    start_terminal_mode();

    // test_jump_program();
    //    test();
    print("  CONTINUA");
    while (1)
    {
        (*app)();
    }

    // divide_zero();
}

/*
    top 10 planos do ano
criar el botador de fixero aparte amigo :D PITON
tentare rodare programe


//top 1 objetivo é criar o minimo de suporte para aplicaçoes tipo uma calculadora(dar info do rato, dar upload a app pre compilada)

lindo
*/