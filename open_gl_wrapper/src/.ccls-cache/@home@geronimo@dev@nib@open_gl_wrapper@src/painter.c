#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <pthread.h>
#include "./buffer.c" // imports all that is in display.c as well
#include <stddef.h>
// #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>






volatile int buffer_ready = 0;


typedef struct {
  Pixel* buffer;
  int w;
  int h;
} Display;

static Display buffer = {
    .buffer = NULL,
    .w = 1000,
    .h = 1000,
};




typedef struct {
  sds name;
  int w;
  int h;
} WindowInfo;

static WindowInfo window_info = {
    .name = "",
    .w = 0,
    .h = 0,
};

// place holder until i deal with resize at the api call stage
void frame_resize(GLFWwindow *window, int w, int h) {
  if (buffer.buffer) free(buffer.buffer);
  buffer.buffer = init_buffer(w, h);
  buffer.w = w; buffer.h = h;
}

void set_window_info(sds name, int w, int h) {
  window_info.name = name;
  window_info.w = w; window_info.h = h;
}


void buffer_switch_signal() {
  glfwPostEmptyEvent();
}


void wait_for_buffer() {

  while (1) {
    if (buffer_ready) {
      break;
    }
  }
}


GLFWwindow* init_os_window() {

  int init_w = window_info.w;
  int init_h = window_info.h;

  buffer.buffer = init_buffer(init_w, init_h);

  buffer_ready = 1;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(init_w, init_h, window_info.name, NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);


  buffer.w = init_w;
  buffer.h = init_h;

  glfwSetFramebufferSizeCallback(window, frame_resize);

  return window;
}


int window_is_open(GLFWwindow* window) {
  return !glfwWindowShouldClose(window);
}


int close_window(GLFWwindow* window) {
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}


// main func is only for testing
int main(void) {
  set_window_info("Le Window", 500, 500);
  GLFWwindow* window;
  window = init_os_window();


  wait_for_buffer();


  int offset = 0;
  while (window_is_open(window)) {

    // part of the resize placeholder fix
    test_fill((Pixel){0.5f, 0.9f, 1.0f, 1.0f}, buffer.buffer,
              buffer.w, buffer.h);

    if (offset >= 0 && offset < 1001) {
      draw_square((Pixel){0.9f, 0.2f, 0.1f, 1.0f}, buffer.buffer, buffer.w, buffer.h, offset);
      offset++;
    }


    display_buffer(window, buffer.buffer, buffer.w, buffer.h);

    // when i know more abt this ill make an alias for it 
    glfwPollEvents();
  }

  if (buffer.buffer != NULL) {
      free(buffer.buffer);
      buffer.buffer = NULL; 
  }

  close_window(window);

  return 0;
}


