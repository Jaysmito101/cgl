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
#define CGL_EXCLUDE_SKY_RENDERER
#define CGL_EXCLUDE_PHONG_RENDERER
#define CGL_EXCLUDE_TILEMAP_RENDERER
#define CGL_IMPLEMENTATION
#include "cgl.h"

// NOTE: Shaders source for shaders/vertex.glsl shaders/fragment.glsl give at the bottom of this file

int main()
{
     srand((uint32_t)time(NULL));
    if(!CGL_init()) return -1;
    CGL_window* main_window = CGL_window_create(640, 480, "Hello World");
    if(!main_window) return -1;
    CGL_window_make_context_current(main_window);
    if(!CGL_gl_init()) return -1;
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(main_window);   
    CGL_mesh_cpu* screen_quad_mesh_cpu = CGL_mesh_cpu_quad((CGL_vec3){ 1.0,  1.0, 0.0},
                                                           (CGL_vec3){ 1.0, -1.0, 0.0},
                                                           (CGL_vec3){-1.0, -1.0, 0.0},
                                                           (CGL_vec3){-1.0,  1.0, 0.0});
    CGL_mesh_gpu* screen_quad_mesh_gpu = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(screen_quad_mesh_gpu, screen_quad_mesh_cpu, true);
    CGL_mesh_cpu_destroy(screen_quad_mesh_cpu);
    CGL_shader* shd = CGL_shader_create_from_files("shaders/vertex.glsl", "shaders/fragment.glsl", NULL);
    int u_tex = CGL_shader_get_uniform_location(shd, "u_tex");
    // Initialize Text Engine
    if(!CGL_text_init()) return EXIT_FAILURE;
    // Load Font file
    CGL_font* font = CGL_font_load("font.ttf");
    // Build the font atlas and bake font bitmaps
    if(!CGL_font_build_atlas(font, 1024, 1024, 72)) {CGL_LOG("Error in building font atlas\n"); return EXIT_FAILURE;}

    //CGL_texture* tex = CGL_font_get_atlas(font);
    const char* text = "Hello World!";
    // bake a string into a texture
    CGL_texture* tex = CGL_text_bake_to_texture(text, strlen(text), font, NULL, NULL);

    while(!CGL_window_should_close(main_window))
    { 
        {
            CGL_framebuffer_bind(default_framebuffer);
            CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
            int rx = 0, ry = 0;
            CGL_framebuffer_get_size(default_framebuffer, &rx, &ry);
            // Render Here
            CGL_shader_bind(shd);
            CGL_texture_bind(tex, 0);
            CGL_shader_set_uniform_int(shd, u_tex, 0);
            CGL_mesh_gpu_render(screen_quad_mesh_gpu);
        }

        CGL_window_poll_events(main_window);
        CGL_window_swap_buffers(main_window);
        if(CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }

    CGL_mesh_gpu_destroy(screen_quad_mesh_gpu);
    CGL_shader_destroy(shd);
    CGL_texture_destroy(tex);

    CGL_framebuffer_destroy(default_framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(main_window);
    CGL_text_shutdown();
    CGL_shutdown();
    return 0;
}

/*
shaders/vertex.glsl
-------------------
#version 430 core

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec4 texcoord;

out vec2 TexCoord; 

void main()
{
	gl_Position = vec4(position.xyz, 1.0f);
	TexCoord = texcoord.xy;		
}
*/

/*
shaders/fragment.glsl
---------------------
#version 430 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D u_tex;

void main()
{
	vec4 color = texture(u_tex, vec2(TexCoord.x, 1-TexCoord.y));
	FragColor = color;
}
*/
