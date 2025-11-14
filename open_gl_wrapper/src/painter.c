 
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
 
#include "linmath.h"
 
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>


#include "./sds/sds.c"




GLFWwindow* init_os_window(sds* a) {

    GLFWwindow* window;

    if (!glfwInit())
        return NULL;


    window = glfwCreateWindow(640, 480, *a, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return window;


}


typedef struct {
    float r, g, b, a;
} Pixel;


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




// static const char* vertex_shader_text =
// "#version 330\n"
// "uniform mat4 MVP;\n"
// "in vec3 vCol;\n"
// "in vec2 vPos;\n"
// "out vec3 color;\n"
// "void main()\n"
// "{\n"
// "    gl_Position = MVP * vec4(vPos, 0.0, 2.0);\n"
// "    color = vCol;\n"
// "}\n";
 
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
"uniform sampler2D screenTexture;\n" // This is the texture unit we'll use
"void main()\n"
"{\n"
"    FragColor = texture(screenTexture, TexCoord);\n"
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
 
int main(void)
{
    glfwSetErrorCallback(error_callback);
 
    if (!glfwInit())
        exit(EXIT_FAILURE);
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
 
    glfwSetKeyCallback(window, key_callback);
 
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSwapInterval(1);
 



    // RASTER

// NOTE: I dunno anything about c but this * thing is important as heck apparently

    Pixel* pixel_buffer = init_buffer(500, 500);



    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Set texture parameters for simple drawing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // For sharp pixels
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(
        GL_TEXTURE_2D, 
        0,                 // Mipmap level
        GL_RGB,            // Internal format
        500, 
        500, 
        0,                 // Border
        GL_RGB,            // Source format (must match BYTES_PER_PIXEL)
        GL_UNSIGNED_BYTE,  // Data type
        pixel_buffer       // The CPU buffer
    );

// -------------------------------------------------------------------------------


    // GLuint vertex_buffer;
    // glGenBuffers(1, &vertex_buffer);
    // glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 



    const GLuint quad_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(quad_vertex_shader, 1, &vertex_shader_text_quad, NULL);
    glCompileShader(quad_vertex_shader);

    const GLuint quad_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(quad_fragment_shader, 1, &fragment_shader_text_texture, NULL);
    glCompileShader(quad_fragment_shader);

    const GLuint quad_program = glCreateProgram();
    glAttachShader(quad_program, quad_vertex_shader);
    glAttachShader(quad_program, quad_fragment_shader);
    glLinkProgram(quad_program);
 
    // const GLint mvp_location = glGetUniformLocation(program, "MVP");
    // const GLint vpos_location = glGetAttribLocation(program, "vPos");
    // const GLint vcol_location = glGetAttribLocation(program, "vCol");

    glUseProgram(quad_program);
    glUniform1i(glGetUniformLocation(quad_program, "screenTexture"), 0); // Tell shader to use Texture Unit 0

 

    GLuint quad_VBO, quad_VAO;
    glGenVertexArrays(1, &quad_VAO);
    glGenBuffers(1, &quad_VBO);

    glBindVertexArray(quad_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, quad_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);



    while (!glfwWindowShouldClose(window))
    {
        // --- 1. RASTERIZATION (CPU WORK) ---
        // NOTE: Call your custom drawing function here (e.g., test_fill or draw_rectangle)
        // For now, let's just fill it with a test color (red)
        test_fill((Pixel){1.0f, 0.0f, 0.0f, 1.0f}, pixel_buffer, 500, 500); // 🟥 Fill with red

        // --- 2. UPLOAD PIXEL DATA TO GPU TEXTURE ---
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA, // Internal format: must match your Pixel struct (float/RGBA)
            500, 500, 0, GL_RGBA, GL_FLOAT, // Source format/type: must match your Pixel struct
            pixel_buffer // The CPU buffer pointer
        );

        // --- 3. DRAW THE TEXTURED QUAD ---
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the texture program
        glUseProgram(quad_program); 
        
        // Bind the texture to Texture Unit 0 (which the shader uses)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id); 
        
        // Draw the full-screen quad
        glBindVertexArray(quad_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6); // 6 vertices for 2 triangles (the quad)

        // --- 4. WINDOW MANAGEMENT ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwDestroyWindow(window);
 
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
