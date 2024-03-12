#include "keyboardHandler.h"
#include "kernel.h"
#include "terminal/terminal.h"
#include "exec/exec.h"
#include "idt/idt.h"
#include "multitasking/multitasking.h"
#include "window_management/window_management.h"

struct keys_from_code_to_ascii keys_struct[170];
char a[2];
char lastKeyPressed = 0x00;
int firstRelease = 0;
int control_pressed = 0;

void set_key_to_code(int hex, char key)
{
    keys_struct[hex].code = hex;
    keys_struct[hex].ascii_code = key;
}

void startKeyboardHandler()
{
    set_key_to_code(0x01, 27);
    set_key_to_code(0x02, 49);
    set_key_to_code(0x03, 50);
    set_key_to_code(0x04, 51);
    set_key_to_code(0x05, 52);
    set_key_to_code(0x06, 53);
    set_key_to_code(0x07, 54);
    set_key_to_code(0x08, 55);
    set_key_to_code(0x09, 56);
    set_key_to_code(0x0A, 57);
    set_key_to_code(0x0B, 48);
    set_key_to_code(0x0C, 39);
    set_key_to_code(0x0D, 60);
    set_key_to_code(0x0E, -2); // backspace
    set_key_to_code(0x0F, 9);
    set_key_to_code(0x10, 'Q');
    set_key_to_code(0x11, 'W');
    set_key_to_code(0x12, 'E');
    set_key_to_code(0x13, 'R');
    set_key_to_code(0x14, 'T');
    set_key_to_code(0x15, 'Y');
    set_key_to_code(0x16, 'U');
    set_key_to_code(0x17, 'I');
    set_key_to_code(0x18, 'O');
    set_key_to_code(0x19, 'P');
    set_key_to_code(0x1A, '+');
    set_key_to_code(0x1B, 96);
    set_key_to_code(0x1C, -3); // enter
    set_key_to_code(0x1D, -1);
    set_key_to_code(0x1E, 'A');
    set_key_to_code(0x1F, 'S');
    set_key_to_code(0x20, 'D');
    set_key_to_code(0x21, 'F');
    set_key_to_code(0x22, 'G');
    set_key_to_code(0x23, 'H');
    set_key_to_code(0x24, 'J');
    set_key_to_code(0x25, 'K');
    set_key_to_code(0x26, 'L');
    set_key_to_code(0x27, 'C');
    set_key_to_code(0x28, 248);
    set_key_to_code(0x29, '~');
    set_key_to_code(0x2A, -1); // left shift
    set_key_to_code(0x2B, 60);
    set_key_to_code(0x2C, 'Z');
    set_key_to_code(0x2D, 'X');
    set_key_to_code(0x2E, 'C');
    set_key_to_code(0x2F, 'V');
    set_key_to_code(0x30, 'B');
    set_key_to_code(0x31, 'N');
    set_key_to_code(0x32, 'M');
    set_key_to_code(0x33, ',');
    set_key_to_code(0x34, '.');
    set_key_to_code(0x35, '-');
    set_key_to_code(0x36, '-');
    set_key_to_code(0x39, ' ');
    set_key_to_code(0x48, -4); // arrow up;
    set_key_to_code(0x50, -5); // arrow down;
    set_key_to_code(0x4B, -6); // arrow left;
    set_key_to_code(0x4D, -7); // arrow right;
    set_key_to_code(0x1D, -8); // left control;
}

void handleRawInfoFromKeyBoard(char c)
{
    for (int i = 0; i < sizeof(keys_struct) / sizeof(keys_struct[0]); i++)
    {
        if (c == lastKeyPressed - 0x80)
        {
            return;
        }
        if (c == keys_struct[i].code)
        {
            if (keys_struct[i].ascii_code == 'B')
            {
                execute("OIU");
                return;
            }
            else if (keys_struct[i].ascii_code == 'N')
            {
                execute("ABC");
                return;
            }
            else if (keys_struct[i].ascii_code == 'M')
            {
                execute("PFF");
                return;
            }
            else if (keys_struct[i].ascii_code == -8)
            // if (keys_struct[i].ascii_code == -8)
            {
                control_pressed = 1;
            }
            else if (keys_struct[i].ascii_code >= 48 && keys_struct[i].ascii_code <= 57 && control_pressed == 1)
            {
                control_pressed = 0;
                change_screens(keys_struct[i].ascii_code - 48);
                return;
            }
            lastKeyPressed = keys_struct[i].code;

            if (terminal_mode == 1)
            {
                if (keys_struct[i].ascii_code == -2)
                {
                    backspace();
                    return;
                }
                else if (keys_struct[i].ascii_code == -3)
                {
                    write_char('\n', 0);
                    return;
                }
                write_char(keys_struct[i].ascii_code, 0x00ff00);
                return;
            }
            else
            {
                (*console_controler)(keys_struct[i].ascii_code);
                return;
            }
        }
    }
}

void handleRawInfoFromKeyBoardInput(char c)
{
    for (int i = 0; i < sizeof(keys_struct) / sizeof(keys_struct[0]); i++)
    {

        if (c == lastKeyPressed - 0x80)
        {
            lastKeyPressed = 0x00;
            return;
        }
        if (keys_struct[i].code == c && lastKeyPressed == 0x00)
        {
            lastKeyPressed = keys_struct[i].code;

            if (keys_struct[i].ascii_code == -2)
            {
                backspace();
                return;
            }
            else if (keys_struct[i].ascii_code == -3)
            {
                write_char('\n', 0);
                return;
            }
            write_char(keys_struct[i].ascii_code, 0x00ff00);
            return;
        }
    }
}
