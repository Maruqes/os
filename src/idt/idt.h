#ifndef IDT_H
#define IDT_H

#include <stdint.h>
struct idt_desc
{
    uint16_t offset_1; // Offset bits 0 - 15
    uint16_t selector; // Selector thats in our GDT
    uint8_t zero;      // Does nothing, unused set to zero
    uint8_t type_attr; // Descriptor type and attributes
    uint16_t offset_2; // Offset bits 16-31
} __attribute__((packed));

struct idtr_desc
{
    uint16_t limit; // Size of descriptor table -1
    uint32_t base;  // Base address of the start of the interrupt descriptor table
} __attribute__((packed));

typedef struct
{
    int ticks;
    int milliseconds;
    int seconds;
    int minutes;
    int hours;

    char c_seconds[2];
    char c_minutes[2];
    char c_hours[2];

} Clock;

void idt_init();
void enable_int();
void disable_int();
int sleep(int mills);
void get_hz(float hz);
void int21h_handler();
void int21h_handler_input();
void int32h_handler();
void finish_int();
void finish_int_slave_pic();
void idt_printINT(int address);
void stop_sleep();
Clock *return_clock();
void finish_int_without_tasks();
void idt_set(int interrupt_no, void *address);

extern void *cur_addr_program;
extern Clock clock;
extern unsigned int interrupts_enabled;

#endif