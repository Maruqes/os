#include <stddef.h>
#include <stdint.h>
#include "os.h"

#define APP_SCREEN_WIDTH 800
#define APP_SCREEN_HEIGHT 600
#define DISPLAY_DIGITS 6

#define OP_NONE -1
#define OP_ADD 0
#define OP_SUB 1
#define OP_DIV 2
#define OP_MUL 3
#define OP_POW 4
#define OP_SQRT 5

#define GLYPH_MINUS 11
#define BUTTON_LEFT_DOWN 0b00000001

char *lett;

int *numbers_save;
int *numbers_save_for_calc;

int mouse_pressed;

int opp;

extern void quit_app();

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

void clear_calc(unsigned int *buf);

static int digits_array_to_int(const int *digits, int *out_value)
{
    int sign = 1;
    int saw_sign = 0;
    int saw_digit = 0;
    int value = 0;

    for (int i = 0; i < DISPLAY_DIGITS; i++)
    {
        int digit = digits[i];
        if (digit == -1)
        {
            if (saw_sign || saw_digit)
            {
                return 0;
            }
            continue;
        }

        if (digit == GLYPH_MINUS && !saw_sign && !saw_digit)
        {
            sign = -1;
            saw_sign = 1;
            continue;
        }

        if (digit < 0 || digit > 9)
        {
            return 0;
        }

        saw_digit = 1;
        value = value * 10 + digit;
    }

    if (!saw_digit)
    {
        return 0;
    }

    *out_value = value * sign;
    return 1;
}

static int value_to_digits_array(int value, int *digits)
{
    for (int i = 0; i < DISPLAY_DIGITS; i++)
    {
        digits[i] = -1;
    }

    int is_negative = value < 0;
    long long abs_value = value;
    if (abs_value < 0)
    {
        abs_value = -abs_value;
    }

    int idx = DISPLAY_DIGITS - 1;
    do
    {
        if (idx < 0)
        {
            return 0;
        }
        digits[idx] = (int)(abs_value % 10);
        abs_value = abs_value / 10;
        idx--;
    } while (abs_value > 0);

    if (is_negative)
    {
        if (idx < 0)
        {
            return 0;
        }
        digits[idx] = GLYPH_MINUS;
    }

    return 1;
}

char *digit_to_number(int n)
{
    int is_negative = n < 0;
    long long value = n;
    if (value < 0)
    {
        value = -value;
    }

    int count = 1;
    long long dig = value;
    while (dig >= 10)
    {
        dig = dig / 10;
        count++;
    }
    if (is_negative)
    {
        count++;
    }

    free(t);
    t = zalloc(count + 1);
    if (!t)
    {
        return 0;
    }

    t[count] = '\0';
    int idx = count - 1;
    do
    {
        t[idx] = (char)('0' + (value % 10));
        value = value / 10;
        idx--;
    } while (value > 0 && idx >= 0);

    if (is_negative)
    {
        t[0] = '-';
    }

    return t;
}

int number_to_digit()
{
    int value = 0;
    if (!digits_array_to_int(numbers_save, &value))
    {
        return -1;
    }
    return value;
}

void put_pixel(int x, int y, int color, unsigned int *buf)
{
    if (!buf || x < 0 || y < 0 || x >= APP_SCREEN_WIDTH || y >= APP_SCREEN_HEIGHT)
    {
        return;
    }

    buf[y * APP_SCREEN_WIDTH + x] = color;
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

void draw_letter_pos(int x, int y, int n, unsigned int *buf)
{
    if (x == -1 && y == -1 && n == -1) // clear
    {
        draw_square(0, 0, APP_SCREEN_WIDTH, 150, buf, 0x000000);

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
    draw_letter_pos(-1, -1, -1, buf);

    for (int i = 0; i < DISPLAY_DIGITS; i++)
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
    if (n < 0 || n > 9)
    {
        return;
    }

    for (int i = 0; i < DISPLAY_DIGITS; i++)
    {
        if (numbers_save[i] != -1 && (numbers_save[i] < 0 || numbers_save[i] > 9))
        {
            clear_calc(buf);
            break;
        }
    }

    for (int i = 0; i < DISPLAY_DIGITS - 1; i++)
    {
        numbers_save[i] = numbers_save[i + 1];
    }

    numbers_save[DISPLAY_DIGITS - 1] = n;

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
    for (int i = 0; i < DISPLAY_DIGITS; i++)
    {
        numbers_save[i] = -1; // -1 = no number
    }
    update_calc(buf);
}

void clear_saved_calc()
{
    for (int i = 0; i < DISPLAY_DIGITS; i++)
    {
        numbers_save_for_calc[i] = -1;
    }
}

int save_operation_state(unsigned int *buf, int operation)
{
    int current_value = 0;
    if (!digits_array_to_int(numbers_save, &current_value))
    {
        return 0;
    }

    (void)current_value;
    memcpy(numbers_save_for_calc, numbers_save, DISPLAY_DIGITS * sizeof(int));
    opp = operation;
    clear_calc(buf);
    return 1;
}

void set_error_display(unsigned int *buf)
{
    for (int i = 0; i < DISPLAY_DIGITS; i++)
    {
        numbers_save[i] = GLYPH_MINUS;
    }
    update_calc(buf);
}

int pow_int_checked(int base, int exponent, int *out_value)
{
    if (exponent < 0)
    {
        return 0;
    }

    long long result = 1;
    long long factor = base;

    while (exponent > 0)
    {
        if ((exponent & 1) == 1)
        {
            result = result * factor;
            if (result > 2147483647LL || result < -2147483648LL)
            {
                return 0;
            }
        }

        exponent = exponent >> 1;
        if (exponent > 0)
        {
            factor = factor * factor;
            if (factor > 2147483647LL || factor < -2147483648LL)
            {
                return 0;
            }
        }
    }

    *out_value = (int)result;
    return 1;
}

int sqrt_int_floor(int value)
{
    if (value <= 0)
    {
        return 0;
    }

    int low = 0;
    int high = (value < 46340) ? value : 46340;
    int result = 0;

    while (low <= high)
    {
        int mid = low + ((high - low) / 2);
        int sq = mid * mid;
        if (sq == value)
        {
            return mid;
        }
        if (sq < value)
        {
            result = mid;
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    return result;
}

int calculate_operation(int left_value, int right_value, int operation, int *out_value)
{
    long long result = 0;

    switch (operation)
    {
    case OP_ADD:
        result = (long long)left_value + right_value;
        break;
    case OP_SUB:
        result = (long long)left_value - right_value;
        break;
    case OP_DIV:
        if (right_value == 0)
        {
            return 0;
        }
        result = left_value / right_value;
        break;
    case OP_MUL:
        result = (long long)left_value * right_value;
        break;
    case OP_POW:
        return pow_int_checked(left_value, right_value, out_value);
    case OP_SQRT:
        if (left_value < 0)
        {
            return 0;
        }
        *out_value = sqrt_int_floor(left_value);
        return 1;
    default:
        return 0;
    }

    if (result > 2147483647LL || result < -2147483648LL)
    {
        return 0;
    }

    *out_value = (int)result;
    return 1;
}

void calculate_solution(unsigned int *buf)
{
    int first_value = 0;
    int second_value = 0;
    int result = 0;

    if (opp == OP_NONE)
    {
        return;
    }

    if (!digits_array_to_int(numbers_save_for_calc, &first_value))
    {
        set_error_display(buf);
        clear_saved_calc();
        opp = OP_NONE;
        return;
    }

    if (opp != OP_SQRT)
    {
        if (!digits_array_to_int(numbers_save, &second_value))
        {
            set_error_display(buf);
            clear_saved_calc();
            opp = OP_NONE;
            return;
        }
    }
    else if (digits_array_to_int(numbers_save, &second_value))
    {
        // Allow "S then number then =" as well as "number then S then =".
        first_value = second_value;
    }

    if (!calculate_operation(first_value, second_value, opp, &result))
    {
        set_error_display(buf);
        clear_saved_calc();
        opp = OP_NONE;
        return;
    }

    if (!value_to_digits_array(result, numbers_save))
    {
        set_error_display(buf);
        clear_saved_calc();
        opp = OP_NONE;
        return;
    }

    update_calc(buf);
    clear_saved_calc();
    opp = OP_NONE;
}

int main()
{
    get_OS_FUNCTIONS_addr();

    enable_int();
    finish_int();

    char *runn = "RUNNING...";

    print(runn);

    new_line();
    // if (test_dot_data() == 0)
    // {
    //     dot_data_crash();
    //     return 0;
    // }

    // if (test_ro_data() == 0)
    // {
    //     // dot_rodata_crash();
    //     return 0;
    // }

    sleep(5000);
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
    memcpy(lett, letters, 18 * 7 * 5);

    draw_calc_numbers(buf);

    numbers_save = zalloc(DISPLAY_DIGITS * sizeof(int));
    numbers_save_for_calc = zalloc(DISPLAY_DIGITS * sizeof(int));

    for (int i = 0; i < DISPLAY_DIGITS; i++)
    {
        numbers_save[i] = -1; // -1 = no number
    }
    clear_saved_calc();
    opp = OP_NONE;
    mouse_pressed = 0;

    update_calc(buf);

    Mouse_struct *ms;
    while (1)
    {
        ms = get_mouse_info();
        if ((ms->mouse_info & BUTTON_LEFT_DOWN) == BUTTON_LEFT_DOWN && mouse_pressed == 0)
        {
            if (ms->y > 150 && ms->y < 300 && ms->x > 0 && ms->x < 133)
            {
                //+
                save_operation_state(buf, OP_ADD);
            }
            else if (ms->y > 150 && ms->y < 300 && ms->x > 133 && ms->x < 266)
            {
                //-
                save_operation_state(buf, OP_SUB);
            }
            else if (ms->y > 150 && ms->y < 300 && ms->x > 266 && ms->x < 400)
            {
                // /
                save_operation_state(buf, OP_DIV);
            } ///////
            else if (ms->y > 300 && ms->y < 450 && ms->x > 0 && ms->x < 133)
            {
                // x
                save_operation_state(buf, OP_MUL);
            }
            else if (ms->y > 450 && ms->y < 600 && ms->x > 0 && ms->x < 133)
            {
                // c
                clear_calc(buf);
                clear_saved_calc();
                opp = OP_NONE;
            }
            else if (ms->y > 450 && ms->y < 600 && ms->x > 266 && ms->x < 400)
            {
                // =
                calculate_solution(buf);
            }
            else if (ms->y > 300 && ms->y < 450 && ms->x > 133 && ms->x < 266)
            {
                // p
                save_operation_state(buf, OP_POW);
            }
            else if (ms->y > 300 && ms->y < 450 && ms->x > 266 && ms->x < 400)
            {
                // s
                save_operation_state(buf, OP_SQRT);
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
        if ((ms->mouse_info & BUTTON_LEFT_DOWN) == 0)
        {
            mouse_pressed = 0;
        }
    }

    quit_app();
    return 0;
}
