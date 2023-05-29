#ifndef KERNEL_H
#define KERNEL_H
#define SCREEN_WIDHT 800
#define SCREEN_HEIGHT 600
#include <stdint.h>
#include "config.h"

void put_pixel(int x, int y, int color);
void get_keyboard_input(void *func);
void clear_pixels_screen();
void test_jump_program();

extern int terminal_mode;
extern void (*console_controler)(char);
extern unsigned int *framebuffer;
extern void (*OS_FUNCTIONS[OS_EXTERN_FUNCTIONS])();
extern int execute_function;

#endif