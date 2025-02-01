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
#define CGL_IMPLEMENTATION
#include "cgl.h"


#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif


static const char* VS_main_source = 
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
"in layout(location = 0) vec4 position;\n"
"in layout(location = 1) vec4 normal;\n"
"in layout(location = 2) vec4 texcoord;\n"
#endif
"\n"
"\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform float time;\n"
"uniform float sound_level;\n"
"\n"
"\n"
"out vec3 Position;\n"
"out float Disto;\n"
"\n"
"mat4 translate(vec3 v)\n"
"{\n"
"    mat4 m = mat4(1.0f);\n"
"    m[3] = vec4(v, 1.0f);\n"
"    return m;\n"
"}\n"
"\n"
"mat4 scale(float scale)\n"
"{\n"
"    scale = clamp(scale, 0.0f, 1.0f);\n"
"    mat4 m = mat4(1.0f);\n"
"    m[0][0] = scale;\n"
"    m[1][1] = scale;\n"
"    m[2][2] = scale;\n"
"    return m;\n"
"}\n"
"\n"
"vec2 hash( vec2 p ) // replace this by something better\n"
"{\n"
"	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );\n"
"	return -1.0 + 2.0*fract(sin(p)*43758.5453123);\n"
"}\n"
"\n"
"float noise( in vec2 p )\n"
"{\n"
"    const float K1 = 0.366025404; // (sqrt(3)-1)/2;\n"
"    const float K2 = 0.211324865; // (3-sqrt(3))/6;\n"
"\n"
"	vec2  i = floor( p + (p.x+p.y)*K1 );\n"
"    vec2  a = p - i + (i.x+i.y)*K2;\n"
"    float m = step(a.y,a.x); \n"
"    vec2  o = vec2(m,1.0-m);\n"
"    vec2  b = a - o + K2;\n"
"	vec2  c = a - 1.0 + 2.0*K2;\n"
"    vec3  h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );\n"
"	vec3  n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));\n"
"    return dot( n, vec3(70.0) );\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    int x_id = gl_InstanceID % 100;\n"
"    int y_id = gl_InstanceID / 100;\n"
"    float x_o = float(x_id - 50) / 50.0f;\n"
"    float y_o = float(y_id - 50) / 50.0f;\n"
"    vec3 offset = vec3(x_o, 0.0f, y_o);\n"
"    float disto = (x_o * x_o + y_o * y_o);\n"
"    float dist = pow((1.0f - disto) * 0.6f, 5.0f);\n"
"    if(dist > 0.001f) dist += 0.01f;\n"
"    Disto = dist * 100.0f;\n"
"    offset *=  5.0f;\n"
"    offset.y = pow(sin(disto * 16.0f + time * 2.0f), 2.0f) * 8.0f * dist;\n"
"    offset.y += offset.y * sound_level * 4.0f;\n"
"    offset.y += noise(vec2(x_o, y_o) * 10.0f) * sound_level;\n"
"    mat4 model = translate(offset) * scale(dist);\n"
"    vec4 pos = projection * view * model * vec4(position.xyz, 1.0f);\n"
"    gl_Position = pos;\n"
"    Position = offset;\n"
"}";

static const char* FS_main_source = 
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
"in float Disto;\n"
"\n"
"uniform float time;\n"
"\n"
"void main()\n"
"{\n"
"    vec3 top_color = vec3(0.8, 0.8, 0.0);\n"
"    vec3 bottom_color = vec3(0.8, 0.1, 0.0);\n"
"    vec3 color = mix(bottom_color, top_color, pow(Position.y, 5.0f));\n"
"    vec3 back_color = vec3(0.2, 0.2, 0.2);\n"
"    FragColor = vec4(color * 1.5f, Disto);\n"
"}";

static const char* PASS_THROUGH_VERTEX_SHADER_SOURCE = 
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

struct {
    CGL_wav_file current_wav_file;
    CGL_bool is_playing;
    CGL_bool is_loaded;

    CGL_window* window;

    CGL_framebuffer* default_framebuffer;
    CGL_framebuffer* bloom_framebuffer;

    CGL_audio_context* audio_context;
    CGL_audio_source* audio_source;
    CGL_audio_buffer* audio_buffer;

    CGL_mesh_gpu* mesh_gpu;

    CGL_shader* shader;
    CGL_shader* present_shader;

    CGL_mat4 projection;
    CGL_mat4 view;

    CGL_float start_time;
    CGL_float current_sound_level;

#ifndef CGL_WASM
    CGL_bloom* bloom;
#endif
} g_State;

#ifdef CGL_WASM
EMSCRIPTEN_KEEPALIVE
#endif
CGL_bool load_file(const CGL_byte* path) {
    if(g_State.is_loaded) {
        CGL_wav_file_destroy(&g_State.current_wav_file);
    }
    g_State.is_loaded = false;

    if(!CGL_wav_file_load(&g_State.current_wav_file, path)) {
        CGL_error("Failed to load wav file: %s", path);
        return CGL_FALSE;
    } else {
        CGL_info("Loaded wav file: %s", path);
    }
    g_State.is_loaded = true;
    g_State.is_playing = false;
    return CGL_TRUE;
}


void on_drop_file(CGL_window* window, const CGL_byte** paths, CGL_int count)
{
    (void)window;
    (void)count;

    if(!load_file(paths[0])) {
        CGL_error("Failed to load file: %s", paths[0]);
    }
}

CGL_bool init() {
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return CGL_FALSE;
    g_State.window = CGL_window_create(600, 600, "Audio Visualizer - Jaysmito Mukherjee");
    if(!g_State.window) return 1;
    CGL_window_make_context_current(g_State.window);
    if(!CGL_gl_init()) return CGL_FALSE;
    CGL_window_set_drag_n_drop_callback(g_State.window, on_drop_file);

    g_State.default_framebuffer = CGL_framebuffer_create_from_default(g_State.window);
    g_State.bloom_framebuffer = CGL_framebuffer_create_basic(600, 600);
 
    g_State.audio_context = CGL_audio_context_create(NULL);
    CGL_audio_make_context_current(g_State.audio_context);
 
    g_State.audio_source = CGL_audio_source_create();
    g_State.audio_buffer = CGL_audio_buffer_create();

    CGL_mesh_cpu* mesh = CGL_mesh_cpu_sphere(8, 8);
    g_State.mesh_gpu = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(g_State.mesh_gpu, mesh, false);
    CGL_mesh_cpu_destroy(mesh);

    g_State.shader = CGL_shader_create(VS_main_source, FS_main_source, NULL);
    g_State.present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);

    g_State.projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    g_State.view = CGL_mat4_identity();

#ifndef CGL_WASM
    g_State.bloom = CGL_bloom_create(600, 600, 3);
#endif

    g_State.start_time = CGL_utils_get_time();
    g_State.current_sound_level = 0.0f;

    return CGL_TRUE;
}

CGL_void cleanup() {
    if(g_State.is_loaded) {
        CGL_wav_file_destroy(&g_State.current_wav_file);
    }
    CGL_shader_destroy(g_State.shader);
    CGL_shader_destroy(g_State.present_shader);
    CGL_mesh_gpu_destroy(g_State.mesh_gpu);
    CGL_audio_buffer_destroy(g_State.audio_buffer);
    CGL_audio_source_destroy(g_State.audio_source);
    CGL_audio_make_context_current(NULL);
    CGL_audio_context_destroy(g_State.audio_context);
    CGL_framebuffer_destroy(g_State.default_framebuffer);
    CGL_framebuffer_destroy(g_State.bloom_framebuffer);
#ifndef CGL_WASM
    CGL_bloom_destroy(g_State.bloom);
#endif
    CGL_gl_shutdown();
    CGL_window_destroy(g_State.window);    
    CGL_shutdown();
}

EM_BOOL loop(double time, void* userData) {
    (void)time;
    (void)userData;


    CGL_float curr_time = CGL_utils_get_time() - g_State.start_time;
    CGL_window_set_size(g_State.window, 600, 600); // force window size to be 600x600

    g_State.view = CGL_mat4_look_at(CGL_vec3_init(0.0f, 8.0f, 8.0f), CGL_vec3_init(0.0f, 0.0f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));

    CGL_framebuffer_bind(g_State.bloom_framebuffer);
    CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
    CGL_shader_bind(g_State.shader);
    CGL_shader_set_uniform_mat4(g_State.shader, CGL_shader_get_uniform_location(g_State.shader, "projection"), &g_State.projection);
    CGL_shader_set_uniform_mat4(g_State.shader, CGL_shader_get_uniform_location(g_State.shader, "view"), &g_State.view);
    CGL_shader_set_uniform_float(g_State.shader, CGL_shader_get_uniform_location(g_State.shader, "time"), curr_time);
    CGL_shader_set_uniform_float(g_State.shader, CGL_shader_get_uniform_location(g_State.shader, "sound_level"), g_State.current_sound_level);
    CGL_mesh_gpu_render_instanced(g_State.mesh_gpu, 10000);
#ifndef CGL_WASM
    CGL_bloom_apply(g_State.bloom, CGL_framebuffer_get_color_texture(g_State.bloom_framebuffer));
#endif

    CGL_framebuffer_bind(g_State.default_framebuffer);
    CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
    CGL_shader_bind(g_State.present_shader);
    CGL_texture_bind(CGL_framebuffer_get_color_texture(g_State.bloom_framebuffer), 0);
    CGL_shader_set_uniform_int(g_State.present_shader, CGL_shader_get_uniform_location(g_State.present_shader, "u_tex"), 0);
    CGL_gl_render_screen_quad();


    g_State.current_sound_level = 0.0f;
    if(g_State.is_loaded && !g_State.is_playing) {
        if(CGL_audio_source_is_playing(g_State.audio_source)) CGL_audio_source_stop(g_State.audio_source);
        CGL_audio_buffer_destroy(g_State.audio_buffer);
        g_State.audio_buffer = CGL_audio_buffer_create();
        CGL_audio_buffer_set_data_from_wav_file(g_State.audio_buffer, &g_State.current_wav_file);
        CGL_audio_source_set_buffer(g_State.audio_source, g_State.audio_buffer);
        CGL_audio_source_play(g_State.audio_source);
        g_State.is_playing = true;
        CGL_info("Started Playing");
    }

    if(g_State.is_loaded && g_State.is_playing) {
        if(!CGL_audio_source_is_playing(g_State.audio_source)) {
            CGL_info("Finished playing");
            CGL_wav_file_destroy(&g_State.current_wav_file);
            g_State.is_loaded = false;
            g_State.is_playing = false;
        } else {
            CGL_float time_offset = CGL_audio_source_get_seconds_offset(g_State.audio_source);
            CGL_int sd = CGL_wav_file_sample_at_time(&g_State.current_wav_file, 0, time_offset);
            float volume = (float)sd / SHRT_MAX;
            g_State.current_sound_level = fabsf(volume * 2.0f);
        }
    }

    CGL_window_poll_events(g_State.window);
    CGL_window_swap_buffers(g_State.window);

    return CGL_TRUE;
}


CGL_int main()
{
    if(!init()) return EXIT_FAILURE;
#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while(!CGL_window_should_close(g_State.window))
    {
        if(!loop(0.0, NULL)) break;
    }
    cleanup();
#endif
    return EXIT_SUCCESS;
}
