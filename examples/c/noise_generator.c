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
#define CGL_EXCLUDE_AUDIO
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_EXCLUDE_NODE_EDITOR
#include "cgl.h"

#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif

static const char* PASS_THROUGH_VERTEX_SHADER_SOURCE = 
#ifdef CGL_WASM
"#version 300 es\n"
"precision highp float;\n"
"precision highp int;\n"
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
"precision highp int;\n"
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
//"	color = acesFilm(color);\n"
//"   color = pow(color, vec3(1.0f / 2.2f));\n"
"	FragColor = vec4(color, 1.0f);\n"
"}";


#define MAP_RESOLUTION 256

// Game state structure
struct {
    CGL_window* window;
    CGL_framebuffer* default_framebuffer;
    CGL_texture* output_tex;
    CGL_shader* present_shader;
    CGL_noise_params np;
    CGL_float scale;
    CGL_float offset_x;
    CGL_float offset_y;
    CGL_float texture_data_local[MAP_RESOLUTION * MAP_RESOLUTION * 4];
    CGL_float prev_time;
    CGL_float curr_time;
    CGL_float delta_time;
    CGL_float frame_time;
    CGL_int frames;
    CGL_int fps;
} g_State;

CGL_float noise(CGL_float x, CGL_float y)
{
    return CGL_noise_get(&g_State.np, x, y, 0.0f);
}

CGL_bool init()
{
    srand((uint32_t)time(NULL));
    if (!CGL_init()) return CGL_FALSE;

    g_State.window = CGL_window_create(700, 700, "Perlin Noise - Jaysmito Mukherjee");
    if (!g_State.window) return CGL_FALSE;

    CGL_window_make_context_current(g_State.window);
    if (!CGL_gl_init()) return CGL_FALSE;

    CGL_widgets_init();
    CGL_noise_init();
    CGL_noise_params_default(&g_State.np);
    g_State.np.octaves = 8;

    g_State.default_framebuffer = CGL_framebuffer_create_from_default(g_State.window);
    g_State.output_tex = CGL_texture_create_blank(MAP_RESOLUTION, MAP_RESOLUTION, GL_RGBA, GL_RGBA32F, GL_FLOAT);
    g_State.present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);

    g_State.scale = 1.0f;
    g_State.offset_x = 0.0f;
    g_State.offset_y = 0.0f;
    g_State.prev_time = CGL_utils_get_time();
    g_State.frames = 0;
    g_State.fps = 0;
    g_State.frame_time = 0.0f;

    return CGL_TRUE;
}

void cleanup()
{
    CGL_texture_destroy(g_State.output_tex);
    CGL_shader_destroy(g_State.present_shader);
    CGL_framebuffer_destroy(g_State.default_framebuffer);
    CGL_noise_shutdown();
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(g_State.window);
    CGL_shutdown();
}

EM_BOOL loop(double time, void* userData)
{
    (void)time;
    (void)userData;

    CGL_window_set_size(g_State.window, 700, 700);

    g_State.curr_time = CGL_utils_get_time();
    g_State.delta_time = g_State.curr_time - g_State.prev_time;
    g_State.prev_time = g_State.curr_time;
    g_State.frame_time += g_State.delta_time;
    g_State.frames++;
    
    if (g_State.frame_time >= 1.0f) {
        g_State.fps = g_State.frames;
        g_State.frames = 0;
        g_State.frame_time = 0.0f;
    }

    // Update noise texture
    for (CGL_int i = 0; i < MAP_RESOLUTION; i++) {
        for (CGL_int j = 0; j < MAP_RESOLUTION; j++) {
            CGL_float u = (CGL_float)j / (CGL_float)MAP_RESOLUTION;
            CGL_float v = (CGL_float)i / (CGL_float)MAP_RESOLUTION;
            CGL_float x = u * g_State.scale + g_State.offset_x;
            CGL_float y = v * g_State.scale + g_State.offset_y;
            CGL_float ns = noise(x, y);
            CGL_float r = ns * 0.5f + 0.5f;
            g_State.texture_data_local[(i * MAP_RESOLUTION + j) * 4 + 0] = r;
            g_State.texture_data_local[(i * MAP_RESOLUTION + j) * 4 + 1] = r;
            g_State.texture_data_local[(i * MAP_RESOLUTION + j) * 4 + 2] = r;
            g_State.texture_data_local[(i * MAP_RESOLUTION + j) * 4 + 3] = 1.0f;
        }
    }
    CGL_texture_set_data(g_State.output_tex, g_State.texture_data_local);

    // Render
    CGL_framebuffer_bind(g_State.default_framebuffer);
    CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
    CGL_shader_bind(g_State.present_shader);
    CGL_texture_bind(g_State.output_tex, 0);
    CGL_shader_set_uniform_int(g_State.present_shader, CGL_shader_get_uniform_location(g_State.present_shader, "u_tex"), 0);
    CGL_gl_render_screen_quad();

    // UI rendering
    glDisable(GL_DEPTH_TEST);
    CGL_widgets_begin();
    CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.0f, 1.0f);

    static CGL_byte buffer[256];
    sprintf(buffer, "Frame Time: %f", g_State.delta_time);
    CGL_widgets_add_string(buffer, -1.0f, 0.975f, 0.5f, 0.025f);
    sprintf(buffer, "Framerate : %d", g_State.fps);
    CGL_widgets_add_string(buffer, -1.0f, 0.950f, 0.5f, 0.025f);
    sprintf(buffer, "Scale : %f", g_State.scale);
    CGL_widgets_add_string(buffer, -1.0f, 0.925f, 0.5f, 0.025f);
    sprintf(buffer, "Resolution: %dx%d", MAP_RESOLUTION, MAP_RESOLUTION);
    CGL_widgets_add_string(buffer, -1.0f, 0.900f, 0.5f, 0.025f);
    sprintf(buffer, "Offset X: %f", g_State.offset_x);
    CGL_widgets_add_string(buffer, -1.0f, 0.875f, 0.5f, 0.025f);
    sprintf(buffer, "Offset Y: %f", g_State.offset_y);
    CGL_widgets_add_string(buffer, -1.0f, 0.850f, 0.5f, 0.025f);

    CGL_widgets_end();

    // Handle input
    if (CGL_window_get_key(g_State.window, CGL_KEY_T) == CGL_PRESS)
        for (CGL_int i = 0; i < CGL_NOISE_TYPE_COUNT; i++)
            if (CGL_window_is_key_pressed(g_State.window, CGL_KEY_0 + i))
                g_State.np.type = i;

    if (CGL_window_get_key(g_State.window, CGL_KEY_F) == CGL_PRESS)
        for (CGL_int i = 0; i < CGL_NOISE_FRACTAL_TYPE_COUNT; i++)
            if (CGL_window_is_key_pressed(g_State.window, CGL_KEY_0 + i))
                g_State.np.fractal_type = i;

    if (CGL_window_get_key(g_State.window, CGL_KEY_S) == CGL_PRESS) {
        if (CGL_window_get_key(g_State.window, CGL_KEY_UP) == CGL_PRESS) g_State.scale *= 1.02f;
        if (CGL_window_get_key(g_State.window, CGL_KEY_DOWN) == CGL_PRESS) g_State.scale *= 0.98f;
    } else {
        if (CGL_window_get_key(g_State.window, CGL_KEY_UP) == CGL_PRESS) g_State.offset_y += 0.1f;
        if (CGL_window_get_key(g_State.window, CGL_KEY_DOWN) == CGL_PRESS) g_State.offset_y -= 0.1f;
        if (CGL_window_get_key(g_State.window, CGL_KEY_LEFT) == CGL_PRESS) g_State.offset_x += 0.1f;
        if (CGL_window_get_key(g_State.window, CGL_KEY_RIGHT) == CGL_PRESS) g_State.offset_x -= 0.1f;
    }

    CGL_window_poll_events(g_State.window);
    CGL_window_swap_buffers(g_State.window);

    return !CGL_window_should_close(g_State.window);
}

int main()
{
    if (!init()) return 1;

#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while (!CGL_window_should_close(g_State.window)) {
        loop(0, NULL);
    }
    cleanup();
#endif

    return 0;
}
