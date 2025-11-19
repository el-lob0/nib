#define GLAD_GL_IMPLEMENTATION
#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "nib.h"

static Display nib_buffer = {
    .buffer = NULL,
    .w = 1000,
    .h = 1000,
};

static WindowInfo nib_window_info = {
    .name = "",
    .w = 0,
    .h = 0,
};

volatile int nib_buffer_ready = 0;

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


