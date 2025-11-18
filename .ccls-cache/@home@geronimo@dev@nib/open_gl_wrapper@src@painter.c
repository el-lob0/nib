#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <unistd.h>
// #include "linmath.h"

#include "./display.c"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>











extern Pixel *shared_buffer;
extern volatile int shared_pause;
extern volatile int buffer_ready;

void api_request_buffer_edit(void (*edit_fn)(Pixel*, int, int));



Pixel *shared_buffer = NULL;
volatile int shared_pause = 0;
volatile int buffer_ready = 0;





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
}

void compute_radius(Pixel* buffer, int radius, int corners) {
  // check which corners need radius
  // remove/make transparent the necessary pixels, and return updated buffer
}

void scale(Pixel* buffer, int width /* aspect ratio is locked, so only one axis needed */  ) {
  // scale the buffer up or down based on the new width
}

void rotate(Pixel* buffer, int degree) {
  // calculate the correct translation for each pixel to achieve the demanded the degree of rotation
  // NOTE: must locate the center first i think
} 

Pixel* build_rectangle(int w, int h) {
  return init_buffer(w, h);
}

// useable, but needs to be checked
void test_fill(Pixel rgba, Pixel *buffer, int w, int h) {
  for (int i = 0; i < w * h; i++) {
    buffer[i] = rgba;
  }
}

void draw_square(Pixel rgba, Pixel *buffer, int w, int h) {
  for (int i = 0; i < w * h; i++) {
    int current_h = floor(i / w);
    int current_w = i % w;
    if (current_h > 200 && current_w > 200 && 300 > current_w &&
        300 > current_h) {
      buffer[i] = rgba;
    }
  }
}

typedef struct {
  int w;
  int h;
} ResizeElement;

ResizeElement RESIZE_ELEMENT = {
    .w = 1000,
    .h = 1000,
};

void frame_resize(GLFWwindow *window, int w, int h) {
  printf("resizin' %d %d", w, h);
  glViewport(0, 0, w, h);
  RESIZE_ELEMENT.w = w;
  RESIZE_ELEMENT.h = h;
}



typedef void (*FrameCallback)(Pixel* buffer, int width, int height, volatile int* pause_flag);

// static FrameCallback api_callback = NULL;
//
// void set_frame_callback(FrameCallback cb) {
//     api_callback = cb;
// }

int init_os_window(sds a, int init_w, int init_h) {

  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(640, 480, a, NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);

  RESIZE_ELEMENT.w = init_w;
  RESIZE_ELEMENT.h = init_h;

  shared_buffer = init_buffer(RESIZE_ELEMENT.w, RESIZE_ELEMENT.h);

  glfwSetFramebufferSizeCallback(window, frame_resize);


  while (!glfwWindowShouldClose(window)) {






    if (!shared_pause) {
      free(shared_buffer);
      shared_buffer = init_buffer(RESIZE_ELEMENT.w, RESIZE_ELEMENT.h);

      display_buffer(window, shared_buffer, RESIZE_ELEMENT.w, RESIZE_ELEMENT.h);

      glfwSwapBuffers(window);
    }





    // havent set up a pause trigger yet

    while (shared_pause && !buffer_ready) {
      glfwPollEvents(); 
    }



    buffer_ready = 0;



    glfwPollEvents();
  }

  free(shared_buffer);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

























void api_request_buffer_edit(void (*edit_fn)(Pixel* b, int w, int h)) {
    if (!shared_buffer) return;

    shared_pause = 1;                

    while (shared_pause == 1) {      
    }

    edit_fn(shared_buffer, RESIZE_ELEMENT.w, RESIZE_ELEMENT.h);

    buffer_ready = 1;         
}





int main(void) { 


  init_os_window("Le Window", 500, 500); 


  return 0;
}
