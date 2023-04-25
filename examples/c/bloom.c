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
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_IMPLEMENTATION
#include "cgl.h"

static const char* PASS_THROUGH_VERTEX_SHADER_SOURCE = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"out vec3 Position;\n"
"out vec2 TexCoord;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(position.xyz, 1.0f);\n"
"	Position = position.xyz;\n"
"	TexCoord = texcoord.xy;\n"
"}";

static const char* PASS_THROUGH_FRAGMENT_SHADER_SOURCE = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Position;\n"
"in vec2 TexCoord;\n"
"\n"
"uniform sampler2D u_tex;\n"
"\n"
"void main()\n"
"{\n"
"	vec4 color = texture(u_tex, TexCoord);\n"
"	FragColor = color;\n"
"}";


int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(640, 360, "CGL Text Widgets - Jaysmito Mukherjee");
    CGL_window_make_context_current(window); 
    CGL_gl_init();
    CGL_widgets_init();
    if(window == NULL) return false; 
    CGL_framebuffer* framebuffer = CGL_framebuffer_create(640, 360);
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);

    CGL_mesh_cpu* quad_mesh_cpu = CGL_mesh_cpu_quad(
        CGL_vec3_init(1.0f, 1.0f, 0.0f),
        CGL_vec3_init(1.0f, -1.0f, 0.0f),
        CGL_vec3_init(-1.0f, -1.0f, 0.0f),
        CGL_vec3_init(-1.0f, 1.0f, 0.0f)
    );

    CGL_mesh_gpu* quad_mesh = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(quad_mesh, quad_mesh_cpu, true);
    CGL_mesh_cpu_destroy(quad_mesh_cpu);
    CGL_shader* shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);

    CGL_bloom* bloom = CGL_bloom_create(640, 360, 3);
    CGL_bloom_set_offset(bloom, 3, 3);

    bool bloom_enabled = true;
    float intensity = 4.0f;
    float aspect_ratio = 16.0f / 9.0f;
    int wx = 0, wy = 0;

    while(!CGL_window_should_close(window)) 
    {
        CGL_window_get_framebuffer_size(window, &wx, &wy);
        aspect_ratio = (float)wx / (float)wy;

        CGL_framebuffer_bind(framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_widgets_begin();
        CGL_widgets_add_string("CGL", -0.5f, -0.5f, 1.0f, 1.0f);

        CGL_widgets_set_fill_colorf(0.78f * intensity, 0.44f * intensity, 0.89f * intensity, 1.0f * intensity);

        CGL_widgets_add_rect2f(-0.75f, -0.75f, 1.5f, 0.05f * aspect_ratio);
        CGL_widgets_add_rect2f(-0.75f,  0.70f, 1.5f, 0.05f * aspect_ratio);
        CGL_widgets_add_rect2f(-0.75f, -0.75f, 0.05f, 1.5f);
        CGL_widgets_add_rect2f(0.7f, -0.75f, 0.05f, 1.5f);
        
        CGL_widgets_end();

        if(bloom_enabled) CGL_bloom_apply(bloom, CGL_framebuffer_get_color_texture(framebuffer));

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shader_bind(shader);
        CGL_texture_bind(CGL_framebuffer_get_color_texture(framebuffer), 0);
        CGL_shader_set_uniform_int(shader, CGL_shader_get_uniform_location(shader, "u_tex"), 0);
        CGL_mesh_gpu_render(quad_mesh);

        CGL_window_swap_buffers(window);
        CGL_window_poll_events(window);

        if(CGL_window_get_key(window, CGL_KEY_A) == CGL_PRESS) bloom_enabled = true;
        if(CGL_window_get_key(window, CGL_KEY_S) == CGL_PRESS) bloom_enabled = false;
    }


    CGL_bloom_destroy(bloom);    
    CGL_shader_destroy(shader);
    CGL_framebuffer_destroy(framebuffer);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_mesh_gpu_destroy(quad_mesh);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
    return EXIT_SUCCESS;
}
