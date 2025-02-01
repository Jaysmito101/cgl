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
#define CGL_EXCLUDE_TEXT_RENDER
#include "cgl.h"

#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else
#define EM_BOOL int
#endif

#define TRAIL_COUNT 16

static const char* PASS_THROUGH_VERTEX_SHADER_SOURCE = 
#ifdef CGL_WASM
"#version 300 es\n"
"precision highp float;\n"
"precision highp int;\n"
"in vec4 position;\n"
"in vec4 normal;\n"
"in vec4 texcoord;\n"
#else
"#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
#endif
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

static const char* PASS_THROUGH_FRAGMENT_SHADER_SOURCE =
#ifdef CGL_WASM
"#version 300 es\n"
"precision highp float;\n"
"precision highp int;\n"
#else
"#version 430 core\n"
#endif
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


static const char* TRAIL_VERTEX_SHADER = 
#ifdef CGL_WASM
"#version 300 es\n"
"precision highp float;\n"
"\n"
"in vec4 position;\n"
"in vec4 normal;\n"
"in vec4 texcoord;\n"
#else
"#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n" // w is lifespan
"layout (location = 1) in vec4 normal;\n" // w is distance
"layout (location = 2) in vec4 texcoord;\n" // zw is reserved for future use
#endif
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

static const char* TRAIL_FRAGMENT_SHADER = 
#ifdef CGL_WASM
"#version 300 es\n"
"precision highp float;\n"
#else
"#version 430 core\n"
#endif
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

static struct {
    CGL_window* window;
    CGL_framebuffer* default_framebuffer;
    CGL_framebuffer* bloom_framebuffer;
    CGL_shader* present_shader;
    CGL_shader* trail_shader;
    CGL_trail* trail[TRAIL_COUNT];
    CGL_vec3 trail_color[TRAIL_COUNT];
    CGL_float start_time;
    CGL_mat4 projection;
    CGL_mat4 view;
    CGL_vec3 trail_tip_pos[TRAIL_COUNT];
    CGL_vec3 trail_tip_pos_tar[TRAIL_COUNT];
    CGL_vec3 trail_tip_pos_trpt[TRAIL_COUNT * 2];
    CGL_float trail_tip_pos_thresh[TRAIL_COUNT];

#ifndef CGL_WASM
    CGL_bloom* bloom;
#endif
} g_State;

bool init()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return false;
    g_State.window = CGL_window_create(600, 600, "Trails - Jaysmito Mukherjee");
    if(!g_State.window) return false;
    CGL_window_make_context_current(g_State.window);
    CGL_gl_init();
    g_State.default_framebuffer = CGL_framebuffer_create_from_default(g_State.window);
    g_State.bloom_framebuffer = CGL_framebuffer_create_basic(600, 600);
    g_State.present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    g_State.trail_shader = CGL_shader_create(TRAIL_VERTEX_SHADER, TRAIL_FRAGMENT_SHADER, NULL);
    g_State.projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    g_State.view = CGL_mat4_identity();
    g_State.start_time = CGL_utils_get_time();
#ifndef CGL_WASM
    g_State.bloom = CGL_bloom_create(600, 600, 3);
#endif

    for(int i = 0; i < TRAIL_COUNT; i++)
    {
        g_State.trail[i] = CGL_trail_create();
        CGL_trail_set_min_points_distance(g_State.trail[i], 0.01f);
        CGL_trail_set_resolution(g_State.trail[i], 32);
        CGL_trail_set_point_update_function(g_State.trail[i], point_function);
    }

    CGL_float max_ar = 3.0f;
    CGL_float min_ar = -3.0f;
    for(int i = 0; i < TRAIL_COUNT; i++) { g_State.trail_tip_pos[i] = CGL_vec3_init(CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar); g_State.trail_tip_pos_tar[i] = CGL_vec3_init(CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar); g_State.trail_tip_pos_thresh[i] = 0.0f; }
    for(int i = 0; i < TRAIL_COUNT; i++) { g_State.trail_tip_pos_tar[i] = CGL_vec3_init(CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar, CGL_utils_random_float() * (max_ar - min_ar) + min_ar); g_State.trail_tip_pos_thresh[i] = 0.0f; }
    
    for(int i = 0; i < TRAIL_COUNT; i++) g_State.trail_color[i] = CGL_vec3_init(CGL_utils_random_float() * 0.8f , CGL_utils_random_float() * 0.5f + 0.5f, CGL_utils_random_float() * 0.8f);

    return true;
}

void cleanup()
{
    for(int i = 0; i < TRAIL_COUNT; i++) CGL_trail_destroy(g_State.trail[i]);
    CGL_shader_destroy(g_State.trail_shader);
    CGL_shader_destroy(g_State.present_shader);
    CGL_framebuffer_destroy(g_State.default_framebuffer);
    CGL_framebuffer_destroy(g_State.bloom_framebuffer);
#ifndef CGL_WASM
    CGL_bloom_destroy(g_State.bloom);
#endif
    CGL_gl_shutdown();
    CGL_window_destroy(g_State.window);
    CGL_shutdown();
}

EM_BOOL loop(double time, void* userData)
{
    (void)time;
    (void)userData;

    CGL_float current_time = CGL_utils_get_time() - g_State.start_time;
    CGL_window_set_size(g_State.window, 600, 600); // force window size to be 600x600

    g_State.view = CGL_mat4_look_at(CGL_vec3_init(cosf(current_time*0.5f) * 8.0f, 8.0f, sinf(current_time*0.5f) *  8.0f), CGL_vec3_init(0.0f, 0.0f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));

    for(int i = 0; i < TRAIL_COUNT; i++)
    {
        // walk a random path within -1.0f to 1.0f
        CGL_vec3 v = CGL_vec3_init(0.0f, 0.0f, 0.0f);
        v.x = CGL_float_cubic_lerp(g_State.trail_tip_pos[i].x, g_State.trail_tip_pos_tar[i].x, g_State.trail_tip_pos_trpt[i * 2].x, g_State.trail_tip_pos_trpt[i*2+1].x, g_State.trail_tip_pos_thresh[i]);
        v.y = CGL_float_cubic_lerp(g_State.trail_tip_pos[i].y, g_State.trail_tip_pos_tar[i].y, g_State.trail_tip_pos_trpt[i * 2].y, g_State.trail_tip_pos_trpt[i*2+1].y, g_State.trail_tip_pos_thresh[i]);
        v.z = CGL_float_cubic_lerp(g_State.trail_tip_pos[i].z, g_State.trail_tip_pos_tar[i].z, g_State.trail_tip_pos_trpt[i * 2].z, g_State.trail_tip_pos_trpt[i*2+1].z, g_State.trail_tip_pos_thresh[i]);
        g_State.trail_tip_pos_thresh[i] += 0.005f;
        if(g_State.trail_tip_pos_thresh[i] >= 1.0f) {g_State.trail_tip_pos_thresh[i] = 0.0f;g_State.trail_tip_pos[i] = g_State.trail_tip_pos_tar[i]; g_State.trail_tip_pos_tar[i] = CGL_vec3_init(CGL_utils_random_float() * (3.0f - (-3.0f)) + (-3.0f), CGL_utils_random_float() * (3.0f - (-3.0f)) + (-3.0f), CGL_utils_random_float() * (3.0f - (-3.0f)) + (-3.0f)); }
        CGL_trail_add_point(g_State.trail[i], v, 10.0f, 0.05f);
        CGL_trail_update(g_State.trail[i], 0.1f);
        CGL_trail_bake_mesh(g_State.trail[i]);
    }
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    CGL_framebuffer_bind(g_State.bloom_framebuffer);
    CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
    CGL_shader_bind(g_State.trail_shader);
    for(CGL_int i = 0; i < TRAIL_COUNT; i++)
    {
        CGL_shader_set_uniform_vec3v(g_State.trail_shader, CGL_shader_get_uniform_location(g_State.trail_shader, "color"), g_State.trail_color[i].x, g_State.trail_color[i].y, g_State.trail_color[i].z);
        CGL_trail_render(g_State.trail[i], &g_State.view, &g_State.projection, g_State.trail_shader);
    }

#ifndef CGL_WASM
    CGL_bloom_apply(g_State.bloom, CGL_framebuffer_get_color_texture(g_State.bloom_framebuffer));
#endif

    CGL_framebuffer_bind(g_State.default_framebuffer);
    CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
    CGL_shader_bind(g_State.present_shader);
    CGL_texture_bind(CGL_framebuffer_get_color_texture(g_State.bloom_framebuffer), 0);
    CGL_shader_set_uniform_int(g_State.present_shader, CGL_shader_get_uniform_location(g_State.present_shader, "u_tex"), 0);
    CGL_gl_render_screen_quad();

    CGL_window_poll_events(g_State.window);
    CGL_window_swap_buffers(g_State.window);

    return true;
}

int main()
{
    if(!init()) return 1;
#ifdef CGL_WASM
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while(!CGL_window_should_close(g_State.window)) {
        if(!loop(0, NULL)) break;
    }
#endif
    return 0;
}
