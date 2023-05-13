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
#include "cgl.h"

#define MAX_LIGHTS 64
#define BOARD_SIZE 16

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

static const char* PRIMARY_VERTEX_SHADER_SOURCE = "#version 430 core\n"
"\n"
"in layout(location = 0) vec4 position;\n"
"in layout(location = 1) vec4 normal;\n"
"in layout(location = 2) vec4 texcoord;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(position.xyz, 1.0);\n"
"}\n"
"";

static const char* PRIMARY_FRAGMENT_SHADER_SOURCE = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"#define MAX_LIGHTS 64\n"
"#define BOARD_RESOLUTION 16\n"
"\n"
"// uniforms\n"
"uniform vec4 light_positions[MAX_LIGHTS]; // w is intensity\n"
"uniform vec4 light_colors[MAX_LIGHTS]; // w is reserved\n"
"uniform int light_count;\n"
"uniform int board_piece_pos[BOARD_RESOLUTION][BOARD_RESOLUTION];\n"
"uniform float time;\n"
"uniform vec2 player_pos;\n"
"\n"
"// constants\n"
"const float PI = 3.14159265359f;\n"
"const int MAX_RAYMARCH_STEPS = 128;\n"
"const float MIN_DIST = 0.00001f;\n"
"const float MAX_DIST = 100.0f;\n"
"const float EPSILON = 0.00001f;\n"
"const vec2 RESOLUTION = vec2(600, 600);\n"
"\n"
"// colors\n"
"const vec3 COLOR_WOOD = vec3(133/256.0f, 94/256.0f, 66/256.0f);\n"
"const vec3 COLOR_WOOD_DARK = vec3(75/256.0f, 57/256.0f, 35/256.0f) * 0.5f;\n"
"\n"
"// sdf functions\n"
"float sphereSDF(vec3 p, float r)\n"
"{\n"
"    return length(p) - r;\n"
"}\n"
"\n"
"float boxSDF(vec3 p, vec3 b)\n"
"{\n"
"    vec3 d = abs(p) - b;\n"
"    return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));\n"
"}\n"
"\n"
"vec3 ray_direction(float fov, vec2 uv)\n"
"{\n"
"    vec2 xy = 2.0f * uv - 1.0f;\n"
"    vec3 dir = vec3(xy, -1.0f / tan(fov / 2.0f));\n"
"    return normalize(dir);\n"
"}\n"
"\n"
"mat3 set_camera( in vec3 ro, in vec3 ta, float cr )\n"
"{\n"
"	vec3 cw = normalize(ta-ro);\n"
"	vec3 cp = vec3(sin(cr), cos(cr),0.0);\n"
"	vec3 cu = normalize( cross(cw,cp) );\n"
"	vec3 cv =          ( cross(cu,cw) );\n"
"    return mat3( cu, cv, cw );\n"
"}\n"
"\n"
"vec3 color_select_mn(float d1, float d2, vec3 c1, vec3 c2)\n"
"{\n"
"    return d1 < d2 ? c1 : c2;\n"
"}\n"
"\n"
"vec3 color_select_mx(float d1, float d2, vec3 c1, vec3 c2)\n"
"{\n"
"    return d1 > d2 ? c1 : c2;\n"
"}\n"
"\n"
"vec4 map(vec3 p)\n"
"{\n"
"    vec3 color = vec3(0.0f);\n"
"    float d = 0.0f, tmp = 0.0f;\n"
"\n"
"    // floor\n"
"    d = boxSDF(p, vec3(3.0f, 0.1f, 3.0f)); \n"
"    color = vec3(0.1f, 0.1f, 0.1f);\n"
"\n"
"    // game board\n"
"    vec3 color_board = vec3(0.0f);\n"
"    float d_board = 0.0f;\n"
"    // base\n"
"    float board_size = 2.0f;\n"
"    {\n"
"        d_board = boxSDF(p, vec3(board_size, 0.45f, board_size));\n"
"        color_board = COLOR_WOOD;\n"
"    }\n"
"\n"
"    // pieces\n"
"    float cell_size = 2.0f * board_size / float(BOARD_RESOLUTION);\n"
"    {\n"
"        vec2 tmp_v2 = clamp(p.xz, -board_size, board_size);\n"
"        ivec2 tmp2_v2 = ivec2( ( (tmp_v2 + board_size)*0.5f/board_size) * BOARD_RESOLUTION );\n"
"        tmp_v2 = mod(tmp_v2, cell_size) - cell_size * 0.5f;\n"
"\n"
"        \n"
"\n"
"        \n"
"        vec3 p_pcs = vec3(tmp_v2.x, p.y - 0.5f, tmp_v2.y);\n"
"        //float d_pcs = sphereSDF(p_pcs, cell_size * 0.75f);\n"
"        float d_pcs = boxSDF(p_pcs, vec3(cell_size, 0.3f, cell_size));\n"
"        float mask = float(board_piece_pos[tmp2_v2.x][tmp2_v2.y]);\n"
"        mask = clamp(mask, 0.0f, 1.0f);\n"
"        d_board = max(d_board, mix(d_board, -d_pcs, mask));\n"
"    }\n"
"    color = color_select_mn(d, d_board, color, color_board);\n"
"    \n"
"    // board borders\n"
"    {\n"
"        float d_border = boxSDF(p - vec3(0.0f, 0.0f, board_size+0.1f), vec3(board_size+0.1f, 0.6f, 0.01f));\n"
"        d_border = min(d_border, boxSDF(p - vec3(0.0f, 0.0f, -board_size-0.1f), vec3(board_size+0.1f, 0.6f, 0.01f)));\n"
"        d_border = min(d_border, boxSDF(p - vec3(board_size+0.1f, 0.0f, 0.0f), vec3(0.01f, 0.6f, board_size+0.1f)));\n"
"        d_border = min(d_border, boxSDF(p - vec3(-board_size-0.1f, 0.0f, 0.0f), vec3(0.01f, 0.6f, board_size+0.1f)));\n"
"        color_board = color_select_mn(d_board, d_border, color_board, COLOR_WOOD_DARK);\n"
"        d_board = min(d_board, d_border);\n"
"    }\n"
"    \n"
"    color = color_select_mn(d, d_board, color, color_board);\n"
"    d = min(d, d_board - 0.1f);\n"
"\n"
"    // balls\n"
"    for(int i = 4 ; i < light_count ; i++)\n"
"    {\n"
"        float dt = sphereSDF(p - light_positions[i].xyz, light_colors[i].w);\n"
"        color = color_select_mn(d, dt, color, light_colors[i].xyz);\n"
"        d = min(d, dt);\n"
"    }\n"
"\n"
"    // player\n"
"    {\n"
"        float dt = sphereSDF(p - vec3(player_pos.x, 0.5f, player_pos.y), 0.05f);\n"
"        color = color_select_mn(d, dt, color, vec3(0.2f, 1.0f, 0.3f));\n"
"        d = min(d, dt);\n"
"    }\n"
"\n"
"\n"
"    return vec4(color, d);\n"
"}\n"
"\n"
"vec3 estimate_normal(vec3 p)\n"
"{\n"
"    vec2 e = vec2(0.005f, 0.0f);\n"
"    vec3 n = vec3(\n"
"        map(p + e.xyy).w - map(p - e.xyy).w,\n"
"        map(p + e.yxy).w - map(p - e.yxy).w,\n"
"        map(p + e.yyx).w - map(p - e.yyx).w\n"
"    );\n"
"    return normalize(n);\n"
"}\n"
"\n"
"vec3 ray_march(vec3 ray_origin, vec3 ray_dir)\n"
"{\n"
"    float dist = 0.0f;\n"
"    int i = 0;\n"
"    vec4 tmp = vec4(0.0f);\n"
"    for (i = 0; i < MAX_RAYMARCH_STEPS; i++)\n"
"    {\n"
"        vec3 p = ray_origin + ray_dir * dist;\n"
"        tmp = map(p);\n"
"        float d = tmp.w;\n"
"        if (d <= EPSILON) break;\n"
"        if (dist >= MAX_DIST) break;\n"
"        dist += d;\n"
"    }\n"
"    if(dist >= MAX_DIST) return vec3(0.0f);\n"
"\n"
"    // calculate lighting\n"
"    vec3 p = ray_origin + ray_dir * dist;\n"
"    vec3 n = estimate_normal(p);\n"
"    vec3 color = vec3(0.0f);\n"
"    for (int i = 0; i < light_count; i++)\n"
"    {\n"
"        vec3 light_dir = normalize(light_positions[i].xyz - p);\n"
"        float diffuse = max(dot(n, light_dir), 0.0f);\n"
"        float l_distance = length(light_positions[i].xyz - p);\n"
"        float l_intensity = light_positions[i].w / (l_distance * l_distance);\n"
"        color += diffuse * light_colors[i].xyz * l_intensity;\n"
"    }\n"
"    return color * tmp.xyz;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    vec2 uv = gl_FragCoord.xy / RESOLUTION.xy;\n"
"    uv = uv * 2.0f - 1.0f;\n"
"\n"
"    vec3 camera_pos = vec3(3.0f*cos(time*0.2f), 5.0f, 3.0f*sin(time*0.2f));\n"
"    vec3 camera_target = vec3(0.0f, 0.0f, 0.0f);\n"
"\n"
"    vec3 ww = normalize(camera_target - camera_pos);\n"
"    vec3 uu = normalize(cross(ww, vec3(0.0f, 1.0f, 0.0f)));\n"
"    vec3 vv = cross(uu, ww);\n"
"\n"
"    vec3 ray_origin = camera_pos;\n"
"    vec3 ray_dir = normalize(uv.x * uu + uv.y * vv + 1.5f * ww);\n"
"\n"
"    vec3 color = ray_march(ray_origin, ray_dir);\n"
"\n"
"    FragColor = vec4(color, 1.0);\n"
"}";

typedef struct
{
    CGL_vec3 position;
    CGL_vec3 color;
    CGL_float radius;
    CGL_float intensity;
    CGL_bool claimed;
} light;

static light lights[MAX_LIGHTS];
static CGL_int num_lights = 0;

light create_light(CGL_vec3 position, CGL_vec3 color, CGL_float radius, CGL_float intensity)
{
    light l;
    l.position = position;
    l.color = color;
    l.radius = radius;
    l.intensity = intensity;
    l.claimed = false;
    return l;
}

static CGL_byte board[BOARD_SIZE][BOARD_SIZE] = { 0 };
static CGL_byte ke_sets[BOARD_SIZE][BOARD_SIZE] = { 0 };
static CGL_int walls[BOARD_SIZE * BOARD_SIZE] = { 0 };
static CGL_int num_walls = 0;
static CGL_vec2 player_pos;
static CGL_ivec4 player_cell;

static void shuffle(void* array, size_t n, size_t size)
{
    char tmp[256];
    char* arr = array;
    size_t stride = size * sizeof(char);

    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; ++i) {
            size_t rnd = (size_t)rand();
            size_t j = i + rnd / (RAND_MAX / (n - i) + 1);

            memcpy(tmp, arr + j * stride, size);
            memcpy(arr + j * stride, arr + i * stride, size);
            memcpy(arr + i * stride, tmp, size);
        }
    }
}

void propagate_set(CGL_int src, CGL_int dst)
{
    for (CGL_int i = 0; i < 16; i++)
        for (CGL_int j = 0; j < 16; j++)
            if (ke_sets[i][j] == src) ke_sets[i][j] = dst;
}

void get_partitioning_sets(CGL_int index, CGL_int* a, CGL_int* b)
{
    CGL_int i = index / BOARD_SIZE;
    CGL_int j = index % BOARD_SIZE;
    if (i % 2 != 0)
    {

        *a = ke_sets[i - 1][j];
        *b = ke_sets[i + 1][j];
    }
    else if (j % 2 != 0)
    {
        *a = ke_sets[i][j - 1];
        *b = ke_sets[i][j + 1];
    }
    else
    {
        *a = -1;
        *b = -1;
    }
}

void generate_maze()
{
    num_walls = 0;
    CGL_int counter = 1;
    for (CGL_int i = 0; i < 16; i++)
    {
        for (CGL_int j = 0; j < 16; j++)
        {
            ke_sets[i][j] = 0;
            if (i % 2 == 0 && j % 2 == 0) { board[i][j] = 1; ke_sets[i][j] = counter++; }
            else { board[i][j] = 0; walls[num_walls++] = i * 16 + j; }
        }
    }
    shuffle(walls, num_walls, sizeof(CGL_int));
    for (CGL_int i = 0; i < num_walls; i++)
    {
        CGL_int a = 0, b = 0;
        get_partitioning_sets(walls[i], &a, &b);
        if (a != b)
        {
            propagate_set(a, b);
            CGL_int i1 = walls[i] / BOARD_SIZE;
            CGL_int j1 = walls[i] % BOARD_SIZE;
            board[i1][j1] = 1;
            walls[i] = -1;
        }
    }
    // place the balls
    CGL_int lamps_left = 5;
    while (lamps_left > 0)
    {
        CGL_int i = rand() % 16;
        CGL_int j = rand() % 16;
        if (board[i][j] == 1)
        {
            CGL_float tmpi = (CGL_float)i / BOARD_SIZE * 2.0f - 1.0f;
            tmpi = (tmpi + 1.0f / BOARD_SIZE) * 2.0f;

            CGL_float tmpj = (CGL_float)j / BOARD_SIZE * 2.0f - 1.0f;
            tmpj = (tmpj + 1.0f / BOARD_SIZE) * 2.0f;
            lights[3 + lamps_left] = create_light(
                CGL_vec3_init(tmpi, 0.5f, tmpj),
                CGL_vec3_init(1.5f, 1.5f, 1.5f),
                0.05f,
                0.1f
            );
            board[i][j] = 3 + lamps_left;
            lamps_left--;
        }
    }
    num_lights = 9;

    player_pos = CGL_vec2_init(1.0f, 1.0f);
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++)
            if (board[i][j] == 1)
            {
                player_pos.x = (CGL_float)i / BOARD_SIZE * 2.0f - 1.0f;
                player_pos.x = (player_pos.x + 1.0f / BOARD_SIZE) * 2.0f;
                player_pos.y = (CGL_float)j / BOARD_SIZE * 2.0f - 1.0f;
                player_pos.y = (player_pos.y + 1.0f / BOARD_SIZE) * 2.0f;
                return;
            }
}


int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(600, 600, "3D Maze - Jaysmito Mukherjee");
    if (!window) return 1;
    CGL_window_make_context_current(window);
    CGL_gl_init();
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);
    CGL_framebuffer* bloom_framebuffer = CGL_framebuffer_create(600, 600);

    lights[num_lights++] = create_light(
        CGL_vec3_init(-3.0f, 1.0f, 3.0f),
        CGL_vec3_init(1.0f, 1.0f, 1.0f),
        1.0f,
        2.0f
    );
    lights[num_lights++] = create_light(
        CGL_vec3_init(3.0f, 1.0f, 3.0f),
        CGL_vec3_init(1.0f, 1.0f, 1.0f),
        1.0f,
        2.0f
    );
    lights[num_lights++] = create_light(
        CGL_vec3_init(3.0f, 1.0f, -3.0f),
        CGL_vec3_init(1.0f, 1.0f, 1.0f),
        1.0f,
        2.0f
    );
    lights[num_lights++] = create_light(
        CGL_vec3_init(-3.0f, 1.0f, -3.0f),
        CGL_vec3_init(1.0f, 1.0f, 1.0f),
        1.0f,
        2.0f
    );

    generate_maze();

    CGL_shader* present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    CGL_shader* primary_shader = CGL_shader_create(PRIMARY_VERTEX_SHADER_SOURCE, PRIMARY_FRAGMENT_SHADER_SOURCE, NULL);

    CGL_bloom* bloom = CGL_bloom_create(600, 600, 3);



    CGL_float curr_time = CGL_utils_get_time();
    CGL_float prev_time = CGL_utils_get_time();
    CGL_float time = 0.0f;


    CGL_float angle = 0.0f;

    while (!CGL_window_should_close(window))
    {
        curr_time = CGL_utils_get_time();
        CGL_float delta_time = curr_time - prev_time;
        prev_time = curr_time;

        time += CGL_utils_clamp(delta_time, 0.0f, 0.05f); // to avoid sudden jumps in time due to lag or things like window resizing

        CGL_window_set_size(window, 600, 600); // force window size to be 600x600

        CGL_vec2 player_pos_c = player_pos;

        if (CGL_window_get_key(window, CGL_KEY_RIGHT) == CGL_PRESS) player_pos_c.y += 0.05f;
        if (CGL_window_get_key(window, CGL_KEY_LEFT) == CGL_PRESS) player_pos_c.y -= 0.05f;
        if (CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) player_pos_c.x -= 0.05f;
        if (CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) player_pos_c.x += 0.05f;
        if (CGL_window_get_key(window, CGL_KEY_Q) == CGL_PRESS) angle -= 0.1f;
        if (CGL_window_get_key(window, CGL_KEY_E) == CGL_PRESS) angle += 0.1f;

        CGL_ivec4 player_cell_c;
        player_cell_c.x = (CGL_int)(((player_pos_c.x + 2.0f) * 0.5f / 2.0f) * BOARD_SIZE);
        player_cell_c.y = (CGL_int)(((player_pos_c.y + 2.0f) * 0.5f / 2.0f) * BOARD_SIZE);

        if (player_cell_c.x >= 0 && player_cell_c.x < BOARD_SIZE && player_cell_c.y >= 0 && player_cell_c.y < BOARD_SIZE)
        {
            if (board[player_cell_c.x][player_cell_c.y] != 0)
            {
                player_pos = player_pos_c;
                player_cell = player_cell_c;
            }
        }
        CGL_byte vl = board[player_cell.x][player_cell.y];
        if (vl > 3)
        {
            lights[vl].claimed = true;
            lights[vl].color = CGL_vec3_init(0.0f, 1.0f, 0.0f);
        }




        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        CGL_framebuffer_bind(bloom_framebuffer);
        CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
        CGL_shader_bind(primary_shader);
        // bind lights
        CGL_shader_set_uniform_int(primary_shader, CGL_shader_get_uniform_location(primary_shader, "light_count"), num_lights);
        for (CGL_int i = 0; i < num_lights; i++)
        {
            static CGL_byte uniform_name[64];
            sprintf(uniform_name, "light_positions[%d]", i);
            CGL_shader_set_uniform_vec4v(primary_shader, CGL_shader_get_uniform_location(primary_shader, uniform_name), lights[i].position.x, lights[i].position.y, lights[i].position.z, lights[i].intensity);
            sprintf(uniform_name, "light_colors[%d]", i);
            CGL_shader_set_uniform_vec4v(primary_shader, CGL_shader_get_uniform_location(primary_shader, uniform_name), lights[i].color.x, lights[i].color.y, lights[i].color.z, lights[i].radius);
        }
        for (int i = 0; i < BOARD_SIZE; i++) for (int j = 0; j < BOARD_SIZE; j++)
        {
            static char uniform_name[64];
            sprintf(uniform_name, "board_piece_pos[%d][%d]", i, j);
            CGL_int out = board[i][j];
            CGL_shader_set_uniform_int(primary_shader, CGL_shader_get_uniform_location(primary_shader, uniform_name), out);
        }
        CGL_shader_set_uniform_float(primary_shader, CGL_shader_get_uniform_location(primary_shader, "time"), angle);
        CGL_shader_set_uniform_vec2v(primary_shader, CGL_shader_get_uniform_location(primary_shader, "player_pos"), player_pos.x, player_pos.y);
        CGL_gl_render_screen_quad();

        CGL_bloom_apply(bloom, CGL_framebuffer_get_color_texture(bloom_framebuffer));

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shader_bind(present_shader);
        CGL_texture_bind(CGL_framebuffer_get_color_texture(bloom_framebuffer), 0);
        CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 0);
        CGL_gl_render_screen_quad();

        CGL_window_poll_events(window);
        CGL_window_swap_buffers(window);

        if (CGL_window_get_key(window, CGL_KEY_R) == CGL_PRESS)
        {
            generate_maze();
            CGL_utils_sleep(200);
        }

    }



    CGL_shader_destroy(primary_shader);
    CGL_shader_destroy(present_shader);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_framebuffer_destroy(bloom_framebuffer);
    CGL_bloom_destroy(bloom);
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
    return 0;
}
