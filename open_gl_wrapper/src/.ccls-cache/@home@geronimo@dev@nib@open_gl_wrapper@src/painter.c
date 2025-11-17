#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "./buffer.c" // imports all that is in display.c as well
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
  int pause;
  int resume;
} BufferState;

static BufferState buffer = {
    .buffer = NULL,
    .pause = 0,
    .resume = 1,
};

void frame_resize(GLFWwindow *window, int w, int h) {
  printf("resizin' %d %d", w, h);
  glViewport(0, 0, w, h);
  RESIZE_ELEMENT.w = w;
  RESIZE_ELEMENT.h = h;
}

int init_os_window(sds a, int init_w, int init_h) {

  buffer.buffer = init_buffer(10, 10);

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

  glfwSetFramebufferSizeCallback(window, frame_resize);

  while (!glfwWindowShouldClose(window)) {

    if (!buffer.pause) {
      buffer.buffer = init_buffer(RESIZE_ELEMENT.w, RESIZE_ELEMENT.h);

      test_fill((Pixel){0.5f, 0.9f, 1.0f, 1.0f}, buffer.buffer,
                RESIZE_ELEMENT.w, RESIZE_ELEMENT.h);

      display_buffer(window, buffer.buffer, RESIZE_ELEMENT.w, RESIZE_ELEMENT.h);

      glfwSwapBuffers(window);
    }

    while (buffer.pause && !buffer.resume) {
      glfwPollEvents();
    }

    buffer.resume = 0;

    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

int main(void) {

  init_os_window("Le Window", 500, 500);

  return 0;
}
