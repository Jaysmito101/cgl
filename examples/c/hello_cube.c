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
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_AUDIO
#define CGL_IMPLEMENTATION
#include "cgl.h"


const CGL_byte* DIFFUSE_VERTEX_SHADER_SOURCE = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"out vec3 Position;\n"
"out vec3 Normal;\n"
"out vec3 Color;\n"
"out vec2 TexCoord;\n"
"\n"
"uniform mat4 view_proj;\n"
"\n"
"void main()\n"
"{\n"
"   vec3 pos = position.xyz;\n"
"	gl_Position = view_proj * vec4(pos, 1.0f);\n"
"	Position = position.xyz;\n"
"	Normal = normal.xyz;\n"
"	TexCoord = texcoord.xy;\n"
"   Color = vec3(0.4f);\n"
"   Color = Normal * 0.5f + vec3(0.5f);"
"}";

const CGL_byte* DIFFUSE_FRAGMENT_SHADER_SOURCE = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Position;\n"
"in vec3 Normal;\n"
"in vec3 Color;\n"
"in vec2 TexCoord;\n"
"\n"
"uniform vec3 u_light_pos;\n"
"\n"
"void main()\n"
"{\n"
"	vec3 object_color = Color;\n"
"	vec3 light_color = vec3(1.0f, 1.0f, 1.0f);\n"
"	vec3 light_dir = normalize(u_light_pos - Position);\n"
"	float diffuse = max(dot(-Normal, light_dir), 0.0f) + 0.18f;\n"
"	vec3 result = (diffuse * light_color) * object_color;\n"
"	FragColor = vec4(result, 1.0f);\n"
"}";



CGL_int main()
{
    if (!CGL_init()) return 1;
    CGL_window* window = CGL_window_create(600, 600, "Hello Cube!");
    CGL_window_make_context_current(window);
    if(!CGL_gl_init()) return 1;
    CGL_framebuffer* framebuffer = CGL_framebuffer_create_from_default(window);

    CGL_shader* diffuse_shader = CGL_shader_create(DIFFUSE_VERTEX_SHADER_SOURCE, DIFFUSE_FRAGMENT_SHADER_SOURCE, NULL);

    CGL_mesh_cpu* base_mesh = CGL_mesh_cpu_cube(false);
    CGL_mesh_cpu_recalculate_normals(base_mesh);
    CGL_mesh_gpu* base_mesh_gpu = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(base_mesh_gpu, base_mesh, true);
    CGL_mesh_cpu_destroy(base_mesh);


    CGL_mat4 perspective = CGL_mat4_perspective(CGL_PI_2 * 0.5f, 1.0f, 0.01f, 100.0f); // create a perspective matrix
    CGL_mat4 view = CGL_mat4_look_at(CGL_vec3_init(0.0f, 0.0f, 8.0f), CGL_vec3_init(0.0f, -1.5f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f)); // create a view matrix

    glEnable(GL_DEPTH_TEST);

    while(!CGL_window_should_close(window))
    {
        CGL_framebuffer_bind(framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);

        CGL_mat4 view_proj = CGL_mat4_mul(perspective, view);

        view = CGL_mat4_look_at(CGL_vec3_init(8.0f * sinf(CGL_utils_get_time()), 5.0f, 8.0f * cosf(CGL_utils_get_time())), CGL_vec3_init(0.0f, 0.0f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));

        CGL_shader_bind(diffuse_shader);
        CGL_shader_set_uniform_mat4(diffuse_shader, CGL_shader_get_uniform_location(diffuse_shader, "view_proj"), &view_proj);
        CGL_shader_set_uniform_vec3v(diffuse_shader, CGL_shader_get_uniform_location(diffuse_shader, "u_light_pos"), 5.0f, 5.0f, 5.0f);
        CGL_mesh_gpu_render(base_mesh_gpu);
        
        
        CGL_window_swap_buffers(window); 
        CGL_window_poll_events(window);
        if(CGL_window_get_key(window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }

    CGL_mesh_gpu_destroy(base_mesh_gpu);
    CGL_shader_destroy(diffuse_shader);
    CGL_framebuffer_destroy(framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
    return 0;
}
