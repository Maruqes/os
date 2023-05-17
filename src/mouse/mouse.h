#ifndef MOUSE_H
#define MOUSE_H

unsigned char mouse_read();
void mouse_write(unsigned char a);
void mouse_start();
void handle_mouse_info();

extern unsigned char mouse_cycle;
extern char mouse_byte[4];
extern char mouse_x;
extern char mouse_y;
extern int cur_x;
extern int cur_y;

#endif