#include "kernel.h"
#include "raycaster.h"
#include "math/math.h"
#include "disk/disk.h"
#include "memory/memory.h"
#include "terminal/terminal.h"
#include "string/string.h"
#include "idt/idt.h"
#include "keyboardHandler/keyboardHandler.h"
#include "math/math.h"

unsigned int *screenbuffer_save;
unsigned int *screen_buffer;
unsigned int screen_buffer_size;
Player player;

int map_width = 8;
int map_height = 8;
int map_size = 64;

int map[] =
    {
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        1,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
};

double normalize(double x, double y)
{
    double length = sqrt(x * x + y * y);
    return length;
}

void put_pixel_save_buffer(int x, int y, int color)
{
    screenbuffer_save[y * SCREEN_WIDHT + x] = color;
}

void raycaster_keyboard_input(char c)
{
    if (c == 'W')
    {
        player.x -= player.dx;
        player.y -= player.dy;
        return;
    }
    else if (c == 'A')
    {
        player.angle -= 5;
        player.angle = FixAng(player.angle);
        player.dx = cos(degToRad(player.angle));
        player.dy = sin(degToRad(player.angle));
        return;
    }
    else if (c == 'S')
    {
        player.x += player.dx;
        player.y += player.dy;
        return;
    }
    else if (c == 'D')
    {
        player.angle += 5;
        player.angle = FixAng(player.angle);
        player.dx = cos(degToRad(player.angle));
        player.dy = sin(degToRad(player.angle));
        return;
    }
    return;
}

void draw_cube(int x, int y, int color)
{
    for (int i = 0; i < map_size - 1; i++)
    {
        for (int j = 0; j < map_size - 1; j++)
        {
            put_pixel_save_buffer((x * map_size) + j, (y * map_size) + i, color);
        }
    }
}
void draw_point(Point p1, int color)
{
    put_pixel_save_buffer(p1.x, p1.y, color);
}

float distance(Point p1, Point p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance;
}

float angle(float p1x, float p1y, float p2x, float p2y)
{
    float dx = p2x - p1x;
    float dy = p2y - p1y;

    float angle = atan2f(dy, dx) * 180.0f / M_PI;
    return angle;
}

float distance_with_angle(Point p1, Point p2, float angle)
{
    float d = distance(p1, p2);
    float theta = angle * M_PI / 180.0f;
    float distance = d * cos(theta);
    return distance;
}

void draw_line2P(Point p1, Point p2)
{
    // y = ax + b

    double dy = p2.y - p1.y;
    double dx = p2.x - p1.x;

    double c;

    if (p1.x < p2.x)
    {
        c = p1.x;
    }
    else
    {
        c = p2.x;
    }

    float d = distance(p1, p2);
    for (int i = 0; i < d; i++)
    {
        double p = (dy / dx * i) + p1.y;
        put_pixel_save_buffer(i + c, p, 0x0000ff);
    }
}

void draw_map()
{
    for (int y = 0; y < map_height; y++)
    {
        for (int x = 0; x < map_width; x++)
        {
            if (map[y * map_height + x] == 1)
            {
                draw_cube(x, y, 0xff0000);
            }
        }
    }
}

void draw_player()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            put_pixel_save_buffer(player.x + i, player.y + j, 0x00ff00);
        }
    }
}

void raycaster_init()
{
    terminal_mode = 0;

    screen_buffer_size = SCREEN_WIDHT * SCREEN_HEIGHT;
    screenbuffer_save = zalloc(screen_buffer_size);

    clear_pixels_screen();
    get_keyboard_input(&raycaster_keyboard_input);
    player.x = 64;
    player.y = 64;
    player.angle = M_PI_2;
    player.dx = cos(player.angle) * 5;
    player.dy = sin(player.angle) * 5;

    Point a = {50, 50};
    Point b = {500, 500};
    while (1)
    {
        for (int i = 0; i < screen_buffer_size; i++)
        {
            screenbuffer_save[i] = 0;
        }

        draw_map();
        draw_player();

        a.x = player.x;
        a.y = player.y;

        b.x = player.x + (player.dx * 10);
        b.y = player.y + (player.dy * 10);

        draw_line2P(a, b);
        for (int i = 0; i < screen_buffer_size; i++)
        {
            framebuffer[i] = screenbuffer_save[i];
        }
    }
}