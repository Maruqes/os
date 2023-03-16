#ifndef MATH_H
#define MATH_H

#define M_PI 3.14159265358979323846
#define M_PI_2 (M_PI / 2.0f)
#define M_PI_M_2 (M_PI * 2.0f)

double sqrt(double x);
int pow(int a, int b);
float atan2f(float y, float x);
float atanf(float x);
int min(int a, int b);
int max(int a, int b);
int abs(int a);
void swap(int *a, int *b);
double cos(double x);
double sin(double x);
double tan(double x);
float degToRad(int a);
int FixAng(int a);
#endif