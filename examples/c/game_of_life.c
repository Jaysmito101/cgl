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
#define CGL_EXCLUDE_AUDIO
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_EXCLUDE_NETWORKING
#define CGL_IMPLEMENTATION
#include "cgl.h"

#define MAIN_FRAME_BUFFER_SIZE  700


static struct
{
    CGL_float scale_x;
    CGL_float scale_y;
    CGL_float offset_x;
    CGL_float offset_y;
    CGL_tilemap* tilemap;
} tilemap_data;

#define TILE_COUNT_X 35
#define TILE_COUNT_Y 35

#define ID(x, y) ((x) + (y) * TILE_COUNT_X)

static CGL_int hovered_tile_x = -1, hovered_tile_y = -1;
static CGL_ubyte b0[TILE_COUNT_X][TILE_COUNT_Y], b1[TILE_COUNT_X][TILE_COUNT_Y];
static CGL_ubyte *board, *board_next;
static CGL_vec3 colors[] = {
    {0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}
};


void upload_tilemap()
{
    for(int i = 0 ; i < TILE_COUNT_X; i++)
        for(int j = 0 ; j < TILE_COUNT_Y; j++)
            CGL_tilemap_set_tile_color(tilemap_data.tilemap, i, j, colors[board[ID(i, j)]].x, colors[board[ID(i, j)]].y, colors[board[ID(i, j)]].z);
}

CGL_ubyte sample_from_board(int i, int j)
{
    CGL_ubyte val = board[ID(i, j)]; if(val == 1) val = 0; return val;
}

CGL_ubyte get_neighbour_count(int x, int y)
{
    CGL_int count = 0, val = 0, spx = 0, spy = 0;
    for(int i = -1 ; i <= 1 ; i ++) for(int j = -1 ; j <= 1 ; j ++)
    {
        spx = x + i; spy = y + j;
        if(i == 0 && j == 0) continue;
        if (spx < 0) spx = TILE_COUNT_X - 1;
        if (spx >= TILE_COUNT_X) spx = 0;
        if (spy < 0) spy = TILE_COUNT_Y - 1;
        if (spy >= TILE_COUNT_Y) spy = 0;
        val = (CGL_int)sample_from_board(spx, spy);
        if(val == 2) count ++;
    }
    return (CGL_ubyte)count;
}

void update_board()
{
    CGL_ubyte val = 0, nc;
    for(int i = 0 ; i < TILE_COUNT_X ; i ++) for(int j = 0 ; j < TILE_COUNT_Y ; j ++)
    {
        val = sample_from_board(i, j);
        nc = get_neighbour_count(i, j);
        if(val == 0 && nc == 3) board_next[ID(i, j)] = 2;
        else
        {
            if(nc < 2 || nc > 3) board_next[ID(i, j)] = 0;
            else board_next[ID(i, j)] = val;
        }
    }
}

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return -1;
    CGL_window* main_window = CGL_window_create(700, 700, "Conway's Game of Life - Jaysmito Mukherjee");
    if(!main_window) return -1;
    CGL_window_make_context_current(main_window);
    if(!CGL_gl_init()) return -1;
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(main_window);   
    tilemap_data.offset_x = 0.0f; tilemap_data.offset_y = 0.0f;
    tilemap_data.scale_x = 1.0f; tilemap_data.scale_y = 1.0f;
    tilemap_data.tilemap = CGL_tilemap_create(TILE_COUNT_X, TILE_COUNT_Y, 700 / TILE_COUNT_X, 700 / TILE_COUNT_Y, 0);
    board = (CGL_ubyte*)b0;
    board_next = (CGL_ubyte*)b1;
    CGL_bool playing = false;
    CGL_float last_board_update = CGL_utils_get_time();
    CGL_float board_update_interval = 0.3f;
    while(!CGL_window_should_close(main_window))
    { 
        {
            CGL_framebuffer_bind(default_framebuffer);
            CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
            CGL_tilemap_render(tilemap_data.tilemap, tilemap_data.scale_x, tilemap_data.scale_y, tilemap_data.offset_x, tilemap_data.offset_y, NULL);
        }

        CGL_window_poll_events(main_window);


        if(CGL_utils_get_time() - last_board_update > board_update_interval) 
        {
            if(playing) {last_board_update = CGL_utils_get_time(); update_board(); }
        }
        else memcpy(board_next, board, sizeof(CGL_ubyte) * TILE_COUNT_X * TILE_COUNT_Y);
        CGL_double mx = 0.0, my = 0.0;
        CGL_window_get_mouse_position(main_window, &mx, &my);
        hovered_tile_x = (int)(mx / (700.0 / TILE_COUNT_X));
        hovered_tile_y = (int)((700.0 - my) / (700.0 / TILE_COUNT_Y));
        if(CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS) board_next[ID(hovered_tile_x, hovered_tile_y)] = 2;
        if(CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_RIGHT) == CGL_PRESS) board_next[ID(hovered_tile_x, hovered_tile_y)] = 0;
        { CGL_ubyte* temp = (CGL_ubyte*)board; board = board_next; board_next = temp; }

        upload_tilemap();

        CGL_window_swap_buffers(main_window);
        if(CGL_window_get_key(main_window, CGL_KEY_SPACE) == CGL_PRESS) playing = true;
        if(CGL_window_get_key(main_window, CGL_KEY_ENTER) == CGL_PRESS) playing = false;
        if(CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }

    CGL_tilemap_destroy(tilemap_data.tilemap);

    CGL_framebuffer_destroy(default_framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(main_window);
    CGL_shutdown();
    return 0;
}
