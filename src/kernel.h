#ifndef KERNEL_H
#define KERNEL_H
#define SCREEN_WIDHT 800
#define SCREEN_HEIGHT 600

void put_pixel(int x, int y, int color);
extern int terminal_mode;
#endif