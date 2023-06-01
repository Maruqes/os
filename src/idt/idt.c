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
#include "multitasking/multitasking.h"

struct idt_desc idt_descriptors[OS_TOTAL_INTERRUPTS];
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
extern void test_screent_INT();
extern void freeINT();
extern void screenINT();
extern void get_addrINT();
extern void debbugINT();
extern void create_task_int();
extern void change_task_int();

int timer_ticks = 0;
int clock_ticks = 0;
int is_sleeping = 0;
int hz;

unsigned int interrupts_enabled;

void get_hz(float hzz)
{
    hz = hzz;
}

void call_multi_tasking_system()
{
    if (tasks_n < 2)
        return;

    clock_ticks++;
    if (clock_ticks >= 1000)
    {
        print("M");
        clock_ticks = 0;
        change_tasks();
    }
}

void finish_int()
{
    outb(0x20, 0x20);
    call_multi_tasking_system();
    if (is_sleeping)
    {
        timer_ticks++;
    }
}

void finish_int_slave_pic()
{
    if (interrupts_enabled == 0)
        return;

    outb(0xA0, 0x20);
    call_multi_tasking_system();

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

void stop_sleep()
{
    is_sleeping = 0;
    timer_ticks = 0;
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
void *cur_addr_program;

void idt_printINT(int address)
{
    print((char *)(cur_addr_program + address)); // probemas no linker do programa
    // print((char *)(addr_program + 0x2178));
}

void idt_quit_appINT()
{
    quit_curApp();
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

void int_screenINT()
{
    terminal_mode = 0;
}

int function_given_counter;
void (*CURRENT_FUNCTION)();
void int_give_OS_FUNCTIONS()
{
    CURRENT_FUNCTION = OS_FUNCTIONS[function_given_counter];
    function_given_counter++;
    if (function_given_counter == OS_EXTERN_FUNCTIONS)
    {
        function_given_counter = 0;
    }
}

int result_screen_test;
void int_test_screent_INT()
{
    if (framebuffer[0] == 0x00ff00 && framebuffer[478601] == 0x00ff00)
    {
        result_screen_test = 1;
    }
    else
    {
        result_screen_test = 0;
    }
}

void int_debbugINT()
{
}

void int_debbugINT2()
{
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
    disable_int();
    function_given_counter = 0;

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

    for (int i = 0; i < OS_TOTAL_INTERRUPTS; i++)
    {
        idt_set(i, no_interrupt);
    }

    idt_set(0, idt_zero);
    idt_set(0x21, int21h); // 33 em decimal
    idt_set(0x2C, int32h); // mouse(driver) //44 em decimal

    idt_set(16, printINT);         // print
    idt_set(17, quit_appINT);      // quit
    idt_set(18, inputINT);         // input
    idt_set(19, printADDRINT);     // printADDR
    idt_set(20, test_screent_INT); // testar o screenINT
    idt_set(21, debbugINT);        // debuug
    idt_set(22, screenINT);        // get_screen_buf
    idt_set(23, get_addrINT);      // sleep
    idt_set(24, create_task_int);  // create_task_int
    idt_set(25, change_task_int);  // debuug3

    // keyboard, mouse, tela

    idt_load(&idtr_descriptor);
}