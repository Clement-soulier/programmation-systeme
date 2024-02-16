#include <stdio.h>
#include <stdlib.h>
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
        unsigned char r, g, b;
    } rgb;
    struct
    {
        colormode mode;
        double h, s, l;
    } hsl;
} pixel;

unsigned char char_for_base[64] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";

/*XPM*/
static char *image[] = {
    /*columnsrows colorschars-per-pixel*/
    "8 4 2 1",
    "X c #FF0000",
    "O c #0000FF",
    /*pixels*/
    "XOXOXOXO",
    "OXOXOXOX",
    "XOXOXOXO",
    "OXOXOXOX"};

double max(double a, double b){
    if( a < b){
        return b;
    }
    return a;
}

double min(double a, double b){
    if(a < b){
        return a;
    }
    return b;
}

double get_absolute_value(double value){
    if(value > 0){
        return value;
    }
    return -value;
}

pixel copy_pixel(pixel pix)
{
    return pix;
}

pixel to_hsl(pixel pix){
    if(pix.mode == HSL){
        return pix;
    }
    
    pixel new_pix;
    new_pix.mode = HSL;
    
    double r = (double) pix.rgb.r / 255;
    double g = (double) pix.rgb.g / 255;
    double b = (double) pix.rgb.b / 255;

    double c_max = max(r, max(g, b));
    double c_min = min(r, min(g, b));
    double delta = c_max - c_min;
   
    new_pix.hsl.l = ((double) c_max + (double) c_min) / 2;

    if(delta == 0){
        new_pix.hsl.h = 0;
        new_pix.hsl.s = 0;
    } else 
    if(c_max == r){
        new_pix.hsl.h = 60 * ((int) ((g - b) / delta) % 6);
        new_pix.hsl.s = (double) delta / (1 - get_absolute_value(2 * new_pix.hsl.l - 1));
    } else 
    if(c_max == g){
        new_pix.hsl.h = 60 * (2 + ((double) b - (double) r) / delta);
        new_pix.hsl.s = (double) delta / (1 - get_absolute_value(2 * new_pix.hsl.l - 1));
    } else 
    if(c_max == b){
        new_pix.hsl.h = 60 * (4 + ((double) r - (double) g) / delta);
        new_pix.hsl.s = (double) delta / (1 - get_absolute_value(2 * new_pix.hsl.l - 1));
    }
    return new_pix;
}

pixel to_rgb(pixel pix){
    if(pix.mode == RGB){
        return pix;
    }

    pixel new_pix;
    pix.mode = RGB;
    
    double c = (double) (1 - get_absolute_value(2 * pix.hsl.l - 1) * pix.hsl.s);
    double x = (double) (c * (1 - get_absolute_value((double) (((int) pix.hsl.h / 60) % 2) - 1)));
    double m = (double) (pix.hsl.l - (c / 2));
  
    double r = 0;
  	double g = 0;
  	double b = 0;

    if(0 <= pix.hsl.h &&  pix.hsl.h < 60){
        r = c;
        g = x;
        b = 0;
    } else
    if(60 <= pix.hsl.h && pix.hsl.h < 120){
        r = x;
        g = c;
        b = 0;
    } else 
    if(120 <= pix.hsl.h && pix.hsl.h < 180){
        r = 0;
        g = c;
        b = x;
    } else 
    if(180 <= pix.hsl.h && pix.hsl.h < 240){
        r = 0;
        g = x;
        b = c;
    } else
    if(240 <= pix.hsl.h && pix.hsl.h < 300){
        r = x;
        g = 0;
        b = c;
    } else
    if(300 <= pix.hsl.h && pix.hsl.h < 360){
        r = c;
        g = 0;
        b = x;
    }

    new_pix.rgb.r = (unsigned char) ((r + m) * 255);
    new_pix.rgb.g = (unsigned char) ((g + m) * 255);
    new_pix.rgb.b = (unsigned char) ((b + m) * 255);

    return new_pix;
}

void gradient(pixel start, pixel stop, pixel *tab, unsigned int len){
    pixel correct_start = to_hsl(start);
    pixel correct_stop = to_hsl(stop);
    for(unsigned int i = 0; i < len; i++){
        pixel new_pix;    
        new_pix.mode = HSL;
        new_pix.hsl.h = (correct_start.hsl.h * ((len - 1) - i) + correct_stop.hsl.h * i) / (len - 1);
        new_pix.hsl.s = (correct_start.hsl.s * ((len - 1) - i) + correct_stop.hsl.s * i) / (len - 1);
        new_pix.hsl.l = (correct_start.hsl.l * ((len - 1) - i) + correct_stop.hsl.l * i) / (len - 1); 
        if(start.mode == RGB){
            tab[i] = to_rgb(new_pix);
        }
        else{
            tab[i] = new_pix;
        }
    }
}

char* color_to_html(pixel pix){
    pixel correct_pix = to_rgb(pix);
    char *html_color = (char*)malloc(sizeof(char) * 8);
    sprintf(html_color, "#%02X%02X%02X", correct_pix.rgb.r, correct_pix.rgb.g, correct_pix.rgb.b);
    return html_color;
}

int main(void)
{
    pixel start = {.rgb.mode = RGB, .rgb.r = 150, .rgb.g = 89, .rgb.b = 90};
    pixel stop = {.rgb.mode = RGB, .rgb.r = 190, .rgb.g = 40, .rgb.b = 200};
    pixel tab[64];
    gradient(start, stop, tab, 64);
    printf("%i %i %i %i\n", 8, 8, 64, 64);
    for(int i = 0; i < 64; i++){
        printf("%c c %s\n", char_for_base[i] , color_to_html(tab[i]));
    }
    for(int i = 0; i < 64; i += 7){
        printf("%c%c%c%c%c%c%c%c\n", char_for_base[i], char_for_base[i + 1], char_for_base[i + 2], char_for_base[i + 3], char_for_base[i + 4], char_for_base[i + 5], char_for_base[i + 6], char_for_base[i + 7]);
    }
    
}

