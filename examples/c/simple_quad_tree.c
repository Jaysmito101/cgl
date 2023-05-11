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

#define CGL_EXCLUDE_NETWORKING
#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_AUDIO
#define CGL_IMPLEMENTATION
#include "cgl.h"

#define MAX_POINTS 1000
#define POINT_SIZE 0.01f

CGL_sizei selected_points[MAX_POINTS];
CGL_vec2 points_clone[MAX_POINTS];
CGL_vec2 points[MAX_POINTS];
CGL_vec2 mouse_selector_size;
CGL_nd_tree* quad_tree;
CGL_sizei selected_points_count = 0;
CGL_float selector_aabb_min[2], selector_aabb_max[2];
CGL_int mode = 0;

static struct
{
    CGL_window* window; // the main window
    int window_height;
    int window_width;
    CGL_framebuffer* framebuffer;
} g_context;

CGL_void init_points()
{
    for (int i = 0; i < MAX_POINTS; i++)
    {
		points[i].x = CGL_utils_random_float_in_range(-1.0f, 1.0f);
		points[i].y = CGL_utils_random_float_in_range(-1.0f, 1.0f);
	}
    memcpy(points_clone, points, sizeof(points));
}

CGL_void update_points(CGL_float dt, CGL_float tm)
{
    for (CGL_sizei i = 0; i < MAX_POINTS; i++)
    {
		points[i].y = points_clone[i].y + sinf(points[i].y - points_clone[i].y + tm + i) * 0.1f;
        points[i].x = points_clone[i].x + cosf(points[i].x - points_clone[i].x + tm + i) * 0.1f;
        if(mode == 0) CGL_quad_tree_add(quad_tree, points[i].x, points[i].y, &i);
	}
}

CGL_void render_points()
{
    for (int i = 0; i < MAX_POINTS; i++)
    {
        CGL_widgets_add_rect2f(points[i].x, points[i].y, POINT_SIZE, POINT_SIZE);
    }
}

CGL_void render_quad_tree()
{
    // NOTE: this is only for debugging purposes and
    //       the cgl_nd_tree data ideally should not be accessed directly
    for (CGL_sizei i = 0; i < quad_tree->nodes_bank_size; i++)
    {
        if (quad_tree->aabb_intersects_aabb_function(2, quad_tree->nodes_bank[i].aabb_min, quad_tree->nodes_bank[i].aabb_max, selector_aabb_min, selector_aabb_max )) CGL_widgets_set_stroke_colorf(0.4f, 0.4f, 0.6f, 0.6f);
        else CGL_widgets_set_stroke_colorf(0.4f, 0.4f, 0.4f, 0.6f);
        CGL_widgets_add_rect2f(
            quad_tree->nodes_bank[i].aabb_min[0],
			quad_tree->nodes_bank[i].aabb_min[1],
            quad_tree->nodes_bank[i].aabb_max[0] - quad_tree->nodes_bank[i].aabb_min[0],
            quad_tree->nodes_bank[i].aabb_max[1] - quad_tree->nodes_bank[i].aabb_min[1]
        );
    }
}


CGL_int main()
{
	CGL_init();
    mouse_selector_size = CGL_vec2_init(0.369f, 0.214f);
    g_context.window_height = g_context.window_width = 700;
    g_context.window = CGL_window_create(g_context.window_width, g_context.window_height, "Quad Tree - Jaysmito Mukherjee"); // create the window
    if (g_context.window == NULL) return false; // window creation failed
    CGL_window_make_context_current(g_context.window); // make the opengl context for the window current
    if (!CGL_gl_init()) return EXIT_FAILURE; // initialize cgl opengl module    
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window); // load the default framebuffer (0) into CGL_framebuffer object
    if (g_context.framebuffer == NULL) return false; // failed
    if (!CGL_widgets_init()) return EXIT_FAILURE;

    init_points();

    quad_tree = CGL_quad_tree_create(sizeof(CGL_sizei), 128, 100000, 100000, true);

    CGL_int frame_rate = 0, frames = 0;
    CGL_float timer = 0.0f, time_last = CGL_utils_get_time(), time_now = 0.0f;

    // the main loop
    while (!CGL_window_should_close(g_context.window)) // run till the close button is clicked
    {
        CGL_double mdx, mdy; CGL_window_get_mouse_position(g_context.window, &mdx, &mdy);
        CGL_float mx = (CGL_float)mdx / (CGL_float)g_context.window_width * 2.0f - 1.0f;
        CGL_float my = (1.0f - (CGL_float)mdy / (CGL_float)g_context.window_height) * 2.0f - 1.0f;
        time_now = CGL_utils_get_time(); timer += time_now - time_last; time_last = time_now; frames++;
        if (timer > 1.0f) { frame_rate = frames; frames = 0; timer = 0.0f; }


        CGL_window_set_size(g_context.window, g_context.window_width, g_context.window_height); // set window size
        CGL_framebuffer_bind(g_context.framebuffer); // bind default framebuffer and also adjust viewport size and offset
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f); // clear screen with a dark gray color

        if(mode == 0) CGL_quad_tree_reset(quad_tree, -1.0f, -1.0f, 1.0f, 1.0f, 12, 6, CGL_window_is_key_pressed(g_context.window, CGL_KEY_F));

        update_points(0.001f, CGL_utils_get_time());

        CGL_widgets_begin();

        CGL_widgets_set_fill_mode(true);
        CGL_widgets_set_fill_colorf(0.6f, 0.6f, 0.6f, 1.0f);
        render_points();

        // selected_points_count = CGL_quad_tree_get_items_in_range(quad_tree, mx - mouse_selector_size.x / 2.0f, my - mouse_selector_size.y / 2.0f, mouse_selector_size.x, mouse_selector_size.y, selected_points, MAX_POINTS);
        selector_aabb_min[0] = mx - mouse_selector_size.x / 2.0f; selector_aabb_min[1] = my - mouse_selector_size.y / 2.0f;
        selector_aabb_max[0] = mx + mouse_selector_size.x / 2.0f; selector_aabb_max[1] = my + mouse_selector_size.y / 2.0f;
        if (mode == 0) selected_points_count = CGL_nd_tree_get_items_in_range(quad_tree, selector_aabb_min, selector_aabb_max, selected_points, MAX_POINTS);

        CGL_widgets_set_fill_colorf(0.0f, 0.8f, 0.0f, 1.0f);
        if (mode == 0) { for (CGL_int i = 0; i < selected_points_count; i++) CGL_widgets_add_rect2f(points[selected_points[i]].x, points[selected_points[i]].y, POINT_SIZE, POINT_SIZE); }
        else if(mode == 1)
        {
            for (CGL_int i = 0; i < MAX_POINTS; i++) 
                if (CGL_aabb_contains_point_2d(2, selector_aabb_min, selector_aabb_max, &points[i]))
                    CGL_widgets_add_rect2f(points[i].x, points[i].y, POINT_SIZE, POINT_SIZE);
        }

               


        CGL_widgets_set_fill_mode(false);
        CGL_widgets_set_stroke_thickness(0.005f);
        CGL_widgets_set_stroke_colorf(0.0f, 0.8f, 0.0f, 1.0f);
        CGL_widgets_add_rect2f(mx - mouse_selector_size.x / 2.0f, my - mouse_selector_size.y / 2.0f, mouse_selector_size.x, mouse_selector_size.y);

        CGL_widgets_set_stroke_thickness(0.005f);
        if(mode == 0 && CGL_window_is_key_pressed(g_context.window, CGL_KEY_Q)) render_quad_tree();


        static CGL_byte frame_rate_string[32];
        sprintf(frame_rate_string, "FPS: %d", frame_rate);
        CGL_widgets_add_string(frame_rate_string, -0.99f, 0.90f, 0.2f, 0.05f);
        CGL_widgets_add_string(mode == 0 ? "QT" : "NM", -0.99f, 0.80f, 0.1f, 0.05f);


        CGL_widgets_end();

        if (CGL_window_is_key_pressed(g_context.window, CGL_KEY_SPACE))
        {
            mode = (mode + 1) % 2;
            CGL_utils_sleep(100);
        }

        CGL_window_swap_buffers(g_context.window); // swap framebuffers
        CGL_window_poll_events(g_context.window); // poll events (if this is not called every frame window will stop responding)

        if (CGL_window_get_key(g_context.window, CGL_KEY_ESCAPE) == CGL_PRESS) break; // quit on pressing escape
    }

    
    // cleanup
    CGL_nd_tree_destroy(quad_tree);
    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(g_context.framebuffer); // destory framebuffer object
    CGL_gl_shutdown(); // shutdown cgl opengl module
    CGL_window_destroy(g_context.window); // destroy window
	CGL_shutdown();
	return EXIT_SUCCESS;
}
