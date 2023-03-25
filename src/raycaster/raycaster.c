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

int left_control_ray = 0;
int run = 1;

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

void quit_ray()
{
    run = 0;
}

void raycaster_keyboard_input(char c)
{
    if (c == -8)
    {
        left_control_ray = 1;
        return;
    }
    else if (c == 'Q' && left_control_ray == 1)
    {
        quit_ray();
        return;
    }
    if (c == 'W')
    {
        player.x -= player.dx;
        player.y -= player.dy;
        return;
    }
    if (c == 'A')
    {
        player.angle -= 0.05;
        player.dx = cos(player.angle);
        player.dy = sin(player.angle);
        if (player.angle < 0)
        {
            player.angle += M_PI * 2;
        }
        return;
    }
    if (c == 'S')
    {
        player.x += player.dx;
        player.y += player.dy;
        return;
    }
    if (c == 'D')
    {
        player.angle += 0.05;
        player.dx = cos(player.angle);
        player.dy = sin(player.angle);
        if (player.angle > M_PI * 2)
        {
            player.angle -= M_PI * 2;
        }
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

void draw_line(Point p1, Point p2, int color)
{
    Point p1t = p1;
    double dy = p2.y - p1.y;
    double dx = p2.x - p1.x;

    float maxvalue = max(fabs(dx), fabs(dy));
    dx /= maxvalue;
    dy /= maxvalue;

    for (float n = 0; n < maxvalue; ++n)
    {
        put_pixel_save_buffer(p1t.x, p1t.y, color);
        p1t.x += dx;
        p1t.y += dy;
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
float Fix_angle(float angle)
{
    if (angle < 0)
    {
        angle += M_PI * 2;
    }
    if (angle > M_PI * 2)
    {
        angle -= M_PI * 2;
    }
    return angle;
}

float vertical_ray(Point Point_1, Point ray, float angle, int r)
{
    float ang = angle;
    Point_1.x = player.x;
    Point_1.y = player.y;

    // vertical
    float xn, yn;

    float y_final;
    float x_final;

    if (ang > P2 && ang < P3)
    {
        int fixed_pos = (player.x / 64);
        fixed_pos++;
        xn = (64 * fixed_pos) - (player.x / 64) * 64;
        yn = xn * tan(ang);
        xn += 1;
        x_final = 64;
        y_final = x_final * tan(ang);
    }
    else if (ang > P3 || ang < P2)
    {
        int fixed_pos = (player.x / 64);
        xn = (64 * fixed_pos) - (player.x / 64) * 64;
        yn = xn * tan(ang);
        xn -= 1;
        x_final = -64;
        y_final = x_final * tan(ang);
    }

    ray.x = player.x + player.dx + xn;
    ray.y = player.y + player.dy + yn;
    int tries = 0;
aqui:
    int mp = (((int)(ray.y) / 64) * 8) + ((int)(ray.x) / 64);
    if (map[mp] != 1)
    {
        ray.x += x_final;
        ray.y += y_final;
        if (tries < 8)
        {
            tries++;
            goto aqui;
        }
    }

    if (ray.x > 0 && ray.y > 0)
    {
        float distT = distance(Point_1, ray);
        if (distT > 512)
            distT = 0;
        return distT;
    }
    return 0;
}

float horizontal_ray(Point Point_1, Point ray, float angle, int r)
{
    float ang = angle;
    Point_1.x = player.x;
    Point_1.y = player.y;

    // horizontal
    float xn, yn;

    float y_final;
    float x_final;
    int tries = 0;
    float aTan = -1 / tan(ang);
    if (ang < M_PI)
    {
        int fixed_pos = (player.y / 64);
        fixed_pos++;
        yn = (64 * fixed_pos) - (player.y / 64) * 64;
        xn = yn * aTan;
        yn += 1;
        y_final = 64;
        x_final = y_final * aTan;
    }
    else if (ang > M_PI)
    {
        int fixed_pos = (player.y / 64);
        yn = (64 * fixed_pos) - (player.y / 64) * 64;
        xn = yn * aTan;
        yn -= 1;
        y_final = -64;
        x_final = y_final * aTan;
    }
    else if (ang == 0 || ang == M_PI)
    {
        xn = player.x;
        yn = player.y;
        tries = 8;
    }

    ray.x = player.x + player.dx + xn;
    ray.y = player.y + player.dy + yn;
aqui:
    int mp = (((int)(ray.y) / 64) * 8) + ((int)(ray.x) / 64);
    if (map[mp] != 1)
    {
        ray.x += x_final;
        ray.y += y_final;
        if (tries < 8)
        {
            tries++;
            goto aqui;
        }
    }

    if (ray.x > 0 && ray.y > 0)
    {
        // draw_line(Point_1, ray, 0x0000ff);
        float distT = distance(Point_1, ray);
        if (distT > 512)
            distT = 0;
        return distT;
    }
    return 0;
}

void raycaster_init()
{
    terminal_mode = 0;
    run = 1;
    screen_buffer_size = SCREEN_WIDHT * SCREEN_HEIGHT;
    screenbuffer_save = zalloc(screen_buffer_size);

    clear_pixels_screen();
    get_keyboard_input(&raycaster_keyboard_input);
    player.x = 70;
    player.y = 70;
    player.angle = 10 * RAD;
    player.dx = cos(player.angle);
    player.dy = sin(player.angle);

    Point ray = {player.x + player.dx,
                 player.y + player.dy};
    Point Point_1 = {0,
                     0};

    player.angle = M_PI / 4;
    float disT = 99999;

    Point c = {0, 0};
    Point d = {0, 0};
    while (run)
    {
        for (int i = 0; i < screen_buffer_size; i++)
        {
            screenbuffer_save[i] = 0;
        }

        // draw_map();
        // draw_player();

        for (int i = 0; i < 60; i++)
        {
            float a = horizontal_ray(Point_1, ray, Fix_angle(-(player.angle - (30 * RAD) + i * RAD)), i);
            float b = vertical_ray(Point_1, ray, Fix_angle(player.angle - (30 * RAD) + i * RAD), i);
            float color = 0x00ff00;
            if (a < b)
            {
                disT = a;
                color = 0x00ff00;
            }
            else if (b < a)
            {
                disT = b;
                color = 0x008800;
            }

            if (a == 0)
            {
                disT = b;
                color = 0x008800;
            }
            if (b == 0)
            {
                disT = a;
                color = 0x00ff00;
            }

            float lineH = (map_size * 600) / disT;
            if (lineH > 600)
                lineH = 600;

            float lineO = 160 - (lineH / 2);
            c.x = i * 8;
            c.y = lineO + 140;

            d.x = (i * 8);
            d.y = lineH + lineO + 140;

            draw_line(c, d, color);
        }

        for (int i = 0; i < screen_buffer_size; i++)
        {
            framebuffer[i] = screenbuffer_save[i];
        }
    }
    quit_app();
    free(screenbuffer_save);
    free(screen_buffer);
    terminal_mode = 1;
    clear_pixels_screen();
    start_terminal_mode();
}