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
#define CGL_EXCLUDE_NETWORKING
#define CGL_IMPLEMENTATION
#include "cgl.h"

#define ASPECT_RATIO (16.0f / 9.0f)
#define HITTER_HEIGHT 0.3f
#define HITTER_WIDTH 0.05f
#define BORDER_THICKNESS 0.01f
#define COLOR_GREEN 0.1f, 0.8f, 0.1f
#define COLOR_RED 0.8f, 0.1f, 0.1f
#define COLOR_PURE_RED 1.0f, 0.0f, 0.0f
#define COLOR_WHITE 0.8f, 0.8f, 0.8f
#define PLAYER_SPEED 0.001f
#define BALL_RADIUS 0.03f
#define BORDER_COLOR COLOR_PURE_RED
#define HINT_LINE_COLOR 0.1f, 0.1f, 0.1f

const char* quad_shader_vertex_source = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"out vec2 TexCoord;\n"
"\n"
"uniform vec2 ustart;\n"
"uniform vec2 usize;\n"
"\n"
"void main()\n"
"{\n"
"    vec2 pos = position.xy * usize * 0.5f + ustart;\n"
"    gl_Position = vec4(pos, 0.0f, 1.0f);\n"
"    TexCoord = texcoord.xy;\n"
"}";

const char* quad_shader_fragment_source = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec2 TexCoord;\n"
"\n"
"uniform vec3 ucolor;\n"
"uniform bool uiscolor;\n"
"uniform bool uiscircle;\n"
"uniform sampler2D utex;\n"
"\n"
"void main()\n"
"{\n"
"    if(uiscircle)\n"
"    {\n"
"        if(length(TexCoord*2.0f - vec2(1.0f)) > 1.0f) discard;\n"
"    }\n"
"    if(uiscolor)\n"
"        FragColor = vec4(ucolor, 1.0f);\n"
"    else\n"
"    {\n"
"        float factor = texture(utex, vec2(TexCoord.x, 1.0f - TexCoord.y)).r;\n"
"        if(factor < 0.5f) discard;\n"
"        FragColor = vec4(vec3(factor), 1.0f);\n"
"    }\n"
"}";

static struct
{
    int prev_window_height;
    int prev_window_width;
    CGL_window* main_window;
    CGL_framebuffer* main_framebuffer;
    CGL_mesh_gpu* quad_mesh;
    CGL_shader* quad_shader;
    int quad_shader_ustart;
    int quad_shader_usize;
    int quad_shader_ucolor;
    int quad_shader_utex;
    int quad_shader_uiscolor;
    int quad_shader_uiscircle;
} g_context;

static struct
{
    float player_height;
    float computer_height;
    float computer_speed;
    bool allow_input;
    bool is_over;
    bool is_game_won;
    int player_score;
    float ball_pos_x;
    float ball_pos_y;
    float ball_velocity_x;
    float ball_velocity_y;
    float total_time;
    float prev_time;
    float speed_factor;
    CGL_texture* you_win_text;
    CGL_texture* you_lose_text;
    CGL_texture* instruction_text;
} g_game;

void setup_quad_renderer()
{
    // setup mesh
    CGL_mesh_cpu* quad_mesh_cpu = CGL_mesh_cpu_quad(
        CGL_vec3_init(1.0f, 1.0f, 0.0f),
        CGL_vec3_init(1.0f, -1.0f, 0.0f),
        CGL_vec3_init(-1.0f, -1.0f, 0.0f),
        CGL_vec3_init(-1.0f, 1.0f, 0.0f)
    );
    g_context.quad_mesh = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(g_context.quad_mesh, quad_mesh_cpu, true);
    CGL_mesh_cpu_destroy(quad_mesh_cpu);

    g_context.quad_shader = CGL_shader_create(quad_shader_vertex_source, quad_shader_fragment_source, NULL);
    g_context.quad_shader_ustart = CGL_shader_get_uniform_location(g_context.quad_shader, "ustart");
    g_context.quad_shader_usize = CGL_shader_get_uniform_location(g_context.quad_shader, "usize");
    g_context.quad_shader_ucolor = CGL_shader_get_uniform_location(g_context.quad_shader, "ucolor");
    g_context.quad_shader_utex = CGL_shader_get_uniform_location(g_context.quad_shader, "utex");
    g_context.quad_shader_uiscolor = CGL_shader_get_uniform_location(g_context.quad_shader, "uiscolor");
    g_context.quad_shader_uiscircle = CGL_shader_get_uniform_location(g_context.quad_shader, "uiscircle");
}

void render_quad(float start_x, float start_y, float size_x, float size_y, float color_r, float color_g, float color_b)
{
    CGL_shader_bind(g_context.quad_shader);
    CGL_shader_set_uniform_vec2v(g_context.quad_shader, g_context.quad_shader_ustart, start_x + size_x/2.0f, start_y + size_y/2.0f);
    CGL_shader_set_uniform_vec2v(g_context.quad_shader, g_context.quad_shader_usize, size_x, size_y);
    CGL_shader_set_uniform_vec3v(g_context.quad_shader, g_context.quad_shader_ucolor, color_r, color_g, color_b);
    CGL_shader_set_uniform_bool(g_context.quad_shader, g_context.quad_shader_uiscolor, true);
    CGL_shader_set_uniform_bool(g_context.quad_shader, g_context.quad_shader_uiscircle, false);
    CGL_mesh_gpu_render(g_context.quad_mesh);
}

void render_circle(float pos_x, float pos_y, float radius, float color_r, float color_g, float color_b)
{
    CGL_shader_bind(g_context.quad_shader);
    CGL_shader_set_uniform_vec2v(g_context.quad_shader, g_context.quad_shader_ustart, pos_x, pos_y);
    CGL_shader_set_uniform_vec2v(g_context.quad_shader, g_context.quad_shader_usize, radius * 2.0f, radius * 2.0f * ASPECT_RATIO);
    CGL_shader_set_uniform_vec3v(g_context.quad_shader, g_context.quad_shader_ucolor, color_r, color_g, color_b);
    CGL_shader_set_uniform_bool(g_context.quad_shader, g_context.quad_shader_uiscolor, true);
    CGL_shader_set_uniform_bool(g_context.quad_shader, g_context.quad_shader_uiscircle, true);
    CGL_mesh_gpu_render(g_context.quad_mesh);
}

void render_quad_texture(float start_x, float start_y, float size_x, float size_y, CGL_texture* texture)
{
    CGL_shader_bind(g_context.quad_shader);
    CGL_shader_set_uniform_vec2v(g_context.quad_shader, g_context.quad_shader_ustart, start_x + size_x/2.0f, start_y + size_y/2.0f);
    CGL_shader_set_uniform_vec2v(g_context.quad_shader, g_context.quad_shader_usize, size_x, size_y);
    CGL_texture_bind(texture, 0);
    CGL_shader_set_uniform_int(g_context.quad_shader, g_context.quad_shader_utex, 0);
    CGL_shader_set_uniform_bool(g_context.quad_shader, g_context.quad_shader_uiscircle, false);
    CGL_shader_set_uniform_bool(g_context.quad_shader, g_context.quad_shader_uiscolor, false);
    CGL_mesh_gpu_render(g_context.quad_mesh);
}

void bake_game_over_text()
{
    CGL_font* font = CGL_font_load("font.ttf");
    CGL_font_build_atlas(font, 1024, 1024, 72);
    g_game.you_lose_text = CGL_text_bake_to_texture("You Lose", strlen("You Lose"), font, NULL, NULL);
    g_game.you_win_text = CGL_text_bake_to_texture("You Win", strlen("You Win"), font, NULL, NULL);
    g_game.instruction_text = CGL_text_bake_to_texture("Press R to restart", strlen("Press R to restart"), font, NULL, NULL);
    CGL_font_destory(font);
}

void handle_player_input()
{
    if(g_game.is_over || !g_game.allow_input) return;
    if(CGL_window_get_key(g_context.main_window, CGL_KEY_UP) == CGL_PRESS) g_game.player_height += PLAYER_SPEED * CGL_utils_clamp(g_game.speed_factor * 0.5f, 1.0f, 100.0f);
    if(CGL_window_get_key(g_context.main_window, CGL_KEY_DOWN) == CGL_PRESS) g_game.player_height -= PLAYER_SPEED * CGL_utils_clamp(g_game.speed_factor * 0.5f, 1.0f, 100.0f);
    g_game.player_height = CGL_utils_clamp(g_game.player_height, -1.0f + HITTER_HEIGHT/2.0f, 1.0f - HITTER_HEIGHT/2.0f);
}


void update_computer()
{
    if(g_game.is_over || g_game.allow_input) return; // do not give computer move is ball is in player area or game is over
    
    // the aim here is not creating a perfect AI or algorithm to win every time
    // but a kind of bad/weak logic so that the player also has a chance of winning

    /*
    // This logic is likely never to loose
    if((g_game.ball_pos_y - g_game.computer_height) > (HITTER_HEIGHT / 2.0f) )
        g_game.computer_height += PLAYER_SPEED;
    if((g_game.ball_pos_y - g_game.computer_height) < (HITTER_HEIGHT / 2.0f) )
        g_game.computer_height -= PLAYER_SPEED;
    */

    if(rand() % 100 > 95) g_game.computer_speed = CGL_utils_random_float() * g_game.speed_factor;
    if((g_game.ball_pos_y - g_game.computer_height) > (HITTER_HEIGHT / 2.0f) )
        g_game.computer_height += PLAYER_SPEED * g_game.computer_speed;
    if((g_game.ball_pos_y - g_game.computer_height) < (HITTER_HEIGHT / 2.0f) )
        g_game.computer_height -= PLAYER_SPEED * g_game.computer_speed;

    g_game.computer_height = CGL_utils_clamp(g_game.computer_height, -1.0f + HITTER_HEIGHT/2.0f, 1.0f - HITTER_HEIGHT/2.0f);
}

void update_ball()
{
    if(g_game.is_over) return;
    float curr_time = CGL_utils_get_time();
    float delta_time = CGL_utils_clamp(curr_time - g_game.prev_time, 0.0f, 0.2f);
    g_game.prev_time = curr_time;
    g_game.total_time += delta_time;
    g_game.speed_factor = 1.0f + 0.25f * logf(g_game.total_time + 1.0f) + 0.0002f * powf(logf(powf(g_game.total_time + 1, 2.0f)), 5.0f);
    g_game.speed_factor = CGL_utils_clamp(g_game.speed_factor, 0.5f, 10.0f);

    // now to make things insteresting ball speed should rise with time

    g_game.ball_pos_x += g_game.ball_velocity_x * delta_time * g_game.speed_factor;
    g_game.ball_pos_y += g_game.ball_velocity_y * delta_time * g_game.speed_factor;

    // allow player input only if ball is in player side
    g_game.allow_input = (g_game.ball_pos_x >= 0.0f);

    // handle collisions
    if(g_game.ball_pos_y <= (-1.0f + BALL_RADIUS + BORDER_THICKNESS))
        g_game.ball_velocity_y = (float)fabs(g_game.ball_velocity_y);
    if(g_game.ball_pos_y >= (1.0f - BALL_RADIUS - BORDER_THICKNESS))
        g_game.ball_velocity_y = -1.0f * (float)fabs(g_game.ball_velocity_y);
    if(g_game.ball_pos_x >= (1.0f - BALL_RADIUS - HITTER_WIDTH))
        g_game.ball_velocity_x = -1.0f * (float)fabs(g_game.ball_velocity_x);
    if(g_game.ball_pos_x <= (-1.0f + BALL_RADIUS + HITTER_WIDTH))
        g_game.ball_velocity_x = (float)fabs(g_game.ball_velocity_x);
    if ((g_game.ball_pos_y <= (-1.0f + BALL_RADIUS + BORDER_THICKNESS)) || (g_game.ball_pos_y >= (1.0f - BALL_RADIUS - BORDER_THICKNESS)) || (g_game.ball_pos_x >= (1.0f - BALL_RADIUS - HITTER_WIDTH)) || (g_game.ball_pos_x <= (-1.0f + BALL_RADIUS + HITTER_WIDTH)))
        if(rand() % 1000 > 980) 
            g_game.total_time = 0.0f;
}


void check_is_game_over()
{
    if(g_game.ball_pos_x >= (1.0f - BALL_RADIUS - HITTER_WIDTH))
    {
        if(fabs(g_game.ball_pos_y - g_game.player_height) >= (1.25f * HITTER_HEIGHT / 2.0f))
        {
            g_game.is_game_won = false;
            g_game.is_over = true;
        }
    }
        
    if(g_game.ball_pos_x <= (-1.0f + BALL_RADIUS + HITTER_WIDTH))
    {
        if(fabs(g_game.ball_pos_y - g_game.computer_height) >= (1.25f * HITTER_HEIGHT / 2.0f))
        {
            g_game.is_game_won = true;
            g_game.is_over = true;
        }
    }        
}


void reset_game()
{
    g_game.is_over = false;
    g_game.is_game_won = false;
    g_game.player_height = 0.0f;
    g_game.computer_height = 0.0f;
    g_game.computer_speed = CGL_utils_random_float();
    g_game.allow_input = true;
    g_game.ball_pos_x = 0.0f;
    g_game.ball_pos_y = 0.0f;
    g_game.ball_velocity_x = (CGL_utils_random_float() * 2.0f - 1.0f) * 0.4f;
    g_game.ball_velocity_y = (CGL_utils_random_float() * 2.0f - 1.0f) * 0.4f;
    g_game.prev_time = 0.0f;
    g_game.total_time = 0.0f;
    g_game.speed_factor = 1.0f;
}

int main()
{
    // setup cgl
    srand((uint32_t)time(NULL));    
    if(!CGL_init()) return EXIT_FAILURE;

    // setup window
    g_context.prev_window_height = (int)(640 / ASPECT_RATIO);
    g_context.prev_window_width = 640;
    g_context.main_window = CGL_window_create(g_context.prev_window_width, g_context.prev_window_height, "Pong -Jaysmito Mukherjee");
    if(!g_context.main_window) return EXIT_FAILURE;
    CGL_window_make_context_current(g_context.main_window);

    // setup opengl (glad)
    if(!CGL_gl_init()) return EXIT_FAILURE;

    // intialize CGL text
    if(!CGL_text_init()) return EXIT_FAILURE;

    bake_game_over_text();

    setup_quad_renderer();

    // pepare framebuffer
    g_context.main_framebuffer = CGL_framebuffer_create_from_default(g_context.main_window);

    reset_game();

    // main loop
    while(!CGL_window_should_close(g_context.main_window))
    {
        // render
        CGL_framebuffer_bind(g_context.main_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);

        if(!g_game.is_over)
        {
            // highlight which side can move
            if(g_game.allow_input) render_quad(0.0f, -1.0f, 1.0f, 2.0f, 0.4f, 0.4f, 0.4f);
            else render_quad(-1.0f, -1.0f, 1.0f, 2.0f, 0.4f, 0.4f, 0.4f);

            // render hint if H is pressed
            // hint is a line showint the current position y of ball
            if(CGL_window_get_key(g_context.main_window, CGL_KEY_H) == CGL_PRESS) render_quad(-1.0f, g_game.ball_pos_y - BORDER_THICKNESS / 2.0f, 2.0f, BORDER_THICKNESS, HINT_LINE_COLOR);
        }

        // render board border
        render_quad(-1.0f, -1.0f, 2.0f, BORDER_THICKNESS, BORDER_COLOR); // bottom
        render_quad(-1.0f, 1.0f - BORDER_THICKNESS, 2.0f, BORDER_THICKNESS, BORDER_COLOR); // top
        render_quad(-1.0f, -1.0f, BORDER_THICKNESS/ASPECT_RATIO, 2.0f, BORDER_COLOR); // left
        render_quad(1.0f - BORDER_THICKNESS/ASPECT_RATIO, -1.0f, BORDER_THICKNESS, 2.0f, BORDER_COLOR); // right

        // render area seperator
        render_quad(-BORDER_THICKNESS/ASPECT_RATIO*0.5f, -1.0f, BORDER_THICKNESS/ASPECT_RATIO, 2.0f, COLOR_WHITE);

        // render player hitter
        render_quad(1.0f - HITTER_WIDTH, g_game.player_height - HITTER_HEIGHT / 2.0f, HITTER_WIDTH, HITTER_HEIGHT, COLOR_GREEN);
        // render computer hitter
        render_quad(-1.0f, g_game.computer_height - HITTER_HEIGHT / 2.0f, HITTER_WIDTH, HITTER_HEIGHT, COLOR_RED);

        // render ball
        render_circle(g_game.ball_pos_x, g_game.ball_pos_y, BALL_RADIUS, COLOR_WHITE);

        if(g_game.is_over)
        {
            // render the text info
            render_quad_texture(-0.5f, -0.3f, 1.0f, 0.7f, g_game.is_game_won ? g_game.you_win_text : g_game.you_lose_text);
            render_quad_texture(-0.5f, -0.7f, 1.0f, 0.125f, g_game.instruction_text);

            // reset the game on pressing R
            if(CGL_window_get_key(g_context.main_window, CGL_KEY_R) == CGL_PRESS) reset_game();
        }

        // seap buffer
        CGL_window_swap_buffers(g_context.main_window);
        // poll the events
        CGL_window_poll_events(g_context.main_window);

        // handle input and update g_game.player_height
        handle_player_input();

        // update computer move
        update_computer();
    
        // update ball position and velocity
        update_ball();

        // check if game over codition is met and decide whether game is won or lost by user
        check_is_game_over();

        // force window to stay in ASPECT_RATIO
        CGL_window_get_size(g_context.main_window, &g_context.prev_window_width, &g_context.prev_window_height);
        g_context.prev_window_width = (int)(g_context.prev_window_height * ASPECT_RATIO);
        CGL_window_set_size(g_context.main_window, g_context.prev_window_width, g_context.prev_window_height);

        // quit if escape is pressed (just a convinience)
        if(CGL_window_get_key(g_context.main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }

    // cleanup
    CGL_texture_destroy(g_game.you_lose_text);
    CGL_texture_destroy(g_game.you_win_text);
    CGL_texture_destroy(g_game.instruction_text);

    CGL_text_shutdown();
    CGL_framebuffer_destroy(g_context.main_framebuffer);
    CGL_mesh_gpu_destroy(g_context.quad_mesh);
    CGL_shader_destroy(g_context.quad_shader);
    CGL_gl_shutdown();
    CGL_window_destroy(g_context.main_window);
    CGL_shutdown();
    return EXIT_SUCCESS;
}
