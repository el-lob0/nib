#define GLAD_GL_IMPLEMENTATION
#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "nib.h"
#include <stdlib.h>


typedef struct {
  int w;
  int h;
} WindowInfo;



static Display nib_buffer = {
    .buffer = NULL,
    .w = 500,
    .h = 500,
};

static WindowInfo nib_window_info = {
    .w = 500,
    .h = 500,
};


int start_anim = 0;

static void key_press_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_A && action == GLFW_PRESS)
    start_anim = 1;
}

static void mouse_callback(GLFWwindow *window, int button, int action,
                         int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    start_anim = 1;
}






void frame_resize(GLFWwindow *window, int w, int h) {
  if (nib_buffer.buffer) free(nib_buffer.buffer);
  nib_buffer.buffer = nib_init_buffer(w, h);
  nib_buffer.w = w; nib_buffer.h = h;
}


// volatile int nib_buffer_ready = 0;

int main(void) {
  GLFWwindow* window;
  const char* title = "title";
  window = nib_init_os_window(title);

  nib_buffer.buffer = nib_init_buffer(500, 500);
  nib_wait_for_buffer();

  nib_read_window_size(window, frame_resize);

  nib_set_key_callback(window, key_press_callback);
  nib_set_mouse_click_callback(window, mouse_callback);


  int offset = 0;
  while (nib_window_is_open(window)) {



    nib_fill_buffer((Pixel){0.5f, 0.9f, 1.0f, 1.0f}, nib_buffer.buffer,
                nib_buffer.w, nib_buffer.h);


    if (offset > 0 && offset <= 100 || start_anim) {

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

        Pixel* resized = nib_resize(square, 100+offset*2, sq_w, sq_h, &sq_w, &sq_h);
        free(square);   // free padded buffer
        square = resized;

        // Step 5: merge with main buffer
        int adjusted_x0 = 200 - (sq_w - 100)/2;
        int adjusted_y0 = 200 - (sq_h - 100)/2;
        nib_merge_buffers(nib_buffer.buffer, nib_buffer.w, nib_buffer.h, square, sq_w, sq_h,
                      adjusted_x0, adjusted_y0);

        free(square);   // finally free resized buffer

        start_anim = 0;
        offset++;
    }
    if (offset >=100) { offset = 0; }


    nib_display_buffer(window, nib_buffer.buffer, nib_buffer.w, nib_buffer.h);

    nib_wait_events();
    // glfwPollEvents();
  }

  if (nib_buffer.buffer != NULL) {
      free(nib_buffer.buffer);
      nib_buffer.buffer = NULL; 
  }

  nib_close_window(window);

  return 0;
}


