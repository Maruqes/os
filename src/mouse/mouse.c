#include "mouse.h"
#include "kernel.h"
#include "terminal/terminal.h"
#include "io/io.h"

unsigned char mouse_cycle;
char mouse_byte[4];
char mouse_x;
char mouse_y;
int cur_x;
int cur_y;
Mouse_struct mouse_struct;

void mouse_wait_output()
{
    unsigned int _time_out = 100000;
    while (_time_out--)
    {
        if (((insb(0x64) >> 1) & 1) == 0)
        {
            return;
        }
    }
    return;
}

void mouse_wait_read()
{
    unsigned int _time_out = 100000;
    while (_time_out--)
    {
        if (((insb(0x64) >> 0) & 1) == 1)
        {
            return;
        }
    }
    return;
}

void mouse_write(unsigned char a)
{
    // Tell the mouse we are sending a command
    mouse_wait_output();
    outb(0x64, 0xD4);

    mouse_wait_output();
    outb(0x60, a);
}

unsigned char mouse_read()
{
    mouse_wait_read();
    return insb(0x60);
}

void mouse_start()
{
    mouse_cycle = 0;
    mouse_x = 0;
    mouse_y = 0;
    cur_x = 0;
    cur_y = 0;

    // Aux Input Enable Command
    mouse_wait_output();
    outb(0x64, 0xA8);

    // Set Compaq Status/Enable IRQ12
    mouse_wait_output();
    outb(0x64, 0x20);
    mouse_wait_read();
    unsigned char statusB = (insb(0x60));
    statusB |= 1 << 1;
    statusB &= ~(1 << 5);
    mouse_wait_output();
    outb(0x64, 0x60);
    mouse_wait_output();
    outb(0x60, statusB);

    // default settings
    mouse_write(0xF6);
    mouse_read(); // Acknowledge

    // Enable the mouse
    mouse_write(0xF4);
    mouse_read(); // Acknowledge
}

void handle_mouse_info()
{
    if (mouse_cycle == 0)
    {
        mouse_byte[0] = mouse_read();

        // buttons
        // if ((mouse_byte[0] & 0b00000001) == 0b00000001)
        // {
        //     print("l");
        // }
        // if ((mouse_byte[0] & 0b00000010) == 0b00000010)
        // {
        //     print("r");
        // }
        // if ((mouse_byte[0] & 0b00000100) == 0b00000100)
        // {
        //     print("m");
        // }
        mouse_struct.mouse_info = mouse_byte[0];

        //
        mouse_cycle++;
    }
    else if (mouse_cycle == 1)
    {
        mouse_byte[1] = mouse_read();
        mouse_cycle++;
    }
    else if (mouse_cycle == 2)
    {
        mouse_byte[2] = mouse_read();
        mouse_x = mouse_byte[1];
        mouse_y = mouse_byte[2];
        mouse_cycle = 0;
    }

    // print(digit_to_number(((mouse_byte[0] >> 0) & 1)));

    // cursor
    cur_x += mouse_x;
    cur_y -= mouse_y;
    if (cur_x < 0)
        cur_x = 0;
    if (cur_x > SCREEN_WIDHT)
        cur_x = SCREEN_WIDHT - 1;
    if (cur_y < 0)
        cur_y = 0;
    if (cur_y > SCREEN_HEIGHT)
        cur_y = SCREEN_HEIGHT - 1;

    mouse_struct.x = cur_x;
    mouse_struct.y = cur_y;
    // put_pixel(cur_x, cur_y, 0x00ff00);
}

Mouse_struct *get_mouse_info()
{
    return &mouse_struct;
}