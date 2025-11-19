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


volatile int nib_buffer_ready = 0;

typedef struct {
  Pixel* buffer;
  int w;
  int h;
} Display;


static Display nib_buffer = {
    .buffer = NULL,
    .w = 1000,
    .h = 1000,
};


typedef struct {
  sds name;
  int w;
  int h;
} WindowInfo;

static WindowInfo nib_window_info = {
    .name = "",
    .w = 0,
    .h = 0,
};

void nib_frame_resize(GLFWwindow *window, int w, int h) {
  if (nib_buffer.buffer) free(nib_buffer.buffer);
  nib_buffer.buffer = nib_init_buffer(w, h);
  nib_buffer.w = w; nib_buffer.h = h;
}

void nib_set_window_info(sds name, int w, int h) {
  nib_window_info.name = name;
  nib_window_info.w = w; nib_window_info.h = h;
}


void nib_wait_for_buffer() {

  while (1) {
    if (nib_buffer_ready) {
      break;
    }
  }
}


GLFWwindow* nib_init_os_window() {

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

  GLFWwindow *window = glfwCreateWindow(init_w, init_h, nib_window_info.name, NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glfwSwapInterval(1);


  nib_buffer.w = init_w;
  nib_buffer.h = init_h;

  glfwSetFramebufferSizeCallback(window, nib_frame_resize);

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


// main func is only for testing
int main(void) {
  nib_set_window_info("Le Window", 500, 500);
  GLFWwindow* window;
  window = nib_init_os_window();


  nib_wait_for_buffer();


  int offset = 0;
  while (nib_window_is_open(window)) {

    nib_fill_buffer((Pixel){0.5f, 0.9f, 1.0f, 1.0f}, nib_buffer.buffer,
                nib_buffer.w, nib_buffer.h);

    if (offset >= 0 && offset < 361) {

        Pixel* square = nib_rectangle((Pixel){0.9f, 0.2f, 0.1f, 0.7f}, 100, 100);

        nib_apply_radius(square, 100, 100, 20);

        Pixel* anti_aliased = nib_apply_antialiasing(square, 100, 100, 1);
        free(square);   // free padded buffer
        square = anti_aliased;

        int sq_w, sq_h;
        Pixel* padded = nib_add_padding(square, 100, 100, 140, 140, 140, 140,
                                    (Pixel){0.0f, 0.0f, 0.0f, 0.0f}, &sq_w, &sq_h);

        free(square);   // free original square
        square = padded; // square now points to padded buffer

        Pixel* resized = nib_resize(square, 100+offset, sq_w, sq_h, &sq_w, &sq_h);
        free(square);   // free padded buffer
        square = resized;

        // Step 5: merge with main buffer
        int adjusted_x0 = 200 - (sq_w - 100)/2;
        int adjusted_y0 = 200 - (sq_h - 100)/2;
        nib_merge_buffers(nib_buffer.buffer, nib_buffer.w, nib_buffer.h, square, sq_w, sq_h,
                      adjusted_x0, adjusted_y0);

        free(square);   // finally free resized buffer

        offset++;
    }


    nib_display_buffer(window, nib_buffer.buffer, nib_buffer.w, nib_buffer.h);

    // when i know more abt this ill make an alias for it 
    glfwPollEvents();
  }

  if (nib_buffer.buffer != NULL) {
      free(nib_buffer.buffer);
      nib_buffer.buffer = NULL; 
  }

  nib_close_window(window);

  return 0;
}


