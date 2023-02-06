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


static const char* PARTICLE_VERTEX_SHADER = "#version 430 core\n"
"\n"
"out vec3 Color;\n"
"\n"
"struct Particle\n"
"{\n"
"	vec4 position;\n"
"	vec4 velocity;\n"
"	vec4 color;\n"
"	vec4 colori;\n"
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
"layout (std430, binding = 0) buffer Particles\n"
"{\n"
"	Particle particles[];\n"
"};\n"
"\n"
"void main()\n"
"{\n"
"   int particle_index = gl_VertexID / 12;\n"
"   int index = gl_VertexID % 12;\n"
"   vec3 vertex = VERTICES[INDICES[index]];\n"
"   float distance = length(particles[particle_index].position.xyz);\n"
"   distance = clamp(7.5f - distance, 0.0f, 7.5f) / 7.5f;\n"
"   distance = pow(distance, 1.0f) + 0.4f;\n"
"   vertex = particles[particle_index].position.xyz + vertex * distance * 0.07f;\n"
"	gl_Position = view_proj * vec4(vertex, 1.0f);\n"
"   Color = particles[particle_index].color.xyz;\n"
"}\n";

static const char* PARTICLE_FRAGMENT_SHADER = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Color;\n"
"\n"
"void main()\n"
"{\n"
"	FragColor = vec4(Color, 1.0f);\n"
"}\n";

static const char* PARTICLE_COMPUTE_SHADER = "#version 430 core\n"
"\n"
"layout (local_size_x = 100, local_size_y = 1, local_size_z = 1) in;\n"
"\n"
"struct Particle\n"
"{\n"
"	vec4 position;\n"
"	vec4 velocity;\n"
"	vec4 color;\n"
"	vec4 colori;\n"
"};\n"
"\n"
"layout (std430, binding = 0) buffer Particles\n"
"{\n"
"	Particle particles[];\n"
"};\n"
"\n"
"uniform float delta_time;\n"
"uniform int mode;\n"
"uniform vec3 seed = vec3(0.0f);\n"
"\n"
"float rand(vec2 co)\n"
"{\n"
"    co += seed.xy;\n"
"    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);\n"
"}\n"
"\n"
"vec3 hash( vec3 p )\n"
"{\n"
"   p += seed;\n"
"	p = vec3( dot(p,vec3(127.1,311.7, 74.7)),\n"
"			  dot(p,vec3(269.5,183.3,246.1)),\n"
"			  dot(p,vec3(113.5,271.9,124.6)));\n"
"\n"
"	return -1.0 + 2.0*fract(sin(p)*43758.5453123);\n"
"}\n"
"\n"
"// return value noise (in x) and its derivatives (in yzw)\n"
"vec4 noised( in vec3 x )\n"
"{\n"
"    // grid\n"
"    vec3 i = floor(x);\n"
"    vec3 w = fract(x);\n"
"    \n"
"    #if 1\n"
"    // quintic interpolant\n"
"    vec3 u = w*w*w*(w*(w*6.0-15.0)+10.0);\n"
"    vec3 du = 30.0*w*w*(w*(w-2.0)+1.0);\n"
"    #else\n"
"    // cubic interpolant\n"
"    vec3 u = w*w*(3.0-2.0*w);\n"
"    vec3 du = 6.0*w*(1.0-w);\n"
"    #endif    \n"
"    \n"
"    // gradients\n"
"    vec3 ga = hash( i+vec3(0.0,0.0,0.0) );\n"
"    vec3 gb = hash( i+vec3(1.0,0.0,0.0) );\n"
"    vec3 gc = hash( i+vec3(0.0,1.0,0.0) );\n"
"    vec3 gd = hash( i+vec3(1.0,1.0,0.0) );\n"
"    vec3 ge = hash( i+vec3(0.0,0.0,1.0) );\n"
"	vec3 gf = hash( i+vec3(1.0,0.0,1.0) );\n"
"    vec3 gg = hash( i+vec3(0.0,1.0,1.0) );\n"
"    vec3 gh = hash( i+vec3(1.0,1.0,1.0) );\n"
"    \n"
"    // projections\n"
"    float va = dot( ga, w-vec3(0.0,0.0,0.0) );\n"
"    float vb = dot( gb, w-vec3(1.0,0.0,0.0) );\n"
"    float vc = dot( gc, w-vec3(0.0,1.0,0.0) );\n"
"    float vd = dot( gd, w-vec3(1.0,1.0,0.0) );\n"
"    float ve = dot( ge, w-vec3(0.0,0.0,1.0) );\n"
"    float vf = dot( gf, w-vec3(1.0,0.0,1.0) );\n"
"    float vg = dot( gg, w-vec3(0.0,1.0,1.0) );\n"
"    float vh = dot( gh, w-vec3(1.0,1.0,1.0) );\n"
"	\n"
"    // interpolations\n"
"    return vec4( va + u.x*(vb-va) + u.y*(vc-va) + u.z*(ve-va) + u.x*u.y*(va-vb-vc+vd) + u.y*u.z*(va-vc-ve+vg) + u.z*u.x*(va-vb-ve+vf) + (-va+vb+vc-vd+ve-vf-vg+vh)*u.x*u.y*u.z,    // value\n"
"                 ga + u.x*(gb-ga) + u.y*(gc-ga) + u.z*(ge-ga) + u.x*u.y*(ga-gb-gc+gd) + u.y*u.z*(ga-gc-ge+gg) + u.z*u.x*(ga-gb-ge+gf) + (-ga+gb+gc-gd+ge-gf-gg+gh)*u.x*u.y*u.z +   // derivatives\n"
"                 du * (vec3(vb,vc,ve) - va + u.yzx*vec3(va-vb-vc+vd,va-vc-ve+vg,va-vb-ve+vf) + u.zxy*vec3(va-vb-ve+vf,va-vb-vc+vd,va-vc-ve+vg) + u.yzx*u.zxy*(-va+vb+vc-vd+ve-vf-vg+vh) ));\n"
"}\n"
"\n"
"\n"
"void initialize_particles()\n"
"{\n"
"	uint index = gl_GlobalInvocationID.x;\n"
"	Particle particle;\n"
"	particle.position = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
"	particle.velocity = vec4(rand(vec2(index, 454.45f)) * 2.0f - 1.0f, rand(vec2(958.454f, index * 21.0f)) * 2.0f - 1.0f, rand(vec2(4.684 * index, 4568.41f)) * 2.0f - 1.0f, 0.0f);\n"
"   particle.velocity *= 0.8f * pow(noised(particle.velocity.xyz * 8.0f).x * 2.0f + 0.2f, 0.5f);\n"
"	particle.color = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"	particle.colori = normalize(particle.velocity) * 0.5f + vec4(0.5f);\n"
"	particles[index] = particle;\n"
"}\n"
"\n"
"void update_particles()\n"
"{\n"
"	uint index = gl_GlobalInvocationID.x;\n"
"	Particle particle = particles[index];\n"
"   vec4 total_noise = noised(particle.position.xyz * 0.7f);\n"
"   vec3 acceleration = total_noise.yzw;\n"
"	particle.position += particle.velocity * clamp(delta_time, 0.00f, 0.03f);\n"
"	particle.velocity += acceleration * 0.15f;\n"
"   float lntgh = length(particle.position.xyz);"
"   vec3 noise_at_pos = total_noise.xyz;\n"
"   total_noise.x *= 4.0f - 2.0f;"
"   if(lntgh > 7.0f + total_noise.x && lntgh < 7.05f + total_noise.x)\n"
"   {\n"
"       particle.velocity = 2.0f * normalize(vec4(-particle.position.xyz, 0.0f));\n"
"       particle.velocity *= 0.2f * vec4(noise_at_pos * 0.5f + vec3(0.5f), 1.0f);\n"
"       if(noise_at_pos.x > 0.0f)\n"
"       {\n"
"           particle.velocity *= 0.0f;\n"
"           particle.position = vec4(normalize(particle.position.xyz) * (7.06f + total_noise.x), 0.0f);\n"
"       }\n"
"       particle.velocity *= 5.0f;\n"
"   }\n"
"   else if(lntgh > 7.05f + noise_at_pos.x * 2.5f)\n"
"   {\n"
"       particle.velocity *= 0.0f;\n"
"       particle.position = vec4(normalize(particle.position.xyz) * (7.06f + total_noise.x), 0.0f);\n"
"   }\n"
"   vec3 central_color = vec3(255, 238, 0) / 256.0f;\n"
"   vec3 middle_color = vec3(255, 94, 0) / 256.0f;\n"
"   vec3 border_color = vec3(255, 30, 0) / 256.0f;\n"
"   float factor = pow(lntgh / 7.0f, 1.0f / 8.0f);\n"
"   vec3 color = mix(central_color, middle_color, factor);\n"
"   color = mix(color, border_color, pow(lntgh / 7.0f, 4.0f));\n"
"   particle.color =  vec4(color * (1.0f + pow(1.0f - lntgh / 7.0f, 4.0f)), 0.0f);\n"
"	particles[index] = particle;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"   if(mode == 0) initialize_particles();\n"
"   else if(mode == 1) update_particles();\n"
"}\n";


#define PARTICLE_COUNT 1000000

int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(700, 700, "Particles - Jaysmito Mukherjee");
    if(!window) return 1;
    CGL_window_make_context_current(window);
    CGL_gl_init();
    CGL_widgets_init();

    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);
    CGL_framebuffer* bloom_framebuffer = CGL_framebuffer_create(600, 600);

    CGL_shader* present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    CGL_shader* particle_shader = CGL_shader_create(PARTICLE_VERTEX_SHADER, PARTICLE_FRAGMENT_SHADER, NULL);
    CGL_shader* particle_compute_shader = CGL_shader_compute_create(PARTICLE_COMPUTE_SHADER, NULL);

    CGL_bloom* bloom = CGL_bloom_create(600, 600, 3);

    CGL_ssbo* particle_ssbo = CGL_ssbo_create(0);
    CGL_ssbo_set_data(particle_ssbo, sizeof(CGL_float) * 12 * PARTICLE_COUNT, NULL, GL_DYNAMIC_DRAW);
    
    CGL_float curr_time = CGL_utils_get_time();
    CGL_float prev_time = CGL_utils_get_time();
    CGL_float time = 0.0f, frame_time = 0.0f;
    CGL_int frames = 0, fps = 0;


    CGL_bool started = false;

    CGL_mat4 view, projection;
    projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    view = CGL_mat4_identity();

    CGL_shader_bind(particle_compute_shader);
    CGL_shader_set_uniform_int(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "mode"), 0);
    CGL_shader_compute_dispatch(particle_compute_shader, PARTICLE_COUNT / 100, 1, 1);

    while(!CGL_window_should_close(window))
    {
        CGL_window_set_size(window, 600, 600); // force window size to be 600x600

        curr_time = CGL_utils_get_time();
        CGL_float delta_time = curr_time - prev_time;
        //delta_time = CGL_utils_clamp(delta_time, 0.0f, 0.03f); // to avoid sudden jumps in time due to lag or things like window resizing
        prev_time = curr_time;
        time += delta_time; frame_time += delta_time;
        frames++;
        if(frame_time >= 1.0f) { fps = frames; frames = 0; frame_time = 0.0f; }
        

        view = CGL_mat4_look_at(CGL_vec3_init(15.0f * cosf(time), 15.0f, 15.0f*sinf(time)), CGL_vec3_init(0.0f, 0.0f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));
        view = CGL_mat4_mul(projection, view);

        CGL_shader_bind(particle_compute_shader);
        CGL_shader_set_uniform_int(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "mode"), 1);
        CGL_shader_set_uniform_float(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "delta_time"), delta_time);
        CGL_shader_compute_dispatch(particle_compute_shader, PARTICLE_COUNT / 100, 1, 1);


        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        CGL_framebuffer_bind(bloom_framebuffer);
        CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
        CGL_shader_bind(particle_shader);
        CGL_shader_set_uniform_mat4(particle_shader, CGL_shader_get_uniform_location(particle_shader, "view_proj"), &view);
        glDrawArrays(GL_TRIANGLES, 0, 12 * PARTICLE_COUNT);


        CGL_bloom_apply(bloom, CGL_framebuffer_get_color_texture(bloom_framebuffer));

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shader_bind(present_shader);
        CGL_texture_bind(CGL_framebuffer_get_color_texture(bloom_framebuffer), 0);
        CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 0);
        CGL_gl_render_screen_quad();

        glDisable(GL_DEPTH_TEST);

        CGL_widgets_begin();
        
        CGL_widgets_set_fill_colorf(1.0f, 1.0f, 1.0f, 1.0f);
        CGL_widgets_set_stroke_colorf(1.0f, 1.0f, 1.0f, 1.0f);

        static CGL_byte buffer[256];
        sprintf(buffer, "Frame Time: %f", delta_time);
        CGL_widgets_add_string(buffer, -1.0f, 0.95f, 1.0f, 0.05f);
        sprintf(buffer, "Framerate : %d", fps);
        CGL_widgets_add_string(buffer, -1.0f, 0.9f, 1.0f, 0.05f);
        sprintf(buffer, "Particles : %d", PARTICLE_COUNT);
        CGL_widgets_add_string(buffer, -1.0f, 0.85f, 1.0f, 0.05f);  

        if(CGL_window_get_key(window, CGL_KEY_SPACE) == CGL_PRESS)
        {
            CGL_shader_bind(particle_compute_shader);
            CGL_shader_set_uniform_int(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "mode"), 0);
            CGL_shader_set_uniform_vec3v(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "seed"), CGL_utils_random_float() * 100.0f, CGL_utils_random_float() * 100.0f, CGL_utils_random_float() * 100.0f);
            CGL_shader_compute_dispatch(particle_compute_shader, PARTICLE_COUNT / 100, 1, 1);
        }
        



        CGL_widgets_end();

        CGL_window_poll_events(window);
        CGL_window_swap_buffers(window);

        if(!started) if(CGL_window_get_key(window, CGL_KEY_SPACE) == CGL_PRESS) started = true;
    }


    CGL_ssbo_destroy(particle_ssbo);
    CGL_shader_destroy(particle_compute_shader);
    CGL_shader_destroy(particle_shader);
    CGL_shader_destroy(present_shader);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_framebuffer_destroy(bloom_framebuffer);
    CGL_bloom_destroy(bloom);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);    
    CGL_shutdown();
    return 0;
}
