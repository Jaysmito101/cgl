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
} g_context;

CGL_float function_to_plot_0(CGL_float x)
{
    x += CGL_utils_get_time();
    return sinf(x) + sinf(x * 2) * 0.5f + sinf(x * 4) * 0.25f + sinf(x * 8) * 0.125f;
}

CGL_float function_to_plot_1(CGL_float x)
{
    return fabsf(sinf(powf(x, x) / powf(2.0f, powf(x, x) - CGL_PI_2)/CGL_PI));
}

CGL_float function_to_plot_2(CGL_float x)
{
    return x - floorf(x);
}

CGL_float function_to_plot_3(CGL_float x)
{
    x += CGL_utils_get_time();
    return (sinf(x) + cosf(x * 3.0f)) / 2.0f;
}

void draw_function_plot_widgets()
{
    CGL_widgets_add_plot_function(
        -1.0f, 0.0f, 0.9f, 0.9f, // position and size
        function_to_plot_0, // function to plot
        256, // number of points to plot (more points = more accurate/smooth plot)
        -5.0f, 5.0f, // x range
        -1.5f, 1.5f, // y range
        0.005f, CGL_vec3_init(1.0f, 0.0f, 0.0f), // plot line width and color
        true, 0.05f, CGL_vec3_init(0.0f, 1.0f, 0.0f) // axes line width and color
    );

    CGL_widgets_add_plot_function(
        0.0f, 0.0f, 0.9f, 0.9f, // position and size
        function_to_plot_1, // function to plot
        256, // number of points to plot (more points = more accurate/smooth plot)
        -0.5f, 4.0f, // x range
        -0.5f, 1.0f, // y range
        0.005f, CGL_vec3_init(1.0f, 0.0f, 0.0f), // plot line width and color
        true, 0.05f, CGL_vec3_init(0.0f, 1.0f, 0.0f) // axes line width and color
    );

    CGL_widgets_add_plot_function(
        -1.0f, -1.0f, 0.9f, 0.9f, // position and size
        function_to_plot_2, // function to plot
        256, // number of points to plot (more points = more accurate/smooth plot)
        -5.0f, 5.0f, // x range
        -2.0f, 2.0f, // y range
        0.005f, CGL_vec3_init(1.0f, 0.0f, 0.0f), // plot line width and color
        true, 0.05f, CGL_vec3_init(0.0f, 1.0f, 0.0f) // axes line width and color
    );

    CGL_widgets_add_plot_function(
        0.0f, -1.0f, 0.9f, 0.9f, // position and size
        function_to_plot_3, // function to plot
        256, // number of points to plot (more points = more accurate/smooth plot)
        -5.0f, 5.0f, // x range
        -1.5f, 1.5f, // y range
        0.005f, CGL_vec3_init(1.0f, 0.0f, 0.0f), // plot line width and color
        true, 0.05f, CGL_vec3_init(0.0f, 1.0f, 0.0f) // axes line width and color
    );
}

void draw_array_plot_widgets()
{
    srand(0);
    CGL_vec2 points[64];
    for(int i = 0; i < 64; i++)
    {
        points[i] = CGL_vec2_init(
            i / 64.0f * 10.0f - 2.5f,
            CGL_utils_random_float() * 4.0f - 1.0f
        );
    }

    CGL_widgets_add_plot_array(
        -1.0f, -1.0f, 2.0f, 2.0f, // position and size
        points, 64, // points
        0.03f, CGL_vec3_init(1.0f, 0.0f, 0.0f), // marker size and color
        true, 0.01f, CGL_vec3_init(0.0f, 1.0f, 0.0f) // axes line width and color
    );
}

void draw_pie_chart_plot_widgets()
{
    srand(0);
    CGL_float values[8];
    CGL_vec3 colors[8];
    for(CGL_int i = 0; i < 8; i++)
    {
        values[i] = CGL_utils_random_float() * 100.0f / (i + 1);
        colors[i] = CGL_vec3_init(
            CGL_utils_random_float(),
            CGL_utils_random_float(),
            CGL_utils_random_float()
        );
    }

    CGL_widgets_add_plot_pie_chart(
        0.0f, 0.0f, 1.0f, // position and size
        values, colors, 8, // values and colors
        64
    );
}

void draw_bar_plot_widgets()
{
    srand(1);
    CGL_float values[8];
    CGL_vec3 colors[8];
    for(CGL_int i = 0; i < 8; i++)
    {
        values[i] = CGL_utils_random_float() * 100.0f / (i + 1) + 10.0f;
        colors[i] = CGL_vec3_init(CGL_utils_random_float(), CGL_utils_random_float(), CGL_utils_random_float());
    }

    CGL_widgets_add_bar_graph(
        -1.0f, 0.0f, 0.9f, 0.9f, 
        values, colors, 8,
        true, 0.01f, CGL_vec3_init(0.0f, 1.0f, 0.0f),
        true
    );

    CGL_widgets_add_bar_graph(
        0.0f, 0.0f, 0.9f, 0.9f, 
        values, colors, 8,
        true, 0.01f, CGL_vec3_init(0.0f, 1.0f, 0.0f),
        false
    );

    for(CGL_int i = 0; i < 8; i++)
    {
        values[i] = CGL_utils_random_float() * 100.0f / (i + 1) + 10.0f;
        colors[i] = CGL_vec3_init(CGL_utils_random_float(), CGL_utils_random_float(), CGL_utils_random_float());
    }

    CGL_widgets_add_bar_graph(
        -1.0f, -1.0f, 0.9f, 0.9f, 
        values, colors, 8,
        true, 0.01f, CGL_vec3_init(0.0f, 1.0f, 0.0f),
        true
    );

    CGL_widgets_add_bar_graph(
        0.0f, -1.0f, 0.9f, 0.9f, 
        values, colors, 8,
        true, 0.01f, CGL_vec3_init(0.0f, 1.0f, 0.0f),
        false
    );

}

CGL_bool init() {
    
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return CGL_FALSE;
    g_context.window_height = 600;
    g_context.window_width = 600;
    g_context.window = CGL_window_create(g_context.window_width, g_context.window_height, "Plotting - Jaysmito Mukherjee"); // create the window
    if(g_context.window == NULL) return CGL_FALSE; // window creation failed
    CGL_window_make_context_current(g_context.window); // make the opengl context for the window current
    if(!CGL_gl_init()) return CGL_FALSE; // initialize cgl opengl module    
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window); // load the default framebuffer (0) into CGL_framebuffer object
    if(g_context.framebuffer == NULL) return CGL_FALSE; // failed
    if(!CGL_widgets_init()) return CGL_FALSE; 

    return CGL_TRUE;
}

CGL_void cleanup() {
    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(g_context.framebuffer); // destory framebuffer object
    CGL_gl_shutdown(); // shutdown cgl opengl module
    CGL_window_destroy(g_context.window); // destroy window
    CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
}

EM_BOOL loop(double time, void* userData) {
    (void)time;
    (void)userData;

    static CGL_int mode = 1;

    CGL_window_set_size(g_context.window, g_context.window_width, g_context.window_height); // set window size
        // rendering
    CGL_framebuffer_bind(g_context.framebuffer); // bind default framebuffer and also adjust viewport size and offset
    CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f); // clear screen with a dark gray color

    CGL_widgets_begin();

        
    if(mode == 1) draw_function_plot_widgets();
    else if(mode == 2) draw_array_plot_widgets();
    else if(mode == 3) draw_pie_chart_plot_widgets();
    else if(mode == 4) draw_bar_plot_widgets();

    CGL_widgets_end();

    CGL_window_swap_buffers(g_context.window); // swap framebuffers
    CGL_window_poll_events(g_context.window); // poll events (if this is not called every frame window will stop responding)



    if(CGL_window_get_key(g_context.window, CGL_KEY_ESCAPE) == CGL_PRESS) return CGL_FALSE;
    for(CGL_int i = 1 ; i < 9 ; i++) if(CGL_window_get_key(g_context.window, CGL_KEY_0 + i) == CGL_PRESS) mode = i;

    return CGL_TRUE;
}

int main()
{
    if (!init()) return EXIT_FAILURE;
#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    // run till the close button is clicked
    while(!CGL_window_should_close(g_context.window)) {
        if(!loop(0, NULL)) break; // run the loop function
    }
    cleanup();
#endif    
    return EXIT_SUCCESS;
}
