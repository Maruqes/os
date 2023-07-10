#ifndef MOUSE_H
#define MOUSE_H

typedef struct Mouse_struct
{
    int x;
    int y;
    char mouse_info;
} Mouse_struct;

unsigned char mouse_read();
void mouse_write(unsigned char a);
void mouse_start();
void handle_mouse_info();
Mouse_struct *get_mouse_info();

extern unsigned char mouse_cycle;
extern char mouse_byte[4];
extern char mouse_x;
extern char mouse_y;
extern int cur_x;
extern int cur_y;
extern Mouse_struct mouse_struct;

#endif