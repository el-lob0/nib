# Nib  
Nib is a header-only C library that wraps OpenGL and GLFW to easily enable window creation and the use of openGL for showing pixel buffers as textures in that window.  
  
    
Feel free to make an issue or pr.


# Usage:
Go to ``include/`` and copy ``nib.h`` into your corresponding project folder.  
Make sure to have glfw and opengl correctly installed for building.  
I used ``nob.h`` in the example with ``gcc`` for building.  
  
# Dependencies
OpenGL  
GLFW  
Glad (I assume you can just copy mine)


--- 

# Example  
  
See `src/example.c` for a working example.



# Documentation

Let's start by creating the window, and initializing a pixel buffer.  
```c
GLFWwindow* window; // no type alias here
const char* title = "title";
window = nib_init_os_window(title);

Pixel *main_buffer = nib_init_buffer(1000, 1000);
// A 1D array of (Pixel){r, g, b, a} 
// and each color is a floating number between 0.0f and 1.0f

```
  
Now we start the main loop:
```c
while (nib_window_is_open(window)) { 

}
```
  
  
Inside the main loop, we can color the buffer:
```c
nib_fill_buffer((Pixel){1.0f, 0.2f, 1.0f, 1.0f}, main_buffer, 1000, 1000); // size needs to match the size of main_buffer

```  
  
We can draw a square inside it
```c
Pixel* square = nib_rectangle((Pixel){0.9f, 0.2f, 0.1f, 0.7f}, 100, 100);
nib_merge_buffers(a_buffer, w, h, square, 100, 100, 0, 0);
//                                         w    h   x  y

```  
  
Finally, tell opengl to show the buffer:
```c
nib_display_buffer(window, main_buffer, 1000, 1000);

// block loop until events, the buffer doesnt get switched untill an event is set off
nib_wait_events();
  
```
  
If i want the main buffer to fit the screen size every frame:
```c
static int h = 500; 
static int w = 500;

void size_callback(GLFWwindow *window, int nw, int nh) {
  h = nh; w = nw;
}

// and then in the main loop
glfwSetFramebufferSizeCallback(window, size_callback);
// so this will call size callback whenever the window changes size between 2 frames
```

[!NOTE] 
> I made some macros for events, however they are incomplete, and it is easier to just use GLFW's events directly.
```c
glfw_set_key_callback(glfw_window, callback);  // and so on...
```


