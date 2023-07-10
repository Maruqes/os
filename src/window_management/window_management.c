#include "window_management.h"
#include "kernel.h"
#include "terminal/terminal.h"
#include "memory/memory.h"
#include "multitasking/multitasking.h"

Window_manager buffers_arr[10];
int current_window = 0;

unsigned int *set_buffer()
{
    print("BUFFER");
    int unused_buffer = -1;
    for (int i = 0; i < 10; i++)
    {
        if (buffers_arr[i].used == 0)
        {
            unused_buffer = i;
            break;
        }
    }

    if (unused_buffer == -1)
    {
        print("ALL BUFFERS ARE BEING USED");
        return 0;
    }

    buffers_arr[unused_buffer].buffer = malloc(SCREEN_HEIGHT * SCREEN_WIDHT * 4); // tem *4 pq é int
    buffers_arr[unused_buffer].used = 1;
    buffers_arr[unused_buffer].proc_pid = cur_task;

    current_window = unused_buffer;

    return buffers_arr[unused_buffer].buffer;
}

void free_buffer(int buf)
{
    buffers_arr[buf].used = 0;
}

void change_screens(int screen)
{
    current_window = screen;
}