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
#define CGL_EXCLUDE_AUDIO
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_EXCLUDE_WIDGETS
#include "cgl.h"

#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif

// Update shaders for WebGL compatibility
static const char* PASS_THROUGH_VERTEX_SHADER_SOURCE =
#ifdef CGL_WASM
"#version 300 es\n"
"precision highp float;\n"
"precision highp int;\n"
"\n"
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
"precision highp int;\n"
"\n"
"in vec4 position;\n" // w is lifespan
"in vec4 normal;\n" // w is distance
"in vec4 texcoord;\n" // zw is reserved for future use
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
"uniform mat4 model;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4(position.xyz, 1.0f);\n"
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
"uniform float total_life;\n"
"uniform float LifeFactor;\n"
"\n"
"void main()\n"
"{\n"
"   float factor = (total_life - (Life+LifeFactor)) / total_life;\n"
"   factor = pow(factor, 4.0f);\n"
"	FragColor = vec4(mix(color, vec3(0.0f), factor), 1.0f);\n"
"}\n";

typedef struct
{
    CGL_trail* particle_trails[64];
    CGL_vec3 particle_positions[64];
    CGL_vec3 particle_velocities[64];
    CGL_vec3 particle_colors[64];
    CGL_float particle_timers[64];
    CGL_trail* trail[5];
    CGL_vec3 velocity;
    CGL_vec3 color;
    CGL_vec3 position;
    CGL_int particle_trails_count;
    CGL_float timer;
    CGL_float wtimer;
    CGL_int busted_smallones;
} Firework;

#define SMALL_FIRE_WORKS_COUNT 32

Firework smaller_fireworks[SMALL_FIRE_WORKS_COUNT];

static bool point_function(CGL_trail* trail, CGL_trail_point* point)
{
    point->thickness = 0.1f * (trail->length - point->distance) / trail->length;
    return false;
}

void allocate_small_firework(Firework* firework)
{
    for(CGL_int i = 0 ; i < 5 ; i ++) firework->trail[i] = CGL_trail_create();
    firework->particle_trails_count = CGL_utils_random_int(16, 32);
    for(CGL_int i = 0 ; i < firework->particle_trails_count ; i ++) firework->particle_trails[i] = CGL_trail_create();  
}

void setup_small_firework(Firework* firework, CGL_float xd)
{
    firework->wtimer = CGL_utils_random_float() * 20.0f;
    firework->position = CGL_vec3_init(xd, 0.0f, CGL_utils_random_float() * 10.0f - 5.0f);
    firework->velocity = CGL_vec3_init(CGL_utils_random_float() * 4.0f - 2.0f, CGL_utils_random_float() * 5.0f + 20.0f, CGL_utils_random_float() * 4.0f - 2.0f);
    firework->timer = firework->velocity.y / 9.81f;
    firework->color = CGL_vec3_init(CGL_utils_random_float(), CGL_utils_random_float(), CGL_utils_random_float());
    firework->busted_smallones = 0;
    for(CGL_int i = 0 ; i < 5 ; i ++)
    {
        CGL_trail_set_min_points_distance(firework->trail[i], 0.07f);
        CGL_trail_set_resolution(firework->trail[i], 3);
        CGL_trail_set_point_update_function(firework->trail[i], point_function);
    }
    for(CGL_int i = 0 ; i < firework->particle_trails_count ; i ++)
    {
        CGL_trail_clear(firework->particle_trails[i]);
        CGL_trail_set_min_points_distance(firework->particle_trails[i], 0.07f);
        CGL_trail_set_resolution(firework->particle_trails[i], 3);
        CGL_trail_set_point_update_function(firework->particle_trails[i], point_function);
        firework->particle_positions[i] = CGL_vec3_init(xd, 0.0f, 5.0f + CGL_utils_random_float() * 2.0f - 1.0f);
        CGL_float smx = 5.0f;
        CGL_float smn = -5.0f;
        firework->particle_velocities[i] = CGL_vec3_init(CGL_utils_random_float() * (smx - smn) + smn, CGL_utils_random_float() * (smx - smn) + smn, CGL_utils_random_float() * (smx - smn) + smn);
        firework->particle_timers[i] = CGL_utils_random_float() * 1.0f + 1.0f;
        firework->particle_colors[i] = CGL_vec3_add_(firework->color, CGL_vec3_init(CGL_utils_random_float() * 0.2f - 0.1f, CGL_utils_random_float() * 0.2f - 0.1f, CGL_utils_random_float() * 0.2f - 0.1f));
    }
}

void destroy_small_firework(Firework* firework);
void update_small_firework(Firework* firework, CGL_float delta_time)
{
    firework->wtimer -= delta_time;
    if(firework->wtimer >= 0.0f) return;
    if(firework->busted_smallones >= firework->particle_trails_count) setup_small_firework(firework, CGL_utils_random_float() * 32.0f - 16.0f);
    firework->timer -= delta_time;
    if(fabsf(firework->timer) < 0.05f) for(CGL_int i = 0 ; i < firework->particle_trails_count ; i ++) firework->particle_positions[i] = firework->position;
    static CGL_float tot_time = 0.0f;
    tot_time += delta_time;
    if(firework->timer >= 0.0f)
    {
        firework->position = CGL_vec3_add_(firework->position, CGL_vec3_scale_(firework->velocity, delta_time));
        firework->velocity = CGL_vec3_add_(firework->velocity, CGL_vec3_scale_(CGL_vec3_init(0.0f, -9.8f, 0.0f), delta_time));
        firework->velocity.y = CGL_utils_clamp(firework->velocity.y, 0.0f, 100.0f);
        for(CGL_int i = 0 ; i < 5 ; i ++)
        {
            CGL_float angle = (CGL_float)i * 2.0f * CGL_PI / 5.0f + tot_time * 10.0f;
            CGL_vec3 direction = CGL_vec3_init(cosf(angle), 0.0f, sinf(angle));
            CGL_vec3 position = CGL_vec3_add_(firework->position, CGL_vec3_scale_(direction, 0.08f));
            CGL_trail_add_point(firework->trail[i], position, 0.15f, 0.07f);
            CGL_trail_update(firework->trail[i], delta_time);
            CGL_trail_bake_mesh(firework->trail[i]);
        }
    }
    else
    {
        for(CGL_int i = 0 ; i < firework->particle_trails_count ; i ++)
        {
            firework->particle_timers[i] -= delta_time;
            if(fabsf(firework->particle_timers[i]) < 0.03f) firework->busted_smallones ++;
            if(firework->particle_timers[i] <= 0.0f) continue;
            firework->particle_positions[i] = CGL_vec3_add_(firework->particle_positions[i], CGL_vec3_scale_(firework->particle_velocities[i], delta_time));
            firework->particle_velocities[i] = CGL_vec3_add_(firework->particle_velocities[i], CGL_vec3_scale_(CGL_vec3_init(0.0f, -4.8f, 0.0f), delta_time));
            CGL_trail_add_point(firework->particle_trails[i], firework->particle_positions[i], 0.7f, 0.01f);
            CGL_trail_update(firework->particle_trails[i], delta_time);
            CGL_trail_bake_mesh(firework->particle_trails[i]);
        }
    }
}

void render_small_firework(Firework* firework, CGL_shader* shader, CGL_mat4* view, CGL_mat4* projection)
{
    if(firework->wtimer >= 0.0f) return;
    CGL_vec3 orange_red_shades[5] = {
        CGL_vec3_init(1.5f, 0.5f+ 0.3f, 0.0f),
        CGL_vec3_init(1.5f, 0.4f+ 0.3f, 0.0f),
        CGL_vec3_init(1.5f, 0.3f+ 0.3f, 0.0f),
        CGL_vec3_init(1.5f, 0.2f+ 0.3f, 0.0f),
        CGL_vec3_init(1.5f, 0.1f+ 0.3f, 0.0f)
    };
    CGL_mat4 model = CGL_mat4_identity();
    CGL_shader_set_uniform_float(shader, CGL_shader_get_uniform_location(shader, "LifeFactor"), 0.0f);
    CGL_shader_set_uniform_mat4(shader, CGL_shader_get_uniform_location(shader, "model"), &model);

    if(firework->timer >= 0)
    {
        for(CGL_int i = 0 ; i < 5 ; i ++)
        {
            CGL_shader_set_uniform_vec3v(shader, CGL_shader_get_uniform_location(shader, "color"), orange_red_shades[i].x, orange_red_shades[i].y, orange_red_shades[i].z);
            CGL_shader_set_uniform_float(shader, CGL_shader_get_uniform_location(shader, "total_life"), 0.15f);
            CGL_trail_render(firework->trail[i], view, projection, shader);
        }
    }
    else
    {
        for(CGL_int i = 0 ; i < firework->particle_trails_count ; i ++)
        {
            if(firework->particle_timers[i] <= 0.0f) continue;
            CGL_shader_set_uniform_vec3v(shader, CGL_shader_get_uniform_location(shader, "color"), firework->particle_colors[i].x, firework->particle_colors[i].y, firework->particle_colors[i].z);
            CGL_shader_set_uniform_float(shader, CGL_shader_get_uniform_location(shader, "total_life"), 0.7f);
            CGL_trail_render(firework->particle_trails[i], view, projection, shader);
        }
    }
}

void destroy_small_firework(Firework* firework)
{
    for(CGL_int i = 0 ; i < 5 ; i ++) CGL_trail_destroy(firework->trail[i]);
    for(CGL_int i = 0 ; i < firework->particle_trails_count ; i ++) CGL_trail_destroy(firework->particle_trails[i]);
}

// Global state structure
struct {
    CGL_window* window;
    CGL_framebuffer* default_framebuffer;
    CGL_framebuffer* bloom_framebuffer;
    CGL_shader* present_shader;
    CGL_shader* trail_shader;
    CGL_mat4 view;
    CGL_mat4 projection;
    CGL_float curr_time;
    CGL_float prev_time;
    CGL_float time;
    CGL_bool started;
    Firework smaller_fireworks[SMALL_FIRE_WORKS_COUNT];

#ifndef CGL_WASM
    CGL_bloom* bloom;
#endif
} g_state;

CGL_bool init()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return false;
    
    g_state.window = CGL_window_create(600, 600, "Fireworks - Jaysmito Mukherjee");
    if(!g_state.window) return false;
    
    CGL_window_make_context_current(g_state.window);
    if(!CGL_gl_init()) return false;

    g_state.default_framebuffer = CGL_framebuffer_create_from_default(g_state.window);
    g_state.bloom_framebuffer = CGL_framebuffer_create_basic(600, 600);
    g_state.present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    g_state.trail_shader = CGL_shader_create(TRAIL_VERTEX_SHADER, TRAIL_FRAGMENT_SHADER, NULL);

#ifndef CGL_WASM
    g_state.bloom = CGL_bloom_create(600, 600, 3);
#endif

    g_state.curr_time = CGL_utils_get_time();
    g_state.prev_time = CGL_utils_get_time();
    g_state.time = 0.0f;
    g_state.started = false;

    for(CGL_int i = 0; i < SMALL_FIRE_WORKS_COUNT; i++) allocate_small_firework(&g_state.smaller_fireworks[i]);
    for(CGL_int i = 0; i < SMALL_FIRE_WORKS_COUNT; i++) setup_small_firework(&g_state.smaller_fireworks[i], 32 * (float)i / SMALL_FIRE_WORKS_COUNT - 32*0.5f);

    g_state.projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    g_state.view = CGL_mat4_identity();

    return true;
}

void cleanup()
{
    for(int i = 0; i < SMALL_FIRE_WORKS_COUNT; i++) destroy_small_firework(&g_state.smaller_fireworks[i]);
    CGL_shader_destroy(g_state.trail_shader);
    CGL_shader_destroy(g_state.present_shader);
    CGL_framebuffer_destroy(g_state.default_framebuffer);
    CGL_framebuffer_destroy(g_state.bloom_framebuffer);
#ifndef CGL_WASM    
    CGL_bloom_destroy(g_state.bloom);
#endif
    CGL_gl_shutdown();
    CGL_window_destroy(g_state.window);    
    CGL_shutdown();
}

EM_BOOL loop(double time, void* userData)
{
    (void)time;
    (void)userData;

    g_state.curr_time = CGL_utils_get_time();
    CGL_float delta_time = g_state.curr_time - g_state.prev_time;
    delta_time = CGL_utils_clamp(delta_time, 0.0f, 0.03f);
    g_state.prev_time = g_state.curr_time;
    g_state.time += delta_time;

    CGL_window_set_size(g_state.window, 600, 600);
    g_state.view = CGL_mat4_look_at(
        CGL_vec3_init(60.0f * cosf(g_state.time*0.3f), 15.0f, 60.0f * sinf(g_state.time*0.3f)), 
        CGL_vec3_init(0.0f, 16.0f, 0.0f), 
        CGL_vec3_init(0.0f, 1.0f, 0.0f)
    );

    if(g_state.started) {
        for(CGL_int i = 0; i < SMALL_FIRE_WORKS_COUNT; i++) 
            update_small_firework(&g_state.smaller_fireworks[i], delta_time);
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Render to bloom framebuffer
    CGL_framebuffer_bind(g_state.bloom_framebuffer);
    CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
    CGL_shader_bind(g_state.trail_shader);
    for(CGL_int i = 0; i < SMALL_FIRE_WORKS_COUNT; i++) 
        render_small_firework(&g_state.smaller_fireworks[i], g_state.trail_shader, &g_state.view, &g_state.projection);
#ifndef CGL_WASM
    // Apply bloom and render to screen
    CGL_bloom_apply(g_state.bloom, CGL_framebuffer_get_color_texture(g_state.bloom_framebuffer));
#endif
    
    CGL_framebuffer_bind(g_state.default_framebuffer);
    CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
    CGL_shader_bind(g_state.present_shader);
    CGL_texture_bind(CGL_framebuffer_get_color_texture(g_state.bloom_framebuffer), 0);
    CGL_shader_set_uniform_int(g_state.present_shader, CGL_shader_get_uniform_location(g_state.present_shader, "u_tex"), 0);
    CGL_gl_render_screen_quad();

    CGL_window_poll_events(g_state.window);
    CGL_window_swap_buffers(g_state.window);

    if(!g_state.started && CGL_window_get_key(g_state.window, CGL_KEY_SPACE) == CGL_PRESS) 
        g_state.started = true;

    return !CGL_window_should_close(g_state.window);
}

int main()
{
    if(!init()) return 1;

#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while (!CGL_window_should_close(g_state.window)) {
        loop(0, NULL);
    }
    cleanup();
#endif

    return 0;
}
