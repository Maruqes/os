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
#include "window_management/window_management.h"

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

Clock clock;
int number_of_clocks_to_one_milisecond = 5; // int a = (mills * hz) / 1000;

int timer_ticks = 0;
int clock_ticks = 0;
int is_sleeping = 0;
int hz;
int screen_ticks;

unsigned int interrupts_enabled;

void get_hz(float hzz)
{
    clock.ticks = 0;
    clock.milliseconds = 0;
    clock.seconds = 0;
    clock.minutes = 0;
    clock.hours = 0;
    clock.c_hours[0] = 48; // 48 = '0'
    clock.c_minutes[0] = 48;
    hz = hzz;
    number_of_clocks_to_one_milisecond = hz / 1000;
}

void update_screen()
{
    if (tasks_n < 1)
        return;
    if (!(buffers_arr[current_window].proc_pid == return_pid(cur_task)))
        return;

    screen_ticks++;

    if (screen_ticks >= 80)
    {
        if (buffers_arr[current_window].used == 0)
            return;

        screen_ticks = 0;
        for (int i = 0; i < SCREEN_WIDHT * SCREEN_HEIGHT; i++)
        {
            swipe_screen_buffer[i] = buffers_arr[current_window].buffer[i];
        }
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 6; j++)
            {
                swipe_screen_buffer[(mouse_struct.y - 3 + j) * SCREEN_WIDHT + mouse_struct.x - 3 + i] = 0x00ff00;
            }
        }
        draw_screen();
    }
}

void call_multi_tasking_system()
{
    // if (tasks_n == 1)
    // {
    //     Task t = get_task(0);
    //     if (t.end_task == 1)
    //     {
    //     }
    // }
    // if (tasks_n < 2)
    //     return;

    if (tasks_n < 1)
        return;
    clock_ticks++;
    if (clock_ticks >= 1000)
    {
        clock_ticks = 0;
        change_tasks();
    }
}

Clock *return_clock()
{
    return &clock;
}

void update_time()
{
    if (clock.seconds < 10)
    {
        write_char_pos(70, 24, '0', 0x00ff00);
        write_char_pos(71, 24, clock.c_seconds[0], 0x00ff00);
    }
    else
    {
        write_char_pos(70, 24, clock.c_seconds[0], 0x00ff00);
        write_char_pos(71, 24, clock.c_seconds[1], 0x00ff00);
    }

    if (clock.minutes < 10)
    {
        write_char_pos(67, 24, '0', 0x00ff00);
        write_char_pos(68, 24, clock.c_minutes[0], 0x00ff00);
    }
    else
    {
        write_char_pos(67, 24, clock.c_minutes[0], 0x00ff00);
        write_char_pos(68, 24, clock.c_minutes[1], 0x00ff00);
    }

    if (clock.hours < 10)
    {
        write_char_pos(64, 24, '0', 0x00ff00);
        write_char_pos(65, 24, clock.c_hours[0], 0x00ff00);
    }
    else
    {
        write_char_pos(64, 24, clock.c_hours[0], 0x00ff00);
        write_char_pos(65, 24, clock.c_hours[1], 0x00ff00);
    }
}

void step_clock()
{
    if (input_mode == 1)
    {
        return;
    }
    clock.ticks++;

    if (clock.ticks == number_of_clocks_to_one_milisecond)
    {
        clock.milliseconds++;
        clock.ticks = 0;
    }

    if (clock.milliseconds == 1000)
    {
        clock.seconds++;
        clock.milliseconds = 0;
        memcpy(&clock.c_seconds, digit_to_number(clock.seconds), 2);
        update_time();
    }

    if (clock.seconds == 60)
    {
        clock.minutes++;
        clock.seconds = 0;
        memcpy(&clock.c_minutes, digit_to_number(clock.minutes), 2);
    }

    if (clock.minutes == 60)
    {
        clock.hours++;
        clock.minutes = 0;
        memcpy(&clock.c_hours, digit_to_number(clock.hours), 2);
    }

    return;
}

void finish_int_without_tasks()
{
    outb(0x20, 0x20);
}

void finish_int()
{
    step_clock();
    if (is_sleeping)
    {
        timer_ticks++;
    }
    call_multi_tasking_system();
    update_screen();
    outb(0x20, 0x20);
}

void finish_int_slave_pic()
{
    outb(0xA0, 0x20);
    step_clock();
    if (is_sleeping)
    {
        timer_ticks++;
    }
}

int sleep(int mills) // mutlitasking does not work with sleep becouse the sleep is called by "n" callers not adapting to each task calling them
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
    print((char *)(address));
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
    if (swipe_screen_buffer[0] == 0x00ff00 && swipe_screen_buffer[478601] == 0x00ff00)
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

    // program crashes
    idt_set(50, dot_data);   // crash .data
    idt_set(51, dot_rodata); // crash .rodata

    // keyboard, mouse, tela

    idt_load(&idtr_descriptor);
}