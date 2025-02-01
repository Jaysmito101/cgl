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
#define CGL_EXCLUDE_AUDIO
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_IMPLEMENTATION
#include "cgl.h"

#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif


/*
WARNING: There are several ways to use CGL_node_editor this is just a very basic example and not by any means the proper way to do it.
*/

static CGL_node_editor_input inpt;
static CGL_node_editor_node nodes[1000];
static CGL_node_editor_pin* links[10000][2];
static CGL_node_editor* editor = NULL;
static CGL_window* main_window = NULL;
static CGL_framebuffer* default_framebuffer = NULL;
static int node_count = 1;
static int link_count = 1;

void scroll_callback(CGL_window* window, double x, double y)
{
    (void)window;
    (void)x;

    inpt.scale += (float)y * 0.01f;
}

void on_connect(CGL_node_editor_pin* a, CGL_node_editor_pin* b)
{
    a->user_data = (void*)(intptr_t)link_count;
    b->user_data = (void*)(intptr_t)link_count;
    links[link_count][0] = a;
    links[link_count][1] = b;
    link_count++;
}

void on_drop(float px, float py, CGL_node_editor_pin* a)
{
    (void)px;
    (void)py;

    if (a->user_data != NULL)
    {
        CGL_node_editor_clear_focused_pins(editor);
        int ind = (int)(intptr_t)a->user_data;
        links[ind][0] = links[ind][1] = NULL;
    }
}


CGL_bool init() {

    srand((uint32_t)time(NULL));
    CGL_init();
    main_window = CGL_window_create(640, 360, "CGL Text Widgets - Jaysmito Mukherjee");
    if(main_window == NULL) return CGL_FALSE;
    CGL_window_make_context_current(main_window); 
    CGL_window_set_mouse_scroll_callback(main_window, scroll_callback);
    CGL_gl_init();
    CGL_widgets_init();
    default_framebuffer = CGL_framebuffer_create_from_default(main_window);

    inpt.scale = 1.0f;
    editor = CGL_node_editor_create();
    CGL_node_editor_set_on_connect(editor, on_connect);
    CGL_node_editor_set_on_drop(editor, on_drop);

    memset(nodes, 0, sizeof(nodes));
    memset(links, 0, sizeof(links));
    
    CGL_window_swap_buffers(main_window);
    CGL_window_poll_events(main_window);

    return CGL_TRUE;
}

CGL_void cleanup() {
    CGL_node_editor_destroy(editor);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(main_window);
    CGL_shutdown();
}

EM_BOOL loop(double time, void* userData) {
    (void)time;
    (void)userData;

    int wx, wy;
    double mxp, myp;
    CGL_window_get_framebuffer_size(main_window, &wx, &wy);
    CGL_window_get_mouse_position(main_window, &mxp, &myp);
    inpt.aspect_ratio = (float)wx / wy;
    inpt.mouse_pos_x = (float)mxp / wx;
    inpt.mouse_pos_y = 1.0f - (float)myp / wy;
    inpt.mouse_pos_x = inpt.mouse_pos_x * 2.0f - 1.0f;
    inpt.mouse_pos_y = inpt.mouse_pos_y * 2.0f - 1.0f;    
    inpt.mouse_button_left = CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS;
    inpt.mouse_button_middle = CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_MIDDLE) == CGL_PRESS;
    inpt.mouse_button_right = CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_RIGHT) == CGL_PRESS;
    inpt.shift = (CGL_window_get_key(main_window, CGL_KEY_LEFT_SHIFT) == CGL_PRESS) || (CGL_window_get_key(main_window, CGL_KEY_RIGHT_SHIFT) == CGL_PRESS);
    inpt.ctrl = (CGL_window_get_key(main_window, CGL_KEY_LEFT_CONTROL) == CGL_PRESS) || (CGL_window_get_key(main_window, CGL_KEY_RIGHT_CONTROL) == CGL_PRESS);
    inpt.escape = (CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS);

    CGL_node_editor_update(editor, &inpt);
    for (int i = 1; i < node_count; i++)
        {
        if (nodes[i].user_data == NULL) CGL_node_editor_node_update(&nodes[i]);
        if (nodes[i].selected && CGL_window_get_key(main_window, CGL_KEY_DELETE) == CGL_PRESS) nodes[i].user_data = (void*)42;
        }
        

    CGL_framebuffer_bind(default_framebuffer);
    CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        

    CGL_node_editor_render_begin(editor);

    // render links
    for(CGL_sizei i = 1 , j = 1; i < sizeof(links) / sizeof(links[0]) && j < (CGL_sizei)link_count ; i++ )
        {
        if(links[i][0] && links[i][1])
            {
            if (links[i][0]->parent->user_data != NULL || links[i][1]->parent->user_data != NULL)
            {
                links[i][0]->user_data = links[i][1]->user_data = NULL;
                links[i][0] = links[i][1] = NULL;
            }
            // CGL_node_editor_render_linkf(links[i][0], links[i][1], 0.8f, 0.6f, 0.2f, 1.0f, 0.1f); // for rendering straight line links
            CGL_node_editor_render_link_curvedf(links[i][0], links[i][1], 0.8f, 0.6f, 0.2f, 1.0f, 0.4f, 0.1f, 8); // for rendering links based on bazier curves
            j++;
            }
        }

    // render nodes
    for (CGL_int i = 1 ; i < node_count ; i++) if(nodes[i].user_data == NULL) CGL_node_editor_node_render(&nodes[i]);

    CGL_node_editor_render_end(editor);

    if(CGL_window_get_key(main_window, CGL_KEY_A) == CGL_PRESS && (CGL_sizei)node_count < sizeof(nodes) / sizeof(nodes[0]))
        {
        static CGL_float last_add_time = 0.0f;
        if(CGL_utils_get_time() - last_add_time > 0.3)
            {
            last_add_time = CGL_utils_get_time();

            CGL_info("Created a new node");
            CGL_node_editor_node_init(editor, &nodes[node_count++]);
            CGL_node_editor_node_set_position(&nodes[node_count - 1], inpt.mouse_pos_x, inpt.mouse_pos_y);
            char buffer[256];
            static int count = 0;
            sprintf(buffer, "Node :%d", count++);
            CGL_node_editor_node_set_title(&nodes[node_count - 1], buffer);
            int r = CGL_utils_random_int(2, 5);
            for(int i = 0 ; i < r ; i++) CGL_node_editor_node_get_pin(&nodes[node_count - 1], true, i)->is_set = true;
            r = CGL_utils_random_int(2, 5);
            for (int i = 0; i < r; i++) CGL_node_editor_node_get_pin(&nodes[node_count - 1], false, i)->is_set = true;
            }
        }

        
    CGL_window_swap_buffers(main_window);
    CGL_window_poll_events(main_window);


    return CGL_TRUE;
}


int main()
{
    if(!init()) return EXIT_FAILURE;
#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while(!CGL_window_should_close(main_window)) {
        if(!loop(0.0, NULL)) return EXIT_FAILURE;
    }
    cleanup();
#endif
    return EXIT_SUCCESS;
}
