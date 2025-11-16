#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
 
#include "linmath.h"
 
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>

#include "./sds/sds.c"




typedef struct {
    float r, g, b, a;
} Pixel;


typedef struct Vertex
{
    vec2 pos;
    vec3 col;
} Vertex;

// this vec 3 field is the rgb color and can later be adjusted 

 
float quadVertices[] = {
    // x, y, u, v
    -1,  1,  0, 0,   // Top-left → v = 0
    -1, -1,  0, 1,   // Bottom-left → v = 1
     1, -1,  1, 1,   // Bottom-right → v = 1

    -1,  1,  0, 0,
     1, -1,  1, 1,
     1,  1,  1, 0
};


 
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
 


int display_buffer(GLFWwindow* window, Pixel* buffer, int w, int h)
{
    static GLuint vao = 0;
    static GLuint vbo = 0;
    static GLuint tex = 0;
    static GLuint program = 0;

    
    if (vao == 0) {
        glfwMakeContextCurrent(window);
        gladLoadGL();

        const char* vertex_shader_text =
            "#version 330 core\n"
            "layout (location = 0) in vec2 aPos;\n"
            "layout (location = 1) in vec2 aTexCoord;\n"
            "out vec2 TexCoord;\n"
            "void main() {\n"
            "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
            "    TexCoord = aTexCoord;\n"
            "}\n";

        const char* fragment_shader_text =
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

        float quadVertices[] = {
            // x, y, u, v
            -1,  1,  0, 1,
            -1, -1,  0, 0,
             1, -1,  1, 0,

            -1,  1,  0, 1,
             1, -1,  1, 0,
             1,  1,  1, 1
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
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

    // Convert float buffer [0,1] → GLubyte [0,255]
    static GLubyte* tex_buffer = NULL;
    if (!tex_buffer) tex_buffer = malloc(w * h * 4);
    for (int i = 0; i < w*h; i++) {
        tex_buffer[i*4+0] = (GLubyte)(buffer[i].r * 255.0f);
        tex_buffer[i*4+1] = (GLubyte)(buffer[i].g * 255.0f);
        tex_buffer[i*4+2] = (GLubyte)(buffer[i].b * 255.0f);
        tex_buffer[i*4+3] = (GLubyte)(buffer[i].a * 255.0f);
    }

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_buffer);

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



    return 0;
}

