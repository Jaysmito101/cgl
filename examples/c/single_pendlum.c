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

#define CGL_EXCLUDE_NETWORKING
#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
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

static const char* COMPUTE_SHADER_SOURCE = "#version 430 core\n"
"\n"
"layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;\n"
"\n"
"layout(rgba32f, binding = 0) uniform image2D output_tex;\n"
"\n"
"uniform int mode;"
"uniform vec2 mass_pos;"
"uniform vec2 fixed_pos;"
"uniform vec4 pre_color;"
"\n"
"bool lies_on_line(vec2 p)\n"
"{\n"
"   vec2 v = p - mass_pos;\n"
"   vec2 u = fixed_pos - mass_pos;\n"
"   float dot = v.x * u.x + v.y * u.y;\n"
"   float len_sq = u.x * u.x + u.y * u.y;\n"
"   float param = dot / len_sq;\n"
"   vec2 p2 = mass_pos + param * u;\n"
"   return length(p - p2) < 0.0001f;\n"
"}\n"
"\n"
"vec4 box_blur_filter(ivec2 coord)\n"
"{\n"
"   vec4 sum = vec4(0.0f, 0.0f, 0.0f, 0.0f);\n"
"   for(int i = -1; i <= 1; i++)\n"
"   {\n"
"       for(int j = 0; j <= 1; j++)\n"
"       {\n"
"           sum += imageLoad(output_tex, coord + ivec2(i, j));\n"
"       }\n"
"   }\n"
"   return sum / 6.0f;\n"
"}\n"
"\n"
"void initialize()\n"
"{\n"
"   ivec2 coord = ivec2(gl_GlobalInvocationID.xy);\n"
"   imageStore(output_tex, coord, vec4(0.0f, 0.0f, 0.0f, 1.0f));\n"
"}\n"
"\n"
"void update()\n"
"{\n"
"   ivec2 coord = ivec2(gl_GlobalInvocationID.xy);\n"
"   vec4 color = vec4(0.0f, 0.0f, 0.0f, 0.0f);\n"
"   vec2 nuv = vec2(imageSize(output_tex).x - coord.x, coord.y) / vec2(imageSize(output_tex)) * 2.0f - vec2(1.0f);\n"
"   nuv.x = nuv.x;\n"
"   // set color to pre_color with a radius of 0.01 exponentially decaying\n"
"   color = mix(box_blur_filter(coord), vec4(vec3(0.0f), 1.0f), 0.001f);\n"
"   float factor = exp(-0.5f * length(nuv - mass_pos) / 0.002f);\n"
"   color = mix(color, 2.0f * pre_color, factor);\n"
"   if(lies_on_line(nuv)) color = 0.9f * vec4(vec3(1.0f) - pre_color.xyz, 1.0f) * exp(-0.5f * length(nuv - fixed_pos) / 0.2f);\n"
"   color.a = 1.0f;\n"
"   imageStore(output_tex, coord, color);\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"   if(mode == 0) initialize();\n"
"   if(mode == 1) update();\n"
"}\n";


static struct
{
    CGL_window* window; // the main window
    int window_height;
    int window_width;
    CGL_framebuffer* framebuffer;
} g_context;

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return EXIT_FAILURE; // initialize CGL (required for setting up internals of CGL)
    g_context.window_width = g_context.window_height = 700;    
    g_context.window = CGL_window_create(g_context.window_width, g_context.window_height, "Single Pendulum - Jaysmito Mukherjee"); // create the window
    CGL_window_make_context_current(g_context.window); // make the opengl context for the window current
    if(!CGL_gl_init()) return EXIT_FAILURE; // initialize cgl opengl module    
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window); // load the default framebuffer (0) into CGL_framebuffer object
    CGL_noise_init();
    CGL_bool is_physics_on = true;
    CGL_vec2 mass_pos = CGL_vec2_init(0.0f, 0.0f), fixed_pos = CGL_vec2_init(0.0f, 1.0f), mass_vel = CGL_vec2_init(0.0f, 0.0f), mass_acc = CGL_vec2_init(0.0f, 0.0f), mass_force = CGL_vec2_init(0.0f, 0.0f);
    CGL_float mass_val = 1.0f, acc_due_to_gravity = 5.0f, spring_constant = 1.0f, damping_constant = 0.1f, time_step = 0.01f, curr_time;
    CGL_texture* out_tex = CGL_texture_create_blank(700, 700, GL_RGBA, GL_RGBA32F, GL_FLOAT);
    CGL_shader* present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    CGL_shader* draw_shader = CGL_shader_compute_create(COMPUTE_SHADER_SOURCE, NULL);
    CGL_vec3 color = CGL_vec3_init(0.0f, 1.0f, 1.0f);
    glBindImageTexture(0, out_tex->handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    CGL_shader_bind(draw_shader);
    CGL_shader_set_uniform_int(draw_shader, CGL_shader_get_uniform_location(draw_shader, "mode"), 0);
    CGL_shader_compute_dispatch(draw_shader, 700 / 16 + 1, 700 / 16 + 1, 1);
    // the main loop
    while(!CGL_window_should_close(g_context.window)) // run till the close button is clicked
    {
        curr_time = CGL_utils_get_time();
        CGL_window_set_size(g_context.window, g_context.window_width, g_context.window_height); // set window size
        CGL_framebuffer_bind(g_context.framebuffer); // bind default framebuffer and also adjust viewport size and offset
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f); // clear screen with a dark gray color
        if(is_physics_on)
        {
            CGL_double mx = 0.0, my = 0.0; CGL_float weight = mass_val * acc_due_to_gravity * -1.0f;
            CGL_vec2 dir_tension = CGL_vec2_normalize_(CGL_vec2_sub_(fixed_pos, mass_pos));
            mass_force.x = 0.0f; mass_force.y = weight; mass_acc = CGL_vec2_scale_(mass_force, 1.0f / mass_val);
            mass_vel = CGL_vec2_add_(mass_vel, CGL_vec2_scale_(mass_acc, time_step));
            mass_vel = CGL_vec2_sub_(mass_vel, CGL_vec2_scale_(dir_tension, CGL_vec2_dot(mass_vel, dir_tension)));
            mass_pos = CGL_vec2_add(mass_pos, CGL_vec2_scale_(mass_vel, time_step));
            mass_vel = CGL_vec2_scale(mass_vel, 0.999f);
            
            color.x = CGL_noise_opensimplex2s(mass_pos.x, mass_pos.y, 0.0f + curr_time) * 0.5f + 0.5f;
            color.y = CGL_noise_opensimplex2s(mass_pos.x, mass_pos.y, 1.0f + curr_time) * 0.5f + 0.5f;
            color.z = CGL_noise_opensimplex2s(mass_pos.x, mass_pos.y, 2.0f + curr_time) * 0.5f + 0.5f;
            if(CGL_window_get_mouse_button(g_context.window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS)
            {
                CGL_window_get_mouse_position(g_context.window, &mx, &my);
                mx = (1.0f - mx / (CGL_double)g_context.window_width) * 2.0 - 1.0; my = (1.0 - my / (CGL_double)g_context.window_height) * 2.0 - 1.0;
                CGL_vec2 dir = CGL_vec2_init((CGL_float)mx, (CGL_float)my - 1.0f);
                CGL_vec2_normalize(dir); CGL_float angle = copysignf(CGL_vec2_angle_between(dir, CGL_vec2_init(0.0f, -1.0f)), dir.x) + CGL_PI_2;
                mass_vel = CGL_vec2_scale(mass_vel, 0.0f); mass_pos = CGL_vec2_init(cosf(angle), 1.0f - sinf(angle));
                glBindImageTexture(0, out_tex->handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
                CGL_shader_bind(draw_shader);
                CGL_shader_set_uniform_int(draw_shader, CGL_shader_get_uniform_location(draw_shader, "mode"), 0);
                CGL_shader_compute_dispatch(draw_shader, 700 / 16 + 1, 700 / 16 + 1, 1);
            }
        }
        glBindImageTexture(0, out_tex->handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        CGL_shader_bind(draw_shader);
        CGL_shader_set_uniform_int(draw_shader, CGL_shader_get_uniform_location(draw_shader, "mode"), 1);
        CGL_shader_set_uniform_vec2v(draw_shader, CGL_shader_get_uniform_location(draw_shader, "fixed_pos"), fixed_pos.x, fixed_pos.y);
        CGL_shader_set_uniform_vec2v(draw_shader, CGL_shader_get_uniform_location(draw_shader, "mass_pos"), mass_pos.x, mass_pos.y);
        CGL_shader_set_uniform_vec4v(draw_shader, CGL_shader_get_uniform_location(draw_shader, "pre_color"), color.x, color.y, color.z, 1.0f);
        CGL_shader_compute_dispatch(draw_shader, 700 / 16 + 1, 700 / 16 + 1, 1);
        CGL_shader_bind(present_shader);
        CGL_texture_bind(out_tex, 5);
        CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 5);
        CGL_gl_render_screen_quad();
        CGL_window_swap_buffers(g_context.window); // swap framebuffers
        CGL_window_poll_events(g_context.window); // poll events (if this is not called every frame window will stop responding)
        if(CGL_window_get_key(g_context.window, CGL_KEY_ESCAPE) == CGL_PRESS) break; // quit on pressing escape
        if(CGL_window_get_key(g_context.window, CGL_KEY_SPACE) == CGL_PRESS) is_physics_on = true;
        if(CGL_window_get_key(g_context.window, CGL_KEY_ENTER) == CGL_PRESS) is_physics_on = false;
    }

    // cleanup
    CGL_noise_shutdown();
    CGL_shader_destroy(present_shader);
    CGL_shader_destroy(draw_shader);
    CGL_texture_destroy(out_tex);
    CGL_framebuffer_destroy(g_context.framebuffer); // destory framebuffer object
    CGL_gl_shutdown(); // shutdown cgl opengl module
    CGL_window_destroy(g_context.window); // destroy window
    CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
    return EXIT_SUCCESS;
}
