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
#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_AUDIO
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_IMPLEMENTATION
#include "cgl.h"


#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif

#define MAX_POINTS 1000000

static struct
{
    CGL_window* window; // the main window
    CGL_int window_height;
    CGL_int window_width;
    CGL_framebuffer* framebuffer;


    CGL_float prev_time, curr_time, delta_time, frame_time;
    CGL_int frame_count, frame_rate;
    CGL_float point_size;
    CGL_double points_in_square, points_in_circle, pi_value;
    CGL_vec2 min_val, max_val;

    CGL_vec2 points[MAX_POINTS];
    CGL_sizei num_points;
} g_State;

CGL_bool init() {
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return EXIT_FAILURE; // initialize CGL (required for setting up internals of CGL)
    g_State.window_height = g_State.window_width = 700;
    g_State.window = CGL_window_create(g_State.window_width, g_State.window_height, "Approximating PI - Jaysmito Mukherjee"); // create the window
    CGL_window_make_context_current(g_State.window); // make the opengl context for the window current
    if(!CGL_gl_init()) return EXIT_FAILURE; // initialize cgl opengl module    
    g_State.framebuffer = CGL_framebuffer_create_from_default(g_State.window); // load the default framebuffer (0) into CGL_framebuffer object
    if(!CGL_widgets_init()) return EXIT_FAILURE; 
    g_State.prev_time = CGL_utils_get_time();
    g_State.curr_time = g_State.delta_time = g_State.frame_time = 0.0f;
    g_State.frame_count = 0, g_State.frame_rate = 60;
    g_State.point_size = 0.01f;
    g_State.points_in_square = g_State.points_in_circle = 1.0;
    g_State.pi_value = 0.0;

    g_State.min_val = CGL_vec2_init(-1.0f, -1.0f);
    g_State.max_val = CGL_vec2_init(1.0f, 1.0f);

    g_State.num_points = 0;

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

EM_BOOL loop(double time, void* userData) {
    (void)time;
    (void)userData;

    g_State.curr_time = CGL_utils_get_time();
    g_State.delta_time = g_State.curr_time - g_State.prev_time; g_State.prev_time = g_State.curr_time; g_State.frame_time += g_State.delta_time; g_State.frame_count++;
    if(g_State.frame_time >= 1.0f) {
        g_State.frame_rate = g_State.frame_count;
        g_State.frame_time = 0.0f;
        g_State.frame_count = 0; 
    }

    CGL_vec2 curr_val = CGL_utils_random_vec2(g_State.min_val, g_State.max_val);

    CGL_window_set_size(g_State.window, g_State.window_width, g_State.window_height); // set the window size
    CGL_framebuffer_bind(g_State.framebuffer); // bind default framebuffer and also adjust viewport size and offset
    CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
    
    
    CGL_widgets_begin();
    
    g_State.points_in_square += 1.0;
    g_State.pi_value = 4.0 * (g_State.points_in_circle / g_State.points_in_square);    
    if(CGL_vec2_length(curr_val) <= 1.0) {
        g_State.points_in_circle += 1.0;
    }

    if(g_State.num_points < MAX_POINTS) {
        g_State.points[g_State.num_points++] = curr_val;
    } else {
        // reset if we reach the max points
        g_State.num_points = 0;
    }

    for (CGL_sizei i = 0; i < g_State.num_points; i++) {
        if(CGL_vec2_length(g_State.points[i]) <= 1.0) {
            CGL_widgets_set_fill_colorf(0.0f, 0.8f, 0.0f, 1.0f);
        } else {
            CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.8f, 1.0f);
        }
        CGL_widgets_add_rect2f(g_State.points[i].x, g_State.points[i].y, g_State.point_size, g_State.point_size);
    }


    CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.0f, 1.0f);
    CGL_widgets_add_rect2f(-0.3f, -0.1f, 0.6f, 0.2f);
    
    static CGL_byte buffer[256]; sprintf(buffer, "%lf", g_State.pi_value);
    CGL_widgets_set_fill_colorf(0.7f, 0.7f, 0.7f, 1.0f);
    CGL_widgets_add_string(buffer, -0.3f, -0.1f, 0.6f, 0.2f);

    CGL_widgets_end();
    
    if(CGL_window_is_key_pressed(g_State.window, CGL_KEY_SPACE)) {
        g_State.points_in_circle = 1.0;
        g_State.points_in_square = 1.0;
        g_State.pi_value = 0.0;
        g_State.num_points = 0;
    }

    CGL_window_swap_buffers(g_State.window); // swap framebuffers
    CGL_window_poll_events(g_State.window); // poll events (if this is not called every frame window will stop responding)
    
    // quit on pressing escape
    if(CGL_window_get_key(g_State.window, CGL_KEY_ESCAPE) == CGL_PRESS) return CGL_FALSE;

    static CGL_byte title_buffer[256]; 
    sprintf(title_buffer, "Approximating PI - Jaysmito Mukherjee | FPS: %d", g_State.frame_rate);
    CGL_window_set_title(g_State.window, title_buffer);

    return CGL_TRUE;
}

int main()
{
    if(!init()) return EXIT_FAILURE;
#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
     // run till the close button is clicked
    while(!CGL_window_should_close(g_State.window))  {
        if(!loop(0, NULL)) break;
    }
    cleanup();
#endif
    return EXIT_SUCCESS;
}
