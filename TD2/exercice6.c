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
        unsigned char r, g, b;
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
  
  	printf("c: %f\n", c);
  	printf("x: %f\n", x);
  	printf("m: %f\n", m);

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


int main(void)
{
  pixel pixel1 = {.rgb.mode = RGB, .rgb.r = 30, .rgb.g = 9, .rgb.b = 89};
  printf("r: %d, g: %d, b: %d\n", pixel1.rgb.r, pixel1.rgb.g, pixel1.rgb.b);
  pixel pixel2 = to_hsl(pixel1);
  printf("h: %f, s: %f, l: %f\n", pixel2.hsl.h, pixel2.hsl.s, pixel2.hsl.l);
  pixel pixel3 = to_rgb(pixel2);
  printf("r: %d, g: %d, b: %d", pixel3.rgb.r, pixel3.rgb.g, pixel3.rgb.b);
 
}
