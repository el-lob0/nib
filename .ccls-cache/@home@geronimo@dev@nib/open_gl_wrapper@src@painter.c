#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
 
#include "linmath.h"
 
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include "./paint_bg.c"




Pixel* init_buffer(int width, int height) {

    Pixel* buffer = malloc(width * height * sizeof(Pixel));

    for (int i = 0; i < width * height; i++) {
        buffer[i] = (Pixel){0.f, 0.f, 0.f, 0.f};
    }
    return buffer;
}


void test_fill(Pixel rgba, Pixel* buffer, int w, int h) {
    for (int i = 0; i < w*h; i++) {
        buffer[i] = rgba;
    }
}

void draw_square(Pixel rgba, Pixel* buffer, int w, int h) {
    for (int i=0; i< w*h; i++) {
        int current_h = floor(i/w);
        int current_w = i % w ;
        if (current_h > 200 && current_w > 200 && 300 > current_w && 300 > current_h) {
            buffer[i] = rgba;
        }
    }
}



 int init_os_window(sds a) {
  

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480,  a, NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);


    Pixel* pixel_buffer = init_buffer(500, 500);


    while (!glfwWindowShouldClose(window))
    {
        // fill buffer on CPU
        test_fill((Pixel){0.0f, 0.0f, 1.0f, 1.0f}, pixel_buffer, 500, 500);
        draw_square((Pixel){0.0f, 1.0f, 0.0f, 1.0f}, pixel_buffer, 500, 500);

        // render it as background (full screen)
        display_buffer(window, pixel_buffer, 500, 500);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


int main(void) {

    init_os_window("Le Window");


}

