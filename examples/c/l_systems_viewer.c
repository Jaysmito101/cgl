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
#include "cgl.h"

#define MAIN_FRAME_BUFFER_SIZE  700

typedef struct {
    CGL_vec2 position;
    CGL_float rotation;
    CGL_float line_size;
} turtle_state;


static struct {
    CGL_byte rules[256][256];
    CGL_byte rules_sizes[256];
    CGL_byte actions[256]; 
    CGL_byte axiom[256]; // null terminated string
    CGL_vec2 start_position;
    CGL_int rule_count;
    CGL_float angle;
    CGL_int iterations;
    CGL_float length_factor;
} l_system_data;

static CGL_byte lsbuffa[1024 * 1024], lsbuffb[1024 * 1024];
static CGL_byte *current_ls_buff, *next_ls_buff;
static turtle_state turtle_stack[1024 * 1024];
static CGL_int turtle_stack_top = -1;
static CGL_vec4 lines_data[1024 * 1024];
static CGL_int lines_count = 0;

// void add_line(CGL_float sx, CGL_float sy, CGL_float ex, CGL_float ey)
// {
//     lines_data[lines_count++] = CGL_vec4_init(sx, sy, ex, ey);
// }

void add_line(CGL_vec2 start_p, CGL_vec2 end_p)
{
    lines_data[lines_count++] = CGL_vec4_init(start_p.x, start_p.y, end_p.x, end_p.y);
}

void log_l_system_data()
{
    CGL_info("Angle: %f", l_system_data.angle);
    CGL_info("Iterations: %d", l_system_data.iterations);
    CGL_info("Length Factor: %f", l_system_data.length_factor);
    CGL_info("Axiom: %s", l_system_data.axiom);
    CGL_info("Start Position: (%f, %f)", l_system_data.start_position.x, l_system_data.start_position.y);
    CGL_info("Rules: ");
    for(CGL_int i = 0; i < 256; i++) if(l_system_data.rules[i][0] != 0) CGL_info("%c -> %s", i, l_system_data.rules[i]);
    CGL_info("Actions: ");
    for(CGL_int i = 0; i < 256; i++) if(l_system_data.actions[i] != 0) CGL_info("%c -> %d", i, l_system_data.actions[i]);
}

bool parse_parameter(const CGL_byte* line, CGL_byte* tbuffer, CGL_int* val)
{
    tbuffer[0] = '\0'; line++; // skip the ':'
    while(*line != ' ') { if(*line == 0) return false; *tbuffer++ = *line++; }
    *tbuffer = '\0'; *val = atoi(line); return true;
}

void parse_settings_file(const CGL_byte* path)
{
    memset(&l_system_data, 0, sizeof(l_system_data));
    FILE* file = fopen(path, "r");
    if(!file) { CGL_warn("Failed to load file %s", path); return; }
    CGL_info("Parsing file %s", path);
    CGL_byte line[1024], tbuffer[1024];
    CGL_int param_val = 0;
    CGL_byte parser_mode = 0; // 0 - parameters, 1 - rules, 2 - action
    while(fgets(line, 1024, file) != NULL)
    {
        if(line[0] == '#' || strlen(line) < 3) continue; // skip comment
        if(parser_mode == 0) 
        {
            if(line[0] == ':') // parse parameter
            {
                if(!parse_parameter(line, tbuffer, &param_val)) { CGL_warn("Error while parsing line %s", line); continue; }
                if(strcmp(tbuffer, "ANGLE") == 0) l_system_data.angle = CGL_deg_to_rad((CGL_float)param_val * 0.001f);
                else if(strcmp(tbuffer, "ITERATIONS") == 0) l_system_data.iterations = param_val;
                else if(strcmp(tbuffer, "LENGTH_FACTOR") == 0) l_system_data.length_factor = (CGL_float)param_val * 0.001f;
                else if(strcmp(tbuffer, "START_X") == 0) l_system_data.start_position.x = (CGL_float)param_val * 0.001f;
                else if(strcmp(tbuffer, "START_Y") == 0) l_system_data.start_position.y = (CGL_float)param_val * 0.001f;
                else if(strcmp(tbuffer, "AXIOM") == 0) { CGL_byte* ln = line + 6; CGL_byte* ax = l_system_data.axiom; while(*ln != '\n' && *ln != '\0') *ax++ = *ln++; *ax = '\0'; }
                else if(strcmp(tbuffer, "RULES") == 0) parser_mode = 1;
                else if(strcmp(tbuffer, "ACTIONS") == 0) parser_mode = 2;
                else CGL_warn("Unknown parameter %s", tbuffer);
            }
        }
        else
        {
            if (parser_mode == 1)
            {
                CGL_byte* ln = line; CGL_int rl_id = 0, ind = (CGL_byte)*ln++; ln++;
                while(*ln != '\n' && *ln != '\0') l_system_data.rules[ind][rl_id++] = *ln++; l_system_data.rules[ind][rl_id] = '\0';
                l_system_data.rules_sizes[ind] = rl_id; l_system_data.rule_count++;
            }
            else if (parser_mode == 2) l_system_data.actions[*line] = atoi(line + 2);
            param_val--; if(param_val == 0) parser_mode = 0;
        }
    }
    fclose(file);
    CGL_info("Parsing complete");
    CGL_info("----------------------------");
    log_l_system_data();
    CGL_info("----------------------------");
}



void drag_n_drop_callback(CGL_window* window, const CGL_byte** paths, CGL_int count)
{
    if(count > 0) parse_settings_file(paths[0]);
}

void generate_l_system_txt()
{
    CGL_int current_ls_buff_len = (CGL_int)strlen(current_ls_buff), next_ls_buff_len = 0;
    for(CGL_int i = 0 ; i < current_ls_buff_len ; i++)
    {
        CGL_byte input = current_ls_buff[i];
        CGL_byte* rule = l_system_data.rules[input];
        CGL_int rule_len = l_system_data.rules_sizes[input];
        if(rule_len == 0) next_ls_buff[next_ls_buff_len++] = input;
        else for(CGL_int j = 0; j < rule_len; j++) next_ls_buff[next_ls_buff_len++] = rule[j];
    }
    next_ls_buff[next_ls_buff_len] = '\0';
    CGL_byte* temp = current_ls_buff; current_ls_buff = next_ls_buff; next_ls_buff = temp;
}


void draw_system()
{
    turtle_stack_top = -1; lines_count = 0;
    turtle_state current_state;
    current_state.position = l_system_data.start_position;
    current_state.rotation = 0.0f;
    current_state.line_size = 0.2f;
    CGL_int current_ls_buff_len = (CGL_int)strlen(current_ls_buff);
    CGL_vec2 new_pos;
    for(CGL_int i = 0 ; i < current_ls_buff_len ; i++)
    {
        switch (l_system_data.actions[current_ls_buff[i]])
        {
        case 1:
            new_pos = CGL_vec2_add_(CGL_vec2_scale_(CGL_vec2_from_angle(current_state.rotation + CGL_PI_2), current_state.line_size), current_state.position);
            add_line(current_state.position, new_pos);
            current_state.position = new_pos;
            current_state.line_size *= l_system_data.length_factor;
            break;
        case 2:
            current_state.rotation += l_system_data.angle;
            break;
        case 3:
            current_state.rotation -= l_system_data.angle;
            break;
        case 4:
            turtle_stack[++turtle_stack_top] = current_state;
            break;
        case 5:
            current_state = turtle_stack[turtle_stack_top--];
            break;        
        default:
            break;
        }
    }
}

void generate_system()
{
    strcpy(current_ls_buff, l_system_data.axiom);
    for(CGL_int i = 0; i < l_system_data.iterations; i++) generate_l_system_txt();
    draw_system();
}

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return -1;
    CGL_window* main_window = CGL_window_create(700, 700, "L Systems - Jaysmito Mukherjee");
    if(!main_window) return -1;
    CGL_window_make_context_current(main_window);
    CGL_window_set_drag_n_drop_callback(main_window, drag_n_drop_callback);
    if(!CGL_gl_init()) return -1;
    CGL_widgets_init();
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(main_window);   
    current_ls_buff = lsbuffa; next_ls_buff = lsbuffb;
    while(!CGL_window_should_close(main_window))
    { 
        CGL_window_set_size(main_window, MAIN_FRAME_BUFFER_SIZE, MAIN_FRAME_BUFFER_SIZE);

        {
            CGL_framebuffer_bind(default_framebuffer);
            CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);

            CGL_widgets_begin();

            CGL_widgets_set_fill_colorf(0.7f, 0.2f, 0.7f, 1.0f);
            CGL_widgets_set_stroke_thicnkess(0.01f);
            for(CGL_int i = 0; i < lines_count; i++)
                CGL_widgets_add_line2f(lines_data[i].x, lines_data[i].y, lines_data[i].z, lines_data[i].w);

            static CGL_byte buff[1024];
            sprintf(buff, "Iterations: %d", l_system_data.iterations);
            CGL_widgets_add_string(buff, -1.0f, 0.9f, 1.0f, 0.1f);
            sprintf(buff, "Lines: %d", lines_count);
            CGL_widgets_add_string(buff, -1.0f, 0.8f, 1.0f, 0.1f);

            CGL_widgets_end();
        }

        CGL_window_poll_events(main_window);
        CGL_window_swap_buffers(main_window);

        if(CGL_window_is_key_pressed(main_window, CGL_KEY_I))
        {
            if(CGL_window_is_key_pressed(main_window, CGL_KEY_UP)) {l_system_data.iterations++; CGL_utils_sleep(200);}
            if(CGL_window_is_key_pressed(main_window, CGL_KEY_DOWN)) {l_system_data.iterations--; CGL_utils_sleep(200);}
        }
        l_system_data.iterations = CGL_utils_max(1, l_system_data.iterations);
        if(CGL_window_is_key_pressed(main_window, CGL_KEY_SPACE)) {generate_system(); CGL_utils_sleep(200);}


        if(CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }

    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(main_window);
    CGL_shutdown();
    return 0;
}

// Example L-system File
/*

# L-System Bushes
# Written by Paul Bourke
# Source: http://paulbourke.net/fractals/lsys/
:ANGLE 25700
# length factor will be divided by 1000 to convert to float
:LENGTH_FACTOR 850
:AXIOM Y
# star x and y will be divided by 1000 to convert to float
# coodinate system is (-1.0, -1.0) to (1.0, 1.0)
:START_X 0
:START_Y -1000
:ITERATIONS 4
# Actions are the character followed by command
# commands are:
# 1 - move forward
# 2 - rotate left
# 3 - rotate right
# 4 - push
# 5 - pop
:ACTIONS 5
F 1
+ 2
- 3
[ 4
] 5
:RULES 2
X X[-FFF][+FFF]FX
Y YFX[+Y][-Y]


*/
