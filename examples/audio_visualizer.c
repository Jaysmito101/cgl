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
#include "cgl.h"

static CGL_wav_file g_wav_file = {0};
static CGL_bool g_is_playing = false;
static CGL_bool g_is_loaded = false;

static const char* VS_main_source = "#version 430 core\n"
"\n"
"in layout(location = 0) vec4 position;\n"
"in layout(location = 1) vec4 normal;\n"
"in layout(location = 2) vec4 texcoord;\n"
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
"    float x_o = (x_id - 50) / 50.0f;\n"
"    float y_o = (y_id - 50) / 50.0f;\n"
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

static const char* FS_main_source = "#version 430 core\n"
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



void on_drop_file(CGL_window* window, const CGL_byte** paths, CGL_int count)
{
    if(g_is_loaded) CGL_wav_file_destroy(&g_wav_file);
    g_is_loaded = false;
    if(!CGL_wav_file_load(&g_wav_file, paths[0]))
    {
        CGL_error("Failed to load wav file: %s", paths[0]);
        return;
    }
    else CGL_info("Loaded wav file: %s", paths[0]);
    g_is_loaded = true;
    g_is_playing = false;
}

int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(600, 600, "Audio Visualizer - Jaysmito Mukherjee");
    if(!window) return 1;
    CGL_window_make_context_current(window);
    CGL_gl_init();
    CGL_window_set_drag_n_drop_callback(window, on_drop_file);
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);
    CGL_framebuffer* bloom_framebuffer = CGL_framebuffer_create(600, 600);
 
    CGL_audio_context* audio_context = CGL_audio_context_create(NULL);
    CGL_audio_make_context_current(audio_context);
 
    CGL_audio_source* audio_source = CGL_audio_source_create();
    CGL_audio_buffer* audio_buffer = CGL_audio_buffer_create();

    CGL_mesh_cpu* mesh = CGL_mesh_cpu_sphere(8, 8);
    CGL_mesh_gpu* mesh_gpu = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(mesh_gpu, mesh, false);
    CGL_mesh_cpu_destroy(mesh);

    CGL_shader* shader = CGL_shader_create(VS_main_source, FS_main_source, NULL);
    CGL_shader* present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);

    CGL_mat4 projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    CGL_mat4 view = CGL_mat4_identity();

    CGL_bloom* bloom = CGL_bloom_create(600, 600, 3);

    CGL_float start_time = CGL_utils_get_time();
    CGL_float current_sound_level = 0.0f;

    while(!CGL_window_should_close(window))
    {
        CGL_float time = CGL_utils_get_time() - start_time;
        CGL_window_set_size(window, 600, 600); // force window size to be 600x600

        view = CGL_mat4_look_at(CGL_vec3_init(0.0f, 8.0f, 8.0f), CGL_vec3_init(0.0f, 0.0f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));

        CGL_framebuffer_bind(bloom_framebuffer);
        CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
        CGL_shader_bind(shader);
        CGL_shader_set_uniform_mat4(shader, CGL_shader_get_uniform_location(shader, "projection"), &projection);
        CGL_shader_set_uniform_mat4(shader, CGL_shader_get_uniform_location(shader, "view"), &view);
        CGL_shader_set_uniform_float(shader, CGL_shader_get_uniform_location(shader, "time"), time);
        CGL_shader_set_uniform_float(shader, CGL_shader_get_uniform_location(shader, "sound_level"), current_sound_level);
        CGL_mesh_gpu_render_instanced(mesh_gpu, 10000);

        CGL_bloom_apply(bloom, CGL_framebuffer_get_color_texture(bloom_framebuffer));

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shader_bind(present_shader);
        CGL_texture_bind(CGL_framebuffer_get_color_texture(bloom_framebuffer), 0);
        CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 0);
        CGL_gl_render_screen_quad();


        current_sound_level = 0.0f;
        if(g_is_loaded && !g_is_playing)
        {
            if(CGL_audio_source_is_playing(audio_source)) CGL_audio_source_stop(audio_source);
            CGL_audio_buffer_destroy(audio_buffer);
            audio_buffer = CGL_audio_buffer_create();
            CGL_audio_buffer_set_data_from_wav_file(audio_buffer, &g_wav_file);
            CGL_audio_source_set_buffer(audio_source, audio_buffer);
            CGL_audio_source_play(audio_source);
            g_is_playing = true;
            CGL_info("Started Playing");
        }

        if(g_is_loaded && g_is_playing)
        {
            if(!CGL_audio_source_is_playing(audio_source))
            {
                CGL_info("Finished playing");
                CGL_wav_file_destroy(&g_wav_file);
                g_is_loaded = false;
                g_is_playing = false;
            }
            else
            {
                CGL_float time_offset = CGL_audio_source_get_seconds_offset(audio_source);
                CGL_int sd = CGL_wav_file_sample_at_time(&g_wav_file, 0, time_offset);
                float volume = (float)sd / SHRT_MAX;
                current_sound_level = fabsf(volume * 2.0f);
            }
        }

        CGL_window_poll_events(window);
        CGL_window_swap_buffers(window);
    }


    if(g_is_loaded) CGL_wav_file_destroy(&g_wav_file);
    CGL_shader_destroy(shader);
    CGL_shader_destroy(present_shader);
    CGL_mesh_gpu_destroy(mesh_gpu);
    CGL_audio_buffer_destroy(audio_buffer);
    CGL_audio_source_destroy(audio_source);
    CGL_audio_make_context_current(NULL);
    CGL_audio_context_destroy(audio_context);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_framebuffer_destroy(bloom_framebuffer);
    CGL_bloom_destroy(bloom);
    CGL_gl_shutdown();
    CGL_window_destroy(window);    
    CGL_shutdown();
    return 0;
}
