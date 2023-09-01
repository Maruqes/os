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
#include "window_management/window_management.h"

unsigned int *public_MultiBootHeaderStruct;
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

unsigned int *swipe_screen_buffer;

void put_pixel(int x, int y, int color)
{
    framebuffer[y * SCREEN_WIDHT + x] = color;
}

void timer_phase(float hz)
{
    get_hz(hz);

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

void program_error(int error)
{
    clear_screen();
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDHT; j++)
        {
            put_pixel(j, i, error);
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
    OS_FUNCTIONS[10] = (void *)finish_int_without_tasks;
    OS_FUNCTIONS[11] = (void *)enable_int;
    OS_FUNCTIONS[12] = (void *)return_clock;
    OS_FUNCTIONS[13] = (void *)set_buffer;
    OS_FUNCTIONS[14] = (void *)zalloc;
    OS_FUNCTIONS[15] = (void *)memcpy_prgm;
    OS_FUNCTIONS[16] = (void *)program_error;
}

void draw_screen()
{
    for (int i = 0; i < SCREEN_WIDHT * SCREEN_HEIGHT; i++)
    {
        framebuffer[i] = swipe_screen_buffer[i];
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
    disk_init();
    read_from_disk();
    timer_phase(5000);
    sleep_time = 0;
    execute_function = 0;
    start_OS_FUNCTIONS();
    multitasking_init();
    swipe_screen_buffer = zalloc(SCREEN_HEIGHT * SCREEN_WIDHT * 4);
    framebuffer = (unsigned int *)MultiBootHeaderStruct[22];

    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDHT; j++)
        {
            put_pixel(j, i, 0x0000FF);
        }
    }

    enable_int();
    start_terminal_mode();

    while (1) // DO NOT DELETE THIS LOOP PLEASE
    {
    }
    // divide_zero();
}

/*
//criar windows management
//top 1 objetivo é criar o minimo de suporte para aplicaçoes tipo uma calculadora(dar info do rato, dar upload a app pre compilada)

lindo
*/