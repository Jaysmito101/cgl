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

static struct {
    float delta_x;
    float delta_y;
    float prev_x;
    float prev_y;
} mouse_input;


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

void dummy_payload_function(void* argument)
{
    static int invocation_id = 0;
    printf("Invocation %d :", invocation_id++);
    for(int i = 0 ; i < 10;i++)
    {
        Sleep(500);
        printf(" %d ", i);
    }
    printf("\n");
}

void spawn_the_thread(CGL_thread* thread)
{
    if(CGL_thread_is_running(thread))
        fprintf(stdout, "Backgound thread is working\n");
    else
        CGL_thread_start(thread, dummy_payload_function, NULL);
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

    CGL_thread* thread = CGL_thread_create();


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

        CGL_window_swap_buffers(main_window);
        if(CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS)
            spawn_the_thread(thread);
        mouse_input.delta_x = mouse_input.delta_y = 0.0f;
        if(CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }
    CGL_thread_destroy(thread);

    CGL_framebuffer_destroy(default_framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(main_window);
    CGL_shutdown();
    return 0;
}
