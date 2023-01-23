// Copyright Jaysmito Mukherjee, 2023
// I am the sole copyright owner of this Work.
// You cannot host, display, distribute or share this Work neither
// as it is or altered, anywhere else, in any
// form including physical and digital. You cannot use this Work in any
// commercial or non-commercial product, website or project. You cannot
// sell this Work and you cannot mint an NFTs of it or train a neural
// network with it without permission. I share this Work for educational
// purposes, and you can link to it, through an URL, proper attribution
// and unmodified screenshot, as part of your educational material. If
// these conditions are too restrictive please contact me and we'll
// definitely work it out.
// email : jaysmito101@gmail.com

#define CGL_EXCLUDE_NETWORKING
#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#include "cgl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


static const char* PASS_THROUGH_VERTEX_SHADER_SOURCE = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
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

static const char* PASS_THROUGH_FRAGMENT_SHADER_SOURCE = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Position;\n"
"in vec2 TexCoord;\n"
"\n"
"uniform sampler2D u_tex;\n"
"uniform vec4 potx = vec4(0.0f, 0.0f, 1.0f, 1.0f);"
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
    CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), ind); \
    CGL_shader_set_uniform_vec4v(present_shader, CGL_shader_get_uniform_location(present_shader, "po"), (stx), (sty), (szx), (szy)); \
    CGL_gl_render_screen_quad(); \
} 

#define PO_TX(stx, sty, szx, szy) CGL_shader_set_uniform_vec4v(present_shader, CGL_shader_get_uniform_location(present_shader, "potx"), (stx), (sty), (szx), (szy));

#define PLAYER_COUNT 10000
#define INPUT_LAYER_SIZE 5
#define HIDDEN_LAYER_SIZE 10
#define OUTPUT_LAYER_SIZE 1
#define GAP_SIZE 0.4f
#define GAP_SIZE_HALF (GAP_SIZE * 0.5f)

typedef struct
{
    CGL_int state;
    CGL_int score;
    CGL_int sprite;
    CGL_float pos;
    CGL_float vel;
    CGL_float acc;
    CGL_float time;
    CGL_simple_neural_network* brain;
} player;


static struct
{
    CGL_window* window; // the main window
    int window_height;
    int window_width;
    CGL_framebuffer* framebuffer;
    CGL_texture *bird_tex[3], *background_tex, *platform_tex, *pipe_tex, *game_over_tex, *messages;
    CGL_vec3 pipes[10];
} g_context;

static player players[PLAYER_COUNT];

CGL_texture* load_texture(const char* filename)
{
    CGL_image img = {0}; img.bytes_per_channel = 8;
    stbi_set_flip_vertically_on_load(true);
    img.data = stbi_load(filename, &img.width, &img.height, &img.channels, 0);
    if (!img.data) { CGL_error("Failed to load image: %s", filename); return NULL; }
    CGL_texture* tex = CGL_texture_create(&img); stbi_image_free(img.data);
    return tex;
}

CGL_int update_pipes(CGL_float player_x)
{
    CGL_float max_x = 0.0, min_dst = 100000.0f; CGL_int min_dst_index = -1;
    for(CGL_int i = 0; i < 10; i++) max_x = CGL_utils_max(max_x, g_context.pipes[i].x); max_x = CGL_utils_max(max_x, 1.0f);
    for(CGL_int i = 0; i < 10; i++)
    {
        g_context.pipes[i].x -= 0.005f;        
        if(g_context.pipes[i].x < -2.0f) 
        {
            CGL_float mid = CGL_utils_random_float_in_range(0.5f, 1.5f);
            g_context.pipes[i].x = max_x + 0.75f; g_context.pipes[i].y = mid; max_x = g_context.pipes[i].x;
        }
        CGL_float dst = fabsf(g_context.pipes[i].x - player_x);
        if(dst < min_dst) { min_dst = dst; min_dst_index = i; }
    }
    return min_dst_index;
}

void reset_player(player* pl, player* fittest)
{
    pl->acc = -5.8f; pl->vel = 0.0f; pl->pos = 0.0f;
     pl->score = 0; pl->state = 0; pl->sprite = 0;
    pl->time = 0.0f; if(!fittest) CGL_simple_neural_network_randomize_weights(pl->brain, -2.0f, 2.0f);
    else { CGL_simple_neural_network_copy_weights(pl->brain, fittest->brain); CGL_simple_neural_network_mutate(pl->brain, 0.03f); }
}

void init_player(player* pl)
{
    pl->brain = CGL_simple_neural_network_create((CGL_int[]){INPUT_LAYER_SIZE, HIDDEN_LAYER_SIZE, OUTPUT_LAYER_SIZE}, 3);
    reset_player(pl, NULL);
}

void destroy_player(player* pl) { CGL_simple_neural_network_destroy(pl->brain); }

CGL_bool evaluate_player(CGL_int player, CGL_int closest_pipe)
{
    CGL_float inputs[5] = {players[player].pos, players[player].vel, g_context.pipes[closest_pipe].x, g_context.pipes[closest_pipe].y - GAP_SIZE_HALF, g_context.pipes[closest_pipe].y + GAP_SIZE_HALF};
    inputs[0] = (inputs[0] + 1.0f) / 2.0f; inputs[1] = (inputs[1] + 10.0f) / 20.0f;
    inputs[2] = (inputs[2] + 2.0f) / 4.0f; inputs[3] = (inputs[3] + 1.0f) / 2.0f;
    inputs[4] = (inputs[4] + 1.0f) / 2.0f; CGL_float outputs[1] = {0.0f};
    CGL_simple_neural_network_evaluate(players[player].brain, inputs, outputs);
    return outputs[0] > 0.5f;
}

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return EXIT_FAILURE; // initialize CGL (required for setting up internals of CGL)
    g_context.window_width = g_context.window_height = 700;    
    g_context.window = CGL_window_create(g_context.window_width, g_context.window_height, "Flappy Bird AI - Jaysmito Mukherjee"); // create the window
    CGL_window_make_context_current(g_context.window); // make the opengl context for the window current
    if(!CGL_gl_init()) return EXIT_FAILURE; // initialize cgl opengl module    
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window); // load the default framebuffer (0) into CGL_framebuffer object
    CGL_widgets_init();
    CGL_shader* present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    // These are from : https://github.com/samuelcust/flappy-bird-assets
    // Liscence (MIT) : https://github.com/samuelcust/flappy-bird-assets/blob/master/LICENSE 
    g_context.bird_tex[0] = load_texture("assets/flappy_bird/bluebird-upflap.png");
    g_context.bird_tex[1] = load_texture("assets/flappy_bird/bluebird-midflap.png");
    g_context.bird_tex[2] = load_texture("assets/flappy_bird/bluebird-downflap.png");
    g_context.background_tex = load_texture("assets/flappy_bird/background-day.png");
    g_context.platform_tex = load_texture("assets/flappy_bird/base.png");
    g_context.pipe_tex = load_texture("assets/flappy_bird/pipe-green.png");
    g_context.game_over_tex = load_texture("assets/flappy_bird/gameover.png");
    g_context.messages = load_texture("assets/flappy_bird/message.png");
    for (int i = 0; i < 10; i++) g_context.pipes[i].x = -100.0f; for(CGL_int i = 0 ; i < PLAYER_COUNT; i++) init_player(&players[i]);
    CGL_float curr_time = CGL_utils_get_time(), time_step = 1.0f / 60.0f, tot_time = 0.0f;
    CGL_int frame_count = 0, current_player_sprite = 0, closest_pipe_index = 0, score = 0, last_scored_pipe = -1, state = -1, generation = 1;
    glfwSwapInterval(1);
    CGL_simple_neural_network* best_brain = CGL_simple_neural_network_create((CGL_int[]){INPUT_LAYER_SIZE, HIDDEN_LAYER_SIZE, OUTPUT_LAYER_SIZE}, 3);
    CGL_float best_fitness = 0.0f;
    while(!CGL_window_should_close(g_context.window)) // run till the close button is clicked
    {
        frame_count++; curr_time = CGL_utils_get_time();
        CGL_window_set_size(g_context.window, g_context.window_width, g_context.window_height); // set window size
        if(state > -1 && state < PLAYER_COUNT)
        {
            closest_pipe_index = update_pipes(-0.2f);
            CGL_float top_ht = g_context.pipes[closest_pipe_index].y - GAP_SIZE_HALF, bottom_ht = 2.0f - g_context.pipes[closest_pipe_index].y - GAP_SIZE_HALF;
            CGL_float dist = (g_context.pipes[closest_pipe_index].x - (-0.2f)); CGL_float dista = fabsf(dist);            
            for(CGL_int i = 0 ; i < PLAYER_COUNT; i++)
            {
                if(players[i].state == 1) continue; // 1 means dead
                players[i].vel += players[i].acc * time_step;
                players[i].pos += players[i].vel * time_step;
                if ((frame_count + i * 2) % 12 == 0) players[i].sprite = (players[i].sprite + 1) % 3;
                if(CGL_window_get_key(g_context.window, CGL_KEY_SPACE) == CGL_PRESS) players[i].vel = 1.2f;
                if(evaluate_player(i, closest_pipe_index)) players[i].vel = 1.2f;
                if(players[i].pos < -0.65f || players[i].pos > 1.0f) players[i].state = 1;
                else if((dista < 0.1f) && (players[i].pos > (1.0f - top_ht) || players[i].pos < (-1.0f + bottom_ht))) players[i].state = 1;
                else if(dist < -0.095f && closest_pipe_index != last_scored_pipe) { score++; last_scored_pipe = closest_pipe_index; }
                players[i].time += time_step;
            }
            tot_time += time_step; state = 0; for(CGL_int i = 0 ; i < PLAYER_COUNT; i++) if(players[i].state == 1) state++;
        } 
        if(CGL_window_get_key(g_context.window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
        CGL_framebuffer_bind(g_context.framebuffer); // bind default framebuffer and also adjust viewport size and offset
        CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f); // clear screen with a dark gray color        
        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD);
        CGL_shader_bind(present_shader);
        PO_TX(0.0f, 0.0f, 1.0f, 1.0f); DRAW_QUAD(g_context.background_tex, 1, -0.5625f, -1.0f, 2.0f * 0.5625f, 2.0f);        
        PO_TX(curr_time * 0.2f * (state > -1 && state < PLAYER_COUNT), 0.0f, 1.0f, 1.0f); DRAW_QUAD(g_context.platform_tex, 2, -0.5625f, -1.0f, 2.0f * 0.5625f, 2.0f * 0.1422f);
        PO_TX(0.0f, 0.0f, 1.0f, 1.0f);
        for(CGL_int i = 0; i < PLAYER_COUNT ; i++) if(players[i].state == 0) DRAW_QUAD(g_context.bird_tex[players[i].sprite], 3, -0.2f + -0.17f * 0.5f, players[i].pos - 0.06f, 0.17f, 0.12f);
        for(CGL_int i = 0;i < 10 ; i++)
        {
            PO_TX(0.0f, 0.0f, 1.0f, -1.0f); DRAW_QUAD(g_context.pipe_tex, 4, g_context.pipes[i].x - 0.1f, 1.0f - g_context.pipes[i].y + GAP_SIZE_HALF, 0.2f, g_context.pipes[i].y - GAP_SIZE_HALF);
            PO_TX(0.0f, 0.0f, 1.0f, 1.0f); DRAW_QUAD(g_context.pipe_tex, 5, g_context.pipes[i].x - 0.1f, -1.0f, 0.2f, 2.0f - g_context.pipes[i].y - GAP_SIZE_HALF);
        }
        if(state == -1)
        {
            PO_TX(0.0f, 0.75f, 1.0f, 0.28f); DRAW_QUAD(g_context.messages, 6, -0.5625f, 0.2f, 2.0f * 0.5625f, 0.2f);
            if(CGL_window_get_key(g_context.window, CGL_KEY_SPACE) == CGL_PRESS) 
            {
                state = 0; tot_time = 0.0f; last_scored_pipe = -1; score = 0;
                for(CGL_int i = 0 ; i < PLAYER_COUNT; i++) reset_player(&players[i], NULL);
                for (int i = 0; i < 10; i++) g_context.pipes[i].x = -100.0f;
            }
        }
        if(state == PLAYER_COUNT)
        {
            PO_TX(0.0f, 0.0f, 1.0f, 1.0f); DRAW_QUAD(g_context.game_over_tex, 7, -0.5625f, 0.2f, 2.0f * 0.5625f, 0.2f);
            CGL_int max_time_index = 0; CGL_float max_time = 0.0f;
            for(CGL_int i = 0 ; i < PLAYER_COUNT; i++) if(players[i].time > max_time) { max_time = players[i].time; max_time_index = i; }
            player temp = players[0]; players[0] = players[max_time_index]; players[max_time_index] = temp;                        
            if(players[0].time > best_fitness) { best_fitness = players[0].time;  CGL_simple_neural_network_copy_weights(best_brain, players[0].brain);  }
            else if(players[0].time < best_fitness * 0.6f) CGL_simple_neural_network_copy_weights(players[0].brain, best_brain);
            {
                state = 0; tot_time = 0.0f; last_scored_pipe = -1; score = 0;
                for(CGL_int i = 1 ; i < PLAYER_COUNT; i++) reset_player(&players[i], &players[0]);
                for (int i = 0; i < 10; i++) g_context.pipes[i].x = -100.0f; generation++;
            }
        }
        glDisable(GL_BLEND); CGL_widgets_begin();
        CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.0f, 1.0f); CGL_widgets_add_rect2f(-1.0f, -1.0f, 1.0f - 0.5625f, 2.0f);CGL_widgets_add_rect2f(0.5625f, -1.0f, 1.0f - 0.5625f, 2.0f);
        static CGL_byte score_str[32];
        CGL_widgets_set_fill_colorf(1.7f, 1.7f, 1.7f, 1.0f); sprintf(score_str, "%d", score); CGL_widgets_add_string(score_str, -0.1f, 0.75f, 0.2f, 0.2f);
        CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.0f, 1.0f);
        sprintf(score_str, "%f", best_fitness); CGL_widgets_add_string(score_str, -0.2f, 0.55f, 0.4f, 0.05f);
        sprintf(score_str, "%f", tot_time); CGL_widgets_add_string(score_str, -0.2f, 0.50f, 0.4f, 0.05f);
        sprintf(score_str, "%d", generation); CGL_widgets_add_string(score_str, -0.05f, 0.45f, 0.1f, 0.05f);
        sprintf(score_str, "%d", (PLAYER_COUNT - state)); CGL_widgets_add_string(score_str, -0.05f, 0.40f, 0.1f, 0.05f);
        CGL_widgets_end();        
        CGL_window_swap_buffers(g_context.window); // swap framebuffers
        CGL_window_poll_events(g_context.window); // poll events (if this is not called every frame window will stop responding)
    }
    for(CGL_int i = 0 ; i < PLAYER_COUNT; i++) destroy_player(&players[i]);
    CGL_texture_destroy(g_context.bird_tex[0]); CGL_texture_destroy(g_context.bird_tex[1]); CGL_texture_destroy(g_context.bird_tex[2]);
    CGL_texture_destroy(g_context.background_tex); CGL_texture_destroy(g_context.platform_tex); CGL_texture_destroy(g_context.pipe_tex);
    CGL_texture_destroy(g_context.game_over_tex); CGL_texture_destroy(g_context.messages);
    CGL_widgets_shutdown();
    CGL_shader_destroy(present_shader);
    CGL_framebuffer_destroy(g_context.framebuffer); // destory framebuffer object
    CGL_gl_shutdown(); // shutdown cgl opengl module
    CGL_window_destroy(g_context.window); // destroy window
    CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
    return EXIT_SUCCESS;
}
