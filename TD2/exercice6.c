#include <stdio.h>

typedef enum
{
    RGB,
    HSL
} colormode;

typedef union
{
    colormode mode;
    struct
    {
        colormode mode;
        unsigned int r : 8;
        unsigned int g : 8;
        unsigned int b : 8;
    } rgb;
    struct
    {
        colormode mode;
        double h, s, l;
    } hsl;
} pixel;

/*XPM*/
static char *image[] = {
    /*columnsrows colorschars-per-pixel*/
    "8421",
    "Xc#FF0000",
    "Oc#0000FF",
    /*pixels*/
    "XOXOXOXO",
    "OXOXOXOX",
    "XOXOXOXO",
    "OXOXOXOX"};

pixel create_rgb_pixel(unsigned int *params)
{
    pixel pix;
    pix.mode = RGB;
    pix.rgb.r = params[0];
    pix.rgb.g = params[1];
    pix.rgb.b = params[2];
    return pix;
}

pixel create_hsl_pixel(double *params)
{
    pixel pix;
    pix.mode = HSL;
    pix.hsl.h = params[0];
    pix.hsl.s = params[1];
    pix.hsl.l = params[2];
    return pix;
}

pixel copy_pixel(pixel pix)
{
    return pix;
}

int main(void)
{
    unsigned int p[] = {25, 90, 7};
    pixel pixel1 = create_rgb_pixel(p);
    pixel pixel2 = copy_pixel(pixel1);
    pixel1.rgb.r = 0;
    printf("pixel1 %i pixel2 %i", pixel1.rgb.r, pixel2.rgb.r);
}
