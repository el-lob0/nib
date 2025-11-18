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




Pixel *init_buffer(int width, int height) {

  Pixel *buffer = malloc(width * height * sizeof(Pixel));

  for (int i = 0; i < width * height; i++) {
    buffer[i] = (Pixel){0.f, 0.f, 0.f, 0.f};
  }
  return buffer;
}



// useable, but needs to be checked
void fill_buffer(Pixel rgba, Pixel *buffer, int w, int h) {
  for (int i = 0; i < w * h; i++) {
    buffer[i] = rgba;
  }
}


/// These functions are helpers for applying transformations to the buffers

Pixel *radius(Pixel *buffer, int radius, int corners) {
  return buffer;
  // check which corners need radius
  // remove/make transparent the necessary pixels, and return updated buffer
}

Pixel *scale(Pixel *buffer,
           int width /* aspect ratio is locked, so only one axis needed */) {
  return buffer;
  // scale the buffer up or down based on the new width
}


Pixel *rotate(Pixel *buffer, int degree, int w, int h) {
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

Pixel *add_padding(Pixel *buffer, int w, int h,
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



void merge_buffers(
    Pixel *bg, int bw, int bh,
    Pixel *fg, int fw, int fh,
    int x0, int y0) {
    for (int fy = 0; fy < fh; fy++) {
        int by = fy + y0;
        if (by < 0 || by >= bh) continue;

        for (int fx = 0; fx < fw; fx++) {
            int bx = fx + x0;
            if (bx < 0 || bx >= bw) continue;

            Pixel src = fg[fy * fw + fx];
            Pixel dst = bg[by * bw + bx];

            float a = src.a;

            bg[by * bw + bx] = (Pixel){
                .r = src.r * a + dst.r * (1 - a),
                .g = src.g * a + dst.g * (1 - a),
                .b = src.b * a + dst.b * (1 - a),
                .a = a + dst.a * (1 - a)
            };
        }
    }
}


Pixel *bitmap_to_buffer(int bitmap /*placeholder param*/ ) {
  return init_buffer(0, 0);
}

Pixel *apply_antialiasing(Pixel *buffer, int feather) {
  return buffer;
}

Pixel *rectangle(Pixel color, int w, int h) {
    Pixel *buf = init_buffer(w, h);
    if (!buf) return NULL;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            buf[y * w + x] = color;
        }
    }

    return buf;
}

void calculate_position() {}


void draw_square(Pixel rgba, Pixel *buffer, int w, int h, int offset) {
    int size = 100; // square size

    int start_x = offset;
    int start_y = offset;
    int end_x   = start_x + size;
    int end_y   = start_y + size;

    if (start_x >= w || start_y >= h) return;
    if (end_x > w) end_x = w;
    if (end_y > h) end_y = h;

    for (int y = start_y; y < end_y; y++) {
        int row = y * w;
        for (int x = start_x; x < end_x; x++) {
            buffer[row + x] = rgba;
        }
    }
}
