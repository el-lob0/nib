#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <pthread.h>
#include "./buffer.c" // imports all that is in display.c as well
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include "./nib.h"




// this is here because of C's stupid library linking mechanism
typedef struct {
  int w;
  int h;
} WindowInfo;

volatile int nib_buffer_ready = 0;

static Display nib_buffer = {
    .buffer = NULL,
    .w = 1000,
    .h = 1000,
};

static WindowInfo nib_window_info = {
    .w = 500,
    .h = 500,
};



void nib_frame_resize(GLFWwindow *window, int w, int h) {
  if (nib_buffer.buffer) free(nib_buffer.buffer);
  nib_buffer.buffer = nib_init_buffer(w, h);
  nib_buffer.w = w; nib_buffer.h = h;
}


void nib_wait_for_buffer() {

  while (1) {
    if (nib_buffer_ready) {
      break;
    }
  }
}


GLFWwindow* nib_init_os_window(const char *title) {

  int init_w = nib_window_info.w;
  int init_h = nib_window_info.h;

  nib_buffer.buffer = nib_init_buffer(init_w, init_h);

  nib_buffer_ready = 1;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(init_w, init_h, title, NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);


  nib_buffer.w = init_w;
  nib_buffer.h = init_h;

  nib_read_window_size(window, nib_frame_resize);

  return window;
}


int nib_window_is_open(GLFWwindow* window) {
  return !glfwWindowShouldClose(window);
}


int nib_close_window(GLFWwindow* window) {
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}


