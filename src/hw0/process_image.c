#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "image.h"

float get_pixel(image im, int x, int y, int c)
{
    // clamp
    x = MIN(x, im.w - 1);
    x = MAX(x, 0);

    y = MIN(y, im.h - 1);
    y = MAX(y, 0);
    
    c = MIN(c, im.c - 1);
    c = MAX(c, 0);

    return im.data[im.w*im.h*c + im.w*y + x];
}

void set_pixel(image im, int x, int y, int c, float v)
{
    // clamp
    x = MIN(x, im.w - 1);
    x = MAX(x, 0);

    y = MIN(y, im.h - 1);
    y = MAX(y, 0);
    
    c = MIN(c, im.c - 1);
    c = MAX(c, 0);

    im.data[im.w*im.h*c + im.w*y + x] = v;
}

image copy_image(image im)
{
    image copy = make_image(im.w, im.h, im.c);
    memcpy(copy.data, im.data, im.w * im.h * im.c * sizeof(float));
    return copy;
}

image rgb_to_grayscale(image im)
{
    assert(im.c == 3);
    image gray = make_image(im.w, im.h, 1);

    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            float r = get_pixel(im, i, j, 0);
            float g = get_pixel(im, i, j, 1);
            float b = get_pixel(im, i, j, 2);
            set_pixel(gray, i, j, 0, 0.299 * r + 0.587 * g + 0.114 * b);
        }
    }
    return gray;
}

void shift_image(image im, int c, float v)
{
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            set_pixel(im, i, j, c, get_pixel(im, i, j, c) + v);
        }
    }
}

void clamp_image(image im)
{
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            for (int k = 0; k < im.c; k++) {
                float curr = get_pixel(im, i, j, k);
                curr = MAX(0, curr);
                curr = MIN(1, curr);
                set_pixel(im, i, j, k, curr);
            }
        }
    }
}


// These might be handy
float three_way_max(float a, float b, float c)
{
    return (a > b) ? ( (a > c) ? a : c) : ( (b > c) ? b : c) ;
}

float three_way_min(float a, float b, float c)
{
    return (a < b) ? ( (a < c) ? a : c) : ( (b < c) ? b : c) ;
}

void rgb_to_hsv(image im)
{
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            float r = get_pixel(im, i, j, 0);
            float g = get_pixel(im, i, j, 1); 
            float b = get_pixel(im, i, j, 2);

            float V = three_way_max(r,g,b);
            float m = three_way_min(r,g,b);
            float C = V - m;
            float S;
            if (V == 0) {
                S = 0;
            }else {
                S = C / V;
            }
            float H_p = 0;
            float H = 0;

            if (C == 0) {
                H = 0;
            } else if (V == r) {
                H_p = (g-b)/C;
            } else if (V == g) {
                H_p = (b-r)/C + 2;
            } else if (V == b) {
                H_p = (r-g)/C + 4;
            }

            if (C != 0) {
                if (H_p < 0) {
                    H = H_p/6 + 1;
                } else {
                    H = H_p/6;
                }
            }

            set_pixel(im, i, j, 0, H);
            set_pixel(im, i, j, 1, S);
            set_pixel(im, i, j, 2, V);
        }
    }
}



float f(float n, float V, float S, float H) {
    float k = fmod((n + H * 6),6);
    return V - V * S * MAX(0, three_way_min(k, 4-k, 1));
}

void hsv_to_rgb(image im)
{
    for (int i = 0; i < im.w; i++) {
        for (int j = 0; j < im.h; j++) {
            float H = get_pixel(im, i, j, 0);
            float S = get_pixel(im, i, j, 1); 
            float V = get_pixel(im, i, j, 2);

            float r = f(5, V, S, H);
            float g = f(3, V, S, H);
            float b = f(1, V, S, H);

            set_pixel(im, i, j, 0, r);
            set_pixel(im, i, j, 1, g);
            set_pixel(im, i, j, 2, b);
        }
    }
}

