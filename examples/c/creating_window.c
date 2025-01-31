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


#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#include <stdio.h>

#define CGL_EXCLUDE_AUDIO
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_TEXT_RENDER
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
    CGL_framebuffer* framebuffer;
} g_State;

void input_mouse_pos_callback(CGL_window* window, double x, double y)
{
    (void)window;    
    CGL_info("Mouse pos: %f, %f", x, y);
}

void input_mouse_button_callback(CGL_window* window, CGL_int button, CGL_int action, CGL_int mods)
{
    (void)window;
    CGL_info("Mouse button: %d, %d, %d", button, action, mods);
}


CGL_bool init() {
    srand((CGL_uint)time(NULL));
    if(!CGL_init()) return CGL_FALSE;

    g_State.window = CGL_window_create(640, 480, "Hello, World!");
    CGL_window_make_context_current(g_State.window);  
    if(!CGL_gl_init()) return CGL_FALSE;

    CGL_window_set_mouse_position_callback(g_State.window, input_mouse_pos_callback);
    CGL_window_set_mouse_button_callback(g_State.window, input_mouse_button_callback);  


    g_State.framebuffer = CGL_framebuffer_create_from_default(g_State.window);
    return true;
}

void cleanup() {
    CGL_framebuffer_destroy(g_State.framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(g_State.window);
    CGL_shutdown();
}

EM_BOOL loop(double time, void* userData) {
    (void)time;
    (void)userData;

    CGL_window_poll_events(g_State.window);

    CGL_framebuffer_bind(g_State.framebuffer);
    CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);

    CGL_window_swap_buffers(g_State.window);    
    return true;
}

EMSCRIPTEN_KEEPALIVE
int main() {    
    if(!init()) return 1;

#ifdef CGL_WASM        
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while (!CGL_window_should_close(g_State.window)) {
        loop(0, NULL);
    }
    cleanup();
#endif
    return 0;
}
