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
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_RAY_CASTER
#define CGL_EXCLUDE_NODE_EDITOR
#define CGL_EXCLUDE_WIDGETS
#include "cgl.h"

#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif

#define TRAIL_COUNT 5
#define DECORATION_COUNT 64

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
"precision highp int;\n"
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
"out vec3 Normal;\n"
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
"	Distance = normal.w;\n"
"	Life = position.w;\n"
"	Normal = normal.xyz;\n"
"}\n";

static const char* TRAIL_FRAGMENT_SHADER = 
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
"in vec3 Normal;\n"
"in float Distance;\n"
"in float Life;\n"
"\n"
"uniform vec3 color;\n"
"uniform float total_length;\n"
"\n"
"void main()\n"
"{\n"
"	FragColor = vec4(mix(color * 0.5f, vec3(0.0f), (10.0f - Life) / 300.0f), 1.0f);\n"
"}\n";

static const char* DECORATIONS_VERTEX_SHADER = 
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
"out vec3 Normal;\n"
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
"	Normal = normal.xyz;\n"
"}\n";

static const char* DECORATIONS_FRAGMENT_SHADER = 
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
"in vec3 Normal;\n"
"\n"
"uniform vec3 color;\n"
"\n"
"void main()\n"
"{\n"
"   vec3 lightPos = vec3(12.0f, 12.0f, 12.0f);\n"
"   vec3 lightDir = normalize(lightPos - Position);\n"
"   float diffuse = clamp(max(dot(Normal, lightDir), 0.0f), 0.0f, 1.0f);\n"
"	FragColor = vec4(color * 1.5f * (diffuse * 0.5f + 0.5f), 1.0f);\n"
"}\n";


typedef struct
{
    CGL_mat4 transform;
    CGL_vec3 color;
    CGL_mesh_gpu* mesh;
} Decoration;

struct {
    CGL_window* window;
    CGL_framebuffer* default_framebuffer;
    CGL_framebuffer* bloom_framebuffer;

    CGL_shader* present_shader;
    CGL_shader* trail_shader;
    CGL_shader* decoration_shader;

    CGL_mat4 projection;
    CGL_mat4 view;

    CGL_mesh_gpu* sphere_mesh_gpu;
    CGL_mesh_gpu* cube_mesh_gpu;
    CGL_mesh_gpu* star_mesh_gpu;

    CGL_trail* trail[TRAIL_COUNT];
    CGL_vec3 trail_color[TRAIL_COUNT];
    Decoration decorations[DECORATION_COUNT];

    CGL_bool started;

    CGL_audio_context* audio_context;
    CGL_audio_source* audio_source;
    CGL_audio_buffer* audio_buffer;

    
    CGL_vec3 trail_tip_pos;
    CGL_vec3 trail_tip_pos_prev;
    CGL_float time_taken_to_reach_top;
    CGL_float max_height;

#ifndef CGL_WASM
    CGL_bloom* bloom;
#endif
} g_State;

bool point_function(CGL_trail* trail, CGL_trail_point* point)
{
    point->thickness = 0.02f *  powf((trail->length - point->distance) / trail->length, 1.5f);
    return false;
}

CGL_bool init() {
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return CGL_FALSE;
    g_State.window = CGL_window_create(600, 600, "Christmas Tree 01 - Jaysmito Mukherjee");
    if(!g_State.window) return 1;
    CGL_window_make_context_current(g_State.window);
    CGL_gl_init();
    g_State.default_framebuffer = CGL_framebuffer_create_from_default(g_State.window);
    g_State.bloom_framebuffer = CGL_framebuffer_create_basic(600, 600);

 
    g_State.present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    g_State.trail_shader = CGL_shader_create(TRAIL_VERTEX_SHADER, TRAIL_FRAGMENT_SHADER, NULL);
    g_State.decoration_shader = CGL_shader_create(DECORATIONS_VERTEX_SHADER, DECORATIONS_FRAGMENT_SHADER, NULL);

    g_State.projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    g_State.view = CGL_mat4_identity();

#ifndef CGL_WASM
    g_State.bloom = CGL_bloom_create(600, 600, 3);
#endif

    CGL_mesh_cpu* sphere_mesh_cpu = CGL_mesh_cpu_sphere(16, 16);
    g_State.sphere_mesh_gpu = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(g_State.sphere_mesh_gpu, sphere_mesh_cpu, true);
    CGL_mesh_cpu_destroy(sphere_mesh_cpu);

    CGL_mesh_cpu* cube_mesh_cpu = CGL_mesh_cpu_cube(false);
    CGL_mesh_cpu_recalculate_normals(cube_mesh_cpu);
    g_State.cube_mesh_gpu = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(g_State.cube_mesh_gpu, cube_mesh_cpu, true);
    CGL_mesh_cpu_destroy(cube_mesh_cpu);

    // mesh from https://sketchfab.com/3d-models/star-f7a96ca9ce3c4a0ba622088fffffa51c
    CGL_mesh_cpu* star_mesh_cpu = CGL_mesh_cpu_load_obj("assets/star.obj");
    CGL_mesh_cpu_recalculate_normals(star_mesh_cpu);
    g_State.star_mesh_gpu = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(g_State.star_mesh_gpu, star_mesh_cpu, true);
    CGL_mesh_cpu_destroy(star_mesh_cpu);


    for(int i = 0; i < TRAIL_COUNT; i++)
    {
        g_State.trail[i] = CGL_trail_create();
        CGL_trail_set_min_points_distance(g_State.trail[i], 0.005f);
        CGL_trail_set_resolution(g_State.trail[i], 32);
        CGL_trail_set_point_update_function(g_State.trail[i], point_function);
    }

    g_State.trail_tip_pos = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    g_State.trail_tip_pos_prev = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    g_State.time_taken_to_reach_top = 8.0f;
    g_State.max_height = 8.0f;

    g_State.started = false;

    for(CGL_int i = 0; i < TRAIL_COUNT; i++) {
        g_State.trail_color[i] = CGL_vec3_init(
            CGL_utils_random_float() * 0.3f,
            CGL_utils_random_float() * 0.5f + 0.5f,
            CGL_utils_random_float() * 0.3f
        );
    }

    for(CGL_int i = 0; i < DECORATION_COUNT ; i++)
    {
        CGL_float height_f = 0.0f;
        // this is a very bad way to do this, but it works for now so .....
        while(!(height_f >= 0.15f && height_f <= 0.85f)) height_f = powf(CGL_E, -6.0f * (CGL_utils_random_float() * 0.7f));
        CGL_float angle = CGL_utils_random_float() * 2.0f * CGL_PI;
        CGL_vec3 pos = CGL_vec3_init(cosf(angle) * 3.0f * (1.0f - height_f), height_f * g_State.max_height, sinf(angle) * 3.0f * (1.0f - height_f));
        CGL_float scalef = CGL_utils_random_float() * 0.3f + 0.85f;
        CGL_vec3 scale = CGL_vec3_init(0.1f * scalef, 0.1f * scalef, 0.1f * scalef);
        g_State.decorations[i].transform = CGL_mat4_translate(pos.x, pos.y, pos.z);
        CGL_mat4 tmp = CGL_mat4_scale(scale.x, scale.y, scale.z);
        g_State.decorations[i].transform = CGL_mat4_mul(g_State.decorations[i].transform, tmp);
        g_State.decorations[i].mesh = rand() % 2 == 0 ? g_State.sphere_mesh_gpu : g_State.cube_mesh_gpu;
        g_State.decorations[i].color = CGL_vec3_init(CGL_utils_random_float(), CGL_utils_random_float(), CGL_utils_random_float());
    }

    g_State.audio_context = CGL_audio_context_create(NULL);
    CGL_audio_make_context_current(g_State.audio_context);
    g_State.audio_source = CGL_audio_source_create();
    g_State.audio_buffer = CGL_audio_buffer_create();
    CGL_wav_file wav_file;

    // i do not own the audio file so i cannot share
    CGL_wav_file_load(&wav_file, "assets/cristmas_wish.wav");
    CGL_audio_buffer_set_data_from_wav_file(g_State.audio_buffer, &wav_file);
    CGL_audio_source_set_buffer(g_State.audio_source, g_State.audio_buffer);
    CGL_wav_file_destroy(&wav_file);

    return CGL_TRUE;
}

CGL_void cleanup() {
    CGL_audio_make_context_current(NULL);
    for(int i = 0; i < TRAIL_COUNT; i++) CGL_trail_destroy(g_State.trail[i]);
    CGL_audio_context_destroy(g_State.audio_context);
    CGL_audio_source_destroy(g_State.audio_source);
    CGL_audio_buffer_destroy(g_State.audio_buffer);
    CGL_mesh_gpu_destroy(g_State.sphere_mesh_gpu);
    CGL_mesh_gpu_destroy(g_State.cube_mesh_gpu);
    CGL_mesh_gpu_destroy(g_State.star_mesh_gpu);
    CGL_shader_destroy(g_State.trail_shader);
    CGL_shader_destroy(g_State.decoration_shader);
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

EM_BOOL loop(double _time, void* userData) {
    (void)userData;
    (void)_time;


    static CGL_float curr_time = 0.0f;
    static CGL_float prev_time = 0.0f;
    static CGL_float time = 0.0f;

    curr_time = CGL_utils_get_time();
    CGL_float delta_time = curr_time - prev_time;
    prev_time = curr_time;
     
    if(g_State.started) time += CGL_utils_clamp(delta_time, 0.0f, 0.05f); // to avoid sudden jumps in time due to lag or things like window resizing
     
    CGL_window_set_size(g_State.window, 600, 600); // force window size to be 600x600

    g_State.view = CGL_mat4_look_at(CGL_vec3_init(8.0f * cosf(time), 12.0f, 8.0f * sinf(time)), CGL_vec3_init(0.0f, 4.5f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));

    CGL_float factor = time / g_State.time_taken_to_reach_top;
    if(factor < 1.0f && g_State.started)
        {
        CGL_float ifactor = 1.0f - factor;
        CGL_float rot_speed = 18.0f;
        g_State.trail_tip_pos = CGL_vec3_init(cosf(time*rot_speed) * 3.0f * ifactor, g_State.max_height * factor, sinf(time*rot_speed) * 3.0f * ifactor);
        CGL_vec3 trail_tip_vec = CGL_vec3_sub(g_State.trail_tip_pos, g_State.trail_tip_pos_prev);
        g_State.trail_tip_pos_prev = g_State.trail_tip_pos;
        CGL_vec3 front = CGL_vec3_init(0.0f, 0.0f, 1.0f);
        CGL_vec3 top = CGL_vec3_init(1.0f, 0.0f, 0.0f);
        CGL_vec3_calculate_orthonormal_basis_from_one_vector(trail_tip_vec, &front, &top);
        CGL_float angle_step = CGL_deg_to_rad(360.0f / TRAIL_COUNT);
        for(int i = 0; i < TRAIL_COUNT; i++)            {
            CGL_float thickness = 0.15f + sinf(time) * 0.05f;
            thickness *= powf(ifactor, 1.0f / 3.0f);
            CGL_float phase = time * 8.0f;
            CGL_float ct = cosf(angle_step * i + phase) * thickness; CGL_float st = sinf(angle_step * i + phase) * thickness;
            CGL_vec3 v = CGL_vec3_add_(g_State.trail_tip_pos, CGL_vec3_add_(CGL_vec3_scale_(front, ct), CGL_vec3_scale_(top, st)));

            CGL_trail_add_point(g_State.trail[i], v, 300.0f, 0.1f);
            CGL_trail_update(g_State.trail[i], 0.1f);
            CGL_trail_bake_mesh(g_State.trail[i]);
        }
    }
        
        
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    CGL_framebuffer_bind(g_State.bloom_framebuffer);
    CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
    CGL_shader_bind(g_State.trail_shader);
    for(CGL_int i = 0; i < TRAIL_COUNT; i++) {
        CGL_shader_set_uniform_vec3v(g_State.trail_shader, CGL_shader_get_uniform_location(g_State.trail_shader, "color"), g_State.trail_color[i].x, g_State.trail_color[i].y, g_State.trail_color[i].z);
        CGL_trail_render(g_State.trail[i], &g_State.view, &g_State.projection, g_State.trail_shader);
    }
    if(factor >= 1.0f) {
        factor = (factor - 1.0f) / 0.2f;
        factor = CGL_utils_clamp(factor, 0.0f, 1.0f);
        CGL_shader_bind(g_State.decoration_shader);
        CGL_shader_set_uniform_mat4(g_State.decoration_shader, CGL_shader_get_uniform_location(g_State.decoration_shader, "view"), &g_State.view);
        CGL_shader_set_uniform_mat4(g_State.decoration_shader, CGL_shader_get_uniform_location(g_State.decoration_shader, "projection"), &g_State.projection);
        CGL_mat4 temp = CGL_mat4_scale(factor, factor, factor);
        CGL_mat4 temp2 = CGL_mat4_identity();
        for(CGL_int i = 0; i < DECORATION_COUNT; i++) {
            temp2 = CGL_mat4_mul(g_State.decorations[i].transform, temp);
            CGL_shader_set_uniform_mat4(g_State.decoration_shader, CGL_shader_get_uniform_location(g_State.decoration_shader, "model"), &temp2);
            CGL_shader_set_uniform_vec3v(g_State.decoration_shader, CGL_shader_get_uniform_location(g_State.decoration_shader, "color"), g_State.decorations[i].color.x, g_State.decorations[i].color.y, g_State.decorations[i].color.z);
            CGL_mesh_gpu_render(g_State.decorations[i].mesh);
        }
        CGL_shader_set_uniform_vec3v(g_State.decoration_shader, CGL_shader_get_uniform_location(g_State.decoration_shader, "color"), 1.0f, 1.0f, 0.0f);
        factor *= 0.3f;

        temp = CGL_mat4_translate(0.0f, g_State.max_height + 0.1f, 0.09f);
        temp2 = CGL_mat4_scale(factor, factor, -factor);
        temp = CGL_mat4_mul(temp, temp2);
        CGL_shader_set_uniform_mat4(g_State.decoration_shader, CGL_shader_get_uniform_location(g_State.decoration_shader, "model"), &temp);
        CGL_mesh_gpu_render(g_State.star_mesh_gpu);

        temp = CGL_mat4_translate(0.0f, g_State.max_height + 0.1f, -0.09f);
        temp2 = CGL_mat4_scale(factor, factor, factor);
        temp = CGL_mat4_mul(temp, temp2);
        CGL_shader_set_uniform_mat4(g_State.decoration_shader, CGL_shader_get_uniform_location(g_State.decoration_shader, "model"), &temp);
        CGL_mesh_gpu_render(g_State.star_mesh_gpu);
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

    if(!g_State.started && CGL_window_get_key(g_State.window, CGL_KEY_SPACE) == CGL_PRESS) { g_State.started = true; CGL_audio_source_play(g_State.audio_source);}

    return CGL_TRUE;
}

int main()
{
    if(!init()) return EXIT_FAILURE;
#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while(!CGL_window_should_close(g_State.window)) {
        if(!loop(0.0, NULL)) break;
    }
    cleanup();
#endif
    return EXIT_SUCCESS;
}
