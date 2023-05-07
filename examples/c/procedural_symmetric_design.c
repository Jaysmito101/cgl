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
#define CGL_EXCLUDE_AUDIO
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
//"	FragColor = vec4(TexCoord, 0.0f, 1.0f);\n"
"}";


static const char* DRAW_COMPUTE_SHADER = "#version 430 core\n"
"\n"
"layout(local_size_x = 16, local_size_y = 16) in;\n"
"\n"
"layout(rgba32f, binding = 0) uniform image2D o_tex;\n"
"\n"
"uniform int u_mode;"
"uniform float u_brush_radius;\n"
"uniform vec2 u_splat_pt;\n"
"uniform vec3 u_brush_color;\n"
"\n"
"void apply_splat()\n"
"{\n"
"    vec2 UV = vec2(gl_GlobalInvocationID.xy) / vec2(imageSize(o_tex));\n"
"    vec2 p = UV - (u_splat_pt * 0.5f + vec2(0.5f));\n"
"    vec3 splat = exp(-dot(p, p) / (0.01f * u_brush_radius)) * u_brush_color;\n"
"    vec3 base = imageLoad(o_tex, ivec2(gl_GlobalInvocationID.xy)).rgb;\n"
"    imageStore(o_tex, ivec2(gl_GlobalInvocationID.xy), vec4(base + splat, 1.0f));\n"
"}\n"
"\n"
"void clear_splat()\n"
"{\n"
"    imageStore(o_tex, ivec2(gl_GlobalInvocationID.xy), vec4(0.0f, 0.0f, 0.0f, 1.0f));\n"
"}\n"
"\n"
"void diff_splat()\n"
"{\n"
"    vec2 UV = vec2(gl_GlobalInvocationID.xy) / vec2(imageSize(o_tex));\n"  
"    vec3 base = imageLoad(o_tex, ivec2(gl_GlobalInvocationID.xy)).rgb;\n"
"    imageStore(o_tex, ivec2(gl_GlobalInvocationID.xy), vec4(base * 0.9f, 1.0f));\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    if(u_mode == 0) apply_splat();\n"
"    else if(u_mode == 1) clear_splat();\n"
"    else if(u_mode == 2) diff_splat();\n"
"}\n";

#define DISPATCH_ON_MODE(mode) \
{ \
    CGL_shader_bind(draw_compute_shader); \
    CGL_shader_set_uniform_int(draw_compute_shader, CGL_shader_get_uniform_location(draw_compute_shader, "u_mode"), mode); \
    CGL_shader_compute_dispatch(draw_compute_shader, 1024 / 16, 1024 / 16, 1); \
} 

#define RANDOM_POINT_COUNT 3

typedef struct
{
    CGL_vec2 pos;
    CGL_vec2 vel;
    CGL_vec2 acc;
    CGL_float radius;
} random_point;

CGL_vec3 calculate_color(CGL_float amt, CGL_float mx, CGL_float my)
{
	CGL_float r = 0.0f, g = 0.0f, b = 0.0f;
    r = CGL_noise_perlin(mx, my, 0.0f + amt) * 0.5f + 0.5f;
    g = CGL_noise_perlin(mx, my, 1.0f + amt) * 0.5f + 0.5f;
    b = CGL_noise_perlin(mx, my, 2.0f + amt) * 0.5f + 0.5f;
	return CGL_vec3_init(r, g, b);
}

CGL_float draw_for_point(CGL_shader* shader, CGL_float px, CGL_float py, CGL_float amt, CGL_float radius)
{
    CGL_vec3 draw_brush_color = calculate_color(amt, px, py);
    CGL_shader_set_uniform_vec2v(shader, CGL_shader_get_uniform_location(shader, "u_splat_pt"), (float)px, (float)py);
    CGL_shader_set_uniform_vec3v(shader, CGL_shader_get_uniform_location(shader, "u_brush_color"), draw_brush_color.x, draw_brush_color.y, draw_brush_color.z);
    CGL_shader_set_uniform_float(shader, CGL_shader_get_uniform_location(shader, "u_brush_radius"), radius);
    CGL_shader_compute_dispatch(shader, 1024 / 16, 1024 / 16, 1);
    return amt;
}

CGL_float draw_symmetrical_for_point(CGL_shader* shader, CGL_int symmetry_count, CGL_float px, CGL_float py, CGL_float amt, CGL_float radius)
{
    CGL_float px2 = 0.0f, py2 = 0.0f, angle = CGL_2PI / (CGL_float)symmetry_count;
    for (CGL_int i = 0; i < symmetry_count; i++)
    {
        px2 = (CGL_float)(px * cosf(angle * i) - py * sinf(angle * i));
        py2 = (CGL_float)(px * sinf(angle * i) + py * cosf(angle * i));
        draw_for_point(shader, px2, py2, amt, radius);
        draw_for_point(shader, px2, -py2, amt, radius);
    }
}

CGL_void update_random_points(random_point* pnts)
{
    // each point will get attracted to the other points
    for (CGL_int i = 0; i < RANDOM_POINT_COUNT; i++)
    {
        pnts[i].acc.x = pnts[i].acc.y = 0.0f;
        for (CGL_int j = 0; j < RANDOM_POINT_COUNT; j++)
        {
			if (i == j) continue;
			CGL_float dx = pnts[j].pos.x - pnts[i].pos.x;
			CGL_float dy = pnts[j].pos.y - pnts[i].pos.y;
			CGL_float dist = sqrtf(dx * dx + dy * dy);
			CGL_float force = 0.0f;
			if (dist < 0.01f) force = 0.0f;
			else force = 0.0001f / dist;
			pnts[i].acc.x += force * dx;
			pnts[i].acc.y += force * dy;
		}
        pnts[i].acc.x *= 2.0f; pnts[i].acc.y *= 2.0f;
        // add some noise to the acceleration
        pnts[i].acc.x += CGL_utils_random_float_in_range(-0.1f, 0.1f) * 4.0f;
        pnts[i].acc.y += CGL_utils_random_float_in_range(-0.1f, 0.1f) * 4.0f;
    }

    for (CGL_int i = 0; i < RANDOM_POINT_COUNT; i++)
    {
		pnts[i].pos.x += pnts[i].vel.x * 0.1f; pnts[i].pos.y += pnts[i].vel.y * 0.1f;
		pnts[i].vel.x += pnts[i].acc.x * 0.1f; pnts[i].vel.y += pnts[i].acc.y * 0.1f;
        pnts[i].vel.x = pnts[i].vel.x * 0.98f; pnts[i].vel.y = pnts[i].vel.y * 0.98f;
		if (pnts[i].pos.x < -1.0f || pnts[i].pos.x > 1.0f) pnts[i].vel.x *= -1.0f;
		if (pnts[i].pos.y < -1.0f || pnts[i].pos.y > 1.0f) pnts[i].vel.y *= -1.0f;
        pnts[i].pos.x = CGL_utils_clamp(pnts[i].pos.x, -0.9999f, 0.9999f);
        pnts[i].pos.y = CGL_utils_clamp(pnts[i].pos.y, -0.9999f, 0.9999f);
	}
}

int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(700, 700, "Procedural Symmetrical Designer - Jaysmito Mukherjee");
    CGL_window_make_context_current(window); 
    CGL_gl_init();
    CGL_widgets_init();
    CGL_noise_init();
    if(window == NULL) return false; 
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);
    CGL_shader* present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    CGL_shader* draw_compute_shader = CGL_shader_compute_create(DRAW_COMPUTE_SHADER, NULL);
    CGL_texture* o_tex = CGL_texture_create_blank(1024, 1024, GL_RGBA, GL_RGBA32F, GL_FLOAT);
    CGL_int symmetry_count = 6;
    CGL_float draw_brush_radius = 0.02f, drawn_strokes_amount = 0.0f;

    random_point random_points[RANDOM_POINT_COUNT];

    for (CGL_int i = 0; i < RANDOM_POINT_COUNT; i++)
    {
        random_points[i].pos = CGL_utils_random_vec2(CGL_vec2_init(-1.0f, -1.0f), CGL_vec2_init(1.0f, 1.0f));
        random_points[i].vel = CGL_vec2_init(0.0f, 0.0f);
        random_points[i].acc = CGL_vec2_init(0.0f, 0.0f);
        random_points[i].radius = CGL_utils_random_float_in_range(0.01f, 0.005f);
    }

    while(!CGL_window_should_close(window)) 
    {
        CGL_window_set_size(window, 700, 700);
        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.002f, 0.002f, 0.002f, 1.0f);

        if (CGL_window_get_mouse_button(window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS)
        {
            glBindImageTexture(0, CGL_texture_get_internal_handle(o_tex), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

            double mouse_x, mouse_y;
            CGL_window_get_mouse_position(window, &mouse_x, &mouse_y);
            mouse_x = 1.0f - mouse_x / 700.0f; mouse_y = 1.0f - mouse_y / 700.0f;
            CGL_float px = (CGL_float)mouse_x * 2.0f - 1.0f, py = (CGL_float)mouse_y * 2.0f - 1.0f;
            CGL_shader_bind(draw_compute_shader);
            CGL_shader_set_uniform_int(draw_compute_shader, CGL_shader_get_uniform_location(draw_compute_shader, "u_mode"), 0);
            draw_symmetrical_for_point(draw_compute_shader, symmetry_count, px, py, drawn_strokes_amount, draw_brush_radius);
            drawn_strokes_amount += 0.01f;
        }

        update_random_points(random_points);

        if (CGL_window_is_key_pressed(window, CGL_KEY_R))
        {
            for (CGL_int i = 0; i < RANDOM_POINT_COUNT; i++)
            {
                CGL_shader_bind(draw_compute_shader);
                CGL_shader_set_uniform_int(draw_compute_shader, CGL_shader_get_uniform_location(draw_compute_shader, "u_mode"), 0);
                draw_symmetrical_for_point(draw_compute_shader, symmetry_count, random_points[i].pos.x, random_points[i].pos.y, drawn_strokes_amount, draw_brush_radius);
                //draw_for_point(draw_compute_shader, random_points[i].pos.x, random_points[i].pos.y, drawn_strokes_amount, random_points[i].radius);
                drawn_strokes_amount += 0.01f;
            }
        }

        CGL_shader_bind(present_shader);
        CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), CGL_texture_bind(o_tex, 0));
        CGL_gl_render_screen_quad();
    
        if (CGL_window_is_key_pressed(window, CGL_KEY_C)) DISPATCH_ON_MODE(1);
        if (CGL_window_is_key_pressed(window, CGL_KEY_D)) DISPATCH_ON_MODE(2);

        if (CGL_window_is_key_pressed(window, CGL_KEY_ESCAPE)) break;

        CGL_window_swap_buffers(window);
        CGL_window_poll_events(window);
    }

    CGL_texture_destroy(o_tex);
    CGL_shader_destroy(draw_compute_shader);
    CGL_shader_destroy(present_shader);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_noise_shutdown();
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
    return EXIT_SUCCESS;
}
