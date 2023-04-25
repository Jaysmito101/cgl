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
#define CGL_IMPLEMENTATION
#include "cgl.h"


#pragma warning(push, 0)
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "nuklear_glfw_gl4.h"
#pragma warning(pop)


static struct {
    float delta_x;
    float delta_y;
    float prev_x;
    float prev_y;
} mouse_input;


static struct {
    struct nk_context* ctx;
    struct nk_colorf bg;
    struct nk_glfw glfw;
} nuklear_data;


void input_scroll_callback(CGL_window* window, double x, double y)
{
}

void input_mouse_pos_callback(CGL_window* window, double x, double y)
{
    mouse_input.delta_x = ((float)x - mouse_input.prev_x);
    mouse_input.delta_y = ((float)y - mouse_input.prev_y);
    mouse_input.prev_x = (float)x;
    mouse_input.prev_y = (float)y;
}


int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return -1;
    CGL_window* main_window = CGL_window_create(640, 480, "Hello World");
    if(!main_window) return -1;
    CGL_window_make_context_current(main_window);
    if(!CGL_gl_init()) return -1;
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(main_window);   
    mouse_input.delta_x = 0.0f;
    mouse_input.delta_y = 0.0f;
    mouse_input.prev_x = 0.0f;
    mouse_input.prev_y = 0.0f;
    CGL_window_set_mouse_scroll_callback(main_window, input_scroll_callback);
    CGL_window_set_mouse_position_callback(main_window, input_mouse_pos_callback);
    CGL_window_resecure_callbacks(main_window);

    // setup nuklear
    nuklear_data.ctx = nk_glfw3_init(&nuklear_data.glfw, CGL_window_get_glfw_handle(main_window), NK_GLFW3_INSTALL_CALLBACKS);

    {
        struct nk_font_atlas* atlas;
        nk_glfw3_font_stash_begin(&nuklear_data.glfw, &atlas);
        nk_glfw3_font_stash_end(&nuklear_data.glfw);
    }

    while(!CGL_window_should_close(main_window))
    { 
        {
            CGL_framebuffer_bind(default_framebuffer);
            CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
            int rx = 0, ry = 0;
            CGL_framebuffer_get_size(default_framebuffer, &rx, &ry);
            // render here
        }

        CGL_window_poll_events(main_window);

        nk_glfw3_new_frame(&nuklear_data.glfw);

         if (nk_begin(nuklear_data.ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            nk_layout_row_static(nuklear_data.ctx, 30, 80, 1);
            if (nk_button_label(nuklear_data.ctx, "button"))
            {
                CGL_LOG("Hello World!\n");
            }


        }
        nk_end(nuklear_data.ctx);
        nk_glfw3_render(&nuklear_data.glfw, NK_ANTI_ALIASING_ON, 1024 * 512, 1024 * 128);


        CGL_window_swap_buffers(main_window);
        if(CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_MIDDLE) == CGL_PRESS)
        {            
        }
        else if(CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_RIGHT) == CGL_PRESS)
        {

        }
        mouse_input.delta_x = mouse_input.delta_y = 0.0f;
        if(CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }

    nk_glfw3_shutdown(&nuklear_data.glfw);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(main_window);
    CGL_shutdown();
    return 0;
}
