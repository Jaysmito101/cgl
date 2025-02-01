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

#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_AUDIO
#define CGL_IMPLEMENTATION
#include "cgl.h"

#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif

struct {
    CGL_window* window;
    CGL_framebuffer* default_framebuffer;
    CGL_int selected_point;
    CGL_int line_resolution;
    CGL_vec2 points[4];
    CGL_float aspect_ratio;
} g_State;

CGL_bool init() {
    srand((CGL_uint)time(NULL));
    if(!CGL_init()) return CGL_FALSE;
    g_State.window = CGL_window_create(512, 512, "CGL Text Widgets - Jaysmito Mukherjee");
    CGL_window_make_context_current(g_State.window); 
    CGL_gl_init();
    CGL_widgets_init();
    if(g_State.window == NULL) return false; 
    g_State.default_framebuffer = CGL_framebuffer_create_from_default(g_State.window);

    g_State.selected_point = 0;
    g_State.line_resolution = 3;
    g_State.points[0] = CGL_vec2_init(-0.8f, 0.0f);
    g_State.points[1] = CGL_vec2_init(0.8f, 0.0f);
    g_State.points[2] = CGL_vec2_init(0.0f, 0.5f);
    g_State.points[3] = CGL_vec2_init(0.0f, -0.5f);

    g_State.aspect_ratio = 16.0f / 9.0f;
    
    return CGL_TRUE;
}

CGL_void cleanup() {
    CGL_framebuffer_destroy(g_State.default_framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(g_State.window);
    CGL_shutdown();
}

EM_BOOL loop(double time, void* userData) {
    (void)time;
    (void)userData;
    
    static CGL_int wx = 0, wy = 0;
    static CGL_byte buffer[25];
    static CGL_int frame_number = 0;

    frame_number++;
    CGL_window_set_size(g_State.window, 512, 512);
    CGL_window_get_framebuffer_size(g_State.window, &wx, &wy);
    g_State.aspect_ratio = (CGL_float)wx / (CGL_float)wy;

    CGL_framebuffer_bind(g_State.default_framebuffer);
    CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);

    CGL_widgets_begin();

    for(CGL_int i = 0; i < 4; i++) {
        CGL_widgets_set_fill_colorf(0.9f, 0.8f, 0.2f, 1.0f);
        if(i == g_State.selected_point) CGL_widgets_add_circle2f(g_State.points[i].x, g_State.points[i].y, 0.06f);
        if(i < 2) CGL_widgets_set_fill_colorf(0.2f, 0.8f, 0.2f, 1.0f);
        else CGL_widgets_set_fill_colorf(0.8f, 0.2f, 0.2f, 1.0f);
        CGL_widgets_add_circle2f(g_State.points[i].x, g_State.points[i].y, 0.05f);
    }

    CGL_widgets_set_stroke_colorf(0.8f, 0.7f, 0.3f, 1.0f);
    CGL_widgets_set_stroke_thicnkess(0.01f);
    CGL_widgets_add_cubic_bazier2v(g_State.points[0], g_State.points[1], g_State.points[2], g_State.points[3], g_State.line_resolution);
    CGL_widgets_add_cubic_bazier_points2v(g_State.points[0], g_State.points[1], g_State.points[2], g_State.points[3], g_State.line_resolution);

    CGL_widgets_set_fill_colorf(0.8f, 0.8f, 0.8f, 1.0f);
    sprintf(buffer, "Line Resolution: %d", g_State.line_resolution);
    CGL_widgets_add_string(buffer, -1.0f, 0.9f, 1.0f, 0.1f);

    static CGL_int old_frame_number = 0;
    static CGL_int delta = 0;
    delta = 0;
    if(CGL_window_get_key(g_State.window, CGL_KEY_UP) == CGL_PRESS) delta = 1;
    if(CGL_window_get_key(g_State.window, CGL_KEY_DOWN) == CGL_PRESS) delta = -1;
    if(delta != 0 && (frame_number - old_frame_number) > 30) {
        old_frame_number = frame_number;
        g_State.line_resolution += delta;
        g_State.line_resolution = CGL_utils_clamp(g_State.line_resolution, 3, 100);
    }

    double mpx, mpy;
    CGL_window_get_mouse_position(g_State.window, &mpx, &mpy);
    mpx = mpx / (double)wx * 2.0 - 1.0;
    mpy = 2.0f - mpy / (double)wy * 2.0 - 1.0;
    g_State.selected_point = -1;
    for(CGL_int i = 0 ; i < 4 ; i++) {
        if(sqrt(pow(mpx - g_State.points[i].x, 2) + pow(mpy - g_State.points[i].y, 2)) < 0.05f) {
            g_State.selected_point = i;            
        }
    }

    if(g_State.selected_point != -1) {
        if(CGL_window_get_mouse_button(g_State.window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS) {
            g_State.points[g_State.selected_point].x = (CGL_float)mpx;
            g_State.points[g_State.selected_point].y = (CGL_float)mpy;
        }
    }


    CGL_widgets_end();

    CGL_window_swap_buffers(g_State.window);
    CGL_window_poll_events(g_State.window);    

    return CGL_TRUE;
}

int main()
{
    if(!init()) return EXIT_FAILURE;
#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while(!CGL_window_should_close(g_State.window)) {
        if (!loop(0.0, NULL)) break;
    }
    cleanup();
#endif
    return EXIT_SUCCESS;
}
