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
#define CGL_TRAIL_MAX_POINTS 4096
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
"	TexCoord = vec2(position.xy);\n"
"}";

static const char* PASS_THROUGH_FRAGMENT_SHADER_SOURCE = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Position;\n"
"in vec2 TexCoord;\n"
"\n"
"uniform float u_scale;\n"
"uniform vec2 u_offset;\n"
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
"#define MAX_ITER 250\n"
"#define BAIL_RADIUS 100.0\n"
"\n"
"float magnitude(vec2 z)\n"
"{\n"
"    return z.x * z.x + z.y * z.y;\n"
"}\n"
"\n"
"vec2 multiply(vec2 a, vec2 b)\n"
"{\n"
"    return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);\n"
"}\n"
"\n"
"vec3 get_color()\n"
"{\n"
"    vec2 C = (TexCoord.xy * u_scale) + u_offset;\n"
"    vec2 Z = vec2(0.0, 0.0);\n"
"    int i = 0;\n"
"    for(; i < MAX_ITER; i++)\n"
"    {\n"
"        Z = multiply(Z, Z) + C;\n"
"        if(magnitude(Z) > BAIL_RADIUS) break;\n"
"    }\n"
"    float log_zn = log(magnitude(Z)) / 2.0;\n"
"    float nu = log(log_zn / log(BAIL_RADIUS)) / log(2.0);\n"
"    float iter = float(i) + 1.0 - nu;\n"
"    float r = sqrt(iter / float(MAX_ITER));\n"
"    float g = iter / float(MAX_ITER);\n"
"    float b = sqrt(sqrt(iter / float(MAX_ITER)));\n"
"    return vec3(r, g, b);\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"	vec3 color = get_color();\n"
"	color = acesFilm(color);\n"
"   color = pow(color, vec3(1.0f / 2.2f));\n"
"	FragColor = vec4(color, 1.0f);\n"
"}";


int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(600, 600, "Mandelbrot Set - Jaysmito Mukherjee");
    if(!window) return 1;
    CGL_window_make_context_current(window);
    CGL_gl_init();
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);
 
    CGL_shader* present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);

    CGL_float offset_x = 0.0f;
    CGL_float offset_y = 0.0f;
    CGL_float scale = 1.0f;

    while(!CGL_window_should_close(window))
    {
        CGL_window_set_size(window, 700, 700); // force window size to be 700x700

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shader_bind(present_shader);
        CGL_shader_set_uniform_vec2v(present_shader, CGL_shader_get_uniform_location(present_shader, "u_offset"), offset_x, offset_y);
        CGL_shader_set_uniform_float(present_shader, CGL_shader_get_uniform_location(present_shader, "u_scale"), scale);
        CGL_gl_render_screen_quad();

        CGL_window_poll_events(window);
        CGL_window_swap_buffers(window);

        CGL_float scale_f = CGL_utils_max(scale, 0.00001f);
        if(CGL_window_is_key_pressed(window, CGL_KEY_LEFT)) offset_x -= 0.01f * scale_f;
        if(CGL_window_is_key_pressed(window, CGL_KEY_RIGHT)) offset_x += 0.01f * scale_f;
        if(CGL_window_is_key_pressed(window, CGL_KEY_UP)) offset_y += 0.01f * scale_f;
        if(CGL_window_is_key_pressed(window, CGL_KEY_DOWN)) offset_y -= 0.01f * scale_f;
        if(CGL_window_is_key_pressed(window, CGL_KEY_EQUAL)) scale /= 1.01f;
        if(CGL_window_is_key_pressed(window, CGL_KEY_MINUS)) scale *= 1.01f;
    }


    CGL_shader_destroy(present_shader);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(window);    
    CGL_shutdown();
    return 0;
}
