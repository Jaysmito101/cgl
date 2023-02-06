/*
MIT License

Copyright (c) 2022 Jaysmito Mukherjee (jaysmito101@gmail.com)

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
#include "cgl.h"

#define TRAIL_COUNT 16

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
"\n"
"void main()\n"
"{\n"
"	FragColor = vec4(mix(color, vec3(0.0f), (10.0f - Life) / 10.0f), 1.0f);\n"
"}\n";

bool point_function(CGL_trail* trail, CGL_trail_point* point)
{
    point->thickness = 0.1f * (trail->length - point->distance) / trail->length;
    return false;
}

int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(600, 600, "Trails - Jaysmito Mukherjee");
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

    CGL_trail* trail[TRAIL_COUNT];
    for(int i = 0; i < TRAIL_COUNT; i++)
    {
        trail[i] = CGL_trail_create();
        CGL_trail_set_min_points_distance(trail[i], 0.01f);
        CGL_trail_set_resolution(trail[i], 32);
        CGL_trail_set_point_update_function(trail[i], point_function);
    }

    CGL_float max_ar = 3.0f;
    CGL_float min_ar = -3.0f;
    CGL_vec3 trail_tip_pos[TRAIL_COUNT] = {0};
    CGL_vec3 trail_tip_pos_tar[TRAIL_COUNT] = {0};
    CGL_vec3 trail_tip_pos_trpt[TRAIL_COUNT * 2] = {0};
    CGL_float trail_tip_pos_thresh[TRAIL_COUNT] = {0};
    for(int i = 0; i < TRAIL_COUNT; i++) { trail_tip_pos[i] = CGL_vec3_init(CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar); trail_tip_pos_tar[i] = CGL_vec3_init(CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar); trail_tip_pos_thresh[i] = 0.0f; }
    for(int i = 0; i < TRAIL_COUNT * 2; i++) { trail_tip_pos_tar[i] = CGL_vec3_init(CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar); trail_tip_pos_thresh[i] = 0.0f; }

    CGL_vec3 trail_color[TRAIL_COUNT];
    for(int i = 0; i < TRAIL_COUNT; i++) trail_color[i] = CGL_vec3_init(CGL_utils_random_float() * 0.8f , CGL_utils_random_float() * 0.5f + 0.5f, CGL_utils_random_float() * 0.8f);

    while(!CGL_window_should_close(window))
    {
        CGL_float time = CGL_utils_get_time() - start_time;
        CGL_window_set_size(window, 600, 600); // force window size to be 600x600

        view = CGL_mat4_look_at(CGL_vec3_init(cosf(time*0.5f) * 8.0f, 8.0f, sinf(time*0.5f) *  8.0f), CGL_vec3_init(0.0f, 0.0f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));

        for(int i = 0; i < TRAIL_COUNT; i++)
        {
            // walk a random path within -1.0f to 1.0f
            CGL_vec3 v = CGL_vec3_init(0.0f, 0.0f, 0.0f);
            v.x = CGL_float_cubic_lerp(trail_tip_pos[i].x, trail_tip_pos_tar[i].x, trail_tip_pos_trpt[i * 2].x, trail_tip_pos_trpt[i*2+1].x, trail_tip_pos_thresh[i]);
            v.y = CGL_float_cubic_lerp(trail_tip_pos[i].y, trail_tip_pos_tar[i].y, trail_tip_pos_trpt[i * 2].y, trail_tip_pos_trpt[i*2+1].y, trail_tip_pos_thresh[i]);
            v.z = CGL_float_cubic_lerp(trail_tip_pos[i].z, trail_tip_pos_tar[i].z, trail_tip_pos_trpt[i * 2].z, trail_tip_pos_trpt[i*2+1].z, trail_tip_pos_thresh[i]);
            trail_tip_pos_thresh[i] += 0.005f;
            if(trail_tip_pos_thresh[i] >= 1.0f) {trail_tip_pos_thresh[i] = 0.0f;trail_tip_pos[i] = trail_tip_pos_tar[i]; trail_tip_pos_tar[i] = CGL_vec3_init(CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar); }
            CGL_trail_add_point(trail[i], v, 10.0f, 0.05f);
            CGL_trail_update(trail[i], 0.1f);
            CGL_trail_bake_mesh(trail[i]);
        }
        
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        CGL_framebuffer_bind(bloom_framebuffer);
        CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
        CGL_shader_bind(trail_shader);
        for(CGL_int i = 0; i < TRAIL_COUNT; i++)
        {
            CGL_shader_set_uniform_vec3v(trail_shader, CGL_shader_get_uniform_location(trail_shader, "color"), trail_color[i].x, trail_color[i].y, trail_color[i].z);
            CGL_trail_render(trail[i], &view, &projection, trail_shader);
        }



        CGL_bloom_apply(bloom, CGL_framebuffer_get_color_texture(bloom_framebuffer));

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shader_bind(present_shader);
        CGL_texture_bind(CGL_framebuffer_get_color_texture(bloom_framebuffer), 0);
        CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 0);
        CGL_gl_render_screen_quad();

        CGL_window_poll_events(window);
        CGL_window_swap_buffers(window);
    }


    for(int i = 0; i < TRAIL_COUNT; i++) CGL_trail_destroy(trail[i]);
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
