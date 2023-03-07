#ifndef keyboardHandler_H
#define keyboardHandler_H

struct keys_from_code_to_ascii
{
    char code;       // Size of descriptor table -1
    char ascii_code; // Base address of the start of the interrupt descriptor table
} __attribute__((packed));

void handleRawInfoFromKeyBoard(char c);
void startKeyboardHandler();
void handleRawInfoFromKeyBoardInput(char c);
#endif