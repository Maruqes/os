#include <stddef.h>
#include <stdint.h>
#include "os.h"

char *lett;

int *numbers_save;
int *numbers_save_for_calc;

int mouse_pressed;

int opp;

char letters[18][7][5] = {
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
        {"--#--"},
        {"--#--"},
        {"--#--"},
        {"#####"},
        {"--#--"},
        {"--#--"},
        {"--#--"},
    },
    {
        {"-----"},
        {"-----"},
        {"-----"},
        {"#####"},
        {"-----"},
        {"-----"},
        {"-----"},
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
        {"-----"},
        {"#---#"},
        {"-#-#-"},
        {"--#--"},
        {"-#-#-"},
        {"#---#"},
        {"-----"},
    },

    {
        {"####-"},
        {"#---#"},
        {"#---#"},
        {"####-"},
        {"#----"},
        {"#----"},
        {"#----"},
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
        {"-####"},
        {"#----"},
        {"#----"},
        {"#----"},
        {"#----"},
        {"#----"},
        {"-####"},
    },

    {
        {"#####"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"-----"},
        {"#####"},
    },

};

char *t;

char *digit_to_number(int n)
{
    int count = 0;
    int dig = n;

    if (dig == 0)
        count = 1;
    while (dig != 0)
    {
        dig = dig / 10;
        ++count;
    }

    free(t);
    t = zalloc(count + 1);
    t[count + 1] = '\0';
    for (int j = 0; j < count; j++)
    {
        int res = n;

        for (int i = 0; i < count - (1 * (j + 1)); i++)
        {
            res = res / 10;
        }

        t[j] = res + '0';

        for (int i = 0; i < count - (1 * (j + 1)); i++)
        {
            res = res * 10;
        }

        n = n - res;
    }
    return t;
}

int number_to_digit()
{
    int res = 0;
    for (int i = 0; i < 6; ++i)
    {
        if (numbers_save[i] >= 0 && numbers_save[i] <= 9)
        {
            res = res * 10 + numbers_save[i];
        }
        else
        {
            return -1;
        }
    }

    return res;
}

void put_pixel(int x, int y, int color, unsigned int *buf)
{
    buf[y * 800 + x] = color;
}

void draw_square(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int *buf, unsigned int color)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            put_pixel(x + j, y + i, color, buf);
        }
    }
}

void draw_square_exce(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int *buf, unsigned int color)
{
    for (int j = 0; j < width; j++)
    {
        put_pixel(x + j, y, color, buf);
        put_pixel(x + j, y + height, color, buf);
    }
    for (int j = 0; j < height; j++)
    {
        put_pixel(x, y + j, color, buf);
        put_pixel(x + width, y + j, color, buf);
    }
}

int draw_x_line(unsigned int x_start, unsigned int x_end, unsigned int y, unsigned int *buf, unsigned int color)
{
    int length = -1;
    unsigned int starting_point = -1;
    if (x_start < x_end)
    {
        length = x_end - x_start;
        starting_point = x_start;
    }

    if (x_end < x_start)
    {
        length = x_start - x_end;
        starting_point = x_end;
    }

    if (length == -1)
        return -1;

    for (int i = 0; i < length; i++)
    {
        put_pixel(starting_point + i, y, color, buf);
    }

    return 0;
}

int draw_y_line(unsigned int y_start, unsigned int y_end, unsigned int x, unsigned int *buf, unsigned int color)
{
    int length = -1;
    unsigned int starting_point = -1;
    if (y_start < y_end)
    {
        length = y_end - y_start;
        starting_point = y_start;
    }

    if (y_end < y_start)
    {
        length = y_start - y_end;
        starting_point = y_end;
    }

    if (length == -1)
        return -1;

    for (int i = 0; i < length; i++)
    {
        put_pixel(x, starting_point + i, color, buf);
    }

    return 0;
}

void draw_calc_numbers(unsigned int *buf)
{
    // 1
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 1)] == '#')
            {
                draw_square(j + (j * 4) + 455, i + (i * 4) + 508, 5, 5, buf, 0x000fff);
            }
        }
    }
    // 2
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 2)] == '#')
            {
                draw_square(j + (j * 4) + 588, i + (i * 4) + 508, 5, 5, buf, 0x000fff);
            }
        }
    }
    // 3
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 3)] == '#')
            {
                draw_square(j + (j * 4) + 721, i + (i * 4) + 508, 5, 5, buf, 0x000fff);
            }
        }
    }
    // 4
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 4)] == '#')
            {
                draw_square(j + (j * 4) + 455, i + (i * 4) + 358, 5, 5, buf, 0x000fff);
            }
        }
    }
    // 5
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 5)] == '#')
            {
                draw_square(j + (j * 4) + 588, i + (i * 4) + 358, 5, 5, buf, 0x000fff);
            }
        }
    }
    // 6
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 6)] == '#')
            {
                draw_square(j + (j * 4) + 721, i + (i * 4) + 358, 5, 5, buf, 0x000fff);
            }
        }
    }
    // 7
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 7)] == '#')
            {
                draw_square(j + (j * 4) + 455, i + (i * 4) + 208, 5, 5, buf, 0x000fff);
            }
        }
    }
    // 8
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 8)] == '#')
            {
                draw_square(j + (j * 4) + 588, i + (i * 4) + 208, 5, 5, buf, 0x000fff);
            }
        }
    }
    // 9
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 9)] == '#')
            {
                draw_square(j + (j * 4) + 721, i + (i * 4) + 208, 5, 5, buf, 0x000fff);
            }
        }
    }
    //+
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 10)] == '#')
            {
                draw_square(j + (j * 4) + 055, i + (i * 4) + 208, 5, 5, buf, 0x000fff);
            }
        }
    }
    //-
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 11)] == '#')
            {
                draw_square(j + (j * 4) + 188, i + (i * 4) + 208, 5, 5, buf, 0x000fff);
            }
        }
    }
    // /
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 12)] == '#')
            {
                draw_square(j + (j * 4) + 321, i + (i * 4) + 208, 5, 5, buf, 0x000fff);
            }
        }
    }
    // *
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 13)] == '#')
            {
                draw_square(j + (j * 4) + 055, i + (i * 4) + 358, 5, 5, buf, 0x000fff);
            }
        }
    }
    // p
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 14)] == '#')
            {
                draw_square(j + (j * 4) + 188, i + (i * 4) + 358, 5, 5, buf, 0x000fff);
            }
        }
    }
    // s
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 15)] == '#')
            {
                draw_square(j + (j * 4) + 321, i + (i * 4) + 358, 5, 5, buf, 0x000fff);
            }
        }
    }
    // c
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 16)] == '#')
            {
                draw_square(j + (j * 4) + 055, i + (i * 4) + 508, 5, 5, buf, 0x000fff);
            }
        }
    }

    // =
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * 17)] == '#')
            {
                draw_square(j + (j * 4) + 321, i + (i * 4) + 508, 5, 5, buf, 0x000fff);
            }
        }
    }
}

void draw_letter_pos(unsigned int x, unsigned int y, int n, unsigned int *buf)
{
    if (x == -1 && y == -1 && n == -1) // clear
    {
        draw_square(0, 0, 800, 150, buf, 0x000000);

        return;
    }

    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            draw_square(j + (j * 4) + x, i + (i * 4) + y, 5, 5, buf, 0x000000);
        }
    }
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (lett[(i * 5) + j + ((7 * 5) * n)] == '#')
            {
                draw_square(j + (j * 4) + x, i + (i * 4) + y, 5, 5, buf, 0x000fff);
            }
        }
    }
}

void update_calc(unsigned int *buf)
{
    for (int i = 0; i < 6; i++)
    {
        if (numbers_save[i] == -1)
        {
            continue;
        }
        draw_letter_pos((i * 133 + 055), 58, numbers_save[i], buf);
    }
}

void draw_letter(int n, unsigned int *buf)
{
    if (n == -1)
    {
        draw_letter_pos(n, n, n, buf);
        return;
    }

    for (int i = 0; i < 6; i++)
    {
        numbers_save[i] = numbers_save[i + 1];
    }

    numbers_save[5] = n;

    // for (int i = 0; i < 6; i++)
    // {
    //     if (numbers_save[5 - i] == -1)
    //     {
    //         numbers_save[5 - i] = n;
    //         draw_letter_pos(721 - ((i)*133), 58, n, buf);
    //         sleep_this(50);
    //         break;
    //     }
    // }
    update_calc(buf);
    sleep_this(50);
}

void clear_calc(unsigned int *buf)
{
    for (int i = 0; i < 6; i++)
    {
        numbers_save[i] = -1; // -1 = no number
    }
    draw_letter(-1, buf);
}

void calculate_solution(unsigned int *buf)
{
    if (opp == 0)
    {
        draw_y_line(150, 600, 267, buf, 0xff000f);
    }
}

int main()
{
    get_OS_FUNCTIONS_addr();
    finish_int();
    enable_int();

    // clear_screen();
    // test_prgm();

    unsigned int *buf = set_buffer(); // checkar por 0

    for (int i = 0; i < 600 * 800; i++)
    {
        buf[i] = 0x000000;
    }

    draw_x_line(0, 800, 150, buf, 0xffffff);

    draw_x_line(0, 800, 300, buf, 0xffffff);
    draw_x_line(0, 800, 450, buf, 0xffffff);

    draw_y_line(150, 600, 400, buf, 0xffffff);
    draw_y_line(150, 600, 533, buf, 0xffffff);
    draw_y_line(150, 600, 666, buf, 0xffffff);
    draw_y_line(150, 600, 267, buf, 0xffffff);
    draw_y_line(150, 600, 134, buf, 0xffffff);

    lett = zalloc(18 * 7 * 5);
    memcpy_prgm(lett, letters, 18 * 7 * 5);

    draw_calc_numbers(buf);

    numbers_save = zalloc(6 * sizeof(int));
    numbers_save_for_calc = zalloc(6 * sizeof(int));

    for (int i = 0; i < 6; i++)
    {
        numbers_save[i] = -1; // -1 = no number
    }

    Mouse_struct *ms;
    while (1)
    {
        ms = get_mouse_info();
        if ((ms->mouse_info & 0b00000001) == 0b00000001 && mouse_pressed == 0)
        {
            if (ms->y > 150 && ms->y < 300 && ms->x > 0 && ms->x < 133)
            {
                //+
                memcpy(numbers_save_for_calc, numbers_save, 6 * sizeof(int));
                clear_calc(buf);
                opp = 0;
            }
            else if (ms->y > 150 && ms->y < 300 && ms->x > 133 && ms->x < 266)
            {
                //-
                memcpy(numbers_save_for_calc, numbers_save, 6 * sizeof(int));
                clear_calc(buf);
                opp = 1;
            }
            else if (ms->y > 150 && ms->y < 300 && ms->x > 266 && ms->x < 400)
            {
                // /
                memcpy(numbers_save_for_calc, numbers_save, 6 * sizeof(int));
                clear_calc(buf);
                opp = 2;
            } ///////
            else if (ms->y > 300 && ms->y < 450 && ms->x > 0 && ms->x < 133)
            {
                // x
                memcpy(numbers_save_for_calc, numbers_save, 6 * sizeof(int));
                clear_calc(buf);
                opp = 3;
            }
            else if (ms->y > 450 && ms->y < 600 && ms->x > 0 && ms->x < 133)
            {
                // c
                clear_calc(buf);
            }
            else if (ms->y > 450 && ms->y < 600 && ms->x > 266 && ms->x < 400)
            {
                // =
                calculate_solution(buf);
            }
            else if (ms->y > 300 && ms->y < 450 && ms->x > 133 && ms->x < 266)
            {
                // p
                memcpy(numbers_save_for_calc, numbers_save, 6 * sizeof(int));
                clear_calc(buf);
                opp = 4;
            }
            else if (ms->y > 300 && ms->y < 450 && ms->x > 266 && ms->x < 400)
            {
                // s
                memcpy(numbers_save_for_calc, numbers_save, 6 * sizeof(int));
                clear_calc(buf);
                opp = 5;
            } ///////
            else if (ms->y > 450 && ms->y < 600 && ms->x > 400 && ms->x < 533)
            {
                // 1
                draw_letter(1, buf);
            }
            else if (ms->y > 450 && ms->y < 600 && ms->x > 533 && ms->x < 666)
            {
                // 2
                draw_letter(2, buf);
            }
            else if (ms->y > 450 && ms->y < 600 && ms->x > 666 && ms->x < 800)
            {
                // 3
                draw_letter(3, buf);
            } ///////
            else if (ms->y > 300 && ms->y < 450 && ms->x > 400 && ms->x < 533)
            {
                // 4
                draw_letter(4, buf);
            }
            else if (ms->y > 300 && ms->y < 450 && ms->x > 533 && ms->x < 666)
            {
                // 5
                draw_letter(5, buf);
            }
            else if (ms->y > 300 && ms->y < 450 && ms->x > 666 && ms->x < 800)
            {
                // 6
                draw_letter(6, buf);
            } ///////
            else if (ms->y > 150 && ms->y < 300 && ms->x > 400 && ms->x < 533)
            {
                // 7
                draw_letter(7, buf);
            }
            else if (ms->y > 150 && ms->y < 300 && ms->x > 533 && ms->x < 666)
            {
                // 8
                draw_letter(8, buf);
            }
            else if (ms->y > 150 && ms->y < 300 && ms->x > 666 && ms->x < 800)
            {
                // 9
                draw_letter(9, buf);
            }
            mouse_pressed = 1;
        }
        if ((ms->mouse_info & 0b00000000) == 0b00000000)
        {
            mouse_pressed = 0;
        }
    }

    quit_app();
    return 0;
}