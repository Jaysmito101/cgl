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

#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#include "cgl.h"

static CGL_byte text_buffer[1024];
static CGL_int current_text_cursor = 0;
static CGL_int max_text_length = (CGL_int)(1.6f / 0.05f);

void key_callback(CGL_window* window, CGL_int key, CGL_int scancode, CGL_int action, CGL_int mods)
{
    if(action == CGL_RELEASE)
    {
        if(((key >= CGL_KEY_A && key <= CGL_KEY_Z) || (key >= CGL_KEY_0 && key <= CGL_KEY_9) || key == CGL_KEY_SPACE) && current_text_cursor <= max_text_length)
        {
            text_buffer[current_text_cursor++] = (CGL_byte)key;
            text_buffer[current_text_cursor] = '\0';
        }
        else if(key == CGL_KEY_BACKSPACE && current_text_cursor > 0)
            text_buffer[--current_text_cursor] = '\0';
    }
}

// extract 0s and 1s from the hash to the output buffer
static calculate_hash_binary(CGL_uint hash, CGL_byte* hash_binary)
{
    CGL_byte* hash_bytes = (CGL_byte*)&hash;
    for(CGL_int i = 0, j = 0; i < 4; i++)
        for(CGL_int k = 0; k < 8; k++, j++)
            hash_binary[j] = (hash_bytes[i] >> k) & 1;
}

void update_tilemap(CGL_tilemap* tilemap)
{
    CGL_uint hash = CGL_utils_super_fast_hash(text_buffer, (CGL_int)strlen(text_buffer));
    CGL_byte hash_binary[32]; calculate_hash_binary(hash, hash_binary);
    CGL_ubyte* hash_bytes = (CGL_ubyte*)&hash; CGL_int hash_index = 0;
    CGL_int ri = hash_bytes[3], gi = hash_bytes[2], bi = hash_bytes[1];
    CGL_float r = (CGL_float)ri / 255.0f, g = (CGL_float)gi / 255.0f, b = (CGL_float)bi / 255.0f;
    for(CGL_int y = 0; y < 7 ; y++) for(CGL_int x = 0; x < 7; x++)
    {
        if(x == 0 || x == 6 || y == 0 || y == 6) { CGL_tilemap_set_tile_color(tilemap, x, y, 0.9f, 0.9f, 0.9f); continue; }
        if(x > 3) continue; // only the left half of the tilemap is used
        if(hash_binary[hash_index++]) { CGL_tilemap_set_tile_color(tilemap, x, y, r, g, b); CGL_tilemap_set_tile_color(tilemap, 6 - x, y, r, g, b); }
        else { CGL_tilemap_set_tile_color(tilemap, x, y, 0.9f, 0.9f, 0.9f); CGL_tilemap_set_tile_color(tilemap, 6 - x, y, 0.9f, 0.9f, 0.9f); }
    }
    CGL_tilemap_upload(tilemap);
}

int main()
{
    if(!CGL_init()) return -1;
    CGL_window* main_window = CGL_window_create(700, 700, "GitHub Identicons - Jaysmito Mukherjee");
    if(!main_window) return -1;
    CGL_window_make_context_current(main_window);
    CGL_window_set_key_callback(main_window, key_callback);
    if(!CGL_gl_init()) return -1;
    CGL_widgets_init();
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(main_window);   
    CGL_tilemap* tilemap = CGL_tilemap_create(7, 7, 50, 50, 0);
    text_buffer[current_text_cursor] = '\0';
    CGL_tilemap_set_auto_upload(tilemap, CGL_FALSE);
    while(!CGL_window_should_close(main_window))
    { 
        CGL_window_set_size(main_window, 700, 700);
        update_tilemap(tilemap);
        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_tilemap_render(tilemap, 1.0f, 1.0f, (700 - 50 * 7) * 0.5f, 700 - 50 * 7 - 200, NULL);
        CGL_widgets_begin();
        CGL_byte* buffer = strlen(text_buffer) ? text_buffer : "Type Text";
        CGL_float length = (CGL_int)strlen(buffer) * 0.05f;
        if(!strlen(text_buffer)) CGL_widgets_set_fill_colorf(0.4f, 0.4f, 0.4f, 1.0f);
        else CGL_widgets_set_fill_colorf(0.9f, 0.9f, 0.9f, 1.0f);
        CGL_widgets_add_string(buffer, -length * 0.5f, 0.6f, length, 0.15f);
        CGL_widgets_end();
        CGL_window_poll_events(main_window);
        CGL_window_swap_buffers(main_window);
        
        if(CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }
    CGL_tilemap_destroy(tilemap);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(main_window);
    CGL_shutdown();
    return 0;
}
