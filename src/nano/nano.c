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

int left_control = 0;

char *filename;

void *c;

int current_line;

struct File file;

int filelen;

void terminal_write_blocks(char *buf)
{
    int t = 0;
    for (int i = 0; i < NANO_HEIGHT; i++)
    {
        for (int j = 0; j < NANO_WIDTH; j++)
        {
            write_char_pos(j, i, buf[t], 0x00ff00);
            t++;
        }
    }
}

void read_blocks(int starting_block)
{
    char *buf = zalloc(NANO_WIDTH * NANO_HEIGHT);
    int a = read_block_file(filename, buf, NANO_WIDTH * NANO_HEIGHT, starting_block);
    if (!a)
    {
        terminal_write_blocks(buf);
    }
    free(buf);
}

void update_page()
{
    int starting_byte = current_line * NANO_WIDTH;
    write_block_file(filename, c, NANO_HEIGHT * NANO_WIDTH, starting_byte);
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
        nano_current_Y = 0;
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
        if (current_line > 0)
        {
            update_page();
            current_line--;
            read_blocks(current_line * NANO_WIDTH);
        }
        return;
    }
    else if (c == -5)
    {
        if (current_line * NANO_WIDTH + (NANO_WIDTH * NANO_HEIGHT) < filelen)
        {
            update_page();
            current_line++;
            read_blocks(current_line * NANO_WIDTH);
        }
        // print(digit_to_number(current_line));

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

void set_filename(void *fn)
{
    int l = strlen(fn);
    filename = malloc(l);
    memcpy(filename, fn, l);
    file = get_file_by_name(filename);
}

void activate_nano()
{
    current_line = 0;
    nano_current_X = 0;
    nano_current_Y = 0;
    clear_screen();
    c = get_c_slots();
    get_keyboard_input(&open_file);
    read_blocks(current_line * NANO_WIDTH);
    filelen = (file.last_sector - file.start_sector) * 512;
}

// criar um rr do tamanho do ficheiro sendo limitado a  x mb
// dividir por linhas simplesmente torno o offset uma linha abaixo
// funçao q de acordo com a linha pega numa pag a frente da linha

// pedir ficheiro
// ter offset dividido em paginas width*height