#include "string.h"
#include "memory/memory.h"

char *t;

int strlen(const char *str)
{
	int len = 0;
	while (str[len])
	{
		len++;
	}

	return len;
}

char *strcpy(char *dest, const char *src)
{
	char *res = dest;
	while (*src != 0)
	{
		*dest = *src;
		src += 1;
		dest += 1;
	}
	*dest = 0x00;
	return res;
}

char tolower(char c)
{
	if (c >= 65 && c <= 90)
	{
		c += 32;
	}
	return c;
}

int istrncmp(const char *s1, const char *s2, int n)
{
	unsigned char u1, u2;
	while (n-- > 0)
	{
		u1 = (unsigned char)*s1++;
		u2 = (unsigned char)*s2++;
		if (u1 != u2 && tolower(u1) != tolower(u2))
		{
			return u1 - u2;
		}
		if (u1 == '\0')
			return 0;
	}
	return 0;
}

int cmpstring(const char *str1, const char *str2)
{
	int str1len = strlen(str1);
	int str2len = strlen(str2);
	if (str1len != str2len)
		return 0;
	for (int i = 0; i < str1len; i++)
	{
		if (str1[i] != str2[i])
		{
			return 0;
		}
	}
	return 1;
}

int strncmp(const char *str1, const char *str2, int n)
{
	unsigned char u1, u2;

	while (n-- > 0)
	{
		u1 = (unsigned char)*str1++;
		u2 = (unsigned char)*str2++;

		if (u1 != u2)
			return u1 - u2;
		if (u1 == '\0')
			return 0;
	}
	return 0;
}

int strnlen_terminator(const char *str, int max, char terminator)
{
	int i = 0;
	for (i = 0; i < max; i++)
	{
		if (str[i] == '\0')
		{
			break;
		}
	}
	return i;
}

int strnlen(const char *str, int max)
{
	int i = 0;
	for (i = 0; i < max; i++)
	{
		if (str[i] == 0)
		{
			break;
		}
	}

	return i;
}

bool is_digit(char c)
{
	return c >= 48 && c <= 57;
}
int to_numeric_digit(char c)
{
	return c - 48;
}

int number_to_digit(char *n)
{
	int res = 0;
	for (int i = 0; n[i] != '\0'; ++i)
	{
		if (n[i] >= '0' && n[i] <= '9')
		{
			res = res * 10 + n[i] - '0';
		}
		else
		{
			return -1;
		}
	}

	return res;
}

char *digit_to_number(int n)
{
	int count = 0;
	int dig = n;

	if (dig == 0)
		count = 1;
	while (dig != 0)
	{
		dig = dig / 10;
		++count;
	}

	free(t);
	t = zalloc(count + 1);
	t[count + 1] = '\0';
	for (int j = 0; j < count; j++)
	{
		int res = n;

		for (int i = 0; i < count - (1 * (j + 1)); i++)
		{
			res = res / 10;
		}

		t[j] = res + '0';

		for (int i = 0; i < count - (1 * (j + 1)); i++)
		{
			res = res * 10;
		}

		n = n - res;
	}
	return t;
}