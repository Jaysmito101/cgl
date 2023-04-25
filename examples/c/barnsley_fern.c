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
#define CGL_IMPLEMENTATION
#include "cgl.h"

typedef struct
{
    CGL_double x;
    CGL_double y;
} CGL_dvec2;

#define CGL_dvec2_init(x, y) (CGL_dvec2){x, y}

static struct
{
    CGL_window* window; // the main window
    CGL_int window_height;
    CGL_int window_width;
    CGL_framebuffer* framebuffer;
} g_context;

CGL_double rotation_matrices[4][4] = {
    {0.0, 0.0, 0.0, 0.16},
    {0.85, 0.04, -0.04, 0.85},
    {0.2, -0.26, 0.23, 0.22},
    {-0.15, 0.28, 0.26, 0.24}
};

CGL_double translation_vectors[4][2] = {
    {0.0, 0.0},
    {0.0, 1.6},
    {0.0, 1.6},
    {0.0, 0.44}
};

CGL_float probabilities[4] = {0.01f, 0.85f, 0.07f, 0.07f};

CGL_dvec2 next_point_barnsley(CGL_dvec2 prev_point)
{
    CGL_float random = CGL_utils_random_float();
    CGL_dvec2 next_point = CGL_dvec2_init(0.0, 0.0);
    CGL_sizei index = CGL_utils_get_random_with_probability(probabilities, 4);
    next_point.x = rotation_matrices[index][0] * prev_point.x + rotation_matrices[index][1] * prev_point.y + translation_vectors[index][0];
    next_point.y = rotation_matrices[index][2] * prev_point.x + rotation_matrices[index][3] * prev_point.y + translation_vectors[index][1];
    return next_point;    
}

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return EXIT_FAILURE; // initialize CGL (required for setting up internals of CGL)
    g_context.window_height = g_context.window_width = 700;
    g_context.window = CGL_window_create(g_context.window_width, g_context.window_height, "Barnsley Fern - Jaysmito Mukherjee"); // create the window
    CGL_window_make_context_current(g_context.window); // make the opengl context for the window current
    if(!CGL_gl_init()) return EXIT_FAILURE; // initialize cgl opengl module    
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window); // load the default framebuffer (0) into CGL_framebuffer object
    if(!CGL_widgets_init()) return EXIT_FAILURE; 

    CGL_float prev_time = CGL_utils_get_time(), curr_time = 0.0f, delta_time = 0.0f, frame_time = 0.0f;
    CGL_int frame_count = 0, frame_rate = 60;

    CGL_dvec2 turtle_position = CGL_dvec2_init(0.0f, 0.0f);
    CGL_float point_size = 0.01f;

    CGL_double scale_xy = 1.0, offset_x = 0.0, offset_y = 0.0;

    // the main loop
    while(!CGL_window_should_close(g_context.window)) // run till the close button is clicked
    {
        curr_time = CGL_utils_get_time();
        delta_time = curr_time - prev_time; prev_time = curr_time; frame_time += delta_time; frame_count++;
        if(frame_time >= 1.0f) { frame_rate = frame_count; frame_time = 0.0f; frame_count = 0; }

        CGL_window_set_size(g_context.window, g_context.window_width, g_context.window_height); // set the window size
        // rendering
        CGL_framebuffer_bind(g_context.framebuffer); // bind default framebuffer and also adjust viewport size and offset
        //CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f); // clear screen with a dark gray color


        CGL_widgets_begin_int((CGL_float)scale_xy, (CGL_float)scale_xy, (CGL_float)offset_x, (CGL_float)offset_y);

        CGL_widgets_set_fill_colorf(0.0f, 0.8f, 0.0f, 1.0f);

        for(CGL_sizei i = 0; i < 20000; i++)
        {
            turtle_position = next_point_barnsley(turtle_position);        
            CGL_widgets_add_rect2f((CGL_float)turtle_position.x, (CGL_float)turtle_position.y, point_size, point_size);
        }
        
        CGL_widgets_end();
        


        CGL_bool tmp = false, tmp2 = false;
        if(tmp = CGL_window_is_key_pressed(g_context.window, CGL_KEY_W)) scale_xy *= 1.01; tmp2 |= tmp;
        if(tmp = CGL_window_is_key_pressed(g_context.window, CGL_KEY_S)) scale_xy *= 0.99; tmp2 |= tmp;
        if(tmp = CGL_window_is_key_pressed(g_context.window, CGL_KEY_LEFT)) offset_x -= 0.005; tmp2 |= tmp;
        if(tmp = CGL_window_is_key_pressed(g_context.window, CGL_KEY_RIGHT)) offset_x += 0.005; tmp2 |= tmp;
        if(tmp = CGL_window_is_key_pressed(g_context.window, CGL_KEY_UP)) offset_y += 0.005; tmp2 |= tmp;
        if(tmp = CGL_window_is_key_pressed(g_context.window, CGL_KEY_DOWN)) offset_y -= 0.005; tmp2 |= tmp;
        if(tmp2) CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f); // clear screen with a dark gray color

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
