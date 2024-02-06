#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct
{
    unsigned int base;
    unsigned char *digits;
    unsigned int len;
} number;

unsigned char char_for_base[64] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";

void free_number(number nbr)
{
    free(nbr.digits);
}

number to_number(unsigned int nbr, unsigned int base)
{
    if (base != 2 && base != 4 && base != 8 && base != 8 && base != 16 && base != 32 && base != 64)
    {
        base = 16;
    }

    unsigned int max_len = 64;
    number result;
    result.base = base;
    result.digits = malloc(max_len * sizeof(unsigned char));
    result.len = 0;

    while (nbr > 0)
    {
        result.digits[result.len++] = char_for_base[nbr % base];
        nbr /= base;
    }

    if (result.len < max_len)
    {
        unsigned char *tmp = realloc(result.digits, result.len * sizeof(unsigned char));
        result.digits = tmp;
    }

    return result;
}

unsigned int get_10_from_b(unsigned char str)
{
    for (unsigned int i = 0; i < 64; i++)
    {
        if (char_for_base[i] == str)
        {
            return i;
        }
    }
    exit(-1);
}

unsigned int to_uint(number nbr)
{
    unsigned int result = 0;
    for (unsigned int i = 0; i < nbr.len; i++)
    {
        result += get_10_from_b(nbr.digits[i]) * pow(nbr.base, i);
    }
    return result;
}

char *to_string(number nbr)
{
    char *result = malloc(nbr.len * sizeof(char));
    for (int i = nbr.len - 1; i >= 0; i--)
    {
        result[nbr.len - 1 - i] = nbr.digits[i];
    }
    return result;
}

number from_string(char *str, unsigned int base)
{
    unsigned int nbr = atoi(str);
    return to_number(nbr, base);
}

number to_base(number nbr, unsigned int base)
{
    unsigned int int_number = to_uint(nbr);
    number number = to_number(int_number, base);
    return number;
}

number add_number(number a, number b)
{
    unsigned int nbr_a = to_uint(a);
    unsigned int nbr_b = to_uint(b);
    unsigned int result = nbr_a + nbr_b;
    return to_number(result, a.base);
}

number soustract_number(number a, number b)
{
    unsigned int nbr_a = to_uint(a);
    unsigned int nbr_b = to_uint(b);
    unsigned int result = nbr_a - nbr_b;
    return to_number(result, a.base);
}

number multiply_number(number a, number b)
{
    unsigned int nbr_a = to_uint(a);
    unsigned int nbr_b = to_uint(b);
    unsigned int result = nbr_a * nbr_b;
    return to_number(result, a.base);
}

number divide_number(number a, number b)
{
    unsigned int nbr_a = to_uint(a);
    unsigned int nbr_b = to_uint(b);
    unsigned int result = nbr_a / nbr_b;
    return to_number(result, a.base);
}

int cmp_number(number a, number b)
{
    unsigned int nbr_a = to_uint(a);
    unsigned int nbr_b = to_uint(b);
    if (nbr_a < nbr_b)
    {
        return -1;
    }
    else if (nbr_a == nbr_b)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
