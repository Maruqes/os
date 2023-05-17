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

void idt_init();
void enable_int();
void disable_int();
int sleep(int mills);
void get_hz(float hz);
void int21h_handler();
void int21h_handler_input();
void int32h_handler();
void start_count();
int finish_count();
void finish_int();
void finish_int_slave_pic();

#endif