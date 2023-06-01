#include "kernel.h"
#include "config.h"
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
#include "exec/exec.h"
#include "multitasking/multitasking.h"

unsigned int *framebuffer;
void divide_zero();
void setup();
int terminal_mode;
void (*console_controler)(char);
void (*app)();
unsigned int sleep_time;
void (*OS_FUNCTIONS[OS_EXTERN_FUNCTIONS])();
int execute_function;
uint32_t *main_stack;

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
    terminal_mode = 0;
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

void start_OS_FUNCTIONS()
{
    OS_FUNCTIONS[0] = (void *)sleep;
    OS_FUNCTIONS[1] = (void *)idt_printINT;
    OS_FUNCTIONS[2] = (void *)malloc;
    OS_FUNCTIONS[3] = (void *)free;
    OS_FUNCTIONS[4] = (void *)memset;
    OS_FUNCTIONS[5] = (void *)memcpy;
    OS_FUNCTIONS[6] = (void *)memcmp;
    OS_FUNCTIONS[7] = (void *)new_line;
    OS_FUNCTIONS[8] = (void *)clear_screen;
    OS_FUNCTIONS[9] = (void *)get_mouse_info;
    OS_FUNCTIONS[10] = (void *)finish_int;
    OS_FUNCTIONS[11] = (void *)enable_int;
}
int pqp = 0;

void kernel_main(unsigned int *MultiBootHeaderStruct)
{
    terminal_mode = 1;
    i686_GDT_Initialize();
    setup();
    heap_init();
    mouse_start();
    idt_init();
    startKeyboardHandler();
    disk_init();
    read_from_disk();
    get_hz(5000);
    timer_phase(5000);
    sleep_time = 0;
    execute_function = 0;
    start_OS_FUNCTIONS();
    multitasking_init();

    framebuffer = (unsigned int *)MultiBootHeaderStruct[22];

    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDHT; j++)
        {
            put_pixel(j, i, 0x0000FF);
        }
    }

    start_terminal_mode();
    enable_int();
    while (1) // DO NOT DELETE THIS LOOP PLEASE
    {
    }
    // divide_zero();
}

/*
fix la multitasking
create la quite de multitasking
fix da raycastiing problema

//top 1 objetivo é criar o minimo de suporte para aplicaçoes tipo uma calculadora(dar info do rato, dar upload a app pre compilada)

lindo
*/