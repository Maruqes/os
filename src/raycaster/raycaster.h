#ifndef RAYCASTER_H
#define RAYCASTER_H

void raycaster_init();
void raycaster_keyboard_input(char c);

typedef struct
{
    float x;
    float y;

    double dx;
    double dy;
    double angle;
} Player;

typedef struct
{
    float x;
    float y;
} Point;
#endif