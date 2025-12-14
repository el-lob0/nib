#define GLAD_GL_IMPLEMENTATION
#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "nib.h"
#include <stdlib.h>


// This flag is used to keep the animation until its finished
int start_anim = 0;

// Callback functions: called when their respective even is set off, do required action inside. 
// (Probably need to have side effects since the function signature is unchangeable)
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

// Resize the background buffer to the size of the frame (buffer gets filled each frame so no filling here)
// void frame_resize(GLFWwindow *window, int w, int h) {
//   if (nib_buffer.buffer) free(nib_buffer.buffer);
//   nib_buffer.buffer = nib_init_buffer(w, h);
//   nib_buffer.w = w; nib_buffer.h = h;
// }

static int h = 500; 
static int w = 500;


void size_callback(GLFWwindow *window, int nw, int nh) {
  h = nh; w = nw;
}

int main(void) {
  GLFWwindow* window;
  const char* title = "title";
  window = nib_init_os_window(title);

  // this init buffer needs to match the size that nib_buffer was set to.
  Pixel *a_buffer = nib_init_buffer(500, 500);


  // the resize callback signal
  // nib_read_window_size(window, frame_resize);
  // the other callbacks
  nib_set_key_callback(window, key_press_callback);
  nib_set_mouse_click_callback(window, mouse_callback);


  glfwSetFramebufferSizeCallback(window, size_callback);

// start of main loop
  int offset = 0;
  while (nib_window_is_open(window)) {



    nib_fill_buffer((Pixel){1.0f, 0.2f, 1.0f, 1.0f}, a_buffer, w, h);

    if (a_buffer != NULL) {
      free(a_buffer);
      a_buffer = nib_rectangle((Pixel){1.0f, 0.2f, 1.0f, 1.0f}, 500, 500);
    }


    Pixel* square = nib_rectangle((Pixel){0.9f, 0.2f, 0.1f, 0.7f}, 100, 100);


    int sq_w, sq_h;


    nib_merge_buffers(a_buffer, w, h, square, 100, 100,
      0, 0);




    nib_display_buffer(window, a_buffer, w, h);

    // block loop until events, the buffer doesnt get switched untill an event is set off
    nib_wait_events();
  }



  nib_close_window(window);

  return 0;
}


