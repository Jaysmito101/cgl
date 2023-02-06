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

static const char* PARTICLE_VERTEX_SHADER_SOURCE = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"struct Particle\n"
"{\n"
"   vec4 position; // w has mass\n"
"   vec4 velocity; // w has velocity factor\n"
"   vec4 color; // w is reserved\n"
"};\n"
"\n"
"uniform mat4 view_proj;\n"
"\n"
"// vertices for a pyramid \n"
"const vec3 VERTICES[4] = vec3[4](\n"
"   vec3(0.5f, -0.5f, -1.732f * 0.5f),\n"
"   vec3(0.0f, 0.5f, 0.0f),\n"
"   vec3( 0.5f, -0.5f, 1.732f * 0.5f),\n"
"   vec3(-1.0f, -0.5f, 0.0f)\n"
");\n"
"\n"
"const int INDICES[12] = int[12](\n"
"   0, 1, 2,\n"
"   0, 1, 3,\n"
"   0, 2, 3,\n"
"   1, 2, 3\n"
");\n"
"\n"
"\n"
"layout(std430, binding = 0) buffer Particles\n"
"{\n"
"    Particle particles[];\n"
"};\n"
"\n"
"out vec3 Color;\n"
"\n"
"float atan2(in float y, in float x)\n"
"{\n"
"    bool s = (abs(x) > abs(y));\n"
"    return mix(3.141f/2.0 - atan(x,y), atan(y,x), s);\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"   int index = gl_VertexID / 12;\n"
"   int vt_index = gl_VertexID % 12;\n"
"   vec3 position = particles[index].position.xyz * 4.0f + VERTICES[INDICES[vt_index]] * 0.04f * particles[index].color.w;\n"
"   float xz_l = length(position.xz);\n"
"   float part_phase = atan2(position.x, position.z);\n"
"   position.xz = vec2(cos(position.y + part_phase), sin(position.y + part_phase)) * xz_l;\n"
"	gl_Position = view_proj * vec4(position.xyz, 1.0f);\n"
"   float dfc = length(particles[index].velocity.xyz) / 10.0f;\n"
"   Color = particles[index].color.rgb * (pow(log(position.y + 1), 4.0f) * 0.1f + 0.05f) ;\n"
"}\n";

static const char* PARTICLE_FRAGMENT_SHADER_SOURCE = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Color;\n"
"\n"
"void main()\n"
"{\n"
"	FragColor = vec4(Color, 1.0f);\n"
"}";

static const char* COMPUTE_SHADER_SOURCE = "#version 430 core\n"
"\n"
"layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;\n"
"\n"
"uniform int mode;\n"
"uniform float dt;\n"
"uniform float r_seed;\n"
"\n"
"struct Particle\n"
"{\n"
"   vec4 position; // w has mass\n"
"   vec4 velocity; // w has velocity factor\n"
"   vec4 color; // w is reserved\n"
"};\n"
"\n"
"layout(std430, binding = 0) buffer Particles\n"
"{\n"
"    Particle particles[];\n"
"};\n"
"\n"
"// Hash function www.cs.ubc.ca/~rbridson/docs/schechter-sca08-turbulence.pdf\n"
"int hash(int state)\n"
"{\n"
"    state ^= 2747636419;\n"
"    state *= 2654435769;\n"
"    state ^= state >> 16;\n"
"    state *= 2654435769;\n"
"    state ^= state >> 16;\n"
"    state *= 2654435769;\n"
"    return state;\n"
"}\n"
"\n"
"// From https://www.shadertoy.com/view/Xt23Ry\n"
"float rand(int co) { return fract(sin(float(co)*(91.3458) + r_seed) * 47453.5453); }\n"
"\n"
"float rand2(int co) { return rand(co) * 2.0f - 1.0f; }\n"
"\n"
"void initialize()\n"
"{\n"
"    int index = int(gl_GlobalInvocationID.x);\n"
"    int sqrt_pt_ct = 300;\n"
"    int x = index % sqrt_pt_ct;\n"
"    int y = index / sqrt_pt_ct;\n"
"    vec2 pos_xy = vec2(x, y) / float(sqrt_pt_ct) * 2.0f - vec2(1.0f);\n"
"    pos_xy *= 2.0f;\n"
"    particles[index].position = vec4(pos_xy.x, rand(index + 6) * 0.1f, pos_xy.y, 0.0f);\n"
"    float mass = rand(hash(index + 485) + 458) * 2.0f + 2.0f;\n"
"    float lent = length(particles[index].position.xz) + 0.05f;\n"
"    mass /= 1.0f - pow(exp(-lent), 8);\n"
"    particles[index].position.w = mass;\n"
"    particles[index].velocity = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
"    particles[index].color = vec4(rand(hash(index + 15)) * 0.2, rand(hash(index + 150))*0.2+0.5, rand(hash(index + 250)) * 0.2, 1.0f);\n"
"    particles[index].color.w = 1.0f + rand2(hash(index)) * 0.2f;\n"
"}\n"
"\n"
"void update()\n"
"{\n"
"   int index = int(gl_GlobalInvocationID.x);\n"
"   vec3 pos = particles[index].position.xyz;\n"
"   vec3 vel = particles[index].velocity.xyz;\n"
"   vec3 acc = vec3(0.0f);\n"
"   float mass = particles[index].position.w;\n"
"   float vel_factor = particles[index].velocity.w;\n"
"   float lent = length(pos.xz);\n"
"   acc += vec3(0.0f, 2.0f / (lent * lent) , 0.0f) / mass;\n"
"   acc += 0.8f * -(vec3(pos.x, 0.0f, pos.z)) / (lent * lent) / mass;\n"
"   vec3 cfgpr_ax = normalize(cross(vec3(0.0f, 1.0f, 0.0f), vec3(pos.x, 0.0f, pos.z)));\n"
"   vel += acc * dt * vel_factor;\n"
"   pos += vel * dt;\n"
"   if(pos.y > 4.8)\n"
"   {\n"
"       if(rand(index + int(dt * 1000)) > 0.90f) initialize();\n"
"       else { particles[index].position.xyz = normalize(vec3(pos.x, rand(index), pos.z)) * 0.2f + vec3(0.0f, 4.8f, 0.0f); particles[index].color.xyz = vec3(1.2f, 1.2f, 0.0f); }\n"
"   }\n"
"   else if(length(pos.xz) > ((0.7f + sin(pos.y * 5.0f)*0.3f) * exp(-pos.y/1.0f)) && pos.y > -0.05f)\n"
"   {\n"
"       particles[index].position.xyz = pos;\n"
"       particles[index].velocity.xyz = vel;\n"
"   }\n"
"   else\n"
"   {\n"
"       initialize();\n"
"   }\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    if(mode == 0) initialize();\n"
"    else if(mode == 1) update();\n"
"}";

#define PARTICLES_COUNT 90000


static CGL_int resolution = 256;
static CGL_framebuffer* default_framebuffer = NULL, *bloom_framebuffer = NULL;
static CGL_shader *particles_shader = NULL, *present_shader = NULL, *compute_shader = NULL;
static CGL_float delta_time = 0.0f;
static CGL_ssbo* particles_ssbo = NULL;

void reset_sim()
{
    CGL_shader_bind(compute_shader);
    CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "mode"), 0);
    CGL_shader_set_uniform_float(compute_shader, CGL_shader_get_uniform_location(compute_shader, "dt"), delta_time);
    CGL_shader_set_uniform_float(compute_shader, CGL_shader_get_uniform_location(compute_shader, "r_seed"), (float)rand());
    CGL_shader_compute_dispatch(compute_shader, PARTICLES_COUNT / 256 + 1, 1, 1);
}

void update_sim()
{
    CGL_shader_bind(compute_shader);
    CGL_shader_set_uniform_int(compute_shader, CGL_shader_get_uniform_location(compute_shader, "mode"), 1);
    CGL_shader_set_uniform_float(compute_shader, CGL_shader_get_uniform_location(compute_shader, "dt"), delta_time);
    CGL_shader_set_uniform_float(compute_shader, CGL_shader_get_uniform_location(compute_shader, "r_seed"), (float)rand());
    CGL_shader_compute_dispatch(compute_shader, PARTICLES_COUNT / 256 + 1, 1, 1);
}

int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(700, 700, "Particle Physics Dance - Jaysmito Mukherjee");
    if(!window) return 1;
    CGL_window_make_context_current(window);
    CGL_gl_init();
    CGL_widgets_init();

    default_framebuffer = CGL_framebuffer_create_from_default(window);
    bloom_framebuffer = CGL_framebuffer_create(700, 700);

    present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    particles_shader = CGL_shader_create(PARTICLE_VERTEX_SHADER_SOURCE, PARTICLE_FRAGMENT_SHADER_SOURCE, NULL);
    compute_shader = CGL_shader_compute_create(COMPUTE_SHADER_SOURCE, NULL);

    particles_ssbo = CGL_ssbo_create(0);
    CGL_ssbo_set_data(particles_ssbo, PARTICLES_COUNT * 12 * sizeof(CGL_float), NULL, GL_DYNAMIC_DRAW);


    CGL_float curr_time = CGL_utils_get_time();
    CGL_float prev_time = CGL_utils_get_time();
    CGL_float time = 0.0f, frame_time = 0.0f;
    CGL_int frames = 0, fps = 0;
    
    CGL_mat4 projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    CGL_mat4 view = CGL_mat4_identity();

    CGL_bloom* bloom = CGL_bloom_create(700, 700, 3);
    CGL_bloom_set_threshold(bloom, 1.0f);
    CGL_bloom_set_offset(bloom, 3.0f, 3.0f);

    reset_sim();

    while(!CGL_window_should_close(window))
    {
        CGL_window_set_size(window, 700, 700); // force window size to be 600x600

        view = CGL_mat4_look_at(CGL_vec3_init(20.0f, 20.0f, 20.0f), CGL_vec3_init(0.0f, 10.0f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));
        view = CGL_mat4_mul(projection, view);

        curr_time = CGL_utils_get_time();
        delta_time = curr_time - prev_time;
        prev_time = curr_time;
        time += delta_time; frame_time += delta_time;
        frames++;
        if(frame_time >= 1.0f) { fps = frames; frames = 0; frame_time = 0.0f; }

        update_sim();

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        CGL_framebuffer_bind(bloom_framebuffer);
        CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
        CGL_shader_bind(particles_shader);
        CGL_shader_set_uniform_mat4(particles_shader, CGL_shader_get_uniform_location(particles_shader, "view_proj"), &view);
        CGL_shader_set_uniform_float(particles_shader, CGL_shader_get_uniform_location(particles_shader, "dt"), delta_time);
        glDrawArrays(GL_TRIANGLES, 0, PARTICLES_COUNT * 12);

        CGL_bloom_apply(bloom, CGL_framebuffer_get_color_texture(bloom_framebuffer));

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shader_bind(present_shader);
        CGL_texture_bind(CGL_framebuffer_get_color_texture(bloom_framebuffer), 0);
        CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 0);
        CGL_gl_render_screen_quad();

        glDisable(GL_DEPTH_TEST);

        CGL_widgets_begin();

        static CGL_byte buffer[1024];
        sprintf(buffer, "Framerate: %d", fps);
        CGL_widgets_add_string(buffer, -1.0f, 0.95f, 1.0f, 0.05f);
        sprintf(buffer, "Frametime: %f", delta_time);
        CGL_widgets_add_string(buffer, -1.0f, 0.9f, 1.0f, 0.05f);



        CGL_widgets_end();

        CGL_window_poll_events(window);
        CGL_window_swap_buffers(window);

        if(CGL_window_is_key_pressed(window, CGL_KEY_SPACE)) reset_sim();


    }

    CGL_bloom_destroy(bloom);

    CGL_ssbo_destroy(particles_ssbo);

    CGL_shader_destroy(present_shader);
    CGL_shader_destroy(particles_shader);
    CGL_shader_destroy(compute_shader); 

    CGL_framebuffer_destroy(bloom_framebuffer);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);    
    CGL_shutdown();
    return 0;
}
