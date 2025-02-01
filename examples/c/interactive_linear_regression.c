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
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_EXCLUDE_AUDIO
#define CGL_IMPLEMENTATION
#include "cgl.h"

#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif

#define POINT_RADIUS 0.02f
#define POINT_COUNT 1000
#define CURVE_SAMPLE_POINTS 1000

static struct
{
    CGL_vec2 points[POINT_COUNT]; // points to be used for linear regression
    CGL_window* window; // window object
    CGL_framebuffer* framebuffer; // framebuffer object
    CGL_int points_count; // number of points 
    CGL_int selected_point; // selected point for dragging and deleting
    CGL_linear_regression_context* lr_context; // linear regression context
    CGL_float curr_time;
    CGL_int frame_count;
} g_context; // global context

// this function will be provided to the linear regression context to sample the data
// this will be used to train the linear regression model using gradient descent
CGL_float sample_function(CGL_void* user_data, CGL_float* input, CGL_float* output, CGL_int id)
{ 
    (void)user_data; 
    (void)output;
    if(id < 0 || id >= g_context.points_count) return 0.0f; // invalid id
    memcpy(input, &g_context.points[id].x, sizeof(CGL_float)); // copy the x value of the point 
    // *input = g_context.points[id].x; // we can also do this here as we have only one input variable
    return g_context.points[id].y; // return the y value of the point
}

CGL_bool init()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return CGL_FALSE;
    
    g_context.window = CGL_window_create(700, 700, "Linear Regression - Jaysmito Mukherjee");
    if(!g_context.window) return CGL_FALSE;
    
    CGL_window_make_context_current(g_context.window);
    if(!CGL_gl_init()) return CGL_FALSE;
    
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window);
    g_context.points_count = 0;
    g_context.selected_point = -1;
    g_context.frame_count = 0;
    g_context.curr_time = CGL_utils_get_time();
    
    CGL_widgets_init();
    
    g_context.lr_context = CGL_linear_regression_context_create(1);
    CGL_linear_regression_randomize_coefficents(g_context.lr_context, -0.5f, 0.5f);
    
    return CGL_TRUE;
}

void cleanup()
{
    CGL_linear_regression_context_destroy(g_context.lr_context);
    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(g_context.framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(g_context.window);
    CGL_shutdown();
}

EM_BOOL loop(double time, void* userData)
{
    (void)time;
    (void)userData;
    
    g_context.frame_count++;
    g_context.curr_time = CGL_utils_get_time();
    
    CGL_window_set_size(g_context.window, 700, 700);
    CGL_framebuffer_bind(g_context.framebuffer);
    CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
    
    CGL_widgets_begin();
    
    // Draw axes
    CGL_widgets_set_stroke_thicnkess(0.01f);
    CGL_widgets_set_stroke_colorf(0.5f, 0.7f, 0.5f, 1.0f);
    CGL_widgets_add_line2f(-1.0f, 0.0f, 1.0f, 0.0f);
    CGL_widgets_add_line2f(0.0f, -1.0f, 0.0f, 1.0f);
    
    // Draw points
    CGL_widgets_set_fill_colorf(0.8f, 0.8f, 0.8f, 1.0f);
    for(CGL_int i = 0; i < g_context.points_count; i++) {
        CGL_vec2 p = g_context.points[i];
        CGL_widgets_add_circle2fr(p.x, p.y, POINT_RADIUS, 8);
    }
    
    // Draw regression curve
    CGL_widgets_set_stroke_colorf(0.7f, 0.5f, 0.5f, 1.0f);
    CGL_widgets_set_stroke_thicnkess(0.005f);
    CGL_vec2 prev_point = CGL_vec2_init(0.0f, 0.0f);
    
    for(CGL_int i = 0; i < CURVE_SAMPLE_POINTS + 10; i++)
    {
        CGL_float x = -1.0f + (2.0f / (CGL_float)CURVE_SAMPLE_POINTS) * (CGL_float)i;
        CGL_float y = CGL_linear_regression_evaluate(g_context.lr_context, &x, NULL);
        if(i % (CGL_int)(CURVE_SAMPLE_POINTS * 0.1f) == 0)
            CGL_widgets_add_circle2fr(x, y, POINT_RADIUS * 0.5f, 4);
        if(i > 0)
            CGL_widgets_add_line2f(prev_point.x, prev_point.y, x, y);
        prev_point = CGL_vec2_init(x, y);
    }
    
    CGL_widgets_end();
    
    // Handle input
    if(CGL_window_is_key_pressed(g_context.window, CGL_KEY_R))
        CGL_linear_regression_randomize_coefficents(g_context.lr_context, -0.5f, 0.5f);
    if(CGL_window_is_key_pressed(g_context.window, CGL_KEY_C))
        g_context.points_count = 0;
    
    // Handle mouse input
    CGL_bool mslft = (CGL_window_get_mouse_button(g_context.window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS);
    CGL_bool msrht = (CGL_window_get_mouse_button(g_context.window, CGL_MOUSE_BUTTON_RIGHT) == CGL_PRESS);
    
    if(mslft || msrht)
    {
        CGL_double mx = 0.0, my = 0.0; CGL_bool caught = false;
        CGL_window_get_mouse_position(g_context.window, &mx, &my);
        mx = (mx / 700.0) * 2.0 - 1.0; my = (1.0 - my / 700.0) * 2.0 - 1.0;
        g_context.selected_point = -1;
        for(CGL_int i = 0; i < g_context.points_count; i++)
        {
            CGL_vec2 p = g_context.points[i]; CGL_float tx = (CGL_float)(p.x - mx), ty = (CGL_float)(p.y - my);
            if(sqrtf(tx * tx + ty * ty) < POINT_RADIUS) { g_context.selected_point = i; caught = true; break;}
        }
        if(caught)
        {
            if(mslft) {g_context.points[g_context.selected_point].x = (CGL_float)mx; g_context.points[g_context.selected_point].y = (CGL_float)my;}
            if(msrht) {for(CGL_int i = g_context.selected_point; i < g_context.points_count - 1; i++)g_context.points[i] = g_context.points[i + 1];g_context.points_count--;}
        }
        else if(mslft)
        {
            g_context.points[g_context.points_count].x = (CGL_float)mx; g_context.points[g_context.points_count].y = (CGL_float)my;
            g_context.selected_point = g_context.points_count; g_context.points_count++;
        }
    }
    
    if(g_context.points_count > 0)
        CGL_linear_regression_train(g_context.lr_context, sample_function, NULL, g_context.points_count, 0.01f, 10);
    
    CGL_window_swap_buffers(g_context.window);
    CGL_window_poll_events(g_context.window);
    
    return !CGL_window_should_close(g_context.window);
}

int main()
{
    if(!init()) return EXIT_FAILURE;

#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while (!CGL_window_should_close(g_context.window)) {
        loop(0, NULL);
    }
    cleanup();
#endif

    return EXIT_SUCCESS;
}
