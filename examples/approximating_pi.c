/*
MIT License

Copyright (c) 2022 Jaysmito Mukherjee (jaysmito101@gmail.com)

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
#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#include "cgl.h"

static struct
{
    CGL_window* window; // the main window
    CGL_int window_height;
    CGL_int window_width;
    CGL_framebuffer* framebuffer;
} g_context;

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return EXIT_FAILURE; // initialize CGL (required for setting up internals of CGL)
    g_context.window_height = g_context.window_width = 700;
    g_context.window = CGL_window_create(g_context.window_width, g_context.window_height, "Approximating PI - Jaysmito Mukherjee"); // create the window
    CGL_window_make_context_current(g_context.window); // make the opengl context for the window current
    if(!CGL_gl_init()) return EXIT_FAILURE; // initialize cgl opengl module    
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window); // load the default framebuffer (0) into CGL_framebuffer object
    if(!CGL_widgets_init()) return EXIT_FAILURE; 
    CGL_float prev_time = CGL_utils_get_time(), curr_time = 0.0f, delta_time = 0.0f, frame_time = 0.0f;
    CGL_int frame_count = 0, frame_rate = 60; CGL_float point_size = 0.01f;
    CGL_double points_in_square = 1.0, points_in_circle = 1.0, pi_value = 0.0;
    CGL_vec2 min_val = CGL_vec2_init(-1.0f, -1.0f), max_val = CGL_vec2_init(1.0f, 1.0f), curr_val;
    // the main loop
    while(!CGL_window_should_close(g_context.window)) // run till the close button is clicked
    {
        curr_time = CGL_utils_get_time();
        delta_time = curr_time - prev_time; prev_time = curr_time; frame_time += delta_time; frame_count++;
        if(frame_time >= 1.0f) { frame_rate = frame_count; frame_time = 0.0f; frame_count = 0; }
        CGL_window_set_size(g_context.window, g_context.window_width, g_context.window_height); // set the window size
        CGL_framebuffer_bind(g_context.framebuffer); // bind default framebuffer and also adjust viewport size and offset
        CGL_widgets_begin();
        curr_val = CGL_utils_random_vec2(min_val, max_val);
        if(CGL_vec2_length(curr_val) <= 1.0) { points_in_circle += 1.0; CGL_widgets_set_fill_colorf(0.0f, 0.8f, 0.0f, 1.0f);}
        else CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.8f, 1.0f); points_in_square += 1.0; 
        pi_value = 4.0 * (points_in_circle / points_in_square);
        CGL_widgets_add_rect2f(curr_val.x, curr_val.y, point_size, point_size);
        CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.0f, 1.0f);
        CGL_widgets_add_rect2f(-0.3f, -0.1f, 0.6f, 0.2f);
        CGL_widgets_set_fill_colorf(0.7f, 0.7f, 0.7f, 1.0f);
        static CGL_byte buffer[256]; sprintf(buffer, "%lf", pi_value);
        CGL_widgets_add_string(buffer, -0.3f, -0.1f, 0.6f, 0.2f);
        CGL_widgets_end();
        if(CGL_window_is_key_pressed(g_context.window, CGL_KEY_SPACE)) {CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f); points_in_circle = 1.0; points_in_square = 1.0;}
        CGL_window_swap_buffers(g_context.window); // swap framebuffers
        CGL_window_poll_events(g_context.window); // poll events (if this is not called every frame window will stop responding)
        if(CGL_window_get_key(g_context.window, CGL_KEY_ESCAPE) == CGL_PRESS) break; // quit on pressing escape
    }
    // cleanup
    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(g_context.framebuffer); // destory framebuffer object
    CGL_gl_shutdown(); // shutdown cgl opengl module
    CGL_window_destroy(g_context.window); // destroy window
    CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
    return EXIT_SUCCESS;
}
