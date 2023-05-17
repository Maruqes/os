#ifndef TERMINAL_H
#define TERMINAL_H
#define TERMINAL_WIDTH 72
#define TERMINAL_HEIGTH 25
struct XY
{
    int x;
    int y;
} __attribute__((packed));

struct Let
{
    int n_pos;
    struct XY *pos;
} __attribute__((packed));

void start_terminal_mode();
void create_letters();
void write_char_pos(int x, int y, char c, int color);
void get_command(char *command);
void write_char(char c, int color);
void backspace();
void print(char *str);
void printN(int n);
void new_line();
void clear_screen();
char *get_c_slots();
void input(int size);

extern char *input_str;

#endif