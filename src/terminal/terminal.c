#include "kernel.h"
#include "terminal.h"
#include "memory/memory.h"
#include "string/string.h"
#include "idt/idt.h"
#include "disk/disk.h"
#include "nano/nano.h"
#include "raycaster/raycaster.h"
#include "exec/exec.h"
char letters[72][7][5] = {
    {
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
    },
    {
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"-----"},
        {"--#--"},
    },
    {
        {"-#-#-"},
        {"-#-#-"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
    },
    {
        {"-#-#-"},
        {"-#-#-"},
        {"#####"},
        {"-#-#-"},
        {"#####"},
        {"-#-#-"},
        {"-#-#-"},
    },
    {
        {"--#--"},
        {"-####"},
        {"#-#--"},
        {"-###-"},
        {"--#-#"},
        {"####-"},
        {"--#--"},
    },
    {
        {"##---"},
        {"##--#"},
        {"---#-"},
        {"--#--"},
        {"-#---"},
        {"#--##"},
        {"---##"},
    },

    {
        {"-###-"},
        {"#---#"},
        {"-#-#-"},
        {"-###-"},
        {"##--#"},
        {"#---"},
        {"-###-"},
    },
    {
        {"--#--"},
        {"--#--"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
    },

    {
        {"---#-"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"---#-"},
    },
    {
        {"-#---"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"-#---"},
    },
    {
        {"-----"},
        {"#-#-#"},
        {"-###-"},
        {"#####"},
        {"-###-"},
        {"#-#-#"},
        {"-----"},
    },
    {
        {"-----"},
        {"--#--"},
        {"--#--"},
        {"#####"},
        {"--#--"},
        {"--#--"},
        {"-----"},
    },
    {
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"--#--"},
        {"--#--"},
        {"-#---"},
    },

    {
        {"-----"},
        {"-----"},
        {"-----"},
        {"-###-"},
        {"-----"},
        {"-----"},
        {"-----"},
    },
    {
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"--#--"},
    },
    {
        {"-----"},
        {"----#"},
        {"---#-"},
        {"--#--"},
        {"-#---"},
        {"#----"},
        {"-----"},
    },

    {
        {"-###-"},
        {"#---#"},
        {"#--##"},
        {"#-#-#"},
        {"##--#"},
        {"#---#"},
        {"-###-"},
    },
    {
        {"--#--"},
        {"-##--"},
        {"#-#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"#####"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"----#"},
        {"--##-"},
        {"-#---"},
        {"#----"},
        {"#####"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"----#"},
        {"-###-"},
        {"----#"},
        {"#---#"},
        {"-###-"},
    },
    {
        {"---#-"},
        {"--##-"},
        {"-#-#-"},
        {"#--#-"},
        {"#####"},
        {"---#-"},
        {"---#-"},
    },
    {
        {"#####"},
        {"#----"},
        {"####-"},
        {"----#"},
        {"----#"},
        {"#---#"},
        {"-###-"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"#----"},
        {"####-"},
        {"#---#"},
        {"#---#"},
        {"-###-"},
    },
    {
        {"#####"},
        {"----#"},
        {"---#-"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"#---#"},
        {"-###-"},
        {"#---#"},
        {"#---#"},
        {"-###-"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"#---#"},
        {"-####"},
        {"----#"},
        {"#---#"},
        {"-###-"},
    },
    {
        {"-----"},
        {"-----"},
        {"--#--"},
        {"-----"},
        {"--#--"},
        {"-----"},
        {"-----"},
    },
    {
        {"-----"},
        {"-----"},
        {"--#--"},
        {"-----"},
        {"--#--"},
        {"-#---"},
        {"-----"},
    },
    {
        {"---#-"},
        {"--#--"},
        {"-#---"},
        {"#----"},
        {"-#---"},
        {"--#--"},
        {"---#-"},
    },

    {
        {"-----"},
        {"-----"},
        {"-###-"},
        {"-----"},
        {"-###-"},
        {"-----"},
        {"-----"},
    },
    {
        {"-#---"},
        {"--#--"},
        {"---#-"},
        {"----#"},
        {"---#-"},
        {"--#--"},
        {"-#---"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"----#"},
        {"---#-"},
        {"--#--"},
        {"-----"},
        {"--#--"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"#-###"},
        {"#-#-#"},
        {"#-##-"},
        {"#----"},
        {"-####"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#####"},
        {"#---#"},
        {"#---#"},
    },
    {
        {"####-"},
        {"#---#"},
        {"#---#"},
        {"####-"},
        {"#---#"},
        {"#---#"},
        {"####-"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"#----"},
        {"#----"},
        {"#----"},
        {"#---#"},
        {"-###-"},
    },
    {
        {"####-"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"####-"},
    },
    {
        {"#####"},
        {"#----"},
        {"#----"},
        {"####-"},
        {"#----"},
        {"#----"},
        {"#####"},
    },
    {
        {"#####"},
        {"#----"},
        {"#----"},
        {"####-"},
        {"#----"},
        {"#----"},
        {"#----"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"#----"},
        {"#-###"},
        {"#-#-#"},
        {"#---#"},
        {"-###-"},
    },
    {
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#####"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
    },
    {
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
    },
    {
        {"---#-"},
        {"---#-"},
        {"---#-"},
        {"---#-"},
        {"---#-"},
        {"#--#-"},
        {"-##--"},
    },
    {
        {"#---#"},
        {"#---#"},
        {"#--#-"},
        {"###--"},
        {"#--#-"},
        {"#---#"},
        {"#---#"},
    },
    {
        {"#----"},
        {"#----"},
        {"#----"},
        {"#----"},
        {"#----"},
        {"#----"},
        {"#####"},
    },
    {
        {"#---#"},
        {"##-##"},
        {"#-#-#"},
        {"#-#-#"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
    },
    {
        {"#---#"},
        {"#---#"},
        {"##--#"},
        {"#-#-#"},
        {"#--##"},
        {"#---#"},
        {"#---#"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"-###-"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"#---#"},
        {"####-"},
        {"#----"},
        {"#----"},
        {"#----"},
    },
    {
        {"-###-"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#-#-#"},
        {"#--##"},
        {"-####"},
    },
    {
        {"####-"},
        {"#---#"},
        {"#---#"},
        {"####-"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
    },
    {
        {"-####"},
        {"#----"},
        {"#----"},
        {"-###-"},
        {"----#"},
        {"----#"},
        {"####-"},
    },
    {
        {"#####"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
    },
    {
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"-###-"},
    },
    {
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"-#-#-"},
        {"--#--"},
    },
    {
        {"#---#"},
        {"#---#"},
        {"#---#"},
        {"#-#-#"},
        {"#-#-#"},
        {"##-##"},
        {"#---#"},
    },
    {
        {"#---#"},
        {"#---#"},
        {"-#-#-"},
        {"--#--"},
        {"-#-#-"},
        {"#---#"},
        {"#---#"},
    },
    {
        {"#---#"},
        {"#---#"},
        {"-#-#-"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
    },
    {
        {"#####"},
        {"----#"},
        {"---#-"},
        {"--#--"},
        {"-#---"},
        {"#----"},
        {"#####"},
    },

    {
        {"--#--"},
        {"-#-#-"},
        {"#---#"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
    },
    {
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"#---#"},
        {"-#-#-"},
        {"--#--"},
    },

    {
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
    },

    {
        {"-----"},
        {"#----"},
        {"-#---"},
        {"--#--"},
        {"---#-"},
        {"----#"},
        {"-----"},
    },
    {
        {"--###"},
        {"-#---"},
        {"####-"},
        {"-#---"},
        {"####-"},
        {"-#---"},
        {"--###"},
    },
    {
        {"--##-"},
        {"-#--#"},
        {"-#---"},
        {"####-"},
        {"-#---"},
        {"-#---"},
        {"#####"},
    },
    {
        {"--##-"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--##-"},
    },
    {
        {"-##--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"-##--"},
    },

    {
        {"---#-"},
        {"--#--"},
        {"--#--"},
        {"-#---"},
        {"--#--"},
        {"--#--"},
        {"---#-"},
    },

    {
        {"-#---"},
        {"--#--"},
        {"--#--"},
        {"---#-"},
        {"--#--"},
        {"--#--"},
        {"-#---"},
    },
    {
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"#####"},
    },
    {
        {"#####"},
        {"#####"},
        {"#####"},
        {"#####"},
        {"#####"},
        {"#####"},
        {"#####"},
    },

};

struct Let lettersF[72];
int current_X = 0;
int current_Y = 0;

char command[20];
int n_command = 0;

int input_mode;
char *input_str;
int input_n = 0;
int wait_one = 0;
char c_slots[TERMINAL_HEIGTH * TERMINAL_WIDTH];

void write_char_pixel_pos(int x, int y, int color)
{
    put_pixel(x, y, color);
    put_pixel(x + 1, y, color);
    put_pixel(x, y + 1, color);
    put_pixel(x + 1, y + 1, color);
}
void new_line()
{
    current_Y++;
    current_X = 0;
}

void write_char_pos(int x, int y, char c, int color)
{
    int k = c;
    if (k != 0)
    {
        k = c - 32;
    }
    if (k < 0 || k > 72)
    {
        return;
    }
    if (lettersF[k].n_pos == 0)
    {
        if (c == 32)
            c_slots[(y * TERMINAL_WIDTH) + x] = 32;
        else
            c_slots[(y * TERMINAL_WIDTH) + x] = 0;

        for (int i = 0; i < 7; i++)
        {
            for (int k = 0; k < 5; k++)
            {
                write_char_pixel_pos((k * 2) + (x * 11), (i * 2) + (y * 24), 0);
            }
        }
        return;
    }

    for (int i = 0; i < 7; i++)
    {
        for (int k = 0; k < 5; k++)
        {
            write_char_pixel_pos((k * 2) + (x * 11), (i * 2) + (y * 24), 0);
        }
    }
    for (int i = 0; i < lettersF[k].n_pos; i++)
    {
        struct XY xy;
        memcpy(&xy, lettersF[k].pos + (i * sizeof(struct XY)), sizeof(struct XY));
        write_char_pixel_pos((xy.x * 2) + (x * 11), (xy.y * 2) + (y * 24), color);
    }

    c_slots[(y * TERMINAL_WIDTH) + x] = c;
}

void clear_screen()
{
    for (int i = 0; i < TERMINAL_HEIGTH; i++)
    {
        for (int j = 0; j < TERMINAL_WIDTH; j++)
        {
            write_char_pos(j, i, 0, 0);
        }
    }
    current_X = 0;
    current_Y = 0;
}
void set_command()
{
    memset(command, 0x00, 20);
    for (int i = 0; i < 20; i++)
    {
        if (((c_slots[(current_Y * TERMINAL_WIDTH) + 9 + i] > 64 && c_slots[(current_Y * TERMINAL_WIDTH) + 9 + i] < 91)) || c_slots[(current_Y * TERMINAL_WIDTH) + 9 + i] == 32)
            command[i] = c_slots[((current_Y)*TERMINAL_WIDTH) + 9 + i];
    }
}

void write_char(char c, int color)
{
    if (wait_one)
    {
        wait_one = 0;
        return;
    }

    if (c == '\n')
    {
        if (input_mode)
        {
            set_command();
            memcpy(input_str, command, strlen(command));
            input_mode = 0;
            input_n = 0;
            wait_one = 1;

            return;
        }
        set_command();
        get_command(command);
        return;
    }
    if (input_mode)
    {
        input_str[input_n] = c;
        input_n++;
    }
    write_char_pos(current_X, current_Y, c, color);
    current_X++;
    if (current_X == TERMINAL_WIDTH)
    {
        current_X = 0;
        current_Y++;
    }
}

void input(int size)
{
    input_str = zalloc(size + 1);
    input_str[size + 1] = '\0';
    input_mode = 1;
    new_line();
    print("INPUT--->");
    while (input_mode)
    {
        int21h_handler_input();
    }
}

void get_command(char *command)
{
    if (cmpstring(command, "start"))
    {
        print("CONSOLE->");
        return;
    }
    else if (cmpstring(command, "new"))
    {
    }
    else if (cmpstring(command, "CLEAR"))
    {
        clear_screen();
        current_X = 0;
        current_Y = 0;
        get_command("start");
        return;
    }
    else if (cmpstring(command, "INPUT"))
    {
        input(6);
        new_line();
        print(input_str);
        free(input_str);
    }
    else if (cmpstring(command, "LS"))
    {
        ls();
    }
    else if (cmpstring(command, "CFILE"))
    {
        // falta checkar se o nome e repetido
        new_line();
        print("Write the name of the file (max 6):");
        input(6);
        if (input_str[0] == 0)
        {
            print("we need a normal input");
            goto console_finish;
        }
        char *file_name = zalloc(6);
        memcpy(file_name, input_str, 6);
        free(input_str);

        // falta checkar se sao numeros
        new_line();
        print("Write the lenght of the file (1 = 1kb = 1000 bytes, max = 5000)");
        input(6);

        if (input_str[0] == 0)
        {
            print("we need a normal input");
            goto console_finish;
        }

        char *file_lenght = zalloc(6);
        memcpy(file_lenght, input_str, 6);
        free(input_str);

        int size = number_to_digit(file_lenght);
        if (size < 1 || size > 5001)
        {
            print("file lenght error");
            free(file_lenght);
            free(file_name);
            goto console_finish;
        }
        size = size * 1000;
        char buf[size];
        memset(buf, 0, size);
        new_line();
        print(file_name);
        new_line();
        print(digit_to_number(size));
        write_file_to_memory(file_name, buf, size);
        free(file_lenght);
        free(file_name);
    }
    else if (cmpstring(command, "DFILE"))
    {
        new_line();
        print("Write the name of the file to be deleted (max 6):");
        input(6);
        char *file_name = zalloc(6);
        memcpy(file_name, input_str, 6);
        free(input_str);

        delete_File(file_name);

        free(file_name);
    }
    else if (cmpstring(command, "SAVE"))
    {
        save_on_disk();
        new_line();
    }
    else if (cmpstring(command, "NANO"))
    {
        new_line();
        print("instert file name(file must be at least 3 sectors/1536 bytes): ");
        input(6);
        new_line();
        set_filename(input_str);

        if (check_file_existance(input_str))
        {
            set_filename(input_str);
            free(input_str);
            terminal_mode = 0;
            activate_nano();
            goto quit_console;
        }
        else
        {
            free(input_str);
            print("file does not exist :D");
        }
    }
    else if (cmpstring(command, "TEST"))
    {
        dtest();
    }
    else if (cmpstring(command, "RAY"))
    {
        app = &raycaster_init;
    }
    else if (cmpstring(command, "RUN"))
    {
        new_line();
        print("instert file name: ");
        input(6);
        new_line();
        execute(input_str);
        free(input_str);
    }
    else
    {
        new_line();
        print(command);
        print(" is unrecognizable");
        new_line();
    }
console_finish:
    new_line();

    print("CONSOLE->");
quit_console:
}

void backspace()
{
    if (input_mode)
    {
        if (current_X <= 9)
        {
            return;
        }
        input_n--;
        input_str[input_n] = 0;
        current_X--;
        write_char_pos(current_X, current_Y, 0, 0);
        return;
    }
    if (current_X <= 9)
    {
        return;
    }
    current_X--;
    write_char_pos(current_X, current_Y, 0, 0);
}

void write_char_terminal(char c, int color)
{
    write_char_pos(current_X, current_Y, c, color);

    current_X++;
    if (current_X == TERMINAL_WIDTH)
    {
        current_X = 0;
        current_Y++;
    }
}

void print(char *str)
{
    if (terminal_mode == 0)
        return;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] > 90)
            str[i] = str[i] - 32;
        write_char_terminal(str[i], 0x00ff00);
    }
}

void printN(int n)
{
    print(digit_to_number(n));
}

void create_letters()
{
    for (int k = 0; k < 72; k++)
    {
        struct Let let;
        let.n_pos = 0;
        for (int i = 0; i < 7; i++)
        {
            for (int j = 0; j < 5; j++)
                if (letters[k][i][j] == '#')
                {
                    let.n_pos++;
                }
        }
        let.pos = zalloc(sizeof(struct XY) * let.n_pos);
        int n = 0;
        for (int i = 0; i < 7; i++)
        {
            for (int j = 0; j < 5; j++)
                if (letters[k][i][j] == '#')
                {
                    struct XY xy;
                    xy.x = j;
                    xy.y = i;
                    memcpy(let.pos + (n * sizeof(struct XY)), &xy, sizeof(struct XY));
                    n++;
                }
        }
        lettersF[k] = let;
    }
}

void start_terminal_mode()
{
    terminal_mode = 1;
    clear_pixels_screen();
    wait_one = 0;
    input_mode = 0;
    current_X = 0;
    current_Y = 0;
    create_letters();
    clear_screen();
    print("CONSOLE->");
}

char *get_c_slots()
{
    return c_slots;
}