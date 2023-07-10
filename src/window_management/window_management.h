#ifndef WINDOWS_MANAGEMENT_H
#define WINDOWS_MANAGEMENT_H

#include <stdint.h>
typedef struct
{
    unsigned int *buffer;
    int used;
    int proc_pid;
} Window_manager;
extern Window_manager buffers_arr[10];
extern int current_window;

unsigned int *set_buffer();
void free_buffer(int buf);
void change_screens(int screen);

#endif