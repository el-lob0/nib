#pragma once

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

/// These are straight-forward
#define nib_read_window_size(window, func) glfwSetFramebufferSizeCallback(window, func)
#define nib_wait_events() glfwWaitEvents()
#define nib_poll_events() glfwPollEvents()
#define nib_set_key_callback(window, key_callback) glfwSetKeyCallback(window, key_callback);
#define nib_set_mouse_click_callback(window, key_callback) glfwSetMouseButtonCallback(window, mouse_callback);
#define nib_set_cursor_position_callback(window, key_callback) glfwSetCursorPosCallback(window, cursor_callback);
  
// ADD: mouse click, mouse movement

typedef struct {
  float r, g, b, a;
} Pixel;

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

/// callback function that edits the window's DATA to follow its actual size
void nib_frame_resize(GLFWwindow *window, int w, int h);

/// start window
GLFWwindow* nib_init_os_window(const char *title);

/// check used to keep main loop running
int nib_window_is_open(GLFWwindow* window);

/// glfw stuff for terminating the program properly
int nib_close_window(GLFWwindow* window);

/// display the buffer as an opengl texture
int nib_display_buffer(GLFWwindow *window, Pixel *buffer, int w, int h);


/* ------------ Misc ------------ */


typedef struct {
    int width;
    int rows;
    int pitch;
    int pixel_mode;   // user fills this from FT_PIXEL_MODE_*
    unsigned char *buffer;
} nib_raw_bitmap;

Pixel *nib_bitmap_to_buffer(const nib_raw_bitmap *bmp, Pixel color);










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

  
  // purely for initialization/default, isnt an api functionality

  nib_buffer.buffer = nib_init_buffer(init_w, init_h);

  nib_buffer_ready = 1;

  // glfwSetErrorCallback(error_callback);

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

#include <stdio.h>
#include "linmath.h"


typedef struct Vertex {
  vec2 pos;
  vec3 col;
} Vertex;

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int nib_display_buffer(GLFWwindow *window, Pixel *buffer, int w, int h) {
  static GLuint vao = 0;
  static GLuint vbo = 0;
  static GLuint tex = 0;
  static GLuint program = 0;

  if (vao == 0) {
    glfwMakeContextCurrent(window);
    gladLoadGL();

    const char *vertex_shader_text =
        "#version 330 core\n"
        "layout (location = 0) in vec2 aPos;\n"
        "layout (location = 1) in vec2 aTexCoord;\n"
        "out vec2 TexCoord;\n"
        "void main() {\n"
        "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
        "    TexCoord = aTexCoord;\n"
        "}\n";

    const char *fragment_shader_text =
        "#version 330 core\n"
        "in vec2 TexCoord;\n"
        "out vec4 FragColor;\n"
        "uniform sampler2D screenTexture;\n"
        "uniform vec2 u_tiling_factor;\n"
        "void main() {\n"
        "    FragColor = texture(screenTexture, TexCoord * u_tiling_factor);\n"
        "}\n";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader_text, NULL);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader_text, NULL);
    glCompileShader(fs);

    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    float quadVertices[] = {// x, y, u, v
                            -1, 1, 0, 1, -1, -1, 0, 0, 1, -1, 1, 0,

                            -1, 1, 0, 1, 1,  -1, 1, 0, 1, 1,  1, 1};

    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, "screenTexture"), 0);
  }

  // Use static variables to track the current allocated size
  static GLubyte *tex_buffer = NULL;
  static int allocated_w = 0;
  static int allocated_h = 0;

  // Check if the required size (w*h) is different from the current allocated
  // size
  if (w != allocated_w || h != allocated_h) {
    // 1. FREE the old buffer (if it exists) to prevent a memory leak
    if (tex_buffer != NULL) {
      free(tex_buffer);
    }

    // 2. ALLOCATE the new, correct size
    tex_buffer = (GLubyte *)malloc(w * h * 4);

    // 3. CRITICAL: Check for allocation failure
    if (tex_buffer == NULL) {
      // Handle error: possibly exit or skip frame
      allocated_w = 0;
      allocated_h = 0;
      return 0;
    }

    // 4. Update the tracking variables
    allocated_w = w;
    allocated_h = h;
  }

  // The loop is now safe because tex_buffer is guaranteed to be the correct
  // size
  for (int i = 0; i < w * h; i++) {
    tex_buffer[i * 4 + 0] = (GLubyte)(buffer[i].r * 255.0f);
    tex_buffer[i * 4 + 1] = (GLubyte)(buffer[i].g * 255.0f);
    tex_buffer[i * 4 + 2] = (GLubyte)(buffer[i].b * 255.0f);
    tex_buffer[i * 4 + 3] = (GLubyte)(buffer[i].a * 255.0f);
  }

  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               tex_buffer);

  int win_w, win_h;
  glfwGetFramebufferSize(window, &win_w, &win_h);

  float tile_x = (float)win_w / w;
  float tile_y = (float)win_h / h;

  glUseProgram(program);
  glUniform2f(glGetUniformLocation(program, "u_tiling_factor"), tile_x, tile_y);

  glViewport(0, 0, win_w, win_h);
  glClear(GL_COLOR_BUFFER_BIT);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glfwSwapBuffers(window);
  return 0;
}


#include <math.h>

Pixel *nib_init_buffer(int width, int height) {

  Pixel *buffer = malloc(width * height * sizeof(Pixel));

  for (int i = 0; i < width * height; i++) {
    buffer[i] = (Pixel){0.f, 0.f, 0.f, 0.f};
  }
  return buffer;
}



// useable, but needs to be checked
void nib_fill_buffer(Pixel rgba, Pixel *buffer, int w, int h) {
  for (int i = 0; i < w * h; i++) {
    buffer[i] = rgba;
  }
}


/// These functions are helpers for applying transformations to the buffers

Pixel *nib_add_padding(Pixel *buffer, int w, int h,
                   int pad_left, int pad_right,
                   int pad_top, int pad_bottom,
                   Pixel pad_color,
                   int *out_w, int *out_h) {
    int nw = w + pad_left + pad_right;
    int nh = h + pad_top + pad_bottom;

    Pixel *newbuf = malloc(nw * nh * sizeof(Pixel));
    if (!newbuf) return NULL;

    // fill everything with the padding color
    for (int i = 0; i < nw * nh; i++) {
        newbuf[i] = pad_color;
    }

    // copy the original buffer into position
    for (int y = 0; y < h; y++) {
        int src_off = y * w;
        int dst_off = (y + pad_top) * nw + pad_left;

        for (int x = 0; x < w; x++) {
            newbuf[dst_off + x] = buffer[src_off + x];
        }
    }

    *out_w = nw;
    *out_h = nh;
    return newbuf;
}


Pixel *scale_buffer_center(Pixel *buffer, int w, int h,
                           int new_w, int new_h)
{
    Pixel *out = malloc(new_w * new_h * sizeof(Pixel));
    if (!out) return NULL;

    float scale_x = (float)new_w / w;
    float scale_y = (float)new_h / h;

// Use (Dimension - 1) / 2.0f for the true geometric center coordinate
    float cx_in = (w - 1) / 2.0f;
    float cy_in = (h - 1) / 2.0f;
    float cx_out = (new_w - 1) / 2.0f;
    float cy_out = (new_h - 1) / 2.0f;

    for (int y = 0; y < new_h; y++) {
        for (int x = 0; x < new_w; x++) {
            float x_rel = x - cx_out;
            float y_rel = y - cy_out;

            float x_in = x_rel / scale_x + cx_in;
            float y_in = y_rel / scale_y + cy_in;

            int xi = (int)(x_in + 0.5f);
            int yi = (int)(y_in + 0.5f);

            if (xi >= 0 && xi < w && yi >= 0 && yi < h)
                out[y * new_w + x] = buffer[yi * w + xi];
            else
                out[y * new_w + x] = (Pixel){0,0,0,0}; // transparent
        }
    }

    return out;
}

void nib_apply_radius(Pixel *buffer, int w, int h, int radius) {
    if (!buffer || radius <= 0) return;

    // Top-left corner
    int cx = radius - 1, cy = radius - 1;
    for (int y = 0; y < radius; y++) {
        for (int x = 0; x < radius; x++) {
            if ((x - cx)*(x - cx) + (y - cy)*(y - cy) >= radius*radius) {
                buffer[y*w + x].a = 0.0f;
            }
        }
    }

    // Top-right corner
    cx = w - radius; cy = radius - 1;
    for (int y = 0; y < radius; y++) {
        for (int x = w - radius; x < w; x++) {
            if ((x - cx)*(x - cx) + (y - cy)*(y - cy) >= radius*radius) {
                buffer[y*w + x].a = 0.0f;
            }
        }
    }

    // Bottom-left corner
    cx = radius - 1; cy = h - radius;
    for (int y = h - radius; y < h; y++) {
        for (int x = 0; x < radius; x++) {
            if ((x - cx)*(x - cx) + (y - cy)*(y - cy) >= radius*radius) {
                buffer[y*w + x].a = 0.0f;
            }
        }
    }

    // Bottom-right corner
    cx = w - radius; cy = h - radius;
    for (int y = h - radius; y < h; y++) {
        for (int x = w - radius; x < w; x++) {
            if ((x - cx)*(x - cx) + (y - cy)*(y - cy) >= radius*radius) {
                buffer[y*w + x].a = 0.0f;
            }
        }
    }

}

Pixel *nib_rectangle(Pixel color, int w, int h) {
    Pixel *buf = nib_init_buffer(w, h);
    if (!buf) return NULL;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            buf[y * w + x] = color;
        }
    }

    return buf;
}


Pixel *nib_resize(Pixel *buffer, int new_w, int w, int h,
              int *out_w, int *out_h)
{
    Pixel *out = scale_buffer_center(buffer, w, h, new_w, 
                                     (int)(h * ((float)new_w / w) + 0.5f));
    *out_w = new_w;
    *out_h = (int)(h * ((float)new_w / w) + 0.5f);
    return out;
}


void nib_merge_buffers(
    Pixel *bg, int bw, int bh,
    Pixel *fg, int fw, int fh,
    int x0, int y0) 
{
    for (int fy = 0; fy < fh; fy++) {
        int by = fy + y0;
        if (by < 0 || by >= bh) continue;

        for (int fx = 0; fx < fw; fx++) {
            int bx = fx + x0;
            if (bx < 0 || bx >= bw) continue;

            Pixel source = fg[fy * fw + fx];
            Pixel destination = bg[by * bw + bx];

            if (source.a == 0.0f) {
                // transparent, leave bg pixel unchanged
                continue;
            }

            float a = source.a;
            bg[by * bw + bx] = (Pixel){
                .r = source.r * a + destination.r * (1 - a),
                .g = source.g * a + destination.g * (1 - a),
                .b = source.b * a + destination.b * (1 - a),
                .a = a + destination.a * (1 - a)
            };
        }
    }
}



