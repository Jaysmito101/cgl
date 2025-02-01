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

#define POINT_RADIUS 0.05f
#define POINT_COUNT 1000

#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif

static struct
{
    CGL_vec2 points[POINT_COUNT];
    CGL_int triangles[POINT_COUNT * 3];
    CGL_window* window;
    CGL_framebuffer* framebuffer;
    CGL_int window_height;
    CGL_int window_width;
    CGL_int points_count;
    CGL_int triangle_count;
    CGL_int selected_point;
} g_context;

CGL_bool init()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return CGL_FALSE;
    g_context.window_width = g_context.window_height = 700;    
    g_context.window = CGL_window_create(g_context.window_width, g_context.window_height, "Triangulation Demo - Jaysmito Mukherjee");
    CGL_window_make_context_current(g_context.window);
    if(!CGL_gl_init()) return CGL_FALSE;   
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window);
    g_context.triangle_count = 0;
    g_context.points_count = 0;
    g_context.selected_point = -1;
    CGL_widgets_init();
    return CGL_TRUE;
}

void cleanup()
{
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

    CGL_window_set_size(g_context.window, g_context.window_width, g_context.window_height);

    CGL_framebuffer_bind(g_context.framebuffer);
    CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);

    CGL_widgets_begin();
    CGL_widgets_set_fill_colorf(0.8f, 0.8f, 0.8f, 1.0f);
    for(CGL_int i = 0; i < g_context.points_count; i++) {
        CGL_vec2 p = g_context.points[i];
        CGL_widgets_add_circle2fr(p.x, p.y, POINT_RADIUS, 8);
    }

    CGL_widgets_set_stroke_thicnkess(0.01f);
    CGL_widgets_set_stroke_colorf(0.5f, 0.5f, 0.5f, 1.0f);

    for(CGL_int i = 0 ; i < g_context.triangle_count;i++) {
        CGL_vec2 p1 = g_context.points[g_context.triangles[i * 3 + 0]];
        CGL_vec2 p2 = g_context.points[g_context.triangles[i * 3 + 1]];
        CGL_vec2 p3 = g_context.points[g_context.triangles[i * 3 + 2]];
        CGL_widgets_add_line2f(p1.x, p1.y, p2.x, p2.y);
        CGL_widgets_add_line2f(p2.x, p2.y, p3.x, p3.y);
        CGL_widgets_add_line2f(p3.x, p3.y, p1.x, p1.y);
    }

    CGL_widgets_set_stroke_colorf(0.5f, 0.3f, 0.3f, 1.0f);
    CGL_triangulate_points_incremental(g_context.points, g_context.points_count, g_context.triangles, &g_context.triangle_count);

    CGL_widgets_end();        

    CGL_bool mslft = (CGL_window_get_mouse_button(g_context.window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS);
    CGL_bool msrht = (CGL_window_get_mouse_button(g_context.window, CGL_MOUSE_BUTTON_RIGHT) == CGL_PRESS);
    if(mslft || msrht)
    {
        CGL_double mx = 0.0, my = 0.0; CGL_bool caught = false;
        CGL_window_get_mouse_position(g_context.window, &mx, &my);
        mx = (mx / (CGL_double)g_context.window_width) * 2.0 - 1.0;
        my = (1.0 - my / (CGL_double)g_context.window_height) * 2.0 - 1.0;
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

    CGL_window_swap_buffers(g_context.window);
    CGL_window_poll_events(g_context.window);

    return !CGL_window_should_close(g_context.window);
}

int main()
{
    if(!init()) return EXIT_FAILURE;

#ifdef CGL_WASM
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while (!CGL_window_should_close(g_context.window)) {
        loop(0, NULL);
    }
    cleanup();
#endif
    return EXIT_SUCCESS;
}
