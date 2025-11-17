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
  int w;
  int h;
} ResizeElement;

static ResizeElement RESIZE_ELEMENT = {
    .w = 1000,
    .h = 1000,
};

typedef struct {
  Pixel *buffer;
  int reciever_pause;
  int control_pause;
} BufferState;

static BufferState buffer = {
    .buffer = NULL,
    .reciever_pause = 0,
    .control_pause = 0,
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


void frame_resize(GLFWwindow *window, int w, int h) {
  glViewport(0, 0, w, h);
  RESIZE_ELEMENT.w = w;
  RESIZE_ELEMENT.h = h;

  if (buffer.buffer != NULL) { free(buffer.buffer); }
  buffer.buffer = init_buffer(RESIZE_ELEMENT.w, RESIZE_ELEMENT.h);
}

void set_window_info(sds name, int w, int h) {
  window_info.name = name;
  window_info.w = w; window_info.h = h;
}

void* init_os_window(void* arg) {

  int init_w = window_info.w;
  int init_h = window_info.h;

  pthread_mutex_lock(&buffer_lock);
  buffer.buffer = init_buffer(init_w, init_h);
  pthread_mutex_unlock(&buffer_lock);



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


  // now the window + buffer are ready
  buffer_ready = 1;

  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);

  RESIZE_ELEMENT.w = init_w;
  RESIZE_ELEMENT.h = init_h;

  glfwSetFramebufferSizeCallback(window, frame_resize);

  while (!glfwWindowShouldClose(window)) {

    while (buffer.control_pause) {
      buffer.reciever_pause = 1;
      glfwPollEvents();
    }

    if (!buffer.reciever_pause) {


      test_fill((Pixel){0.5f, 0.9f, 1.0f, 1.0f}, buffer.buffer,
                RESIZE_ELEMENT.w, RESIZE_ELEMENT.h);

      display_buffer(window, buffer.buffer, RESIZE_ELEMENT.w, RESIZE_ELEMENT.h);

      glfwSwapBuffers(window);
    }
    buffer.reciever_pause = 0;

    glfwPollEvents();
  }

  if (buffer.buffer != NULL) {
      free(buffer.buffer);
      buffer.buffer = NULL; // Optional, but good practice
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

// renderer thread is the one that uses side thread
int start_window_thread(sds s, int h, int w) {
  printf("something");
  set_window_info(s, h, w);
  pthread_t window_thread;
  pthread_create(&window_thread, NULL, init_os_window, NULL);
  return 0;
}

// main func is only for testing
int main(void) {

  printf("something");
  start_window_thread("Le Window", 500, 500);

  while (1) {
    if (buffer_ready) {
      break;
    }
  }

  int i = 300;
  while (i>0) {
    buffer.control_pause = 1;
    draw_square((Pixel){0.9f, 0.2f, 0.1f, 1.0f}, buffer.buffer, RESIZE_ELEMENT.w, RESIZE_ELEMENT.h, 300-i);
    i--;
    buffer.control_pause = 0;
  }

  return 0;
}


