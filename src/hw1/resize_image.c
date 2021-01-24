#include <math.h>
#include "image.h"

typedef struct {
    float a;
    float b;
} pixmap;

pixmap map_pixels(float x1, float y1, float x2, float y2) {
    float det = (x1) - (x2);
    float a = ((y1) - (y2)) / det;
    float b = ((x1 * y2) - (x2 * y1))/ det;
    pixmap map;
    map.a = a;
    map.b = b;
    return map;
} 

float nn_interpolate(image im, float x, float y, int c)
{
    int x_round = round(x);
    int y_round = round(y);
    return get_pixel(im, x_round, y_round, c);
}

image nn_resize(image im, int w, int h)
{
    image result = make_image(w,h,im.c);
    float x1 = -0.5;
    float x2 = w - 0.5;
    float y1 = -0.5;
    float y2 = im.w - 0.5;
    pixmap xmap = map_pixels(x1, y1, x2, y2);

    x2 = h - 0.5;
    y2 = im.h - 0.5;
    pixmap ymap = map_pixels(x1, y1, x2, y2);

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            for (int k = 0; k < result.c; k++) {
                // map (i,j) to old coord
                float i_old = xmap.a * i + xmap.b;
                float j_old = ymap.a * j + ymap.b;
                float interp = nn_interpolate(im, i_old, j_old, k);
                set_pixel(result, i, j, k, interp);
            }
        }
    }
    return result;
}

float bilinear_interpolate(image im, float x, float y, int c)
{
    int V1x = (int) floor(x);
    int V1y = (int) floor(y);

    int V2x = (int) ceil(x);
    int V2y = (int) floor(y);

    int V3x = (int) floor(x);
    int V3y = (int) ceil(y);

    int V4x = (int) ceil(x);
    int V4y = (int) ceil(y);

    float d1 = x - V1x;
    float d2 = 1 - d1;
    float d3 = y - V2y;
    float d4 = 1 - d3;

    float V1 = get_pixel(im, V1x, V1y, c);
    float V2 = get_pixel(im, V2x, V2y, c);
    float V3 = get_pixel(im, V3x, V3y, c);
    float V4 = get_pixel(im, V4x, V4y, c);

    float q1 = V1 * d2 + V2 * d1;
    float q2 = V3 * d2 + V4 * d1;
    float q = q1 * d4 + q2 * d3;
    return q;
}

image bilinear_resize(image im, int w, int h)
{
       image result = make_image(w,h,im.c);
    float x1 = -0.5;
    float x2 = w - 0.5;
    float y1 = -0.5;
    float y2 = im.w - 0.5;
    pixmap xmap = map_pixels(x1, y1, x2, y2);

    x2 = h - 0.5;
    y2 = im.h - 0.5;
    pixmap ymap = map_pixels(x1, y1, x2, y2);

    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            for (int k = 0; k < result.c; k++) {
                // map (i,j) to old coord
                float i_old = xmap.a * i + xmap.b;
                float j_old = ymap.a * j + ymap.b;
                float interp = bilinear_interpolate(im, i_old, j_old, k);
                set_pixel(result, i, j, k, interp);
            }
        }
    }
    return result; 
}