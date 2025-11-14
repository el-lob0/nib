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

 
const float quadVertices[] = {
    // Coords (aPos) // TexCoords (aTexCoord)
    -1.0f,  1.0f,  0.0f, 1.0f, // Top-left
    -1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
     1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right

    -1.0f,  1.0f,  0.0f, 1.0f, // Top-left
     1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
     1.0f,  1.0f,  1.0f, 1.0f  // Top-right
};




static const char* vertex_shader_text_quad =
"#version 330 core\n"
"layout (location = 0) in vec2 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"    TexCoord = aTexCoord;\n"
"}\n";

static const char* fragment_shader_text_texture =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D screenTexture;\n"
"uniform vec2 u_tiling_factor;\n"
"void main()\n"
"{\n"
"    FragColor = texture(screenTexture, TexCoord * u_tiling_factor);\n"
"}\n";

 
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
 
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
 

int set_buffer_as_bg(GLFWwindow* window, Pixel* buffer, int w, int h)
{
    static GLuint vao = 0;
    static GLuint vbo = 0;
    static GLuint tex = 0;
    static GLuint program = 0;

    // 1. Lazy init once
    if (vao == 0) {
        glfwMakeContextCurrent(window);
        gladLoadGL();

        // shader
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vertex_shader_text_quad, NULL);
        glCompileShader(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fragment_shader_text_texture, NULL);
        glCompileShader(fs);

        program = glCreateProgram();
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);

        // quad
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
        glEnableVertexAttribArray(1);

        // texture
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "screenTexture"), 0);
    }

    // 2. upload buffer every call
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        w,
        h,
        0,
        GL_RGBA,
        GL_FLOAT,
        buffer
    );

    // 3. draw
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

