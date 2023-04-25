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
//"	color = acesFilm(color);\n"
//"   color = pow(color, vec3(1.0f / 2.2f));\n"
"	FragColor = vec4(color, 1.0f);\n"
"}";


#define MAP_RESOLUTION 256

static CGL_framebuffer* default_framebuffer = NULL;
static CGL_texture *output_tex = NULL;
static CGL_shader *present_shader = NULL;
static CGL_float scale, offset_x, offset_y;
static CGL_float texture_data_local[MAP_RESOLUTION * MAP_RESOLUTION * 4];
static CGL_noise_params np;

CGL_float noise(CGL_float x, CGL_float y)
{
    return CGL_noise_get(&np, x, y, 0.0f);
}

int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(700, 700, "Julia Set - Jaysmito Mukherjee");
    if(!window) return 1;
    CGL_window_make_context_current(window);
    CGL_gl_init();
    CGL_widgets_init();
    CGL_noise_init();
    CGL_noise_params_default(&np); np.octaves = 8;

    default_framebuffer = CGL_framebuffer_create_from_default(window);
    output_tex = CGL_texture_create_blank(MAP_RESOLUTION, MAP_RESOLUTION, GL_RGBA, GL_RGBA32F, GL_FLOAT);
    present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);

    scale = 1.0; offset_x = 0.0; offset_y = 0.0;

    CGL_float curr_time = CGL_utils_get_time();
    CGL_float prev_time = CGL_utils_get_time();
    CGL_float time = 0.0f, frame_time = 0.0f, delta_time = 0.0f;
    CGL_int frames = 0, fps = 0;

    while(!CGL_window_should_close(window))
    {
        CGL_window_set_size(window, 700, 700); // force window size to be 600x600

        curr_time = CGL_utils_get_time();
        delta_time = curr_time - prev_time;
        prev_time = curr_time;
        time += delta_time; frame_time += delta_time;
        frames++; if(frame_time >= 1.0f) { fps = frames; frames = 0; frame_time = 0.0f; }

        for(CGL_int i = 0 ; i < MAP_RESOLUTION ; i++) for(CGL_int j = 0 ; j < MAP_RESOLUTION ; j++)
        {
            CGL_float u = (CGL_float)j / (CGL_float)MAP_RESOLUTION; CGL_float v = (CGL_float)i / (CGL_float)MAP_RESOLUTION;
            CGL_float x = u * scale + offset_x; CGL_float y = v * scale + offset_y;
            CGL_float ns = noise(x, y);
            CGL_float r = ns * 0.5f + 0.5f;
            CGL_float pixel_data[] = {r, r, r, 1.0f};
            texture_data_local[(i * MAP_RESOLUTION + j) * 4 + 0] = r;
            texture_data_local[(i * MAP_RESOLUTION + j) * 4 + 1] = r;
            texture_data_local[(i * MAP_RESOLUTION + j) * 4 + 2] = r;
            texture_data_local[(i * MAP_RESOLUTION + j) * 4 + 3] = 1.0f;
        }
        CGL_texture_set_data(output_tex, texture_data_local);

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shader_bind(present_shader);
        CGL_texture_bind(output_tex, 0);
        CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 0);
        CGL_gl_render_screen_quad();

        glDisable(GL_DEPTH_TEST);

        CGL_widgets_begin();
        
        CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.0f, 1.0f);

        static CGL_byte buffer[256];
        sprintf(buffer, "Frame Time: %f", delta_time);
        CGL_widgets_add_string(buffer, -1.0f, 0.975f, 0.5f, 0.025f);
        sprintf(buffer, "Framerate : %d", fps);
        CGL_widgets_add_string(buffer, -1.0f, 0.950f, 0.5f, 0.025f);
        sprintf(buffer, "Scale : %f", scale);
        CGL_widgets_add_string(buffer, -1.0f, 0.925f, 0.5f, 0.025f);  
        sprintf(buffer, "Resolution: %dx%d", MAP_RESOLUTION, MAP_RESOLUTION);
        CGL_widgets_add_string(buffer, -1.0f, 0.900f, 0.5f, 0.025f);
        sprintf(buffer, "Offset X: %f", offset_x);
        CGL_widgets_add_string(buffer, -1.0f, 0.875f, 0.5f, 0.025f);
        sprintf(buffer, "Offset Y: %f", offset_y);
        CGL_widgets_add_string(buffer, -1.0f, 0.850f, 0.5f, 0.025f);


        CGL_widgets_end();

        CGL_window_poll_events(window);
        CGL_window_swap_buffers(window);


        if(CGL_window_get_key(window, CGL_KEY_T) == CGL_PRESS) for(CGL_int i = 0;i < CGL_NOISE_TYPE_COUNT ; i++) if(CGL_window_is_key_pressed(window, CGL_KEY_0 + i)) np.type = i;
        if(CGL_window_get_key(window, CGL_KEY_F) == CGL_PRESS) for(CGL_int i = 0;i < CGL_NOISE_FRACTAL_TYPE_COUNT ; i++) if(CGL_window_is_key_pressed(window, CGL_KEY_0 + i)) np.fractal_type = i;
        
        if(CGL_window_get_key(window, CGL_KEY_S) == CGL_PRESS)
        {
            if(CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) scale *= 1.02f;
            if(CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) scale *= 0.98f;
        }
        else
        {
            if(CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) offset_y += 0.1f;
            if(CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) offset_y -= 0.1f;
            if(CGL_window_get_key(window, CGL_KEY_LEFT) == CGL_PRESS) offset_x += 0.1f;
            if(CGL_window_get_key(window, CGL_KEY_RIGHT) == CGL_PRESS) offset_x -= 0.1f;
        }


    }


    CGL_texture_destroy(output_tex);
    CGL_shader_destroy(present_shader);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_noise_shutdown();
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);    
    CGL_shutdown();
    return 0;
}
