#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <unistd.h>
// #include "linmath.h"
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include <pthread.h>
#include "./display.c"
#include <stddef.h>
#include <stdlib.h>




Pixel *nib_init_buffer(int width, int height) {

  Pixel *buffer = malloc(width * height * sizeof(Pixel));

  for (int i = 0; i < width * height; i++) {
    buffer[i] = (Pixel){0.f, 0.f, 0.f, 0.f};
  }
  return buffer;
}



// useable, but needs to be checked
void nib_fill_buffer(Pixel rgba, Pixel *buffer, int w, int h) {
  for (int i = 0; i < w * h; i++) {
    buffer[i] = rgba;
  }
}


/// These functions are helpers for applying transformations to the buffers


Pixel *nib_rotate(Pixel *buffer, int degree, int w, int h) {
    Pixel *out = malloc(w * h * sizeof(Pixel));
    if (!out || !buffer) return NULL;

    float theta = degree * (float)M_PI / 180.0f;
    float cos_t = cosf(theta);
    float sin_t = sinf(theta);

    float cx = (w - 1) / 2.0f;
    float cy = (h - 1) / 2.0f;

    // clear
    for (int i = 0; i < w*h; i++)
        out[i] = (Pixel){0,0,0,0};

    for (int y_out = 0; y_out < h; y_out++) {
        for (int x_out = 0; x_out < w; x_out++) {

            float x0 = x_out - cx;
            float y0 = y_out - cy;

            float x_in =  cos_t * x0 + sin_t * y0 + cx;
            float y_in = -sin_t * x0 + cos_t * y0 + cy;

            int xi = (int)roundf(x_in);
            int yi = (int)roundf(y_in);

            if (xi >= 0 && xi < w && yi >= 0 && yi < h)
                out[y_out * w + x_out] = buffer[yi * w + xi];
        }
    }

    return out;
}

Pixel *nib_add_padding(Pixel *buffer, int w, int h,
                   int pad_left, int pad_right,
                   int pad_top, int pad_bottom,
                   Pixel pad_color,
                   int *out_w, int *out_h) {
    int nw = w + pad_left + pad_right;
    int nh = h + pad_top + pad_bottom;

    Pixel *newbuf = malloc(nw * nh * sizeof(Pixel));
    if (!newbuf) return NULL;

    // fill everything with the padding color
    for (int i = 0; i < nw * nh; i++) {
        newbuf[i] = pad_color;
    }

    // copy the original buffer into position
    for (int y = 0; y < h; y++) {
        int src_off = y * w;
        int dst_off = (y + pad_top) * nw + pad_left;

        for (int x = 0; x < w; x++) {
            newbuf[dst_off + x] = buffer[src_off + x];
        }
    }

    *out_w = nw;
    *out_h = nh;
    return newbuf;
}


Pixel *scale_buffer_center(Pixel *buffer, int w, int h,
                           int new_w, int new_h)
{
    Pixel *out = malloc(new_w * new_h * sizeof(Pixel));
    if (!out) return NULL;

    float scale_x = (float)new_w / w;
    float scale_y = (float)new_h / h;

// Use (Dimension - 1) / 2.0f for the true geometric center coordinate
    float cx_in = (w - 1) / 2.0f;
    float cy_in = (h - 1) / 2.0f;
    float cx_out = (new_w - 1) / 2.0f;
    float cy_out = (new_h - 1) / 2.0f;

    for (int y = 0; y < new_h; y++) {
        for (int x = 0; x < new_w; x++) {
            float x_rel = x - cx_out;
            float y_rel = y - cy_out;

            float x_in = x_rel / scale_x + cx_in;
            float y_in = y_rel / scale_y + cy_in;

            int xi = (int)(x_in + 0.5f);
            int yi = (int)(y_in + 0.5f);

            if (xi >= 0 && xi < w && yi >= 0 && yi < h)
                out[y * new_w + x] = buffer[yi * w + xi];
            else
                out[y * new_w + x] = (Pixel){0,0,0,0}; // transparent
        }
    }

    return out;
}

Pixel *nib_apply_antialiasing(Pixel *buffer, int w, int h, int feather) {
    if (!buffer || feather <= 0) return buffer;

    Pixel *out = malloc(w * h * sizeof(Pixel));
    if (!out) return NULL;

    // copy original buffer first
    for (int i = 0; i < w*h; i++) out[i] = buffer[i];

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float sum_a = 0.0f;
            float sum_r = 0.0f;
            float sum_g = 0.0f;
            float sum_b = 0.0f;
            int count = 0;

            // box blur kernel
            for (int ky = -feather; ky <= feather; ky++) {
                int ny = y + ky;
                if (ny < 0 || ny >= h) continue;
                for (int kx = -feather; kx <= feather; kx++) {
                    int nx = x + kx;
                    if (nx < 0 || nx >= w) continue;

                    Pixel p = buffer[ny * w + nx];
                    sum_r += p.r * p.a;
                    sum_g += p.g * p.a;
                    sum_b += p.b * p.a;
                    sum_a += p.a;
                    count++;
                }
            }

            if (sum_a > 0.0f) {
                out[y * w + x] = (Pixel){
                    sum_r / sum_a,
                    sum_g / sum_a,
                    sum_b / sum_a,
                    sum_a / count
                };
            } else {
                out[y * w + x] = (Pixel){0,0,0,0};
            }
        }
    }

    return out;
}

void nib_apply_radius(Pixel *buffer, int w, int h, int radius) {
    if (!buffer || radius <= 0) return;

    // Top-left corner
    int cx = radius - 1, cy = radius - 1;
    for (int y = 0; y < radius; y++) {
        for (int x = 0; x < radius; x++) {
            if ((x - cx)*(x - cx) + (y - cy)*(y - cy) >= radius*radius) {
                buffer[y*w + x].a = 0.0f;
            }
        }
    }

    // Top-right corner
    cx = w - radius; cy = radius - 1;
    for (int y = 0; y < radius; y++) {
        for (int x = w - radius; x < w; x++) {
            if ((x - cx)*(x - cx) + (y - cy)*(y - cy) >= radius*radius) {
                buffer[y*w + x].a = 0.0f;
            }
        }
    }

    // Bottom-left corner
    cx = radius - 1; cy = h - radius;
    for (int y = h - radius; y < h; y++) {
        for (int x = 0; x < radius; x++) {
            if ((x - cx)*(x - cx) + (y - cy)*(y - cy) >= radius*radius) {
                buffer[y*w + x].a = 0.0f;
            }
        }
    }

    // Bottom-right corner
    cx = w - radius; cy = h - radius;
    for (int y = h - radius; y < h; y++) {
        for (int x = w - radius; x < w; x++) {
            if ((x - cx)*(x - cx) + (y - cy)*(y - cy) >= radius*radius) {
                buffer[y*w + x].a = 0.0f;
            }
        }
    }

}



Pixel *nib_rectangle(Pixel color, int w, int h) {
    Pixel *buf = nib_init_buffer(w, h);
    if (!buf) return NULL;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            buf[y * w + x] = color;
        }
    }

    return buf;
}





Pixel *nib_resize(Pixel *buffer, int new_w, int w, int h,
              int *out_w, int *out_h)
{
    Pixel *out = scale_buffer_center(buffer, w, h, new_w, 
                                     (int)(h * ((float)new_w / w) + 0.5f));
    *out_w = new_w;
    *out_h = (int)(h * ((float)new_w / w) + 0.5f);
    return out;
}

void nib_merge_buffers(
    Pixel *bg, int bw, int bh,
    Pixel *fg, int fw, int fh,
    int x0, int y0) 
{
    for (int fy = 0; fy < fh; fy++) {
        int by = fy + y0;
        if (by < 0 || by >= bh) continue;

        for (int fx = 0; fx < fw; fx++) {
            int bx = fx + x0;
            if (bx < 0 || bx >= bw) continue;

            Pixel source = fg[fy * fw + fx];
            Pixel destination = bg[by * bw + bx];

            if (source.a == 0.0f) {
                // transparent, leave bg pixel unchanged
                continue;
            }

            float a = source.a;
            bg[by * bw + bx] = (Pixel){
                .r = source.r * a + destination.r * (1 - a),
                .g = source.g * a + destination.g * (1 - a),
                .b = source.b * a + destination.b * (1 - a),
                .a = a + destination.a * (1 - a)
            };
        }
    }
}


Pixel *nib_bitmap_to_buffer(int bitmap /*placeholder param*/ ) {
  return nib_init_buffer(0, 0);
}

void nib_calculate_position() {}

