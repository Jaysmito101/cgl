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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MAIN_FRAME_BUFFER_WIDTH     700
#define MAIN_FRAME_BUFFER_HEIGHT    700
#define TILE_SIZE                   14
#define TILE_MAP_SIZE               (700 / TILE_SIZE)
#define TILE_EMPTY_ID               0
#define TILE_START_POS_ID           1
#define TILE_END_POS_ID             2
#define TILE_WALL_ID                3
#define TILE_PATH_ID                4

static CGL_int grid[TILE_MAP_SIZE * TILE_MAP_SIZE] = {0};
static CGL_int start_pos[2] = {0};
static CGL_int end_pos[2] = {TILE_MAP_SIZE - 1, TILE_MAP_SIZE - 1};
static CGL_path_finding_a_star_context* path_finding_context = NULL;

void set_tilemap_from_ai_vector(CGL_tilemap* tilemap, CGL_int* data) // set tilemap from ai vector
{
    CGL_tilemap_set_auto_upload(tilemap, CGL_FALSE); // disable auto upload
    CGL_vec3 n_color = CGL_vec3_init(0.0f, 0.0f, 0.0f); // create color
    for(CGL_int y = 0 ; y < TILE_MAP_SIZE; y++) for(CGL_int x = 0 ; x < TILE_MAP_SIZE; x++) // iterate through all pixels
    {
        switch(data[y * TILE_MAP_SIZE + x])
        {
            case TILE_EMPTY_ID: n_color = CGL_vec3_init(0.1f, 0.1f, 0.1f); break; // set color to black
            case TILE_START_POS_ID: n_color = CGL_vec3_init(0.0f, 0.7f, 0.0f); break; // set color to green
            case TILE_END_POS_ID: n_color = CGL_vec3_init(0.7f, 0.0f, 0.0f); break; // set color to red
            case TILE_WALL_ID: n_color = CGL_vec3_init(0.7f, 0.7f, 0.7f); break; // set color to white
            case TILE_PATH_ID: n_color = CGL_vec3_init(0.0f, 0.0f, 0.7f); break; // set color to blue
        }
        CGL_tilemap_set_tile_color(tilemap, x, y, n_color.x, n_color.y, n_color.z); // set tile color
    }
    CGL_tilemap_upload(tilemap); // upload tilemap
    CGL_tilemap_set_auto_upload(tilemap, CGL_TRUE); // enable auto upload
}

CGL_bool node_equals(void* user_data, CGL_path_finding_node* node1, CGL_path_finding_node* node2)
{
    CGL_int* data1 = (CGL_int*)node1->data_ptr, *data2 = (CGL_int*)node2->data_ptr;
    return *data1 == *data2;
}

CGL_float heuristic(void* user_data, CGL_path_finding_node* node)
{
    CGL_int* data = (CGL_int*)node->data_ptr;
    CGL_int x = *data % TILE_MAP_SIZE, y = *data / TILE_MAP_SIZE;
    return (CGL_float)abs(x - end_pos[0]) + abs(y - end_pos[1]);
    //return (CGL_float)0.0f;
}

CGL_float cost(void* user_data, CGL_path_finding_node* start_node, CGL_path_finding_node* end_node)
{
    CGL_int* start_data = (CGL_int*)start_node->data_ptr, *end_data = (CGL_int*)end_node->data_ptr;
    CGL_int start_x = *start_data % TILE_MAP_SIZE, start_y = *start_data / TILE_MAP_SIZE;
    CGL_int end_x = *end_data % TILE_MAP_SIZE, end_y = *end_data / TILE_MAP_SIZE;
    return (CGL_float)abs(start_x - end_x) + abs(start_y - end_y);
}

CGL_int add_node(CGL_int x, CGL_int y)
{
    CGL_int id = y * TILE_MAP_SIZE + x;
    CGL_path_finding_node node; CGL_path_finding_node_init(&node, &id); node.is_open = CGL_TRUE;
    return CGL_path_finding_a_star_add_node(path_finding_context, node, node_equals);
}

CGL_int get_neighbours(void* user_data, CGL_path_finding_node* node, CGL_int* neighbours_out)
{
    CGL_int* data = (CGL_int*)node->data_ptr;
    CGL_int nx = *data % TILE_MAP_SIZE, ny = *data / TILE_MAP_SIZE, k = 0;
    for(CGL_int i = -1; i <= 1; i++) for(CGL_int j = -1; j <= 1; j++) 
    {
        if(i == 0 && j == 0) continue; // skip current node (itself)
        if(i == -1 && j == -1) continue; // skip top left 
        if(i == 1 && j == -1) continue; // skip top right
        if(i == -1 && j == 1) continue; // skip bottom left
        if(i == 1 && j == 1) continue; // skip bottom right
        CGL_int x = nx + i, y = ny + j; // get neighbour position
        if(x < 0 || x >= TILE_MAP_SIZE || y < 0 || y >= TILE_MAP_SIZE || grid[y * TILE_MAP_SIZE + x] == TILE_WALL_ID) continue; // skip if out of bounds or wall
        neighbours_out[k++] = add_node(x, y); // add neighbour
    }
    return k;
}

void update_path()
{
    CGL_int stnid = start_pos[1] * TILE_MAP_SIZE + start_pos[0], endid = end_pos[1] * TILE_MAP_SIZE + end_pos[0];
    CGL_path_finding_node start_node; CGL_path_finding_node_init(&start_node, &stnid); start_node.is_open = CGL_TRUE;
    CGL_path_finding_node end_node; CGL_path_finding_node_init(&end_node, &endid); end_node.is_open = CGL_TRUE;
    if(!CGL_path_finding_a_star_find_path(path_finding_context, start_node, end_node, heuristic, cost, get_neighbours, node_equals, NULL)) CGL_warn("Path not found!");
}

int main()
{
    srand((uint32_t)time(NULL)); // seed random number generator
    if(!CGL_init()) return -1; // initialize CGL
    CGL_window* main_window = CGL_window_create(MAIN_FRAME_BUFFER_WIDTH, MAIN_FRAME_BUFFER_HEIGHT, "A Star Path Finding - Jaysmito Mukherjee"); // create window
    if(!main_window) return -1; CGL_window_make_context_current(main_window); // make window context current
    if(!CGL_gl_init()) return -1; // initialize OpenGL
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(main_window); // create framebuffer from default window
    CGL_tilemap* tilemap = CGL_tilemap_create(TILE_MAP_SIZE, TILE_MAP_SIZE, TILE_SIZE, TILE_SIZE, 1);
    CGL_widgets_init(); // initialize widgets
    path_finding_context = CGL_path_finding_a_star_context_create(TILE_MAP_SIZE * TILE_MAP_SIZE, CGL_TRUE, sizeof(CGL_int));
    while(!CGL_window_should_close(main_window)) // main loop
    { 
        CGL_window_set_size(main_window, MAIN_FRAME_BUFFER_WIDTH, MAIN_FRAME_BUFFER_HEIGHT); // set window size
        CGL_framebuffer_bind(default_framebuffer); // bind framebuffer
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f); // clear the screen
        if(CGL_window_is_key_pressed(main_window, CGL_KEY_F)) update_path(); CGL_int pos_id = 0;
        CGL_path_finding_a_star_reorder_path(path_finding_context);
        set_tilemap_from_ai_vector(tilemap, grid);
        while(CGL_path_finding_a_star_next_in_path(path_finding_context, &pos_id)) 
        {
            CGL_int x = pos_id % TILE_MAP_SIZE, y = pos_id / TILE_MAP_SIZE;
            CGL_tilemap_set_tile_color(tilemap, x, y, 0.0f, 0.0f, 1.0f);
        }
        CGL_tilemap_set_tile_color(tilemap, start_pos[0], start_pos[1], 0.0f, 1.0f, 0.0f); 
        CGL_tilemap_set_tile_color(tilemap, end_pos[0], end_pos[1], 1.0f, 0.0f, 0.0f);
        CGL_tilemap_render(tilemap, 1.0f, 1.0f, 0.0f, 0.0f, NULL); // render tilemap
        CGL_double mx = 0.0, my = 0.0; CGL_window_get_mouse_position(main_window, &mx, &my); my = MAIN_FRAME_BUFFER_HEIGHT - my; // get mouse position
        if(CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS) 
        {
            CGL_path_finding_a_star_clear_path(path_finding_context);
            CGL_int x = (CGL_int)(TILE_MAP_SIZE * mx / 700), y = (CGL_int)(TILE_MAP_SIZE * my / 700); // get the tilemap coordinates
            grid[y * TILE_MAP_SIZE + x] = TILE_WALL_ID; // set the input vector
            if(CGL_window_is_key_pressed(main_window, CGL_KEY_S)) { start_pos[0] = x; start_pos[1] = y; } // set start position
            if(CGL_window_is_key_pressed(main_window, CGL_KEY_E)) { end_pos[0] = x; end_pos[1] = y; } // set end position
        }
        else if(CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_RIGHT) == CGL_PRESS) 
        {
            CGL_path_finding_a_star_clear_path(path_finding_context);
            CGL_int x = (CGL_int)(TILE_MAP_SIZE * mx / 700), y = (CGL_int)(TILE_MAP_SIZE * my / 700); // get the tilemap coordinates
            grid[y * TILE_MAP_SIZE + x] = TILE_EMPTY_ID; // set the input vector
        }
        CGL_window_poll_events(main_window); CGL_window_swap_buffers(main_window); // swap buffers
        if(CGL_window_is_key_pressed(main_window, CGL_KEY_C)) { memset(grid, 0, sizeof(grid)); set_tilemap_from_ai_vector(tilemap, grid); } // clear the grid
        if(CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break; // if the escape key is pressed, break the loop
    }
    CGL_tilemap_destroy(tilemap); // destroy the tilemap 
    CGL_framebuffer_destroy(default_framebuffer); // destroy the framebuffer
    CGL_widgets_shutdown(); // shutdown widgets
    CGL_gl_shutdown(); // shutdown OpenGL
    CGL_window_destroy(main_window); // destroy the window
    CGL_shutdown(); // shutdown CGL
    return EXIT_SUCCESS; // exit successfully
}
