/*
MIT License

Copyright (c) 2022 Jaysmito Mukherjee (jaysmito101@gmail.com)

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
#define CGL_IMPLEMENTATION
#include "cgl.h"


int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(640, 320, "CGL Text Widgets - Jaysmito Mukherjee");
    CGL_window_make_context_current(window); 
    CGL_gl_init();
    CGL_widgets_init();
    if(window == NULL) return false; 
    CGL_framebuffer* framebuffer = CGL_framebuffer_create_from_default(window);


    while(!CGL_window_should_close(window)) 
    {
        CGL_framebuffer_bind(framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);

        CGL_widgets_begin();
        CGL_widgets_add_string("C G L", -0.5f, -0.5f, 1.0f, 1.0f);
        CGL_widgets_end();

        CGL_window_swap_buffers(window);
        CGL_window_poll_events(window);
    }

    
    CGL_framebuffer_destroy(framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
    return EXIT_SUCCESS;
}
