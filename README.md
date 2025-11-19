# Nib  
Nib is a raster software rendering library that uses glfw and openGL.  
(Thus i can say its gpu-accelerated)  


### > development:
The library's messy project files.  
  

### > target:
The library: `libnib.a`, headers: `nib.h`.  

# Dependencies
OpenGL  
GLFW  
Cmake probably  

# Installation
  
I honestly dont know... C's dependency hell is the worst of them all.  
You can just clone the repo and write your program in target/src/  

--- 

# Documentation
Its all in nib.h but:

```c
#pragma once
#include <stdint.h>


/// pixels, contains rgba values
typedef struct {
  float r, g, b, a;
} Pixel;

/// used to determine the main buffer's size, and the actual buffer to display.
typedef struct {
  Pixel* buffer;
  int w;
  int h;
} Display;


/// initialize a black colored buffer of size width/height
Pixel* nib_init_buffer(int width, int height);

/// fill the buffer in the input parameters with the color input
void nib_fill_buffer(Pixel rgba, Pixel* buffer, int w, int h);

/// make a rectangle buffer of a certain color
Pixel* nib_rectangle(Pixel color, int w, int h);

/// apply radius to a buffer with corners
void nib_apply_radius(Pixel* buffer, int w, int h, int radius);

/// rotate a buffer, padding is needed to avoid clipping
Pixel* nib_rotate(Pixel* buffer, int degree, int w, int h);

/// add padding
Pixel* nib_add_padding(
    Pixel* buffer, int w, int h,
    int pad_left, int pad_right,
    int pad_top, int pad_bottom,
    Pixel pad_color,
    int* out_w, int* out_h
);

/// resize image
Pixel* nib_resize(
    Pixel* buffer, int new_w,
    int w, int h,
    int* out_w, int* out_h
);

/// feather = 1 for anti-aliasing, anything beyond that is more blur than aa. 
Pixel* nib_apply_antialiasing(Pixel* buffer, int w, int h, int feather);

/// merge 2 buffers together
void nib_merge_buffers(
    Pixel* bg, int bw, int bh,
    Pixel* fg, int fw, int fh,
    int x0, int y0
);

/// before initializing the window, give it some initialization values
void nib_set_window_info(char* name, int w, int h);

/// use after initializing a buffer to avoid using a null value
void nib_wait_for_buffer(void);

/// start window
GLFWwindow* nib_init_os_window(const char *title);

/// check used to keep main loop running
int nib_window_is_open(GLFWwindow* window);

/// glfw stuff for terminating the program properly
int nib_close_window(GLFWwindow* window);

/// display the buffer as an opengl texture
int nib_display_buffer(GLFWwindow *window, Pixel *buffer, int w, int h);

// --------------- Adapting to window resize -------------------------------
/// this is the signature of the function but its defined in example and you need to define it in your program aswell 
void nib_frame_resize(GLFWwindow *window, int w, int h);

// If you want frame resizing control you should call this right before the loop
nib_read_window_size(window, frame_resize);
/// this feeds the updated dimentions to the frame_resize function, and you deal with the rest


//----------------------Event Handling-----------------------------------------

/// These are straight-forward
#define nib_read_window_size(window, func) glfwSetFramebufferSizeCallback(window, func)
#define nib_wait_events() glfwWaitEvents()
#define nib_poll_events() glfwPollEvents()
#define nib_set_key_callback(window, key_callback) glfwSetKeyCallback(window, key_callback);
#define nib_set_mouse_click_callback(window, key_callback) glfwSetMouseButtonCallback(window, mouse_callback);
#define nib_set_cursor_position_callback(window, key_callback) glfwSetCursorPosCallback(window, cursor_callback);

```

# TODO:  

[ ]- Bitmap to buffer (for font rendering)  
[X]- Input event forwarding  
[ ]- idk  
