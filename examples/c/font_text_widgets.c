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
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_SKY_RENDERER
#define CGL_EXCLUDE_PHONG_RENDERER
#define CGL_EXCLUDE_TILEMAP_RENDERER
#define CGL_IMPLEMENTATION
#include "cgl.h"

int main()
{
     srand((uint32_t)time(NULL));
    if(!CGL_init()) return -1;
    CGL_window* main_window = CGL_window_create(640, 480, "Hello Font Widgets");
    if(!main_window) return -1;
    CGL_window_make_context_current(main_window);
    if(!CGL_gl_init()) return -1;
    CGL_widgets_init();
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(main_window);   
    if(!CGL_text_init()) return EXIT_FAILURE;
    // Load Font file
    CGL_font* font = CGL_font_load("font.otf"); // you can replace this with any otf/ttf font
    // Build the font atlas and bake font bitmaps
    if(!CGL_font_build_atlas(font, 1024, 1024, 48)) {CGL_LOG("Error in building font atlas\n"); return EXIT_FAILURE;}
    while(!CGL_window_should_close(main_window))
    { 
        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_widgets_begin();
        CGL_widgets_add_string_with_font("Jaysmito Mukherjee!", font, -0.7f, -0.5f, 1.4f);
        CGL_widgets_end();
        CGL_window_poll_events(main_window);
        CGL_window_swap_buffers(main_window);
        if(CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }
    CGL_font_destory(font);
    // CGL_texture_destroy(tex);
    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(main_window);
    CGL_text_shutdown();
    CGL_shutdown();
    return 0;
}
