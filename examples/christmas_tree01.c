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
#include "cgl.h"

#define TRAIL_COUNT 5
#define DECORATION_COUNT 64

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

static const char* TRAIL_FRAGMENT_SHADER = "#version 430 core\n"
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

static const char* DECORATIONS_VERTEX_SHADER = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
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

static const char* DECORATIONS_FRAGMENT_SHADER = "#version 430 core\n"
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

bool point_function(CGL_trail* trail, CGL_trail_point* point)
{
    point->thickness = 0.02f *  powf((trail->length - point->distance) / trail->length, 1.5f);
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
    CGL_shader* decoration_shader = CGL_shader_create(DECORATIONS_VERTEX_SHADER, DECORATIONS_FRAGMENT_SHADER, NULL);

    CGL_mat4 projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    CGL_mat4 view = CGL_mat4_identity();
    CGL_bloom* bloom = CGL_bloom_create(600, 600, 3);

    CGL_mesh_cpu* sphere_mesh_cpu = CGL_mesh_cpu_sphere(16, 16);
    CGL_mesh_gpu* sphere_mesh_gpu = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(sphere_mesh_gpu, sphere_mesh_cpu, true);
    CGL_mesh_cpu_destroy(sphere_mesh_cpu);

    CGL_mesh_cpu* cube_mesh_cpu = CGL_mesh_cpu_cube(false);
    CGL_mesh_cpu_recalculate_normals(cube_mesh_cpu);
    CGL_mesh_gpu* cube_mesh_gpu = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(cube_mesh_gpu, cube_mesh_cpu, true);
    CGL_mesh_cpu_destroy(cube_mesh_cpu);

    // mesh from https://sketchfab.com/3d-models/star-f7a96ca9ce3c4a0ba622088fffffa51c
    CGL_mesh_cpu* star_mesh_cpu = CGL_mesh_cpu_load_obj("assets/star.obj");
    CGL_mesh_cpu_recalculate_normals(star_mesh_cpu);
    CGL_mesh_gpu* star_mesh_gpu = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(star_mesh_gpu, star_mesh_cpu, true);
    CGL_mesh_cpu_destroy(star_mesh_cpu);



    CGL_float curr_time = CGL_utils_get_time();
    CGL_float prev_time = CGL_utils_get_time();
    CGL_float time = 0.0f;

    CGL_trail* trail[TRAIL_COUNT];
    for(int i = 0; i < TRAIL_COUNT; i++)
    {
        trail[i] = CGL_trail_create();
        CGL_trail_set_min_points_distance(trail[i], 0.005f);
        CGL_trail_set_resolution(trail[i], 32);
        CGL_trail_set_point_update_function(trail[i], point_function);
    }

    CGL_vec3 trail_tip_pos = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    CGL_vec3 trail_tip_pos_prev = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    CGL_float time_taken_to_reach_top = 8.0f;
    CGL_float max_height = 8.0f;

    CGL_bool started = false;

    static CGL_vec3 trail_color[TRAIL_COUNT];
    for(CGL_int i = 0; i < TRAIL_COUNT; i++) trail_color[i] = CGL_vec3_init(CGL_utils_random_float() * 0.3f , CGL_utils_random_float() * 0.5f + 0.5f, CGL_utils_random_float() * 0.3f);

    static Decoration decorations[DECORATION_COUNT];
    for(CGL_int i = 0; i < DECORATION_COUNT ; i++)
    {
        CGL_float height_f = 0.0f;
        // this is a very bad way to do this, but it works for now so .....
        while(!(height_f >= 0.15f && height_f <= 0.85f)) height_f = powf(CGL_E, -6.0f * (CGL_utils_random_float() * 0.7f));
        CGL_float angle = CGL_utils_random_float() * 2.0f * CGL_PI;
        CGL_vec3 pos = CGL_vec3_init(cosf(angle) * 3.0f * (1.0f - height_f), height_f * max_height, sinf(angle) * 3.0f * (1.0f - height_f));
        CGL_float scalef = CGL_utils_random_float() * 0.3f + 0.85f;
        CGL_vec3 scale = CGL_vec3_init(0.1f * scalef, 0.1f * scalef, 0.1f * scalef);
        decorations[i].transform = CGL_mat4_translate(pos.x, pos.y, pos.z);
        CGL_mat4 tmp = CGL_mat4_scale(scale.x, scale.y, scale.z);
        decorations[i].transform = CGL_mat4_mul(decorations[i].transform, tmp);
        decorations[i].mesh = rand() % 2 == 0 ? sphere_mesh_gpu : cube_mesh_gpu;
        decorations[i].color = CGL_vec3_init(CGL_utils_random_float(), CGL_utils_random_float(), CGL_utils_random_float());
    }

    CGL_audio_context* audio_context = CGL_audio_context_create(NULL);
    CGL_audio_make_context_current(audio_context);
    CGL_audio_source* audio_source = CGL_audio_source_create();
    CGL_audio_buffer* audio_buffer = CGL_audio_buffer_create();
    CGL_wav_file wav_file;
    // i do not own the audio file so i cannot share
    CGL_wav_file_load(&wav_file, "assets/cristmas_wish.wav");
    CGL_audio_buffer_set_data_from_wav_file(audio_buffer, &wav_file);
    CGL_audio_source_set_buffer(audio_source, audio_buffer);
    CGL_wav_file_destroy(&wav_file);

    while(!CGL_window_should_close(window))
    {
        curr_time = CGL_utils_get_time();
        CGL_float delta_time = curr_time - prev_time;
        prev_time = curr_time;
     
        if(started) time += CGL_utils_clamp(delta_time, 0.0f, 0.05f); // to avoid sudden jumps in time due to lag or things like window resizing
     
        CGL_window_set_size(window, 600, 600); // force window size to be 600x600

        view = CGL_mat4_look_at(CGL_vec3_init(8.0f * cosf(time), 12.0f, 8.0f * sinf(time)), CGL_vec3_init(0.0f, 4.5f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));

        CGL_float factor = time / time_taken_to_reach_top;
        if(factor < 1.0f && started)
        {
            CGL_float ifactor = 1.0f - factor;
            CGL_float rot_speed = 18.0f;
            trail_tip_pos = CGL_vec3_init(cosf(time*rot_speed) * 3.0f * ifactor, max_height * factor, sinf(time*rot_speed) * 3.0f * ifactor);
            CGL_vec3 trail_tip_vec = CGL_vec3_sub(trail_tip_pos, trail_tip_pos_prev);
            trail_tip_pos_prev = trail_tip_pos;
            CGL_vec3 front = CGL_vec3_init(0.0f, 0.0f, 1.0f);
            CGL_vec3 top = CGL_vec3_init(1.0f, 0.0f, 0.0f);
            CGL_vec3_calculate_orthonormal_basis_from_one_vector(trail_tip_vec, &front, &top);
            CGL_float angle_step = CGL_deg_to_rad(360.0f / TRAIL_COUNT);
            for(int i = 0; i < TRAIL_COUNT; i++)
            {
                CGL_float thickness = 0.15f + sinf(time) * 0.05f;
                thickness *= powf(ifactor, 1.0f / 3.0f);
                CGL_float phase = time * 8.0f;
                CGL_float ct = cosf(angle_step * i + phase) * thickness; CGL_float st = sinf(angle_step * i + phase) * thickness;
                CGL_vec3 v = CGL_vec3_add_(trail_tip_pos, CGL_vec3_add_(CGL_vec3_scale_(front, ct), CGL_vec3_scale_(top, st)));

                CGL_trail_add_point(trail[i], v, 300.0f, 0.1f);
                CGL_trail_update(trail[i], 0.1f);
                CGL_trail_bake_mesh(trail[i]);
            }
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
        if(factor >= 1.0f)
        {
            factor = (factor - 1.0f) / 0.2f;
            factor = CGL_utils_clamp(factor, 0.0f, 1.0f);
            CGL_shader_bind(decoration_shader);
            CGL_shader_set_uniform_mat4(decoration_shader, CGL_shader_get_uniform_location(decoration_shader, "view"), &view);
            CGL_shader_set_uniform_mat4(decoration_shader, CGL_shader_get_uniform_location(decoration_shader, "projection"), &projection);
            CGL_mat4 temp = CGL_mat4_scale(factor, factor, factor);
            CGL_mat4 temp2 = CGL_mat4_identity();
            for(CGL_int i = 0; i < DECORATION_COUNT; i++)
            {
                temp2 = CGL_mat4_mul(decorations[i].transform, temp);
                CGL_shader_set_uniform_mat4(decoration_shader, CGL_shader_get_uniform_location(decoration_shader, "model"), &temp2);
                CGL_shader_set_uniform_vec3v(decoration_shader, CGL_shader_get_uniform_location(decoration_shader, "color"), decorations[i].color.x, decorations[i].color.y, decorations[i].color.z);
                CGL_mesh_gpu_render(decorations[i].mesh);
            }
            CGL_shader_set_uniform_vec3v(decoration_shader, CGL_shader_get_uniform_location(decoration_shader, "color"), 1.0f, 1.0f, 0.0f);
            factor *= 0.3f;

            temp = CGL_mat4_translate(0.0f, max_height + 0.1f, 0.09f);
            temp2 = CGL_mat4_scale(factor, factor, -factor);
            temp = CGL_mat4_mul(temp, temp2);
            CGL_shader_set_uniform_mat4(decoration_shader, CGL_shader_get_uniform_location(decoration_shader, "model"), &temp);
            CGL_mesh_gpu_render(star_mesh_gpu);

            temp = CGL_mat4_translate(0.0f, max_height + 0.1f, -0.09f);
            temp2 = CGL_mat4_scale(factor, factor, factor);
            temp = CGL_mat4_mul(temp, temp2);
            CGL_shader_set_uniform_mat4(decoration_shader, CGL_shader_get_uniform_location(decoration_shader, "model"), &temp);
            CGL_mesh_gpu_render(star_mesh_gpu);
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

        if(!started && CGL_window_get_key(window, CGL_KEY_SPACE) == CGL_PRESS) { started = true; CGL_audio_source_play(audio_source);}
    }


    CGL_audio_make_context_current(NULL);
    for(int i = 0; i < TRAIL_COUNT; i++) CGL_trail_destroy(trail[i]);
    CGL_audio_context_destroy(audio_context);
    CGL_audio_source_destroy(audio_source);
    CGL_audio_buffer_destroy(audio_buffer);
    CGL_mesh_gpu_destroy(sphere_mesh_gpu);
    CGL_mesh_gpu_destroy(cube_mesh_gpu);
    CGL_mesh_gpu_destroy(star_mesh_gpu);
    CGL_shader_destroy(trail_shader);
    CGL_shader_destroy(decoration_shader);
    CGL_shader_destroy(present_shader);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_framebuffer_destroy(bloom_framebuffer);
    CGL_bloom_destroy(bloom);
    CGL_gl_shutdown();
    CGL_window_destroy(window);    
    CGL_shutdown();
    return 0;
}
