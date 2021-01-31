#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "image.h"
#define TWOPI 6.2831853

void l1_normalize(image im)
{
    float sum = 0;
    for (int i = 0; i < im.h * im.w * im.c; i++) {
        sum  = sum + im.data[i];
    }
    for (int i = 0; i < im.h * im.w * im.c; i++) {
        im.data[i] = im.data[i] / sum;
    }
}

image make_box_filter(int w)
{
    image result = make_image(w,w,1);
    for (int i = 0; i < result.h * result.w * result.c; i++) {
        result.data[i] = 1;
    }
    
    l1_normalize(result);
    return result;
}

image convolve_image(image im, image filter, int preserve)
{
    image result;
    if (filter.c == im.c && preserve == 0) { // regular convolution
        result = make_image(im.w, im.h, 1);
        for (int i = 0; i < im.w; i++) {
            for (int j = 0; j < im.h; j++) {
                // loop over dimensions of filter
                float weighted_sum = 0;
                for (int c = 0; c < im.c; c++) {
                    for (int n = 0; n < filter.w; n++) {
                        for (int m = 0; m < filter.h; m++) {
                            float filterpix = get_pixel(filter, n, m, c);
                            float impix = get_pixel(im, i - (filter.w - 1)/ 2 + n, j - (filter.h-1) / 2 + m, c);
                            weighted_sum += filterpix * impix;
                        }
                    }
                }
                set_pixel(result, i, j, 1, weighted_sum);
            }
        }
    } else if (filter.c == im.c && preserve == 1) { // apply each channel of the filter to the corresponding channel of the image
        result = make_image(im.w, im.h, im.c);
        for (int c = 0; c < im.c; c++) {
            for (int i = 0; i < im.w; i++) {
                for (int j = 0; j < im.h; j++) {
                    // loop over dimensions of filter
                    float weighted_sum = 0;
                    for (int n = 0; n < filter.w; n++) {
                        for (int m = 0; m < filter.h; m++) {
                            float filterpix = get_pixel(filter, n, m, c);
                            float impix = get_pixel(im, i - (filter.w - 1)/ 2 + n, j - (filter.h-1) / 2 + m, c);
                            weighted_sum += filterpix * impix;
                        }
                    }
                    set_pixel(result, i, j, c, weighted_sum);
                }
            }
        }
    } else if (filter.c == 1 && preserve == 1){
        result = make_image(im.w, im.h, im.c);
        for (int c = 0; c < im.c; c++) {
            for (int i = 0; i < im.w; i++) {
                for (int j = 0; j < im.h; j++) {
                    // loop over dimensions of filter
                    float weighted_sum = 0;
                    for (int n = 0; n < filter.w; n++) {
                        for (int m = 0; m < filter.h; m++) {
                            float filterpix = get_pixel(filter, n, m, 1);
                            float impix = get_pixel(im, i - (filter.w - 1)/ 2 + n, j - (filter.h-1) / 2 + m, c);
                            weighted_sum += filterpix * impix;
                        }
                    }
                    set_pixel(result, i, j, c, weighted_sum);
                }
            }
        }
    } else if (filter.c == 1 && preserve == 0) { // 1 channel filter, condense to 1 channel output
        result = make_image(im.w, im.h, 1);
        for (int i = 0; i < im.w; i++) {
            for (int j = 0; j < im.h; j++) {
                // loop over dimensions of filter
                float weighted_sum = 0;
                for (int c = 0; c < im.c; c++) {
                    for (int n = 0; n < filter.w; n++) {
                        for (int m = 0; m < filter.h; m++) {
                            float filterpix = get_pixel(filter, n, m, 1);
                            float impix = get_pixel(im, i - (filter.w - 1)/ 2 + n, j - (filter.h-1) / 2 + m, c);
                            weighted_sum += filterpix * impix;
                        }
                    }
                }
                set_pixel(result, i, j, 1, weighted_sum);
            }
        }
    } 
    
    return result;
}

image make_highpass_filter()
{
    float filtarr[] = {0,-1,0,-1,4,-1,0,-1,0};
    image filter = make_image(3,3,1);
    memcpy(filter.data, filtarr, sizeof(filtarr));
    return filter;
}

image make_sharpen_filter()
{
    float filtarr[] = {0,-1,0,-1,5,-1,0,-1,0};
    image filter = make_image(3,3,1);
    memcpy(filter.data, filtarr, sizeof(filtarr));
    return filter;
}

image make_emboss_filter()
{
    float filtarr[] = {-2,-1,0,-1,1,1,0,1,2};
    image filter = make_image(3,3,1);
    memcpy(filter.data, filtarr, sizeof(filtarr));
    return filter;
}

// Question 2.2.1: Which of these filters should we use preserve when we run our convolution and which ones should we not? Why?
// Answer: We should preseve the emboss and sharpen output because the output is going to be similar to the original. Thus if the original is an rgb,
//         the emboss and sharpen filters should produce rgb images. The highpass filter output should not be preserved, because it is used for finding edges.
//         The edges contain 2d information, so they dont need another channel to display a 3rd dimension of information such as color.

// Question 2.2.2: Do we have to do any post-processing for the above filters? Which ones and why?
// Answer: Likely, we would have to apply post-processing to the highpass filter. The emboss and sharpen filters are used to produce outputs that are likely the final
//         output, such as for adding filters to an Instagram post. However, the highpass filter is used for finding information about the image: in this case edges.
//         It is unlikely that the user will want to have the highpass filter output as their final product, so likely it would be used in an image processing pipeline
//         to generate a final product after post processing the edge information.

image make_gaussian_filter(float sigma)
{
    int size = (int) 6 * sigma;
    if (size % 2 == 0) {
        size++;
    }
    image filter = make_image(size, size, 1);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int x = i - (size / 2);
            int y = j - (size / 2);
            float val = (1/(2 * M_PI * sigma * sigma)) * exp(-1 * ( (x*x+y*y) / (2*sigma*sigma) ));
            set_pixel(filter, i, j, 1, val);
        }
    }
    l1_normalize(filter);
    return filter;
}

void same_size(image a, image b) {
    assert(a.w == b.w);
    assert(a.h == b.h);
    assert(a.c == b.c);
}

image add_image(image a, image b)
{
    same_size(a,b);
    image result = make_image(a.w, a.h, a.c);
    for (int i = 0; i < a.w; i++) {
        for (int j = 0; j < a.h; j++) {
            for (int k = 0; k < a.c; k++) {
                float apix = get_pixel(a, i, j, k);
                float bpix = get_pixel(b, i, j, k);
                set_pixel(result, i, j, k, apix + bpix);
            }
        }
    }
    return result;
}

image sub_image(image a, image b)
{
    same_size(a,b);
    image result = make_image(a.w, a.h, a.c);
    for (int i = 0; i < a.w; i++) {
        for (int j = 0; j < a.h; j++) {
            for (int k = 0; k < a.c; k++) {
                float apix = get_pixel(a, i, j, k);
                float bpix = get_pixel(b, i, j, k);
                set_pixel(result, i, j, k, apix - bpix);
            }
        }
    }
    return result;
}

image make_gx_filter()
{
    float filtarr[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    image filter = make_image(3,3,1);
    memcpy(filter.data, filtarr, sizeof(filtarr));
    return filter;
}

image make_gy_filter()
{
    float filtarr[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    image filter = make_image(3,3,1);
    memcpy(filter.data, filtarr, sizeof(filtarr));
    return filter;
}

void feature_normalize(image im)
{
    float min = im.data[0];
    float max = im.data[0];
    for(int i = 0; i < im.w * im.h * im.c; i++) {
        float val = im.data[i];
        min = MIN(min, val);
        max = MAX(max, val);
    }
    if (min == max) {
        for (int i = 0; i < im.w * im.h * im.c; i++) {
            im.data[i] = 0;
        }
    } else {
        for (int i = 0; i < im.w * im.h * im.c; i++) {
            im.data[i] = (im.data[i] - min) / (max - min);
        }
    }
}

image *sobel_image(image im)
{
    image* result = (image*) malloc(sizeof(image) * 2);
    result[0] = make_image(im.w, im.h, 1);
    result[1] = make_image(im.w, im.h, 1);
    image gx = make_gx_filter();
    image gy = make_gy_filter();

    image Gx = convolve_image(im, gx, 0);
    image Gy = convolve_image(im, gy, 0);

    for (int i = 0; i < im.h * im.w; i++) {
        result[0].data[i] = sqrt(pow(Gx.data[i], 2) + pow(Gy.data[i],2));
        result[1].data[i] = atan2(Gy.data[i], Gx.data[i]);
    }

    return result;
}

image colorize_sobel(image im)
{
    // TODO
    return make_image(1,1,1);
}
