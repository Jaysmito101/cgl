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
    CGL_double scale_xy, offset_x, offset_y;
} g_State;

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
    CGL_dvec2 next_point = CGL_dvec2_init(0.0, 0.0);
    CGL_sizei index = CGL_utils_get_random_with_probability(probabilities, 4);
    next_point.x = rotation_matrices[index][0] * prev_point.x + rotation_matrices[index][1] * prev_point.y + translation_vectors[index][0];
    next_point.y = rotation_matrices[index][2] * prev_point.x + rotation_matrices[index][3] * prev_point.y + translation_vectors[index][1];
    return next_point;    
}

CGL_bool init() {
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return CGL_FALSE;
    g_State.window_height = g_State.window_width = 700;
    g_State.window = CGL_window_create(g_State.window_width, g_State.window_height, "Barnsley Fern - Jaysmito Mukherjee"); // create the window
    CGL_window_make_context_current(g_State.window); // make the opengl context for the window current
    if(!CGL_gl_init()) return CGL_FALSE; // initialize the opengl functions
    g_State.framebuffer = CGL_framebuffer_create_from_default(g_State.window); // load the default framebuffer (0) into CGL_framebuffer object
    if(!CGL_widgets_init()) return CGL_FALSE;



    // These values are for the best view of the fern
    g_State.scale_xy = 0.179262f;
    g_State.offset_x = 0.0f;
    g_State.offset_y = -0.947767;

    return CGL_TRUE;
}

CGL_void cleanup() {
    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(g_State.framebuffer); // destory framebuffer object
    CGL_gl_shutdown(); // shutdown cgl opengl module
    CGL_window_destroy(g_State.window); // destroy window
    CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
}

EM_BOOL loop(double time, void* userData) {
    (void)time;
    (void)userData;

    const CGL_float point_size = 0.01f;
    static CGL_float prev_time = 0.0f, curr_time = 0.0f, delta_time = 0.0f, frame_time = 0.0f;
    static CGL_int frame_count = 0, frame_rate = 60;
    static CGL_dvec2 turtle_position = {0.0f, 0.0f};


    curr_time = CGL_utils_get_time();
    delta_time = curr_time - prev_time; prev_time = curr_time; frame_time += delta_time; frame_count++;
    if(frame_time >= 1.0f) { frame_rate = frame_count; frame_time = 0.0f; frame_count = 0; }

    CGL_window_set_size(g_State.window, g_State.window_width, g_State.window_height); // set the window size
    // rendering
    CGL_framebuffer_bind(g_State.framebuffer); // bind default framebuffer and also adjust viewport size and offset
    //CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f); // clear screen with a dark gray color
    CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);

    CGL_widgets_begin_int((CGL_float)g_State.scale_xy, (CGL_float)g_State.scale_xy, (CGL_float)g_State.offset_x, (CGL_float)g_State.offset_y);

    CGL_widgets_set_fill_colorf(0.0f, 0.8f, 0.0f, 1.0f);

    for(CGL_sizei i = 0; i < 50000; i++)
    {
        turtle_position = next_point_barnsley(turtle_position);        
        CGL_widgets_add_rect2f((CGL_float)turtle_position.x, (CGL_float)turtle_position.y, point_size, point_size);
    }
        
    CGL_widgets_end();       

    CGL_widgets_begin();
    CGL_widgets_add_string("Arrow keys to move", -0.9f, 0.9f, 0.8f, 0.05f);
    CGL_widgets_add_string("W/S to zoom in/out", -0.9f, 0.85f, 0.8f, 0.05f);
    CGL_widgets_end();       

    if(CGL_window_is_key_pressed(g_State.window, CGL_KEY_W)) { g_State.scale_xy *= 1.01f; }
    if(CGL_window_is_key_pressed(g_State.window, CGL_KEY_S)) { g_State.scale_xy *= 0.99f; } 
    if(CGL_window_is_key_pressed(g_State.window, CGL_KEY_LEFT)) { g_State.offset_x -= 2.0f * delta_time; }
    if(CGL_window_is_key_pressed(g_State.window, CGL_KEY_RIGHT)) { g_State.offset_x += 2.0f * delta_time; }
    if(CGL_window_is_key_pressed(g_State.window, CGL_KEY_UP)) { g_State.offset_y += 2.0f * delta_time; } 
    if(CGL_window_is_key_pressed(g_State.window, CGL_KEY_DOWN)) { g_State.offset_y -= 2.0f * delta_time; }

    CGL_window_swap_buffers(g_State.window); // swap framebuffers
    CGL_window_poll_events(g_State.window); // poll events (if this is not called every frame window will stop responding)

    if(CGL_window_get_key(g_State.window, CGL_KEY_ESCAPE) == CGL_PRESS) return CGL_FALSE;

    static CGL_byte title[256];
    sprintf(title, "Barnsley Fern - Jaysmito Mukherjee | FPS: %d", frame_rate);
    CGL_window_set_title(g_State.window, title);

    return CGL_TRUE;
}

int main()
{
    if(!init()) return EXIT_FAILURE;
#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while(!CGL_window_should_close(g_State.window)) // run till the close button is clicked
    {
        if(!loop(0.0, NULL)) break; 
    }
    cleanup();
#endif
    return EXIT_SUCCESS;
}
