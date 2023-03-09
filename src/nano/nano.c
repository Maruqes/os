#include "kernel.h"
#include "nano.h"
#include "memory/memory.h"
#include "string/string.h"
#include "disk/disk.h"
#include "terminal/terminal.h"

int nano_current_X = 0;
int nano_current_Y = 0;

int cursor_pos_x = 0;
int cursor_pos_y = 0;

int old_cursor_pos_x = 0;
int old_cursor_pos_y = 0;
int old_char = 0;

void *up_mem;
void *down_mem;
int up_lines = 0;
int down_lines = 0;

int left_control = 0;

void *c;

void del_cursor()
{
    write_char_pos(cursor_pos_x, cursor_pos_y, old_char, 0);
}

void get_cursor()
{
    old_char = *(char *)(c + (cursor_pos_y * TERMINAL_WIDTH) + cursor_pos_x);
    write_char_pos(cursor_pos_x, cursor_pos_y, 102, 0x00ff00);
    cursor_pos_x = old_cursor_pos_x;
    cursor_pos_y = old_cursor_pos_y;
}

void update_cursor_pos(int p)
{
    // if (nano_current_X == TERMINAL_WIDTH - 1)
    // {
    //     cursor_pos_x = 0;
    //     cursor_pos_y = nano_current_Y + 1;
    // }
    // else
    // {
    //     cursor_pos_x = nano_current_X + p;
    //     cursor_pos_y = nano_current_Y;
    // }

    // write_char_pos(old_cursor_pos_x, old_cursor_pos_y, old_char, 0x00ff00);
    // old_char = *(char *)(c + (cursor_pos_y * TERMINAL_WIDTH) + cursor_pos_x);
    // write_char_pos(cursor_pos_x, cursor_pos_y, 102, 0x00ff00);
    // old_cursor_pos_x = cursor_pos_x;
    // old_cursor_pos_y = cursor_pos_y;
}

void clear_line(int line)
{
    for (int i = 0; i < TERMINAL_WIDTH; i++)
    {
        write_char_pos(i, line, 0, 0);
    }
}

void backspace_nano()
{
    if (nano_current_X == 0)
    {
        if (nano_current_Y == 0)
        {
            return;
        }
        nano_current_Y--;
        nano_current_X = TERMINAL_WIDTH;
        while (*(char *)(c + (nano_current_Y * TERMINAL_WIDTH) + nano_current_X - 1) == 0)
        {
            nano_current_X--;
        }
        return;
    }
    nano_current_X--; // problema negativo ne
    write_char_pos(nano_current_X, nano_current_Y, 0, 0);
    update_cursor_pos(0);
}

void enter_nano()
{
    nano_current_X = 0;
    nano_current_Y++;
    update_cursor_pos(0);
}

void open_file(char c)
{
    if (c == -8)
    {
        left_control = 1;
        return;
    }
    else if (c == 'P' && left_control == 1)
    {
        left_control = 0;
        return;
    }
    else if (c == 'L' && left_control == 1)
    {
        left_control = 0;
        return;
    }
    else if (c == -2)
    {
        backspace_nano();
        return;
    }
    else if (c == -3)
    {
        enter_nano();
        return;
    }
    else if (c == -4)
    {
        if (nano_current_Y == 0)
        {
            // save_line_down();
            update_cursor_pos(0);
            return;
        }
        nano_current_Y--;
        update_cursor_pos(0);
        return;
    }
    else if (c == -5)
    {
        if (nano_current_Y == TERMINAL_HEIGTH - 1)
        {
            // save_line_up();
            update_cursor_pos(0);
            return;
        }
        nano_current_Y++;
        update_cursor_pos(0);
        return;
    }
    else if (c == -6)
    {
        if (nano_current_X == 0)
            return;
        nano_current_X--;
        update_cursor_pos(0);
        return;
    }
    else if (c == -7)
    {
        if (nano_current_X == TERMINAL_WIDTH - 1)
            return;
        nano_current_X++;
        update_cursor_pos(0);
        return;
    }
    update_cursor_pos(1);
    write_char_pos(nano_current_X, nano_current_Y, c, 0x00ff00);
    nano_current_X++;
    if (nano_current_X == TERMINAL_WIDTH)
    {
        nano_current_X = 0;
        nano_current_Y++;
    }
    return;
}

void activate_nano()
{
    clear_screen();
    c = get_c_slots();
    get_keyboard_input(&open_file);
}

// criar um rr do tamanho do ficheiro sendo limitado a  x mb
// dividir por linhas simplesmente torno o offset uuma linha abaixo
// funçao q de acordo com a linha pega numa pag a frente da linha