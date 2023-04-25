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
#define CGL_IMPLEMENTATION
#include "cgl.h"

#define VECTOR_FIELD_SIZE 35

static const CGL_byte* PASS_THROUGH_VS = "#version 430 core\n"
"layout(location = 0) in vec4 position;\n"
"layout(location = 1) in vec4 normal;\n"
"layout(location = 1) in vec4 tex_coords;\n"
""
"void main()\n"
"{\n"
"    gl_Position = vec4(position.x, position.y, 0.0, 1.0);\n"
"}\n";

static const CGL_byte* PASS_THROUGH_FS = "#version 430 core\n"
"out vec4 color;\n"
""
"uniform sampler2D vec_field;\n"
"uniform int mode;\n"
""
"void main()\n"
"{\n"
"   vec2 uv = gl_FragCoord.xy / 700.0;\n"
"   if(mode == 1) color = vec4(texture(vec_field, uv).xy, 0.0f, 1.0);\n"
"   else if(mode == 2) color = vec4(texture(vec_field, uv).zzz, 1.0);\n"
"   else if(mode == 3) color = vec4(texture(vec_field, uv).www, 1.0);\n"
"   else color = vec4(texture(vec_field, uv).xyz, 1.0);\n"
"}\n";

static const CGL_byte* COMPUTE_SHADER = "#version 430 core\n"
"\n"
"#define VEC_FIELD_SIZE 35\n"
"\n"
"\n"
"layout (local_size_x = 16, local_size_y = 16) in;\n"
"\n"
"layout (rgba32f, binding = 0) uniform image2D vec_field_tex;\n"
"\n"
"layout(std430, binding=0) readonly buffer VecField\n"
"{\n"
"    vec4 vec_field[];\n"
"};\n"
"\n"
"uniform int mode = 0; \n"
"\n"
"void interpolate_vec_field()\n"
"{\n"
"    vec2 uv = gl_GlobalInvocationID.xy / 700.0f;\n"
"    ivec2 iuv0 = ivec2(uv * VEC_FIELD_SIZE);\n"
"    vec2 color = vec_field[iuv0.x + iuv0.y * VEC_FIELD_SIZE].xy;\n"
"    color = (color + vec2(1.0f)) * 0.5f;\n"
"    imageStore(vec_field_tex, ivec2(gl_GlobalInvocationID.xy), vec4(color, 0.0f, 1.0));\n"
"}\n"
"\n"
"void calculate_divergence()\n"
"{\n"
"    vec2 uv = gl_GlobalInvocationID.xy / 700.0f;\n"
"    ivec2 iuv0 = ivec2(uv * VEC_FIELD_SIZE);\n"
"    vec2 samplex0 = vec_field[iuv0.x - 1 + iuv0.y * VEC_FIELD_SIZE].xy;\n"
"    vec2 samplex1 = vec_field[iuv0.x + 1 + iuv0.y * VEC_FIELD_SIZE].xy;\n"
"    vec2 sampley0 = vec_field[iuv0.x + (iuv0.y - 1) * VEC_FIELD_SIZE].xy;\n"
"    vec2 sampley1 = vec_field[iuv0.x + (iuv0.y + 1) * VEC_FIELD_SIZE].xy;\n"
"    float div = (samplex1.x - samplex0.x + sampley1.y - sampley0.y) * 0.5f;\n"
"    vec4 color = imageLoad(vec_field_tex, ivec2(gl_GlobalInvocationID.xy));\n"
"    imageStore(vec_field_tex, ivec2(gl_GlobalInvocationID.xy), vec4(color.xy, div, 1.0f));\n"
"}\n"
"\n"
"void calculate_curl()\n"
"{\n"
"    // This is just a test, it's not correct\n"
"    // actual implementaion is TODO\n"
"    vec2 uv = gl_GlobalInvocationID.xy / 700.0f;\n"
"    ivec2 iuv0 = ivec2(uv * VEC_FIELD_SIZE);\n"
"    vec2 samplex0 = vec_field[iuv0.x - 1 + iuv0.y * VEC_FIELD_SIZE].xy;\n"
"    vec2 samplex1 = vec_field[iuv0.x + 1 + iuv0.y * VEC_FIELD_SIZE].xy;\n"
"    vec2 sampley0 = vec_field[iuv0.x + (iuv0.y - 1) * VEC_FIELD_SIZE].xy;\n"
"    vec2 sampley1 = vec_field[iuv0.x + (iuv0.y + 1) * VEC_FIELD_SIZE].xy;\n"
"    float curl = (samplex1.y - samplex0.y - sampley1.x + sampley0.x) * 0.5f;\n"
"    vec4 color = imageLoad(vec_field_tex, ivec2(gl_GlobalInvocationID.xy));\n"
"    imageStore(vec_field_tex, ivec2(gl_GlobalInvocationID.xy), vec4(color.xyz, curl));\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    if(mode == 0) interpolate_vec_field();\n"
"    else if(mode == 1) calculate_divergence();\n"
"    else if(mode == 2) calculate_curl();\n"
"}";

static CGL_window* window = NULL;
static CGL_vec4 vec_field[VECTOR_FIELD_SIZE * VECTOR_FIELD_SIZE];
static CGL_bool comb_mode = false;
static CGL_float brush_size = 0.3f;
static CGL_float brush_strength = 0.5f;
static CGL_shader *compute = NULL, *display_shader = NULL;
static CGL_texture* vec_field_tex = NULL;
static CGL_ssbo* vec_field_ssbo = NULL;


void add_line_custom(CGL_float start_x, CGL_float start_y, CGL_float end_x, CGL_float end_y, CGL_float thickness)
{
    CGL_vec3 start = CGL_vec3_init(start_x, start_y, 0.0f);
    CGL_vec3 end = CGL_vec3_init(end_x, end_y, 0.0f);

    CGL_vec3 a, b, c, d;

    CGL_float original_slope = (end.y - start.y) / (end.x - start.x);
    CGL_float perp_slope = -1.0f / original_slope;
    CGL_float angle = atanf(perp_slope);
    CGL_float r = thickness / 2.0f;

    a = CGL_vec3_init(start.x + r * cosf(angle), start.y + r * sinf(angle), start.z);
    b = CGL_vec3_init(start.x - r * cosf(angle), start.y - r * sinf(angle), start.z);

    r = r * 0.1f;

    c = CGL_vec3_init(end.x - r * cosf(angle), end.y - r * sinf(angle), end.z);
    d = CGL_vec3_init(end.x + r * cosf(angle), end.y + r * sinf(angle), end.z);

    CGL_widgets_add_quad(a, b, c, d);
}

void randomize_vec_field()
{
    for(int i = 0; i < VECTOR_FIELD_SIZE; i++)
    {
        for(int j = 0; j < VECTOR_FIELD_SIZE; j++)
        {
            vec_field[i * VECTOR_FIELD_SIZE + j] = CGL_vec4_init(CGL_utils_random_float(), CGL_utils_random_float(), -1.0f + 2.0f * (CGL_float)j / VECTOR_FIELD_SIZE, -1.0f + 2.0f * (CGL_float)i / VECTOR_FIELD_SIZE);
            CGL_vec2_normalize(vec_field[i * VECTOR_FIELD_SIZE + j]);
        }
    }
}

void apply_comb_brush()
{
    CGL_double mx, my;
    CGL_window_get_mouse_position(window, &mx, &my);
    my = 700.0 - my;
    CGL_float x = -1.0f + 2.0f * (CGL_float)mx / 700.0f;
    CGL_float y = -1.0f + 2.0f * (CGL_float)my / 700.0f;

    static CGL_float last_x = 0.0f;
    static CGL_float last_y = 0.0f;

    CGL_float dx = x - last_x;
    CGL_float dy = y - last_y;

    dx = CGL_utils_clamp(dx, -0.1f, 0.1f);
    dy = CGL_utils_clamp(dy, -0.1f, 0.1f);
    CGL_vec2 dir = CGL_vec2_init(dx, dy);
    CGL_float delt_len = CGL_vec2_length(dir);
    CGL_vec2_normalize(dir);

    if(CGL_window_get_mouse_button(window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS && delt_len > 0.0001f)
    {

        for(CGL_int i = 0; i < VECTOR_FIELD_SIZE; i++)
        {
            for(CGL_int j = 0; j < VECTOR_FIELD_SIZE; j++)
            {
                CGL_float dist = sqrtf((vec_field[i * VECTOR_FIELD_SIZE + j].z - x) * (vec_field[i * VECTOR_FIELD_SIZE + j].z - x) + (vec_field[i * VECTOR_FIELD_SIZE + j].w - y) * (vec_field[i * VECTOR_FIELD_SIZE + j].w - y));
                if(dist < brush_size)
                {
                    CGL_float strength = brush_strength * (1.0f - dist / brush_size);
                    CGL_vec2 a = CGL_vec2_init(vec_field[i * VECTOR_FIELD_SIZE + j].x, vec_field[i * VECTOR_FIELD_SIZE + j].y);
                    CGL_vec2 dir_s = CGL_vec2_scale(dir, brush_strength * powf(1.0f - dist / brush_size, 4.0f));
                    CGL_vec2 b = CGL_vec2_lerp(a, dir_s, 0.5f);
                    CGL_vec2_normalize(b);
                    vec_field[i * VECTOR_FIELD_SIZE + j].x = b.x;
                    vec_field[i * VECTOR_FIELD_SIZE + j].y = b.y;
                 //   CGL_info("vec[%d][%d] = (%f, %f)", i, j, vec_field[i * VECTOR_FIELD_SIZE + j].x, vec_field[i * VECTOR_FIELD_SIZE + j].y);
                }
            }
        }
    
    }

    last_x = x;
    last_y = y;
}

void randomize_charge_based_vec_field()
{
    static CGL_vec3 points[64];
    CGL_int num_points = CGL_utils_random_int(2, 64);
    for(CGL_int i = 0; i < num_points; i++) points[i] = CGL_vec3_init(CGL_utils_random_float() * 2.0f - 1.0f, CGL_utils_random_float() * 2.0f - 1.0f, CGL_utils_random_float() * 20.0f - 10.0f);

    for(CGL_int i = 0 ; i < VECTOR_FIELD_SIZE; i++)
    {
        for(CGL_int j = 0 ; j < VECTOR_FIELD_SIZE; j++)
        {
            CGL_vec2 force = CGL_vec2_init(0.0f, 0.0f);
            CGL_float x = -1.0f + 2.0f * (CGL_float)j / VECTOR_FIELD_SIZE;
            CGL_float y = -1.0f + 2.0f * (CGL_float)i / VECTOR_FIELD_SIZE;
            
            for(CGL_int k = 0; k < num_points; k++)
            {
                CGL_vec2 dir = CGL_vec2_init(points[k].x - x, points[k].y - y);
                CGL_float dist = CGL_vec2_length(dir);
                CGL_vec2_normalize(dir);
                CGL_vec2 force_dir = CGL_vec2_scale(dir, points[k].z / (dist * dist));
                force = CGL_vec2_add(force, force_dir);
            }
            CGL_vec2_normalize(force);

            vec_field[i * VECTOR_FIELD_SIZE + j].x = force.x;
            vec_field[i * VECTOR_FIELD_SIZE + j].y = force.y;
        }
    }

}

int main(int argc, char** argv, char** envp)
{
    srand((uint32_t)time(NULL));
    CGL_init();
    window = CGL_window_create(700, 700, "CGL Vector Field Visualizer - Jaysmito Mukherjee");
    if(window == NULL) return false; 
    CGL_window_make_context_current(window); 
    CGL_gl_init();
    CGL_widgets_init();
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);

    compute = CGL_shader_compute_create(COMPUTE_SHADER, NULL);
    display_shader = CGL_shader_create(PASS_THROUGH_VS, PASS_THROUGH_FS, NULL);
    vec_field_tex = CGL_texture_create_blank(700, 700, GL_RGBA, GL_RGBA32F, GL_FLOAT);
    vec_field_ssbo = CGL_ssbo_create(0);


    randomize_vec_field();

    CGL_float last_time = CGL_utils_get_time();
    CGL_float curr_time = CGL_utils_get_time();
    CGL_float delta_time = 0.0f;

    CGL_int mode = 1;

    while(!CGL_window_should_close(window)) 
    {
        CGL_window_set_size(window, 700, 700); // force window size to be 700x700

        curr_time = CGL_utils_get_time();
        delta_time = curr_time - last_time;
        last_time = curr_time;

        CGL_ssbo_set_data(vec_field_ssbo, sizeof(vec_field), vec_field, false);

        CGL_shader_bind(compute);
        glBindImageTexture(0, vec_field_tex->handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        
        CGL_shader_set_uniform_int(compute, CGL_shader_get_uniform_location(compute, "mode"), 0);
        CGL_shader_compute_dispatch(compute, 700 / 16 + 1, 700 / 16 + 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        CGL_shader_set_uniform_int(compute, CGL_shader_get_uniform_location(compute, "mode"), 1);
        CGL_shader_compute_dispatch(compute, 700 / 16 + 1, 700 / 16 + 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        CGL_shader_set_uniform_int(compute, CGL_shader_get_uniform_location(compute, "mode"), 2);
        CGL_shader_compute_dispatch(compute, 700 / 16 + 1, 700 / 16 + 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);

        CGL_shader_bind(display_shader);
        CGL_texture_bind(vec_field_tex, 4);
        CGL_shader_set_uniform_int(display_shader, CGL_shader_get_uniform_location(display_shader, "vec_field"), 4);
        CGL_shader_set_uniform_int(display_shader, CGL_shader_get_uniform_location(display_shader, "mode"), mode);
        CGL_gl_render_screen_quad();

        CGL_widgets_begin();

        CGL_widgets_set_fill_colorf(0.0f, 0.0f, 1.0f, 1.0f);
        for(int i = 0; i < VECTOR_FIELD_SIZE * VECTOR_FIELD_SIZE; i++) add_line_custom(vec_field[i].z, vec_field[i].w, vec_field[i].z + 0.05f * vec_field[i].x, vec_field[i].w + 0.05f * vec_field[i].y, 0.02f);

        CGL_widgets_set_fill_colorf(1.0f, 1.0f, 1.0f, 1.0f);
        static CGL_byte buffer[1000];
        sprintf(buffer, "Frame Time: %f", delta_time);
        CGL_widgets_add_string(buffer, -1.0f, 0.95f, 0.5f, 0.05f);
        sprintf(buffer, "Brush Mode: %s", comb_mode ? "On" : "Off");
        CGL_widgets_add_string(buffer, -1.0f, 0.90f, 0.5f, 0.05f);
        sprintf(buffer, "Brush Size: %f", brush_size);
        CGL_widgets_add_string(buffer, -1.0f, 0.85f, 0.7f, 0.05f);
        sprintf(buffer, "Brush Strength: %f", brush_strength);
        CGL_widgets_add_string(buffer, -1.0f, 0.80f, 0.7f, 0.05f);
        sprintf(buffer, "Mode: %s", mode == 1 ? "Field" : (mode == 2 ? "Divergence" : (mode == 3 ? "Curl" : "All"))); // this is dirty but it works for now so ...
        CGL_widgets_add_string(buffer, -1.0f, 0.75f, 0.7f, 0.05f);

        CGL_widgets_end();

        if(CGL_window_get_key(window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
        if(CGL_window_get_key(window, CGL_KEY_R) == CGL_PRESS) randomize_vec_field();
        if(CGL_window_get_key(window, CGL_KEY_C) == CGL_PRESS) comb_mode = true;
        if(CGL_window_get_key(window, CGL_KEY_V) == CGL_PRESS) comb_mode = false;
        if(CGL_window_get_key(window, CGL_KEY_LEFT_SHIFT) == CGL_PRESS)
        {
            if(CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) brush_size += delta_time;
            if(CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) brush_size -= delta_time;
            if(CGL_window_get_key(window, CGL_KEY_LEFT) == CGL_PRESS) brush_strength -= delta_time;
            if(CGL_window_get_key(window, CGL_KEY_RIGHT) == CGL_PRESS) brush_strength += delta_time;
            brush_size = CGL_utils_clamp(brush_size, 0.01f, 10.0f);
        }
        if(CGL_window_get_key(window, CGL_KEY_G) == CGL_PRESS) randomize_charge_based_vec_field();
        if(CGL_window_get_key(window, CGL_KEY_1) == CGL_PRESS) mode = 1;
        if(CGL_window_get_key(window, CGL_KEY_2) == CGL_PRESS) mode = 2;
        if(CGL_window_get_key(window, CGL_KEY_3) == CGL_PRESS) mode = 3;

        if(comb_mode) apply_comb_brush();

        CGL_window_swap_buffers(window);
        CGL_window_poll_events(window);
    }

    CGL_ssbo_destroy(vec_field_ssbo);
    CGL_texture_destroy(vec_field_tex);
    CGL_shader_destroy(compute);
    CGL_shader_destroy(display_shader);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
    return EXIT_SUCCESS;
}
