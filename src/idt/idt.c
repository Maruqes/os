#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"
#include "keyboardHandler/keyboardHandler.h"
#include "terminal/terminal.h"
#include "string/string.h"
#include "mouse/mouse.h"
#include "exec/exec.h"

struct idt_desc idt_descriptors[GAYOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc *ptr);
extern void int21h();
extern void int32h();
extern void no_interrupt();
extern void finish_int();

extern void printINT();
extern void quit_appINT();
extern void inputINT();
extern void printADDRINT();
extern void mallocINT();
extern void freeINT();
extern void screenINT();
extern void sleepINT();

int timer_ticks = 0;
int is_sleeping = 0;
int hz;

void get_hz(float hzz)
{
    hz = hzz;
}

void finish_int()
{
    outb(0x20, 0x20);
    if (is_sleeping)
    {
        timer_ticks++;
    }
}

void finish_int_slave_pic()
{
    outb(0xA0, 0x20);
    if (is_sleeping)
    {
        timer_ticks++;
    }
}

int sleep(int mills)
{
    timer_ticks = 0;
    int a = (mills * hz) / 1000;
    is_sleeping = 1;
wait:
    if (timer_ticks > a)
    {
        is_sleeping = 0;
        timer_ticks = 0;
        return 0;
    }
    else
    {
        goto wait;
    }
}

void start_count()
{
    timer_ticks = 0;
    is_sleeping = 1;
}

int finish_count()
{
    is_sleeping = 0;
    return timer_ticks;
}

void int21h_handler()
{
    char k = insb(0x60);
    handleRawInfoFromKeyBoard(k);
    finish_int();
}
void int21h_handler_input()
{
    char k = insb(0x60);
    handleRawInfoFromKeyBoardInput(k);
    finish_int();
}

void idt_zero()
{
    print("Divide by zero error");
    finish_int();
}

///////

void idt_printINT(uint32_t address)
{
    print((char *)(addr_program + p_offset + address));
}

void idt_quit_appINT()
{
    quit_app();
    if (terminal_mode == 0)
        start_terminal_mode();
}

void idt_inputINT(uint32_t input_len)
{
    // input is always on (input_str)
    new_line();
    input(input_len);
    new_line();
}

void idt_printADDRINT(char *address)
{
    print(address);
}

void *int_malloc_addr;
void int_mallocINT(int size)
{
    int_malloc_addr = malloc(size);
}

void int_freeINT(void *ptr)
{
    free(ptr);
}

void int_screenINT()
{
    terminal_mode = 0;
}

void int_sleepINT(int mili)
{
    // sleep(mili); // not working
}

////////

void idt_set(int interrupt_no, void *address)
{
    struct idt_desc *desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t)address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0xEE;
    desc->offset_2 = (uint32_t)address >> 16;
}

void int32h_handler()
{
    handle_mouse_info();
    finish_int();
    finish_int_slave_pic(); // vem o int 12(basicamente tem 2 plaquinhas (0-6 1º plaquinha)) tem a ver com o pic todo fdd
}

void idt_init()
{

    // PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    // ICW2
    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    // ICW3
    outb(0x21, 0x04); // tell PIC1 that it has a slave at IRQ2 (0000 0100)
    outb(0xA1, 0x02); // tell PIC2 its cascade identity (0000 0010)

    // ICW4
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    // OCW1
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;

    for (int i = 0; i < GAYOS_TOTAL_INTERRUPTS; i++)
    {
        idt_set(i, no_interrupt);
    }

    idt_set(0, idt_zero);
    idt_set(0x21, int21h); // 33 em decimal
    idt_set(0x2C, int32h); // mouse(driver) //44 em decimal

    idt_set(16, printINT);     // print
    idt_set(17, quit_appINT);  // quit
    idt_set(18, inputINT);     // input
    idt_set(19, printADDRINT); // printADDR
    idt_set(20, mallocINT);    // malloc
    idt_set(21, freeINT);      // free
    idt_set(22, screenINT);    // get_screen_buf
    idt_set(23, sleepINT);     // sleep

    // keyboard, mouse, tela

    idt_load(&idtr_descriptor);
}