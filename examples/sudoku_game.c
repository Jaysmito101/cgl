// Copyright Jaysmito Mukherjee, 2023
// I am the sole copyright owner of this Work.
// You cannot host, display, distribute or share this Work neither
// as it is or altered, here on Shadertoy or anywhere else, in any
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

#define ASPECT_RATIO 1.0f // an aspect ratio on 1.0 will be needed since our sudoku game board will be a square
#define BORDER_WIDTH 0.01f
#define FIXED_COLOR "0.9f, 0.9f, 0.9f"
#define CORRECT_COLOR "0.01f, 0.5f, 0.1f"
#define WRONG_COLOR "0.5f, 0.1f, 0.01f"

static const char* quad_mesh_vertex_shader_source = 
"#version 430 core\n"
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
"   vec2 pos = position.xy * usize * 0.5f + ustart;\n"
"   gl_Position = vec4(pos, 0.0f, 1.0f);\n"
"   TexCoord = texcoord.xy;\n"
"}\n"
"\n";

static const char* quad_mesh_fragment_shader_source = 
"#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec2 TexCoord;\n"
"\n"
"uniform vec3 ucolor;\n"
"uniform bool uiscolor;\n"
"uniform sampler2D utex;\n"
"\n"
"void main()\n"
"{\n"
"   if(uiscolor)\n"
"       FragColor = vec4(ucolor, 1.0f);\n"
"   else\n"
"       FragColor = texture(utex, TexCoord);\n"
"}\n"
"\n";

static const char* board_vertex_shader_source = 
"#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"out vec2 TexCoord;\n"
"flat out int InstanceID;\n"
"\n"
"uniform float uborder_width;\n"
"\n"
"void main()\n"
"{\n"
"   float offset_x  = float(mod(gl_InstanceID, 9));\n"
"   float offset_y  = float(gl_InstanceID / 9);\n"
"   float usize  = (2.0f - 10.0f * uborder_width) / 9.0f;\n"
"   offset_x = -1.0f + uborder_width + (usize + uborder_width) * offset_x + usize * 0.5f;\n"
"   offset_y = -1.0f + uborder_width + (usize + uborder_width) * offset_y + usize * 0.5f;\n"
"   vec2 pos = position.xy * usize * 0.5f + vec2(offset_x, offset_y);\n"
"   gl_Position = vec4(pos, 0.0f, 1.0f);\n"
"   TexCoord = texcoord.xy;\n"
"   InstanceID = gl_InstanceID;\n"
"}\n"
"\n";

static const char* board_fragment_shader_source = 
"#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"out int MousePickID0;\n"
"\n"
"in vec2 TexCoord;\n"
"flat in int InstanceID;\n"
"\n"
"struct TileData{vec4 a; vec4 b;};\n"
"\n"
"layout (std430, binding = 0) buffer BoardData\n"
"{\n"
"   TileData texture_offset_size[81];\n"
"};\n"
"\n"
"uniform sampler2D ufont_atlas;\n"
"uniform int uselected_tile;\n"
"uniform int uhovered_tile;\n"
"uniform bool uischeckon;\n"
"\n"
"void main()\n"
"{\n"
"   vec4 data = texture_offset_size[InstanceID].a;\n"
"   vec4 data2 = texture_offset_size[InstanceID].b;\n"
"   vec2 offset = data.xy;\n"
"   vec2 size = data.zw;\n"
"   vec3 backgroud_color = vec3(0.5f);\n"
"   if(uhovered_tile >= 0 && InstanceID == uhovered_tile) backgroud_color = vec3(0.4f);\n"
"   if(InstanceID == uselected_tile) backgroud_color = vec3(0.3f);\n"
"   vec2 texCoord = TexCoord;\n"
"   float factor = texture(ufont_atlas, (vec2(texCoord.x, 1.0f - texCoord.y)) * size + offset).r;\n"
"   vec3 text_color = vec3(0.1f);\n"
"   if(uischeckon)\n"
"   {\n"
"      if(data2.y < 0.5f) text_color = vec3(" WRONG_COLOR ");\n"
"      if(data2.y > 0.5f) text_color = vec3(" CORRECT_COLOR ");\n"
"   }\n"
"   if(data2.x > 0.5f) text_color = vec3(" FIXED_COLOR ");\n"
"   if((factor < 0.5f) || (size.x <= 0.01f))\n"
"      FragColor = vec4(backgroud_color, 1.0f);\n"
"   else\n"
"      FragColor = vec4(text_color, 1.0f);\n"
"   MousePickID0 = InstanceID;\n"
"}\n"
"\n";

struct char10
{
    char d[11];
};

// this will store the gobal application data like window context, mesh handles for renderer, etc.
static struct
{
    CGL_window* window; // the main window
    int window_height;
    int window_width;

    CGL_framebuffer* main_framebuffer;
    CGL_framebuffer* framebuffer;

    CGL_mesh_gpu* quad_mesh;
    CGL_shader* quad_shader;
    int quad_shader_ustart;
    int quad_shader_usize;
    int quad_shader_ucolor;
    int quad_shader_uiscolor;
    int quad_shader_utex;

    CGL_shader* board_shader;
    int board_shader_uborder_width;
    int board_shader_ufont_atlas;
    int board_shader_uselected_tile;
    int board_shader_uhovered_tile;
    int board_shader_uischeckon;
    CGL_ssbo* board_data_ssbo;
    CGL_font* font;
    CGL_texture* you_won_text;
    CGL_texture* instruction_text;

    bool is_check_on;
    bool is_input_allowed;
    int selected_tile;
    int hovered_tile;
    char board_solved[81];
    char board_original[81];
    char board[81];
    bool is_over;

    struct char10 board_gen[81];
    char priority_list[81];
} g_context;

int board_gen_priority_list_sort_comp(const void* el1, const void* el2)
{
    struct char10* elem1 = (struct char10*)el1;
    struct char10* elem2 = (struct char10*)el2;
    if(elem2->d[0] > elem1->d[0]) return -1;
    if(elem2->d[0] < elem1->d[0]) return 1;
    return 0;
}

// i am using selection sort here but it could have been something better like quick sort or merge sort
// but in this case since it is going to run only once every game that too on a very small array so it
// does not matter much
void board_gen_update_priority_list()
{
    int i, j, index;
    for(i = 0 ; i < 81 - 1 ; i ++)
    {
        index = i;
        for(j = i + 1 ; j < 81 ; j++)
            if(board_gen_priority_list_sort_comp(&g_context.board_gen[g_context.priority_list[j]], &g_context.board_gen[g_context.priority_list[index]]) < 0)
                index = j;
        if(index != i)
        {
            char temp = g_context.priority_list[i];
            g_context.priority_list[i] = g_context.priority_list[index];
            g_context.priority_list[index] = temp;
        }
    }
}

void board_gen_collapse_item(int index);

void board_gen_remove_item(int index, char item_val_to_remove)
{
    if(g_context.board_gen[index].d[0] > 1 && g_context.board_gen[index].d[item_val_to_remove] == 1)
    {
        g_context.board_gen[index].d[0] -= 1;
        g_context.board_gen[index].d[item_val_to_remove] = 0;
        if(g_context.board_gen[index].d[0] == 1) board_gen_collapse_item(index);
    }
}

void board_gen_collapse_item(int selected_item)
{
    // get board index of the selected item
    int board_index_x = selected_item % 9;
    int board_index_y = selected_item / 9;
    struct char10* item = &g_context.board_gen[selected_item];
    char item_val_to_remove = -1;
    // select item value to collapse to
    for(int i = 1 ; i < 10 ; i++) 
    {
        if(item->d[i] == 1)
        {
            item->d[i] = 0;
            item_val_to_remove = (char)i;
            item->d[10] = item_val_to_remove;
            break;
        }
    }
    memset(item->d + 1, 0, 9);
    item->d[item_val_to_remove] = 1;
    item->d[0] = 1;
    
    // update the entropies of related items
    // CGL_LOG("Remove %d from Col : %d Row : %d\n", item_val_to_remove, board_index_x, (8 - board_index_y));
    for(int i = 0 ; i < 9 ; i++)
    {        
        board_gen_remove_item(i * 9 + board_index_x, item_val_to_remove); // in the same column
        board_gen_remove_item(board_index_y * 9 + i, item_val_to_remove); // in the same row
    }

    // now in the same 3 x 3 square
    int board_3x3_begin_x = board_index_x - (board_index_x % 3);
    int board_3x3_begin_y = board_index_y - (board_index_y % 3);
    for(int i = board_3x3_begin_y ; i < board_3x3_begin_y + 3 ; i++)
        for(int j = board_3x3_begin_x ; j < board_3x3_begin_x + 3 ; j++)
            board_gen_remove_item(i * 9 + j, item_val_to_remove);            

}

bool board_gen_solve_step()
{
    // first sort the priority list to find item with lowest entropy
    board_gen_update_priority_list(); 

    // then get the range of indices where we have our items with lowest entropy
    int lowest_start = -1;
    int lowest_end = -1;
    char lowest_val = 10;
    for(int i = 0 ; i < 81 ; i++)
    {
        char v = g_context.board_gen[g_context.priority_list[i]].d[0];
        if( v > 1 )
        {
            if(lowest_start == -1 ) {lowest_val = v; lowest_start = i;}
            if(v > lowest_val && lowest_end == -1 ) lowest_end = i - 1;
        }
    }
    if(lowest_end == -1) lowest_end = 81 - 1;
    if(lowest_start == -1) return true; // if all are having 1 entropy then the board is already solved

    // then randomly select a item with lowest entropy to collapse it
    int selected_item = g_context.priority_list[CGL_utils_random_int(lowest_start, lowest_end)];
    board_gen_collapse_item(selected_item);

    return false;
}

void generate_board()
{
    for(int i = 0 ; i < 81 ; i++)
    {
        for(int j = 0 ; j < 10 ; j++)
            g_context.board_gen[i].d[j] = 1;
        g_context.board_gen[i].d[0] = 9;
        g_context.board_gen[i].d[10] = -1;
        g_context.priority_list[i] = (char)i;
    }

    while(!board_gen_solve_step());

    // now copy generated board to board_solved
    for(int i = 0 ; i < 81 ; i++)
        g_context.board_solved[i] = g_context.board_gen[i].d[10];    
}

void upload_board_piece(int peice_index)
{
    CGL_vec4 offset[2];
    offset[0] = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    offset[1] = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    CGL_font_character* characters = CGL_font_get_characters(g_context.font);
    CGL_font_character* character = &characters[g_context.board[peice_index] + 48];
    if(g_context.board[peice_index] > 0) offset[0] = CGL_vec4_init(character->normalized_offset.x, character->normalized_offset.y, character->normalized_size.x, character->normalized_size.y);
    if(g_context.board_original[peice_index] > 0) offset[1].x = 1.0f;
    if(g_context.board_solved[peice_index] == g_context.board[peice_index]) offset[1].y = 1.0f;
    CGL_ssbo_set_sub_data(g_context.board_data_ssbo, sizeof(CGL_vec4) * 2 * peice_index, sizeof(CGL_vec4) * 2, offset, false);
}

void upload_board()
{
    for(int i = 0 ; i < 81 ; i ++)
        upload_board_piece(i);
}

void reset_game();

void key_callback(CGL_window* window, int key, int scancode, int action, int mods)
{
    int selected_tile_x = g_context.selected_tile % 9;
    int selected_tile_y = g_context.selected_tile / 9;

    if(key == CGL_KEY_R && action == CGL_PRESS) reset_game();

    if(!g_context.is_input_allowed) return;

    if(action == CGL_PRESS)
    {
        if(key == CGL_KEY_UP) selected_tile_y = CGL_utils_clamp(selected_tile_y + 1, 0, 8);
        if(key == CGL_KEY_DOWN) selected_tile_y = CGL_utils_clamp(selected_tile_y - 1, 0, 8);
        if(key == CGL_KEY_RIGHT) selected_tile_x = CGL_utils_clamp(selected_tile_x + 1, 0, 8);
        if(key == CGL_KEY_LEFT) selected_tile_x = CGL_utils_clamp(selected_tile_x - 1, 0, 8);
        g_context.selected_tile = selected_tile_y * 9 + selected_tile_x;


        if(g_context.board_original[g_context.selected_tile] == -1)
        {
            if(key >= CGL_KEY_1 && key <= CGL_KEY_9) g_context.board[g_context.selected_tile] = key - CGL_KEY_0;
            if(key >= CGL_KEY_KP_1 && key <= CGL_KEY_KP_9) g_context.board[g_context.selected_tile] = key - CGL_KEY_KP_0;
            if(key == CGL_KEY_DELETE) g_context.board[g_context.selected_tile] = -1;
            upload_board_piece(g_context.selected_tile);
        }

        if(key == CGL_KEY_C) g_context.is_check_on = true;

        if(key == CGL_KEY_S) // auto solve selected tile
        {
            g_context.board[g_context.selected_tile] = g_context.board_solved[g_context.selected_tile];
            upload_board_piece(g_context.selected_tile);
        }
        if(key == CGL_KEY_A) // auto solve random tile
        {
            int r = CGL_utils_random_int(0, 80);
            g_context.board[r] = g_context.board_solved[r];
            upload_board_piece(r);
        }
        if(key == CGL_KEY_F) // auto solve first unsolved tile tile
        {
            for(int i = 0 ; i < 81 ; i++)
            {
                if(g_context.board[i] == -1)
                {
                    g_context.board[i] = g_context.board_solved[i];
                    upload_board_piece(i);
                    break;
                }
            }
        }
    }
    else if(action == CGL_RELEASE)
    {
        if(key == CGL_KEY_C) g_context.is_check_on = false;        
    }
}


bool setup_window()
{
    g_context.window_height = 400;
    g_context.window_width = (int)(g_context.window_height * ASPECT_RATIO);
    g_context.window = CGL_window_create(g_context.window_width, g_context.window_height, "Sudoku - Jaysmito Mukherjee"); // create the window
    if(g_context.window == NULL) return false; // window creation failed
    CGL_window_make_context_current(g_context.window); // make the opengl context for the window current
    CGL_window_set_key_callback(g_context.window, key_callback);
    return true; // window was successfully created
}

bool setup_framebuffer()
{
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window); // load the default framebuffer (0) into CGL_framebuffer object
    if(g_context.framebuffer == NULL) return false; // failed

    g_context.main_framebuffer = CGL_framebuffer_create(1024, 1024); // for the main rendering
    if(!g_context.main_framebuffer) return false;

    return true; // success
}

bool setup_quad_renderer()
{
    // create a unit quad covering entire drawing area
    CGL_mesh_cpu* quad_mesh_cpu = CGL_mesh_cpu_quad(
        CGL_vec3_init(1.0f, 1.0f, 0.0f),            
        CGL_vec3_init(1.0f, -1.0f, 0.0f),            
        CGL_vec3_init(-1.0f, -1.0f, 0.0f),            
        CGL_vec3_init(-1.0f, 1.0f, 0.0f)
    );
    g_context.quad_mesh = CGL_mesh_gpu_create(); // create the gpu mesh
    if(!g_context.quad_mesh) return false; // failed
    CGL_mesh_gpu_upload(g_context.quad_mesh, quad_mesh_cpu, true); // upload quad mesh data to the gpu mesh
    CGL_mesh_cpu_destroy(quad_mesh_cpu); // free the quad mesh data from cpu as we no longer need it

    g_context.quad_shader = CGL_shader_create(quad_mesh_vertex_shader_source, quad_mesh_fragment_shader_source, NULL); // create the shader
    // load the uiform locations
    g_context.quad_shader_usize = CGL_shader_get_uniform_location(g_context.quad_shader, "usize");
    g_context.quad_shader_ustart = CGL_shader_get_uniform_location(g_context.quad_shader, "ustart");
    g_context.quad_shader_ucolor = CGL_shader_get_uniform_location(g_context.quad_shader, "ucolor");
    g_context.quad_shader_uiscolor = CGL_shader_get_uniform_location(g_context.quad_shader, "uiscolor");
    g_context.quad_shader_utex = CGL_shader_get_uniform_location(g_context.quad_shader, "utex");
    
    return true; // success
}

void render_quad(float start_x, float start_y, float size_x, float size_y, float color_r, float color_g, float color_b)
{
    CGL_shader_bind(g_context.quad_shader); // bind the quad shader
    // set the uniforms
    CGL_shader_set_uniform_vec2v(g_context.quad_shader, g_context.quad_shader_ustart, start_x + size_x / 2.0f, start_y + size_y / 2.0f);
    CGL_shader_set_uniform_vec2v(g_context.quad_shader, g_context.quad_shader_usize, size_x, size_y);
    CGL_shader_set_uniform_vec3v(g_context.quad_shader, g_context.quad_shader_ucolor, color_r, color_g, color_b);
    CGL_shader_set_uniform_bool(g_context.quad_shader, g_context.quad_shader_uiscolor, true);
    CGL_mesh_gpu_render(g_context.quad_mesh); // render the quad mesh
}

void render_quad_texture(float start_x, float start_y, float size_x, float size_y, CGL_texture* texture)
{
    CGL_shader_bind(g_context.quad_shader); // bind the quad shader
    // set the uniforms
    CGL_shader_set_uniform_vec2v(g_context.quad_shader, g_context.quad_shader_ustart, start_x + size_x / 2.0f, start_y + size_y / 2.0f);
    CGL_shader_set_uniform_vec2v(g_context.quad_shader, g_context.quad_shader_usize, size_x, size_y);
    CGL_shader_set_uniform_bool(g_context.quad_shader, g_context.quad_shader_uiscolor, false);
    CGL_texture_bind(texture, 0);
    CGL_shader_set_uniform_int(g_context.quad_shader, g_context.quad_shader_utex, 0);
    CGL_mesh_gpu_render(g_context.quad_mesh); // render the quad mesh
}

bool setup_board_renderer()
{
    g_context.font = CGL_font_load("font.ttf"); // load font from file
    if(!g_context.font) return false; // failed
    if(!CGL_font_build_atlas(g_context.font, 1024, 1024, 56)) return false; // build and rastarize the font atlas
    
    g_context.you_won_text = CGL_text_bake_to_texture("You Won!", strlen("You Won!"), g_context.font, NULL, NULL);
    g_context.instruction_text = CGL_text_bake_to_texture("Press R to restart", strlen("Press R to restart"), g_context.font, NULL, NULL);

    g_context.board_shader = CGL_shader_create(board_vertex_shader_source, board_fragment_shader_source, NULL); // setup board shader
    // load up unifrom locations
    g_context.board_shader_uborder_width = CGL_shader_get_uniform_location(g_context.board_shader, "uborder_width");
    g_context.board_shader_ufont_atlas = CGL_shader_get_uniform_location(g_context.board_shader, "ufont_atlas");
    g_context.board_shader_uhovered_tile = CGL_shader_get_uniform_location(g_context.board_shader, "uhovered_tile");
    g_context.board_shader_uselected_tile = CGL_shader_get_uniform_location(g_context.board_shader, "uselected_tile");
    g_context.board_shader_uischeckon = CGL_shader_get_uniform_location(g_context.board_shader, "uischeckon");

    g_context.board_data_ssbo = CGL_ssbo_create(0); // create ssbo board data
    CGL_ssbo_set_data(g_context.board_data_ssbo, sizeof(CGL_vec4) * 2 * 81, NULL, false); // upload fresh data

    return true;
}

void render_board()
{
    CGL_shader_bind(g_context.board_shader); // bind the board shader
    // set the unifroms
    CGL_shader_set_uniform_float(g_context.board_shader, g_context.board_shader_uborder_width, BORDER_WIDTH);
    CGL_texture_bind(CGL_font_get_atlas(g_context.font), 0);
    CGL_shader_set_uniform_int(g_context.board_shader, g_context.board_shader_ufont_atlas, 0);
    CGL_shader_set_uniform_int(g_context.board_shader, g_context.board_shader_uselected_tile, g_context.selected_tile);
    CGL_shader_set_uniform_int(g_context.board_shader, g_context.board_shader_uhovered_tile, g_context.hovered_tile);
    CGL_shader_set_uniform_bool(g_context.board_shader, g_context.board_shader_uischeckon, g_context.is_check_on);
    CGL_mesh_gpu_render_instanced(g_context.quad_mesh, 81); // we need 81 quads for the entire sudoku board
}

void reset_game()
{
    g_context.is_over = false; 
    g_context.is_check_on = false;
    g_context.is_input_allowed = true;
    g_context.hovered_tile = -1;
    g_context.selected_tile = 4 * 9 + 4;
    memset(g_context.board, -1, sizeof(g_context.board));
    
    // this only generates the solved board
    generate_board();

    // now lets generate unsolvd board
    for(int i = 0 ; i < 9 ; i+= 3)
    {
        for(int j = 0 ; j < 9 ; j+= 3)
        {
            int ind_x = CGL_utils_random_int(i, i + 2);
            int ind_y = CGL_utils_random_int(j, j + 2);
            int ind = ind_y * 9 + ind_x;
            g_context.board[ind] = g_context.board_solved[ind];
        }
    }
           
    memcpy(g_context.board_original, g_context.board, sizeof(g_context.board));
    upload_board();
}

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return EXIT_FAILURE; // initialize CGL (required for setting up internals of CGL)
    if(!setup_window()) return EXIT_FAILURE; //setup window
    if(!CGL_gl_init()) return EXIT_FAILURE; // initialize cgl opengl module    
    if(!CGL_text_init()) return EXIT_FAILURE; // initialize cgl text/font module
    if(!setup_framebuffer()) return EXIT_FAILURE; //setup framebuffer
    if(!setup_quad_renderer()) return EXIT_FAILURE; // setup up the mechanism for rendering quads
    if(!setup_board_renderer()) return EXIT_FAILURE; // setup the mechanism to render the 9x9 sudoku board
    reset_game(); // reset game data

    const float board_tile_size = (2.0f - 10.0f * BORDER_WIDTH) / 9.0f;

    // the main loop
    while(!CGL_window_should_close(g_context.window)) // run till the close button is clicked
    {
        // rendering
        CGL_framebuffer_bind(g_context.main_framebuffer); // bind main framebuffer and also adjust viewport size and offset
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f); // clear screen with a dark gray color

        // render the borders
        for(int i = 0 ; i <= 9 ; i+=3)
        {
            render_quad(-1.0f, -1.0f + i * (BORDER_WIDTH + board_tile_size), 2.0f, BORDER_WIDTH, 1.0f, 1.0f, 1.0f);
            render_quad(-1.0f + i * (BORDER_WIDTH + board_tile_size), -1.0f, BORDER_WIDTH, 2.0f, 1.0f, 1.0f, 1.0f);
        }

        // render the board
        render_board();

        if(g_context.is_over)
        {
            // you can optionaly enable it but i am disabling it as i dont want to add more code just for rendering this text!
            // render_quad_texture(-0.5f, -0.5f, 1.0f, 1.0f, g_context.you_won_text);
            // render_quad_texture(-0.5f, -0.8f, 1.0f, 0.15f, g_context.instruction_text);
        }

        CGL_framebuffer_bind(g_context.framebuffer); // bind default framebuffer and also adjust viewport size and offset
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f); // clear screen with a dark gray color
        render_quad_texture(-1.0f, -1.0f, 2.0f, 2.0f, CGL_framebuffer_get_color_texture(g_context.main_framebuffer));

        CGL_window_swap_buffers(g_context.window); // swap framebuffers
        CGL_window_poll_events(g_context.window); // poll events (if this is not called every frame window will stop responding)

        double mouse_x_pos = 0.0, mouse_y_pos = 0.0;
        CGL_window_get_mouse_position(g_context.window, &mouse_x_pos, &mouse_y_pos);
        g_context.hovered_tile = -1; //  reset hovered tile
        if(mouse_x_pos > 0.0 && mouse_y_pos > 0.0 && mouse_x_pos < g_context.window_width && mouse_y_pos < g_context.window_height)
        {
            int mouse_pick_id = CGL_framebuffer_get_mouse_pick_id(g_context.main_framebuffer, (int)CGL_utils_clamp(mouse_x_pos / g_context.window_width * 1024.0, 0.0, 1024.0), (int)CGL_utils_clamp(mouse_y_pos / g_context.window_height * 1024.0, 0.0, 1024.0), 0);
            if(mouse_pick_id >= 0 && mouse_pick_id < 81) g_context.hovered_tile = mouse_pick_id; // set hovered tile
        }
        // select tile if clicked
        if(g_context.hovered_tile >= 0 && CGL_window_get_mouse_button(g_context.window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS) g_context.selected_tile = g_context.hovered_tile;

        if(memcmp(g_context.board, g_context.board_solved, sizeof(g_context.board)) == 0)
        {
            g_context.is_input_allowed = false;
            g_context.is_over = true;
            g_context.is_check_on = true;
        }

        // make sure window aspect ratio is constant
        int new_window_height = 0, new_window_width = 0;
        CGL_window_get_size(g_context.window, &new_window_width, &new_window_height); // get new size
        if(new_window_height != g_context.window_height) new_window_width = (int)(new_window_height * ASPECT_RATIO); // height is changed so width is corrected
        else if(new_window_width != g_context.window_width) new_window_height = (int)(new_window_width / ASPECT_RATIO); // width is changed so height is corrected
        g_context.window_height = new_window_height; // update the context
        g_context.window_width = new_window_width; // update the context
        CGL_window_set_size(g_context.window, g_context.window_width, g_context.window_height); // set the corrected sizes (NOTE: its not ideal to do it every frame but it wont be that big of a performance issue in this case)
                
        if(CGL_window_get_key(g_context.window, CGL_KEY_ESCAPE) == CGL_PRESS) break; // quit on pressing escape
    }

    // cleanup
    CGL_texture_destroy(g_context.you_won_text); // destroy the texture
    CGL_texture_destroy(g_context.instruction_text); // destroy the texture
    CGL_font_destory(g_context.font); // destroy the font
    CGL_shader_destroy(g_context.board_shader); // destory board shader
    CGL_mesh_gpu_destroy(g_context.quad_mesh); // destory quad mesh
    CGL_shader_destroy(g_context.quad_shader); // destory quad shader
    CGL_framebuffer_destroy(g_context.main_framebuffer); // destory framebuffer object
    CGL_framebuffer_destroy(g_context.framebuffer); // destory framebuffer object
    CGL_text_shutdown(); // shutdown cgl text module
    CGL_gl_shutdown(); // shutdown cgl opengl module
    CGL_window_destroy(g_context.window); // destroy window
    CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
    return EXIT_SUCCESS;
}

