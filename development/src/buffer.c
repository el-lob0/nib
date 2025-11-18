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


int rotate(Pixel *buffer, int degree, int w, int h) {
  Pixel *rotated_buffer= malloc(w * h * sizeof(Pixel));
  if (!buffer) return 1;

  float theta = degree*(float)M_PI/180.0f;
  float cos_theta = cosf(theta);
  float sin_theta = sinf(theta);

  float cx = (w - 1)/2.0f;
  float cy = (h - 1)/2.0f;

  int new_w = w;
  int new_h = h;

  fill_buffer((Pixel){0.0f, 0.0f, 0.0f, 0.0f}, rotated_buffer, w, h);

  for (int x_out=0; x_out<new_w*new_h; x_out++) {
    for (int y_out=0; y_out<new_w*new_w; y_out++) {
      float x0 = x_out - cx;
      float y0 = y_out - cy;

      float x_in = cos_theta*x0 + sin_theta*y0 + cx;
      float y_in = -sin_theta*x0 + cos_theta*y0 + cy;

      int xi = (int)(x_in + 0.5f);
      int yi = (int)(y_in + 0.5f);

      if (xi >= 0 && xi < w && yi >=0 && yi < h) {
        rotated_buffer[y_out*new_w + x_out] = buffer[yi * w + xi];
      }
    }
  }

  buffer = rotated_buffer;
  free(rotated_buffer);
  return 0;
  // calculate the correct translation for each pixel to achieve the demanded
  // the degree of rotation NOTE: must locate the center first i think
}

void merge_buffers(Pixel *bg_buffer, Pixel *fg_buffer, int fg_w, int fg_h, int x, int y) {
  int x_center = round(fg_w/2);
  int y_center = round(fg_h/2);
  int x_margin = round(x - x_center)-1;
  int y_margin = round(y - y_center)-1;



  for (int x = x_margin; x < fg_w; x++) {
    int total = x*fg_w;
    for (int y = y_margin; y < fg_h; y++) {
      Pixel {fg_r, fg_g, fg_b, fg_a} = fg_buffer[total + x - x_margin];
      Pixel {bg_r, bg_g, bg_b, bg_a} = bg_buffer[total + x];

    }
  }

  for (int y = start_y; y < end_y; y++) {
    int row = y * w;
    for (int x = start_x; x < end_x; x++) {
      bg_buffer[row + x] = rgba;
    }
  }
}


Pixel *bitmap_to_buffer(int bitmap /*placeholder param*/ ) {
  return init_buffer(0, 0);
}

Pixel *apply_antialiasing(Pixel *buffer, int feather) {
  return buffer;
}

// Pixel *apply_antialiasing(Pixel *buffer, int feather) {
//   return init_buffer(0, 0);
// }

Pixel *build_rectangle(int w, int h) { return init_buffer(w, h); }

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
