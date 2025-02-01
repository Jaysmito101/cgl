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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
    CGL_shader_set_uniform_int(g_State.present_shader, CGL_shader_get_uniform_location(g_State.present_shader, "u_tex"), ind); \
    CGL_shader_set_uniform_vec4v(g_State.present_shader, CGL_shader_get_uniform_location(g_State.present_shader, "po"), (stx), (sty), (szx), (szy)); \
    CGL_gl_render_screen_quad(); \
} 

#define PO_TX(stx, sty, szx, szy) CGL_shader_set_uniform_vec4v(g_State.present_shader, CGL_shader_get_uniform_location(g_State.present_shader, "potx"), (stx), (sty), (szx), (szy));

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

struct
{
	CGL_window* window;
	int window_height;
	int window_width;
	CGL_framebuffer* framebuffer;
	CGL_shader* present_shader;
	CGL_texture* bird_tex[3], * background_tex, * platform_tex, * pipe_tex, * game_over_tex, * messages;
	CGL_vec3 pipes[10];
	CGL_simple_neural_network* best_brain;
	player players[PLAYER_COUNT];

	// Game state variables
	CGL_float curr_time;
	CGL_float time_step;
	CGL_float tot_time;
	CGL_float best_fitness;
	CGL_int frame_count;
	CGL_int closest_pipe_index;
	CGL_int score;
	CGL_int last_scored_pipe;
	CGL_int state;
	CGL_int generation;
} g_State;

CGL_texture* load_texture(const char* filename)
{
	CGL_image img = { 0 }; img.bytes_per_channel = 8;
	stbi_set_flip_vertically_on_load(true);
	img.data = stbi_load(filename, &img.width, &img.height, &img.channels, 0);
	if (!img.data) { CGL_error("Failed to load image: %s", filename); return NULL; }
	CGL_texture* tex = CGL_texture_create(&img); stbi_image_free(img.data);
	return tex;
}

CGL_int update_pipes(CGL_float player_x)
{
	CGL_float max_x = 0.0, min_dst = 100000.0f; CGL_int min_dst_index = -1;
	for (CGL_int i = 0; i < 10; i++) {
		max_x = CGL_utils_max(max_x, g_State.pipes[i].x);
	}
	max_x = CGL_utils_max(max_x, 1.0f);

	for (CGL_int i = 0; i < 10; i++) {
		g_State.pipes[i].x -= 0.005f;
		if (g_State.pipes[i].x < -2.0f)
		{
			CGL_float mid = CGL_utils_random_float_in_range(0.5f, 1.5f);
			g_State.pipes[i].x = max_x + 0.75f;
			g_State.pipes[i].y = mid;
			max_x = g_State.pipes[i].x;
		}
		CGL_float dst = fabsf(g_State.pipes[i].x - player_x);
		if (dst < min_dst) { min_dst = dst; min_dst_index = i; }
	}
	return min_dst_index;
}

void reset_player(player* pl, player* fittest)
{
	pl->acc = -5.8f; pl->vel = 0.0f; pl->pos = 0.0f;
	pl->score = 0; pl->state = 0; pl->sprite = 0;
	pl->time = 0.0f; if (!fittest) CGL_simple_neural_network_randomize_weights(pl->brain, -2.0f, 2.0f);
	else { CGL_simple_neural_network_copy_weights(pl->brain, fittest->brain); CGL_simple_neural_network_mutate(pl->brain, 0.03f); }
}

void init_player(player* pl)
{
	pl->brain = CGL_simple_neural_network_create((CGL_int[]) { INPUT_LAYER_SIZE, HIDDEN_LAYER_SIZE, OUTPUT_LAYER_SIZE }, 3);
	reset_player(pl, NULL);
}

void destroy_player(player* pl) { CGL_simple_neural_network_destroy(pl->brain); }

CGL_bool evaluate_player(CGL_int player, CGL_int closest_pipe)
{
	CGL_float inputs[5] = {
		g_State.players[player].pos,
		g_State.players[player].vel,
		g_State.pipes[closest_pipe].x,
		g_State.pipes[closest_pipe].y - GAP_SIZE_HALF,
		g_State.pipes[closest_pipe].y + GAP_SIZE_HALF
	};
	inputs[0] = (inputs[0] + 1.0f) / 2.0f; inputs[1] = (inputs[1] + 10.0f) / 20.0f;
	inputs[2] = (inputs[2] + 2.0f) / 4.0f; inputs[3] = (inputs[3] + 1.0f) / 2.0f;
	inputs[4] = (inputs[4] + 1.0f) / 2.0f; CGL_float outputs[1] = { 0.0f };
	CGL_simple_neural_network_evaluate(g_State.players[player].brain, inputs, outputs);
	return outputs[0] > 0.5f;
}


CGL_bool init() {
	srand((uint32_t)time(NULL));

	g_State.window_height = g_State.window_width = 700;
	g_State.time_step = 1.0f / 60.0f;
	g_State.state = -1;
	g_State.generation = 1;

	if (!CGL_init()) {
		return CGL_FALSE;
	}

	// Initialize window and OpenGL context
	g_State.window = CGL_window_create(g_State.window_width, g_State.window_height, "Flappy Bird AI - Jaysmito Mukherjee");
	if (!g_State.window) {
		return CGL_FALSE;
	}

	CGL_window_make_context_current(g_State.window); // make the opengl context for the window current
	if (!CGL_gl_init()) return CGL_FALSE; // initialize cgl opengl module    
	g_State.framebuffer = CGL_framebuffer_create_from_default(g_State.window); // load the default framebuffer (0) into CGL_framebuffer object
	CGL_widgets_init();
	g_State.present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
	// These are from : https://github.com/samuelcust/flappy-bird-assets
	// Liscence (MIT) : https://github.com/samuelcust/flappy-bird-assets/blob/master/LICENSE 
	g_State.bird_tex[0] = load_texture("assets/flappy_bird/bluebird-upflap.png");
	g_State.bird_tex[1] = load_texture("assets/flappy_bird/bluebird-midflap.png");
	g_State.bird_tex[2] = load_texture("assets/flappy_bird/bluebird-downflap.png");
	g_State.background_tex = load_texture("assets/flappy_bird/background-day.png");
	g_State.platform_tex = load_texture("assets/flappy_bird/base.png");
	g_State.pipe_tex = load_texture("assets/flappy_bird/pipe-green.png");
	g_State.game_over_tex = load_texture("assets/flappy_bird/gameover.png");
	g_State.messages = load_texture("assets/flappy_bird/message.png");
	for (int i = 0; i < 10; i++) {
        g_State.pipes[i].x = -100.0f;
    }
    for (CGL_int i = 0; i < PLAYER_COUNT; i++) {
        init_player(&g_State.players[i]);
    }
	g_State.curr_time = CGL_utils_get_time();
	g_State.tot_time = 0.0f;
	g_State.frame_count = 0;
	g_State.closest_pipe_index = 0;
	g_State.score = 0;
	g_State.last_scored_pipe = -1;
	g_State.state = -1;
	g_State.generation = 1;
	glfwSwapInterval(1);
	g_State.best_brain = CGL_simple_neural_network_create((CGL_int[]) { INPUT_LAYER_SIZE, HIDDEN_LAYER_SIZE, OUTPUT_LAYER_SIZE }, 3);
	g_State.best_fitness = 0.0f;

	return CGL_TRUE;
}

CGL_void cleanup() {
	for (CGL_int i = 0; i < PLAYER_COUNT; i++) destroy_player(&g_State.players[i]);
	CGL_texture_destroy(g_State.bird_tex[0]); CGL_texture_destroy(g_State.bird_tex[1]); CGL_texture_destroy(g_State.bird_tex[2]);
	CGL_texture_destroy(g_State.background_tex); CGL_texture_destroy(g_State.platform_tex); CGL_texture_destroy(g_State.pipe_tex);
	CGL_texture_destroy(g_State.game_over_tex); CGL_texture_destroy(g_State.messages);
	CGL_widgets_shutdown();
	CGL_shader_destroy(g_State.present_shader);
	CGL_framebuffer_destroy(g_State.framebuffer); // destory framebuffer object
	CGL_gl_shutdown(); // shutdown cgl opengl module
	CGL_window_destroy(g_State.window); // destroy window
	CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
}

EM_BOOL loop(double _time, void* userData) {
	(void)_time;
	(void)userData;

	g_State.frame_count++;
	g_State.curr_time = CGL_utils_get_time();
	CGL_window_set_size(g_State.window, g_State.window_width, g_State.window_height); // set window size
	if (g_State.state > -1 && g_State.state < PLAYER_COUNT)
	{
		g_State.closest_pipe_index = update_pipes(-0.2f);
		CGL_float top_ht = g_State.pipes[g_State.closest_pipe_index].y - GAP_SIZE_HALF, bottom_ht = 2.0f - g_State.pipes[g_State.closest_pipe_index].y - GAP_SIZE_HALF;
		CGL_float dist = (g_State.pipes[g_State.closest_pipe_index].x - (-0.2f)); CGL_float dista = fabsf(dist);
		for (CGL_int i = 0; i < PLAYER_COUNT; i++)
		{
			if (g_State.players[i].state == 1) continue; // 1 means dead
			g_State.players[i].vel += g_State.players[i].acc * g_State.time_step;
			g_State.players[i].pos += g_State.players[i].vel * g_State.time_step;
			if ((g_State.frame_count + i * 2) % 12 == 0) g_State.players[i].sprite = (g_State.players[i].sprite + 1) % 3;
			if (CGL_window_get_key(g_State.window, CGL_KEY_SPACE) == CGL_PRESS) g_State.players[i].vel = 1.2f;
			if (evaluate_player(i, g_State.closest_pipe_index)) g_State.players[i].vel = 1.2f;
			if (g_State.players[i].pos < -0.65f || g_State.players[i].pos > 1.0f) g_State.players[i].state = 1;
			else if ((dista < 0.1f) && (g_State.players[i].pos > (1.0f - top_ht) || g_State.players[i].pos < (-1.0f + bottom_ht))) g_State.players[i].state = 1;
			else if (dist < -0.095f && g_State.closest_pipe_index != g_State.last_scored_pipe) { g_State.score++; g_State.last_scored_pipe = g_State.closest_pipe_index; }
			g_State.players[i].time += g_State.time_step;
		}
		g_State.tot_time += g_State.time_step; g_State.state = 0; for (CGL_int i = 0; i < PLAYER_COUNT; i++) if (g_State.players[i].state == 1) g_State.state++;
	}
	if (CGL_window_get_key(g_State.window, CGL_KEY_ESCAPE) == CGL_PRESS) return CGL_FALSE;
	CGL_framebuffer_bind(g_State.framebuffer); // bind default framebuffer and also adjust viewport size and offset
	CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f); // clear screen with a dark gray color        
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD);
	CGL_shader_bind(g_State.present_shader);
	PO_TX(0.0f, 0.0f, 1.0f, 1.0f); DRAW_QUAD(g_State.background_tex, 1, -0.5625f, -1.0f, 2.0f * 0.5625f, 2.0f);
	PO_TX(g_State.curr_time * 0.2f * (g_State.state > -1 && g_State.state < PLAYER_COUNT), 0.0f, 1.0f, 1.0f); DRAW_QUAD(g_State.platform_tex, 2, -0.5625f, -1.0f, 2.0f * 0.5625f, 2.0f * 0.1422f);
	PO_TX(0.0f, 0.0f, 1.0f, 1.0f);
	for (CGL_int i = 0; i < PLAYER_COUNT; i++) if (g_State.players[i].state == 0) DRAW_QUAD(g_State.bird_tex[g_State.players[i].sprite], 3, -0.2f + -0.17f * 0.5f, g_State.players[i].pos - 0.06f, 0.17f, 0.12f);
	for (CGL_int i = 0; i < 10; i++)
	{
		PO_TX(0.0f, 0.0f, 1.0f, -1.0f); DRAW_QUAD(g_State.pipe_tex, 4, g_State.pipes[i].x - 0.1f, 1.0f - g_State.pipes[i].y + GAP_SIZE_HALF, 0.2f, g_State.pipes[i].y - GAP_SIZE_HALF);
		PO_TX(0.0f, 0.0f, 1.0f, 1.0f); DRAW_QUAD(g_State.pipe_tex, 5, g_State.pipes[i].x - 0.1f, -1.0f, 0.2f, 2.0f - g_State.pipes[i].y - GAP_SIZE_HALF);
	}
	if (g_State.state == -1)
	{
		PO_TX(0.0f, 0.75f, 1.0f, 0.28f); DRAW_QUAD(g_State.messages, 6, -0.5625f, 0.2f, 2.0f * 0.5625f, 0.2f);
		if (CGL_window_get_key(g_State.window, CGL_KEY_SPACE) == CGL_PRESS)
		{
			g_State.state = 0; g_State.tot_time = 0.0f; g_State.last_scored_pipe = -1; g_State.score = 0;
			for (CGL_int i = 0; i < PLAYER_COUNT; i++) reset_player(&g_State.players[i], NULL);
			for (int i = 0; i < 10; i++) g_State.pipes[i].x = -100.0f;
		}
	}
	if (g_State.state == PLAYER_COUNT)
	{
		PO_TX(0.0f, 0.0f, 1.0f, 1.0f); DRAW_QUAD(g_State.game_over_tex, 7, -0.5625f, 0.2f, 2.0f * 0.5625f, 0.2f);
		CGL_int max_time_index = 0; CGL_float max_time = 0.0f;
		for (CGL_int i = 0; i < PLAYER_COUNT; i++) if (g_State.players[i].time > max_time) { max_time = g_State.players[i].time; max_time_index = i; }
		player temp = g_State.players[0]; g_State.players[0] = g_State.players[max_time_index]; g_State.players[max_time_index] = temp;
		if (g_State.players[0].time > g_State.best_fitness) { g_State.best_fitness = g_State.players[0].time;  CGL_simple_neural_network_copy_weights(g_State.best_brain, g_State.players[0].brain); }
		else if (g_State.players[0].time < g_State.best_fitness * 0.6f) CGL_simple_neural_network_copy_weights(g_State.players[0].brain, g_State.best_brain);
		{
			g_State.state = 0; g_State.tot_time = 0.0f; g_State.last_scored_pipe = -1; g_State.score = 0;
			for (CGL_int i = 1; i < PLAYER_COUNT; i++) {
                reset_player(&g_State.players[i], &g_State.players[0]);
            }
			for (int i = 0; i < 10; i++) {
                g_State.pipes[i].x = -100.0f;
            }
            g_State.generation++;
		}
	}
	glDisable(GL_BLEND); CGL_widgets_begin();
	CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.0f, 1.0f); CGL_widgets_add_rect2f(-1.0f, -1.0f, 1.0f - 0.5625f, 2.0f); CGL_widgets_add_rect2f(0.5625f, -1.0f, 1.0f - 0.5625f, 2.0f);
	static CGL_byte score_str[32];
	CGL_widgets_set_fill_colorf(1.7f, 1.7f, 1.7f, 1.0f); sprintf(score_str, "%d", g_State.score); CGL_widgets_add_string(score_str, -0.1f, 0.75f, 0.2f, 0.2f);
	CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.0f, 1.0f);
	sprintf(score_str, "%f", g_State.best_fitness); CGL_widgets_add_string(score_str, -0.2f, 0.55f, 0.4f, 0.05f);
	sprintf(score_str, "%f", g_State.tot_time); CGL_widgets_add_string(score_str, -0.2f, 0.50f, 0.4f, 0.05f);
	sprintf(score_str, "%d", g_State.generation); CGL_widgets_add_string(score_str, -0.05f, 0.45f, 0.1f, 0.05f);
	sprintf(score_str, "%d", (PLAYER_COUNT - g_State.state)); CGL_widgets_add_string(score_str, -0.05f, 0.40f, 0.1f, 0.05f);
	CGL_widgets_end();
	CGL_window_swap_buffers(g_State.window); // swap framebuffers
	CGL_window_poll_events(g_State.window); // poll events (if this is not called every frame window will stop responding)

	return CGL_TRUE;
}

int main()
{
	if (!init()) return EXIT_FAILURE;
#ifdef CGL_WASM
	CGL_info("Running in WASM mode");
	emscripten_request_animation_frame_loop(loop, NULL);
#else
	// run till the close button is clicked
	while (!CGL_window_should_close(g_State.window)) {
		if (!loop(0.0, NULL)) break;
	}
	cleanup();
#endif
	return EXIT_SUCCESS;
}
