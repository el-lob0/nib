#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <unistd.h>
// #include "linmath.h"

#include <pthread.h>
#include "./display.c"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>




Pixel *init_buffer(int width, int height) {

  Pixel *buffer = malloc(width * height * sizeof(Pixel));

  for (int i = 0; i < width * height; i++) {
    buffer[i] = (Pixel){0.f, 0.f, 0.f, 0.f};
  }
  return buffer;
}

// these functions are only for applying transformations to the buffers

// prototype function, if its heavy ill just use a 2D array
int draw_pixel(Pixel *buffer, int width, int x, int y, Pixel pixel) {
  int y_counter = 0;
  int n = 0;
  for (int i = 0; i < width; i++) {
    if (n == x) {
      buffer[i] = pixel;
      return 0;
    };
    if (n == width - 1) {
      y_counter += 1;
      n = 0;
    };
    n++;
  }
  return 0;
}

void compute_radius(Pixel *buffer, int radius, int corners) {
  // check which corners need radius
  // remove/make transparent the necessary pixels, and return updated buffer
}

void scale(Pixel *buffer,
           int width /* aspect ratio is locked, so only one axis needed */) {
  // scale the buffer up or down based on the new width
}

void rotate(Pixel *buffer, int degree) {
  // calculate the correct translation for each pixel to achieve the demanded
  // the degree of rotation NOTE: must locate the center first i think
}

Pixel *build_rectangle(int w, int h) { return init_buffer(w, h); }

// useable, but needs to be checked
void test_fill(Pixel rgba, Pixel *buffer, int w, int h) {
  for (int i = 0; i < w * h; i++) {
    buffer[i] = rgba;
  }
}

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
