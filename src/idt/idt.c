#include "idt.h"
#include "config.h"
#include "kernel.h"
#include "memory/memory.h"
#include "io/io.h"
#include "keyboardHandler/keyboardHandler.h"
#include "terminal/terminal.h"

struct idt_desc idt_descriptors[GAYOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr_descriptor;

extern void idt_load(struct idtr_desc *ptr);
extern void int21h();
extern void no_interrupt();
extern void finish_int();

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

void idt_set(int interrupt_no, void *address)
{
    struct idt_desc *desc = &idt_descriptors[interrupt_no];
    desc->offset_1 = (uint32_t)address & 0x0000ffff;
    desc->selector = KERNEL_CODE_SELECTOR;
    desc->zero = 0x00;
    desc->type_attr = 0xEE;
    desc->offset_2 = (uint32_t)address >> 16;
}
void idt_init()
{
    memset(idt_descriptors, 0, sizeof(idt_descriptors));
    idtr_descriptor.limit = sizeof(idt_descriptors) - 1;
    idtr_descriptor.base = (uint32_t)idt_descriptors;

    for (int i = 0; i < GAYOS_TOTAL_INTERRUPTS; i++)
    {
        idt_set(i, no_interrupt);
    }

    idt_set(0, idt_zero);
    idt_set(0x21, int21h);

    idt_load(&idtr_descriptor);
}