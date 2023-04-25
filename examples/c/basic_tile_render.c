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

#define MAIN_FRAME_BUFFER_WIDTH  800
#define MAIN_FRAME_BUFFER_HEIGHT (int)(MAIN_FRAME_BUFFER_WIDTH * (9.0f / 16.0f))

static struct {
    float delta_x;
    float delta_y;
    float prev_x;
    float prev_y;
} mouse_input;

static struct
{
    float scale_x;
    float scale_y;
    float offset_x;
    float offset_y;
    CGL_tilemap* tilemap;
} tilemap_data;

#define TILE_COUNT_X 16
#define TILE_COUNT_Y 16

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

void randomize_tilemap()
{
    for(int i = 0 ; i < TILE_COUNT_X; i++)
        for(int j = 0 ; j < TILE_COUNT_Y; j++)
            if(CGL_utils_random_float() > 0.5f)
                CGL_tilemap_set_tile_color(tilemap_data.tilemap, i, j, CGL_utils_random_float(), CGL_utils_random_float(), CGL_utils_random_float());
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
    tilemap_data.offset_x = 0.0f;
    tilemap_data.offset_y = 0.0f;
    tilemap_data.scale_x = 1.0f;
    tilemap_data.scale_y = 1.0f;
    CGL_window_set_mouse_scroll_callback(main_window, input_scroll_callback);
    CGL_window_set_mouse_position_callback(main_window, input_mouse_pos_callback);
    CGL_window_resecure_callbacks(main_window);
    tilemap_data.tilemap = CGL_tilemap_create(TILE_COUNT_X, TILE_COUNT_Y, 32, 32, 0);
    randomize_tilemap();            
    while(!CGL_window_should_close(main_window))
    { 
        {
            CGL_framebuffer_bind(default_framebuffer);
            CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
            int rx = 0, ry = 0;
            CGL_framebuffer_get_size(default_framebuffer, &rx, &ry);
            CGL_tilemap_render(tilemap_data.tilemap, tilemap_data.scale_x, tilemap_data.scale_y, tilemap_data.offset_x, tilemap_data.offset_y, NULL);
        }

        CGL_window_poll_events(main_window);

        CGL_window_swap_buffers(main_window);
        if(CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS || CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_RIGHT) == CGL_PRESS || CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_MIDDLE) == CGL_PRESS)
            randomize_tilemap();            
        mouse_input.delta_x = mouse_input.delta_y = 0.0f;
        if(CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }

    CGL_tilemap_destroy(tilemap_data.tilemap);

    CGL_framebuffer_destroy(default_framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(main_window);
    CGL_shutdown();
    return 0;
}
