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
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_RAY_CASTER
#define CGL_EXCLUDE_NODE_EDITOR
#define CGL_EXCLUDE_WIDGETS
#define CGL_EXCLUDE_AUDIO
#define CGL_TRAIL_MAX_POINTS 4096
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
"// Black Box From https://github.com/armory3d/armory/blob/master/Shaders/std/tonemap.glsl\n"
"vec3 acesFilm(const vec3 x) {\n"
"    const float a = 2.51;\n"
"    const float b = 0.03;\n"
"    const float c = 2.43;\n"
"    const float d = 0.59;\n"
"    const float e = 0.14;\n"
"    return clamp((x * (a * x + b)) / (x * (c * x + d ) + e), 0.0, 1.0);\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"	vec3 color = texture(u_tex, vec2(TexCoord.y, 1.0f - TexCoord.x)).rgb;\n"
"	color = acesFilm(color);\n"
"   color = pow(color, vec3(1.0f / 2.2f));\n"
"	FragColor = vec4(color, 1.0f);\n"
"}";


static const char* TRAIL_VERTEX_SHADER = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n" // w is lifespan
"layout (location = 1) in vec4 normal;\n" // w is distance
"layout (location = 2) in vec4 texcoord;\n" // zw is reserved for future use
"\n"
"out vec3 Position;\n"
"out vec2 TexCoord;\n"
"out float Distance;\n"
"out float Life;\n"
"\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * vec4(position.xyz, 1.0f);\n"
"	Position = position.xyz;\n"
"	TexCoord = texcoord.xy;\n"
"   Distance = normal.w;\n"
"   Life = position.w;\n"
"}\n";

static const char* TRAIL_FRAGMENT_SHADER = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Position;\n"
"in vec2 TexCoord;\n"
"in float Distance;\n"
"in float Life;\n"
"\n"
"uniform vec3 color;\n"
"uniform float total_length;\n"
"uniform vec3 colors[10];\n"
"\n"
"void main()\n"
"{\n"
"   vec3 col = vec3(1.2, 1, 0);"
"   if      (Life < 2.0f * 1.0f)  col = colors[0];\n"
"   else if (Life < 2.0f * 2.0f)  col = colors[1];\n"
"   else if (Life < 2.0f * 3.0f)  col = colors[2];\n"
"   else if (Life < 2.0f * 4.0f)  col = colors[3];\n"
"   else if (Life < 2.0f * 5.0f)  col = colors[4];\n"
"   else if (Life < 2.0f * 6.0f)  col = colors[5];\n"
"   else if (Life < 2.0f * 7.0f)  col = colors[6];\n"
"   else if (Life < 2.0f * 8.0f)  col = colors[7];\n"
"   else if (Life < 2.0f * 9.0f)  col = colors[8];\n"
"   else if (Life < 2.0f * 10.0f) col = colors[9];\n"
"   col = col * 1.2f;\n"
"	FragColor = vec4(mix(col, vec3(0.0f), (20.0f - Life) / 20.0f), 1.0f);\n"
"}\n";

bool point_function(CGL_trail* trail, CGL_trail_point* point)
{
    point->thickness = 0.05f + 0.4f * fabsf(sinf(CGL_utils_get_time() + point->distance)) * (trail->length - point->distance) / trail->length;
    return false;
}


// Learn More About Lorenz System Here: https://en.wikipedia.org/wiki/Lorenz_system
CGL_vec3 lorenz_system_new_pos(CGL_vec3 prev_pos, CGL_float dt)
{
    float sigma = 10.0f;
    float rho = 28.0f;
    float beta = 8.0f / 3.0f;

    CGL_float dx = sigma * (prev_pos.y - prev_pos.x);
    CGL_float dy = prev_pos.x * (rho - prev_pos.z) - prev_pos.y;
    CGL_float dz = prev_pos.x * prev_pos.y - beta * prev_pos.z;

    CGL_vec3 new_pos = prev_pos;
    new_pos.x += dx * dt;
    new_pos.y += dy * dt;
    new_pos.z += dz * dt;

    return new_pos;
}

int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(600, 600, "Lorenz system - Jaysmito Mukherjee");
    if(!window) return 1;
    CGL_window_make_context_current(window);
    CGL_gl_init();
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);
    CGL_framebuffer* bloom_framebuffer = CGL_framebuffer_create(600, 600);
 
    CGL_shader* present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    CGL_shader* trail_shader = CGL_shader_create(TRAIL_VERTEX_SHADER, TRAIL_FRAGMENT_SHADER, NULL);

    CGL_mat4 projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    CGL_mat4 view = CGL_mat4_identity();
    CGL_bloom* bloom = CGL_bloom_create(600, 600, 3);
    CGL_float start_time = CGL_utils_get_time();
    CGL_float prev_time = start_time;

    CGL_trail* trail = CGL_trail_create();
    CGL_trail_set_min_points_distance(trail, 0.01f);
    CGL_trail_set_resolution(trail, 32);
    CGL_trail_set_point_update_function(trail, point_function);

    CGL_vec3 position = CGL_vec3_init(0.01f, 0.0f, 0.0f);
    CGL_vec3 ptmp = CGL_vec3_init(0.01f, 0.0f, 0.0f);

    CGL_vec3 colors[10];

    // colors of rainbow (i guess)
    colors[9] = CGL_vec3_init(1.0f, 0.0f, 0.0f);
    colors[8] = CGL_vec3_init(1.0f, 0.5f, 0.0f);
    colors[7] = CGL_vec3_init(1.0f, 1.0f, 0.0f);
    colors[6] = CGL_vec3_init(0.0f, 1.0f, 0.0f);
    colors[5] = CGL_vec3_init(0.0f, 0.0f, 1.0f);
    colors[4] = CGL_vec3_init(0.5f, 0.0f, 1.0f);
    colors[3] = CGL_vec3_init(1.0f, 0.0f, 1.0f);
    colors[2] = CGL_vec3_init(1.0f, 0.0f, 0.5f);
    colors[1] = CGL_vec3_init(1.0f, 0.0f, 0.0f);
    colors[0] = CGL_vec3_init(1.0f, 0.0f, 0.0f);


    

    while(!CGL_window_should_close(window))
    {
        CGL_float time = CGL_utils_get_time();
        CGL_float delta_time = time - prev_time;
        prev_time = time;
        CGL_window_set_size(window, 600, 600); // force window size to be 600x600

        ptmp = position;
        CGL_vec3_normalize(ptmp);
        ptmp = CGL_vec3_scale(ptmp, 4.0f);
        ptmp = CGL_vec3_sub(position, ptmp);

        position = lorenz_system_new_pos(position, delta_time * 0.3f);

        view = CGL_mat4_look_at(CGL_vec3_init(65.0f, 0.0f, 45.0f), position, CGL_vec3_init(0.0f, 1.0f, 0.0f));

        {
            CGL_trail_add_point(trail, position, 20.0f, 0.05f);
            CGL_trail_update(trail, delta_time);
            CGL_trail_bake_mesh(trail);
        }
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        CGL_framebuffer_bind(bloom_framebuffer);
        CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
        CGL_shader_bind(trail_shader);
        for (int i = 0; i < 10; i++)
        {
            static char bufft[32];
            sprintf(bufft, "colors[%d]", i);
            CGL_shader_set_uniform_vec3v(trail_shader, CGL_shader_get_uniform_location(trail_shader, bufft), colors[i].x, colors[i].y, colors[i].z);
        }
        CGL_trail_render(trail, &view, &projection, trail_shader);



        CGL_bloom_apply(bloom, CGL_framebuffer_get_color_texture(bloom_framebuffer));

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shader_bind(present_shader);
        CGL_texture_bind(CGL_framebuffer_get_color_texture(bloom_framebuffer), 0);
        CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 0);
        CGL_gl_render_screen_quad();

        CGL_window_poll_events(window);
        CGL_window_swap_buffers(window);

        if(CGL_window_is_key_pressed(window, CGL_KEY_SPACE))
        {
            //for (int i = 0; i < 10; i++) colors[i] = CGL_vec3_init(rand() % 1000 / 1000.0f, rand() % 1000 / 1000.0f, rand() % 1000 / 1000.0f);
            position = CGL_vec3_init(0.01f, 0.0f, 0.0f);
            CGL_trail_clear(trail);
        }
    }


    CGL_trail_destroy(trail);
    CGL_shader_destroy(trail_shader);
    CGL_shader_destroy(present_shader);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_framebuffer_destroy(bloom_framebuffer);
    CGL_bloom_destroy(bloom);
    CGL_gl_shutdown();
    CGL_window_destroy(window);    
    CGL_shutdown();
    return 0;
}
