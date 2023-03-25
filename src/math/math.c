#include "math.h"
double sqrt(double x)
{
    double res;
    asm("fsqrt"
        : "=t"(res)
        : "0"(x));
    return res;
}

float atanf(float x)
{
    float result;
    if (x >= 0.0f)
    {
        if (x > 1.0f)
        {
            result = M_PI / 2.0f - atanf(1.0f / x);
        }
        else
        {
            float sum = 0.0f;
            float term = x;
            float power = x;
            int n = 1;
            while (sum != sum + term)
            {
                sum += term;
                power *= -x * x;
                term = power / (2 * n + 1);
                n++;
            }
            result = sum;
        }
    }
    else
    {
        result = -atanf(-x);
    }
    return result;
}

float atan2f(float y, float x)
{
    float result;
    if (x == 0.0f)
    {
        if (y > 0.0f)
        {
            result = M_PI / 2.0f;
        }
        else if (y < 0.0f)
        {
            result = -M_PI / 2.0f;
        }
        else
        {
            result = 0.0f;
        }
    }
    else
    {
        float ratio = y / x;
        if (x > 0.0f)
        {
            result = atanf(ratio);
        }
        else if (y >= 0.0f)
        {
            result = atanf(ratio) + M_PI;
        }
        else
        {
            result = atanf(ratio) - M_PI;
        }
    }
    return result;
}

int pow(int a, int b)
{
    int power = 1, i;
    for (i = 1; i <= b; ++i)
    {
        power = power * a;
    }
    return power;
}

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int min(int a, int b)
{
    return (a > b) ? b : a;
}

int abs(int a)
{
    return (a >= 0) ? a : -a;
}

double fabs(double a)
{
    return (a >= 0) ? a : -a;
}

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int compare_float(double f1, double f2)
{
    double precision = 0.00000000000000000001;
    if ((f1 - precision) < f2)
    {
        return -1;
    }
    else if ((f1 + precision) > f2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
float degToRad(int a) { return a * M_PI / 180.0; }
int FixAng(int a)
{
    if (a > 359)
    {
        a -= 360;
    }
    if (a < 0)
    {
        a += 360;
    }
    return a;
}
double cos(double x)
{
    if (x < 0.0f)
        x = -x;

    if (0 <= compare_float(x, M_PI_M_2))
    {
        do
        {
            x -= M_PI_M_2;
        } while (0 <= compare_float(x, M_PI_M_2));
    }

    if ((0 <= compare_float(x, M_PI)) && (-1 == compare_float(x, M_PI_M_2)))
    {
        x -= M_PI;
        return ((-1) * (1.0f - (x * x / 2.0f) * (1.0f - (x * x / 12.0f) * (1.0f - (x * x / 30.0f) * (1.0f - (x * x / 56.0f) * (1.0f - (x * x / 90.0f) * (1.0f - (x * x / 132.0f) * (1.0f - (x * x / 182.0f)))))))));
    }
    return 1.0f - (x * x / 2.0f) * (1.0f - (x * x / 12.0f) * (1.0f - (x * x / 30.0f) * (1.0f - (x * x / 56.0f) * (1.0f - (x * x / 90.0f) * (1.0f - (x * x / 132.0f) * (1.0f - (x * x / 182.0f)))))));
}

double sin(double x) { return cos(x - M_PI_2); }

double tan(double x) { return (sin(x) / cos(x)); }