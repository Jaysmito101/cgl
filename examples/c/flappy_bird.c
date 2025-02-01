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



#define CGL_EXCLUDE_AUDIO
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_EXCLUDE_NETWORKING
#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#include "cgl.h"

#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Update shader versions for WASM compatibility
static const char* PASS_THROUGH_VERTEX_SHADER_SOURCE = 
#ifdef CGL_WASM
"#version 300 es\n"
"precision highp float;\n"
"precision highp int;\n"
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
"uniform vec4 po;\n"
"\n"
"void main()\n"
"{\n"
"   vec2 pos = position.xy;\n"
"   pos = pos * po.zw * 0.5f;\n"
"   vec2 off = po.xy + po.zw * 0.5f;\n"
"   pos = pos + off;\n"
"	gl_Position = vec4(pos, 0.0f, 1.0f);\n"
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
"uniform vec4 potx;"
"\n"
"void main()\n"
"{\n"
"   vec2 tx = vec2(1.0f - TexCoord.y, 1.0f - TexCoord.x);\n"
"   tx = tx * potx.zw;\n"
"   tx = tx + potx.xy;\n"
"	FragColor = texture(u_tex, tx);\n"
"}";

#define DRAW_QUAD(tex, ind, stx, sty, szx, szy) { \
    CGL_texture_bind(tex, ind); \
    CGL_shader_set_uniform_int(g_State.present_shader, CGL_shader_get_uniform_location(g_State.present_shader, "u_tex"), (CGL_int)ind); \
    CGL_shader_set_uniform_vec4v(g_State.present_shader, CGL_shader_get_uniform_location(g_State.present_shader, "po"), (CGL_float)(stx), (CGL_float)(sty), (CGL_float)(szx), (CGL_float)(szy)); \
    CGL_gl_render_screen_quad(); \
} 

#define PO_TX(stx, sty, szx, szy) CGL_shader_set_uniform_vec4v(g_State.present_shader, CGL_shader_get_uniform_location(g_State.present_shader, "potx"), (CGL_float)(stx), (CGL_float)(sty), (CGL_float)(szx), (CGL_float)(szy));


// Game state structure
struct {
    CGL_window* window;
    CGL_framebuffer* framebuffer;
    CGL_shader* present_shader;
    CGL_texture* bird_tex[3];
    CGL_texture* background_tex;
    CGL_texture* platform_tex;
    CGL_texture* pipe_tex;
    CGL_texture* game_over_tex;
    CGL_texture* messages;
    CGL_vec3 pipes[10];
    CGL_int window_height;
    CGL_int window_width;
    CGL_float player_pos;
    CGL_float player_velocity; 
    CGL_int frame_count;
    CGL_int current_player_sprite;
    CGL_int closest_pipe_index;
    CGL_int score;
    CGL_int last_scored_pipe;
    CGL_int state;
    CGL_float prev_time;
    CGL_float curr_time;
    CGL_float delta_time;
} g_State;

// Constants
static const CGL_float GRAVITY = -5.8f;
static const CGL_float JUMP_VELOCITY = 1.2f;
static const CGL_float PLATFORM_SCROLL_SPEED = 0.2f;
static const CGL_float PIPE_SPEED = 0.3f;

// Helper functions
CGL_texture* load_texture(const char* filename)
{
    CGL_image img = {0}; img.bytes_per_channel = 8;
    stbi_set_flip_vertically_on_load(true);
    img.data = stbi_load(filename, &img.width, &img.height, &img.channels, 0);
    if (!img.data) { CGL_error("Failed to load image: %s", filename); return NULL; }
    CGL_texture* tex = CGL_texture_create(&img); stbi_image_free(img.data);
    return tex;
}

CGL_int update_pipes(CGL_float player_x, CGL_float player_y)
{
    (void)player_y;

    CGL_float max_x = 0.0, min_dst = 100000.0f; 
    CGL_int min_dst_index = -1;
    
    for(CGL_int i = 0; i < 10; i++) { 
        max_x = CGL_utils_max(max_x, g_State.pipes[i].x);
    }
    max_x = CGL_utils_max(max_x, 1.0f);
    
    for(CGL_int i = 0; i < 10; i++)
    {
        g_State.pipes[i].x -= PIPE_SPEED * g_State.delta_time; // Scale with delta time
        
        if(g_State.pipes[i].x < -2.0f) 
        {
            CGL_float mid = CGL_utils_random_float_in_range(0.5f, 1.5f);
            g_State.pipes[i].x = max_x + 0.75f; 
            g_State.pipes[i].y = mid; 
            max_x = g_State.pipes[i].x;
        }
        
        CGL_float dst = fabsf(g_State.pipes[i].x - player_x);
        if(dst < min_dst) { min_dst = dst; min_dst_index = i; }
    }
    return min_dst_index;
}

CGL_bool init()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return CGL_FALSE;

    g_State.window_width = g_State.window_height = 700;
    g_State.window = CGL_window_create(g_State.window_width, g_State.window_height, "Flappy Bird - Jaysmito Mukherjee");
    if(!g_State.window) return CGL_FALSE;
    
    CGL_window_make_context_current(g_State.window);
    if(!CGL_gl_init()) return CGL_FALSE;

    g_State.framebuffer = CGL_framebuffer_create_from_default(g_State.window);
    CGL_widgets_init();
    
    g_State.present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    
    // Load textures
    g_State.bird_tex[0] = load_texture("assets/flappy_bird/bluebird-upflap.png");
    g_State.bird_tex[1] = load_texture("assets/flappy_bird/bluebird-midflap.png");
    g_State.bird_tex[2] = load_texture("assets/flappy_bird/bluebird-downflap.png");
    g_State.background_tex = load_texture("assets/flappy_bird/background-day.png");
    g_State.platform_tex = load_texture("assets/flappy_bird/base.png");
    g_State.pipe_tex = load_texture("assets/flappy_bird/pipe-green.png");
    g_State.game_over_tex = load_texture("assets/flappy_bird/gameover.png");
    g_State.messages = load_texture("assets/flappy_bird/message.png");

    // Initialize game state
    g_State.prev_time = CGL_utils_get_time();
    g_State.player_pos = 0.0f;
    g_State.player_velocity = 0.0f;
    g_State.frame_count = 0;
    g_State.score = 0;
    g_State.state = 0;

    for (int i = 0; i < 10; i++) g_State.pipes[i].x = -100.0f;

    return CGL_TRUE;
}

void cleanup()
{
    // Destroy textures
    for(int i = 0; i < 3; i++) CGL_texture_destroy(g_State.bird_tex[i]);
    CGL_texture_destroy(g_State.background_tex);
    CGL_texture_destroy(g_State.platform_tex);
    CGL_texture_destroy(g_State.pipe_tex);
    CGL_texture_destroy(g_State.game_over_tex);
    CGL_texture_destroy(g_State.messages);
    
    CGL_widgets_shutdown();
    CGL_shader_destroy(g_State.present_shader);
    CGL_framebuffer_destroy(g_State.framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(g_State.window);
    CGL_shutdown();
}

EM_BOOL loop(double time, void* userData)
{
    (void)time;
    (void)userData;

    // Update delta time
    g_State.curr_time = CGL_utils_get_time();
    g_State.delta_time = g_State.curr_time - g_State.prev_time;
    g_State.prev_time = g_State.curr_time;
    g_State.delta_time = CGL_utils_clamp(g_State.delta_time, 0.0f, 0.1f);

    // Update game state
    g_State.frame_count++;
    CGL_window_set_size(g_State.window, g_State.window_width, g_State.window_height);

    // Game logic
    if(g_State.state == 1) {
        // Update physics
        g_State.player_velocity += GRAVITY * g_State.delta_time;
        g_State.player_pos += g_State.player_velocity * g_State.delta_time;
        
        // Update bird animation
        if (g_State.frame_count % 12 == 0) 
            g_State.current_player_sprite = (g_State.current_player_sprite + 1) % 3;

        if(CGL_window_get_key(g_State.window, CGL_KEY_SPACE) == CGL_PRESS) 
            g_State.player_velocity = JUMP_VELOCITY;

        g_State.closest_pipe_index = update_pipes(-0.2f, g_State.player_pos);
        CGL_float top_ht = g_State.pipes[g_State.closest_pipe_index].y - 0.3f, bottom_ht = 2.0f - g_State.pipes[g_State.closest_pipe_index].y - 0.3f;
        CGL_float dist = (g_State.pipes[g_State.closest_pipe_index].x - (-0.2f)); CGL_float dista = fabsf(dist);
        if(g_State.player_pos < -0.65f) g_State.state = 2;
        else if((dista < 0.1f) && (g_State.player_pos > (1.0f - top_ht) || g_State.player_pos < (-1.0f + bottom_ht))) g_State.state = 2;
        else if(dist < -0.095f && g_State.closest_pipe_index != g_State.last_scored_pipe) { g_State.score++; g_State.last_scored_pipe = g_State.closest_pipe_index; }
    }        
    CGL_framebuffer_bind(g_State.framebuffer); // bind default framebuffer and also adjust viewport size and offset
    CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f); // clear screen with a dark gray color        
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD);
    CGL_shader_bind(g_State.present_shader);
    PO_TX(0.0f, 0.0f, 1.0f, 1.0f); DRAW_QUAD(g_State.background_tex, 1, -0.5625f, -1.0f, 2.0f * 0.5625f, 2.0f);        
    // Update platform scroll with delta time
    PO_TX(g_State.curr_time * PLATFORM_SCROLL_SPEED * (g_State.state == 1), 0.0f, 1.0f, 1.0f); 
    DRAW_QUAD(g_State.platform_tex, 2, -0.5625f, -1.0f, 2.0f * 0.5625f, 2.0f * 0.1422f);
    PO_TX(0.0f, 0.0f, 1.0f, 1.0f); DRAW_QUAD(g_State.bird_tex[g_State.current_player_sprite], 3, -0.2f + -0.17f * 0.5f, g_State.player_pos - 0.06f, 0.17f, 0.12f);
    for(CGL_int i = 0;i < 10 ; i++)
    {
        PO_TX(0.0f, 0.0f, 1.0f, -1.0f); DRAW_QUAD(g_State.pipe_tex, 4, g_State.pipes[i].x - 0.1f, 1.0f - g_State.pipes[i].y + 0.3f, 0.2f, g_State.pipes[i].y - 0.3f);
        PO_TX(0.0f, 0.0f, 1.0f, 1.0f); DRAW_QUAD(g_State.pipe_tex, 5, g_State.pipes[i].x - 0.1f, -1.0f, 0.2f, 2.0f - g_State.pipes[i].y - 0.3f);
    }
    if(g_State.state == 0)
    {
        PO_TX(0.0f, 0.75f, 1.0f, 0.28f); DRAW_QUAD(g_State.messages, 6, -0.5625f, 0.2f, 2.0f * 0.5625f, 0.2f);
        if(CGL_window_get_key(g_State.window, CGL_KEY_SPACE) == CGL_PRESS) { g_State.player_pos = 0.0f; g_State.player_velocity = 0.0f; g_State.state = 1; g_State.score = 0; for (int i = 0; i < 10; i++) g_State.pipes[i].x = -100.0f; }
    }
    if(g_State.state == 2)
    {
        PO_TX(0.0f, 0.0f, 1.0f, 1.0f); DRAW_QUAD(g_State.game_over_tex, 7, -0.5625f, 0.2f, 2.0f * 0.5625f, 0.2f);
        if(CGL_window_get_key(g_State.window, CGL_KEY_R) == CGL_PRESS) { g_State.player_pos = 0.0f; g_State.player_velocity = 0.0f; g_State.state = 1; g_State.score = 0; for (int i = 0; i < 10; i++) g_State.pipes[i].x = -100.0f; }
    }
    glDisable(GL_BLEND);
    CGL_widgets_begin();
    CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.0f, 1.0f); CGL_widgets_add_rect2f(-1.0f, -1.0f, 1.0f - 0.5625f, 2.0f);CGL_widgets_add_rect2f(0.5625f, -1.0f, 1.0f - 0.5625f, 2.0f);
    CGL_widgets_set_fill_colorf(1.7f, 1.7f, 1.7f, 1.0f); static CGL_byte score_str[32]; sprintf(score_str, "%d", g_State.score); CGL_widgets_add_string(score_str, -0.1f, 0.75f, 0.2f, 0.2f);
    CGL_widgets_end();        
    CGL_window_swap_buffers(g_State.window); // swap framebuffers
    CGL_window_poll_events(g_State.window); // poll events (if this is not called every frame window will stop responding)

    return !CGL_window_should_close(g_State.window);
}

int main()
{
    if(!init()) return 1;

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
