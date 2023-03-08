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

void save_file()
{
    void *buf = zalloc((up_lines * TERMINAL_WIDTH) + (TERMINAL_WIDTH * TERMINAL_HEIGTH) + (down_lines * TERMINAL_WIDTH));

    memcpy(buf, up_mem, (up_lines * TERMINAL_WIDTH));
    memcpy(buf + (up_lines * TERMINAL_WIDTH), c, (TERMINAL_WIDTH * TERMINAL_HEIGTH));
    memcpy(buf + (up_lines * TERMINAL_WIDTH) + (TERMINAL_WIDTH * TERMINAL_HEIGTH), down_mem, (down_lines * TERMINAL_WIDTH));

    // rewrite_file("filename", buf, (up_lines * TERMINAL_WIDTH) + (TERMINAL_WIDTH * TERMINAL_HEIGTH) + (down_lines * TERMINAL_WIDTH));
}

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

void push_line_down()
{
    if (down_lines == 0)
        return;

    down_lines--;
    char t[TERMINAL_WIDTH];
    memcpy(t, down_mem + (down_lines * TERMINAL_WIDTH), TERMINAL_WIDTH);
    for (int i = 0; i < TERMINAL_WIDTH; i++)
    {
        write_char_pos(i, TERMINAL_HEIGTH - 1, t[i], 0x00ff00);
    }
    void *down_mem_temp = zalloc(TERMINAL_WIDTH * down_lines);
    memcpy(down_mem_temp, down_mem, TERMINAL_WIDTH * (down_lines));
    free(down_mem);
    down_mem = down_mem_temp;
}

void push_line_up()
{
    if (up_lines == 0)
        return;
    up_lines--;
    char t[TERMINAL_WIDTH];
    memcpy(t, up_mem + (up_lines * TERMINAL_WIDTH), TERMINAL_WIDTH);
    for (int i = 0; i < TERMINAL_WIDTH; i++)
    {
        write_char_pos(i, 0, t[i], 0x00ff00);
    }
    void *up_mem_t = zalloc(TERMINAL_WIDTH * up_lines);
    memcpy(up_mem_t, up_mem, TERMINAL_WIDTH * up_lines);
    free(up_mem);
    up_mem = up_mem_t;
}

void copy_line_to_memory_up()
{
    up_lines++;
    void *up_mem_temp = zalloc(TERMINAL_WIDTH * up_lines);
    memcpy(up_mem_temp, up_mem, TERMINAL_WIDTH * (up_lines - 1));             // copy old lines
    memcpy(up_mem_temp + TERMINAL_WIDTH * (up_lines - 1), c, TERMINAL_WIDTH); // add new line
    free(up_mem);
    up_mem = up_mem_temp;
}

void copy_line_to_memory_down()
{
    down_lines++;
    void *down_mem_temp = zalloc(TERMINAL_WIDTH * down_lines);
    memcpy(down_mem_temp, down_mem, TERMINAL_WIDTH * (down_lines - 1));                                                                   // copy old lines
    memcpy(down_mem_temp + TERMINAL_WIDTH * (down_lines - 1), c + ((TERMINAL_HEIGTH * TERMINAL_WIDTH) - TERMINAL_WIDTH), TERMINAL_WIDTH); // add new line
    free(down_mem);
    down_mem = down_mem_temp;
}

void clear_line(int line)
{
    for (int i = 0; i < TERMINAL_WIDTH; i++)
    {
        write_char_pos(i, line, 0, 0);
    }
}

void delete_line_up()
{
    for (int j = 0; j < TERMINAL_HEIGTH; j++)
    {
        for (int i = 0; i < TERMINAL_WIDTH; i++)
        {
            write_char_pos(i, j, *(char *)(c + ((j + 1) * TERMINAL_WIDTH) + i), 0x00ff00);
        }
    }
    clear_line(TERMINAL_HEIGTH - 1);
}

void delete_line_down()
{
    for (int j = 0; j < TERMINAL_HEIGTH; j++)
    {
        for (int i = 0; i < TERMINAL_WIDTH; i++)
        {
            write_char_pos(i, (TERMINAL_HEIGTH - j), *(char *)(c + (TERMINAL_HEIGTH - j - 1) * TERMINAL_WIDTH + i), 0x00ff00);
        }
    }
    clear_line(0);
}

void save_line_up()
{
    // del_cursor();
    copy_line_to_memory_up();
    delete_line_up();
    push_line_down();
    //   get_cursor();
}
void save_line_down()
{
    // del_cursor();
    copy_line_to_memory_down();
    delete_line_down();
    push_line_up();
    //   get_cursor();
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
        save_line_up();
        left_control = 0;
        return;
    }
    else if (c == 'L' && left_control == 1)
    {
        save_line_down();
        left_control = 0;
        return;
    }
    else if (c == 'O' && left_control == 1)
    {
        for (int i = 0; i < 200; i++)
        {
            save_line_up();
        }
        left_control = 0;
        return;
    }
    else if (c == 'K' && left_control == 1)
    {
        for (int i = 0; i < 200; i++)
        {
            save_line_down();
        }
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
            save_line_down();
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
            save_line_up();
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
