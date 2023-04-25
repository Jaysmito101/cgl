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
#define CGL_IMPLEMENTATION
#include "cgl.h"

static struct
{
    CGL_window* window; // the main window
    int window_height;
    int window_width;
    CGL_framebuffer* framebuffer;
} g_context;

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return EXIT_FAILURE; // initialize CGL (required for setting up internals of CGL)
    g_context.window_height = 600;
    g_context.window_width = 600;
    g_context.window = CGL_window_create(g_context.window_width, g_context.window_height, "Tests - Jaysmito Mukherjee"); // create the window
    if(g_context.window == NULL) return false; // window creation failed
    CGL_window_make_context_current(g_context.window); // make the opengl context for the window current
    if(!CGL_gl_init()) return EXIT_FAILURE; // initialize cgl opengl module    
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window); // load the default framebuffer (0) into CGL_framebuffer object
    if(g_context.framebuffer == NULL) return false; // failed
    if(!CGL_widgets_init()) return EXIT_FAILURE; 
    CGL_float curr_time = 0.0f;

    CGL_mesh_cpu* mesh = CGL_mesh_cpu_cube(false);
    CGL_mesh_cpu_recalculate_normals(mesh);

    // the main loop
    while(!CGL_window_should_close(g_context.window)) // run till the close button is clicked
    {
        curr_time = CGL_utils_get_time();
        CGL_window_set_size(g_context.window, g_context.window_width, g_context.window_height); // set window size
        // rendering
        CGL_framebuffer_bind(g_context.framebuffer); // bind default framebuffer and also adjust viewport size and offset
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f); // clear screen with a dark gray color

        CGL_widgets_begin();
        
        float offx = -0.4f;
        float offy = -0.4f;
        srand(42);

        CGL_mat4 proj = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
        CGL_mat4 view = CGL_mat4_look_at(CGL_vec3_init(sinf(curr_time) * 5.0f, 2.0f, cosf(curr_time) * 5.0f), CGL_vec3_init(0.0f, 0.0f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));
        CGL_mat4 model = CGL_mat4_scale(0.5f, 0.5f, 0.5f);

        
        CGL_widgets_set_projection_matrix(&proj); CGL_widgets_set_view_matrix(&view); CGL_widgets_set_model_matrix(&model);
        
        CGL_widgets_enable_diffuse_shading(CGL_vec3_init(10.0f, 10.0f, 0.0f), CGL_vec3_init(1.0f, 1.0f, 1.0f));
        glEnable(GL_DEPTH_TEST);
        CGL_color color = CGL_utils_random_color();
        CGL_widgets_set_fill_colorf(color.x, color.y, color.z, 1.0f);
        CGL_widgets_add_mesh(mesh);

        CGL_widgets_set_projection_matrix(NULL); CGL_widgets_set_view_matrix(NULL); CGL_widgets_set_model_matrix(NULL);
        CGL_widgets_disable_diffuse_shading();

        CGL_widgets_add_rect2f(-1.0f, 0.0f, 0.1f, 0.1f);
        
        CGL_widgets_end();

        CGL_window_swap_buffers(g_context.window); // swap framebuffers
        CGL_window_poll_events(g_context.window); // poll events (if this is not called every frame window will stop responding)

        if(CGL_window_get_key(g_context.window, CGL_KEY_ESCAPE) == CGL_PRESS) break; // quit on pressing escape
    }

    // cleanup
    CGL_mesh_cpu_destroy(mesh);
    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(g_context.framebuffer); // destory framebuffer object
    CGL_gl_shutdown(); // shutdown cgl opengl module
    CGL_window_destroy(g_context.window); // destroy window
    CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
    return EXIT_SUCCESS;
}
