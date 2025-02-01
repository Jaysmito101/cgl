/*
MIT License

Copyright (c) 2023 Jaysmito Mukherjee (jaysmito101@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_AUDIO
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#include "cgl.h"


#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif

static struct
{
    CGL_window* window; // the main window
    int window_height;
    int window_width;
    CGL_framebuffer* framebuffer;
    CGL_float curr_time;
} g_State;

CGL_void draw_widgets() {
    CGL_widgets_begin();

    srand((uint32_t)time(NULL));
        
    CGL_widgets_set_fill_color(CGL_utils_random_color());
    CGL_widgets_set_stroke_color(CGL_utils_random_color());
    CGL_widgets_add_quad(
        CGL_vec3_init(-0.5f, -0.5f,  0.0f),    
        CGL_vec3_init( 0.5f, -0.5f,  0.0f),    
        CGL_vec3_init( 0.5f,  0.5f,  0.0f),    
        CGL_vec3_init(-0.5f,  0.5f,  0.0f)
    );
        
    CGL_widgets_set_fill_color(CGL_utils_random_color());
    CGL_widgets_set_stroke_color(CGL_utils_random_color());
    CGL_widgets_add_line(
        CGL_vec3_init(0.1f, 0.1f, 0.0f),
        CGL_vec3_init(0.8f, 0.8f, 0.0f)
    );

    CGL_widgets_set_fill_color(CGL_utils_random_color());
    CGL_widgets_set_stroke_color(CGL_utils_random_color());
    CGL_widgets_add_quad(
        CGL_vec3_init(-0.1f, -0.2f,  0.0f),    
        CGL_vec3_init( 0.2f, -0.3f,  0.0f),    
        CGL_vec3_init( 0.4f,  0.2f,  0.0f),    
        CGL_vec3_init(-0.3f,  0.8f,  0.0f)
    );
        
    CGL_widgets_set_fill_mode(false);
    CGL_widgets_set_fill_color(CGL_utils_random_color());
    CGL_widgets_set_stroke_color(CGL_utils_random_color());
    CGL_widgets_add_quad(
        CGL_vec3_init(-0.1f, -0.2f,  0.0f),    
        CGL_vec3_init( 0.2f, -0.3f,  0.0f),    
        CGL_vec3_init( 0.4f,  0.2f,  0.0f),    
        CGL_vec3_init(-0.3f,  0.8f,  0.0f)
    );


    CGL_widgets_add_triangle(
        CGL_vec3_init(-0.8f, -0.9f, 0.0f),
        CGL_vec3_init(-0.6f, 0.6f, 0.0f),
        CGL_vec3_init(-0.9f, 0.5f, 0.0f)
    );

    CGL_widgets_set_fill_mode(true);
    CGL_widgets_add_circle(CGL_vec3_init(0.0f, 0.0f, 0.0f), 0.2f);

    CGL_widgets_end();
}

CGL_bool init() {
    srand((uint32_t)time(NULL));
    g_State.window_height = 600;
    g_State.window_width = 600;

    if(!CGL_init()) return CGL_FALSE;
    g_State.window = CGL_window_create(g_State.window_width, g_State.window_height, "Advanced Widgets - Jaysmito Mukherjee"); // create the window
    if(g_State.window == NULL) return CGL_FALSE; // failed to create window
    CGL_window_make_context_current(g_State.window); // make the opengl context for the window current
    if(!CGL_gl_init()) return CGL_FALSE; // initialize opengl
    g_State.framebuffer = CGL_framebuffer_create_from_default(g_State.window); // load the default framebuffer (0) into CGL_framebuffer object
    if(g_State.framebuffer == NULL) return CGL_FALSE; // failed
    if(!CGL_widgets_init()) return CGL_FALSE;
    g_State.curr_time = 0.0f;
    return CGL_TRUE;
}

CGL_void cleanup() {
    // cleanup
    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(g_State.framebuffer); // destory framebuffer object
    CGL_gl_shutdown(); // shutdown cgl opengl module
    CGL_window_destroy(g_State.window); // destroy window
    CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
}

EM_BOOL loop(double _time, void* userData) {
    (void)_time;
    (void)userData;

    g_State.curr_time = CGL_utils_get_time();
    CGL_window_set_size(g_State.window, g_State.window_width, g_State.window_height); // set window size
    // rendering
    CGL_framebuffer_bind(g_State.framebuffer); // bind default framebuffer and also adjust viewport size and offset
    CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f); // clear screen with a dark gray color


    draw_widgets();
    

    CGL_window_swap_buffers(g_State.window); // swap framebuffers
    CGL_window_poll_events(g_State.window); // poll events (if this is not called every frame window will stop responding)

    if(CGL_window_get_key(g_State.window, CGL_KEY_ESCAPE) == CGL_PRESS) return CGL_FALSE; // exit if escape key is pressed

    return CGL_TRUE;
}

int main()
{
    if (!init()) return EXIT_FAILURE; // initialize the application
#ifdef CGL_WASM
    CGL_info("Running in WASM mode");   
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    // the main loop
    while(!CGL_window_should_close(g_State.window)) // run till the close button is clicked
    {
        if (!loop(0.0, NULL)) break;
    }
    cleanup();
#endif
    
    return EXIT_SUCCESS;
}
