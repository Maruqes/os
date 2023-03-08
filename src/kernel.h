#ifndef KERNEL_H
#define KERNEL_H
#define SCREEN_WIDHT 800
#define SCREEN_HEIGHT 600

void put_pixel(int x, int y, int color);
void get_keyboard_input(void *func);
void clear_pixels_screen();

extern int terminal_mode;
extern void (*console_controler)(char);
#endif