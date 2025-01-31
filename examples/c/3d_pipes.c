/*
MIT License

Copyright (c) 2024 s-mackeo (https://github.com/s-mackeo)

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


// ignore -Wswitch
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wswitch"
#endif

#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_AUDIO
#define CGL_IMPLEMENTATION
#include "cgl.h"


#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define PIPE_BOX_LENGTH 15
#define PIPE_MAX_LENGTH 100

#define PIPE_3D_RADIUS 0.3f
#define PIPE_3D_CIRCULAR_RESOLUTION 50

#define PIPE_ELBOW_PROBABILITY 0.2f
#define PIPE_MAX_COUNT 5 

// Potential improvements : easter eggs : Utah teapot and candy cane texture
// Non-square grid to conform with window aspect ratio
// fisheye camera and better camera positioning (+ random offset on every restart) 
// Change grid system and elbow/cylinder length + sphere size to get more closely-resembling pipes
// Increase pixel mask block size for the fade effect
// Edit out the hard-coded things

const CGL_byte* DIFFUSE_VERTEX_SHADER_SOURCE = 
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
"out vec3 Normal;\n"
"out vec3 Color;\n"
"out vec2 TexCoord;\n"
"\n"
"uniform mat4 view_proj;\n"
"uniform vec4 pipe_color;\n"

"\n"
"void main()\n"
"{\n"
"   vec3 pos = position.xyz;\n"
"	gl_Position = view_proj * vec4(pos, 1.0f);\n"
"	Position = position.xyz;\n"
"	Normal = normal.xyz;\n"
"	TexCoord = texcoord.xy;\n"
"   Color = pipe_color.xyz;"
"}";

const CGL_byte* DIFFUSE_FRAGMENT_SHADER_SOURCE = 
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
"in vec3 Normal;\n"
"in vec3 Color;\n"
"in vec2 TexCoord;\n"
"\n"
"uniform vec3 u_light_pos;\n"
"uniform vec3 u_eye_pos;\n"
"uniform sampler2D u_TexFade;\n"
"uniform float u_fade_stage;\n"
"uniform float u_WindowWidth;\n"
"uniform float u_WindowHeight;\n"
"\n"
"void main()\n"
"{\n"
"	vec3 object_color = Color;\n"
"	vec3 light_color = vec3(0.5f, 1.0f, 0.5f);\n"
"	vec3 light_dir = normalize(u_light_pos - Position);\n"
"	float diffuse = max(dot(Normal, light_dir), 0.0f);\n"
"    float ambient = 0.1f;\n"
"    float specular_max = 1.0f;\n"
"    float specular = pow(max(dot(reflect(-light_dir, Normal), normalize(u_eye_pos-Position)), 0.0f), 32.0);\n"
"	vec3 result = (specular_max*specular*light_color + diffuse * light_color + ambient) * object_color;\n"
"    float fade_factor = 1.0f;\n"
"    if(u_fade_stage > 0.0f)\n"
"    {\n"
"    vec2 tex_coord = vec2((gl_FragCoord.x)/u_WindowWidth,(gl_FragCoord.y)/u_WindowHeight) ;\n"
"    fade_factor =  min(max(texture(u_TexFade, tex_coord.xy).x ,0.0f),1.0f);\n"
"    if(fade_factor < u_fade_stage)\n"
"    FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
"	else FragColor = vec4(result, 1.0f);\n"
"    }\n"
"	else FragColor = vec4(result, 1.0f);\n"
"}";

// Colors taken from https://github.com/fogleman/pg/blob/master/examples/pipes.py
CGL_int pipe_colors[18] = {
    0x1f77b4, 0xaec7e8, 0xff7f0e, 0xffbb78, 0x2ca02c, 0x98df8a,
    0xd62728, 0xff9896, 0x9467bd, 0xc5b0d5, 0x8c564b, 0xc49c94,
    0xe377c2, 0xf7b6d2, 0x7f7f7f, 0xc7c7c7, 0x17becf, 0x9edae5
};

typedef enum 
{
    PIPE_NONE,
    PIPE_LEFT,
    PIPE_RIGHT,
    PIPE_DOWN,
    PIPE_UP,
    PIPE_BACK,
    PIPE_FRONT
}Pipe_Direction;

typedef struct 
{
    CGL_int x;
    CGL_int y;
    CGL_int z;
} CGL_coord;

typedef struct
{
    CGL_coord coord_end;
    CGL_int length;
    
    CGL_color color;
    Pipe_Direction direction_current;

    CGL_mesh_gpu* mesh_gpu;    
    CGL_mesh_cpu* mesh_cpu;
} Pipe;

void log_direction(Pipe_Direction dir)
{
    switch(dir)
    {
        case PIPE_NONE:CGL_info("NONE\n");break;
        case PIPE_UP:CGL_info("UP\n");break;
        case PIPE_DOWN:CGL_info("DOWN\n");break;
        case PIPE_LEFT:CGL_info("LEFT\n");break;
        case PIPE_RIGHT:CGL_info("RIGHT\n");break;
        case PIPE_FRONT:CGL_info("FRONT\n");break;
        case PIPE_BACK:CGL_info("BACK\n");break;
    }
}


CGL_mesh_cpu* cylinder;
CGL_mesh_cpu* torus;
CGL_mesh_cpu* sphere;

static Pipe pipes[PIPE_MAX_COUNT];
static CGL_int pipe_current_active;
static CGL_float fade_stage;
static CGL_bool grid[PIPE_BOX_LENGTH][PIPE_BOX_LENGTH][PIPE_BOX_LENGTH];

void grid_init()
{
    memset(grid, false, sizeof(CGL_bool)*PIPE_BOX_LENGTH*PIPE_BOX_LENGTH*PIPE_BOX_LENGTH);
    pipe_current_active = 0;
}

void grid_reset()
{
    grid_init();
}

void pipes_init()
{
    for(CGL_int i = 0; i < PIPE_MAX_COUNT; i++)
    {
        pipes[i].length = 0;
        pipes[i].direction_current = PIPE_NONE;
        
        pipes[i].mesh_cpu =  CGL_mesh_cpu_create(1000000, 1000000);
        pipes[i].mesh_gpu = CGL_mesh_gpu_create();
    }
}

void pipes_reset()
{
    for(CGL_int i = 0; i < PIPE_MAX_COUNT; i++)
    {
        CGL_mesh_cpu_destroy(pipes[i].mesh_cpu);
        CGL_mesh_gpu_destroy(pipes[i].mesh_gpu);
    }
    
    pipes_init();
}

CGL_texture* fade_effect_init(CGL_int width, CGL_int height)
{
    CGL_image image;
    image.width = width;
    image.height = height;
    image.bytes_per_channel = 32;
    image.channels = 4; 
    
    image.data = CGL_malloc(width*height*image.bytes_per_channel);
    
    //assert(fade_duration > 1);
    
    for(CGL_int i = 0; i < image.height*image.width*image.channels; i+= image.channels)
    {
        CGL_float* next = ((CGL_float*)(image.data)) + i;
        
        for(CGL_int j = i; j < i+image.channels; j++)
        {
            next[j] = CGL_utils_random_float();
        }
    }
    
    CGL_texture* fade_effect = CGL_texture_create(&image);
    
    CGL_free(image.data);
    
    return fade_effect;
}

void fade_effect_reset(CGL_shader* diffuse_shader)
{
    fade_stage = 0.0f;
    CGL_shader_set_uniform_float(diffuse_shader, CGL_shader_get_uniform_location(diffuse_shader, "u_fade_stage"), fade_stage);
}

bool fade_out_completed()
{
    return fade_stage > 1.0f;
}

void fade_out(CGL_shader* diffuse_shader)
{
    // Increment the threshold used as a mask with the fade texture colour values
    fade_stage += 0.03f;
    CGL_shader_set_uniform_float(diffuse_shader, CGL_shader_get_uniform_location(diffuse_shader, "u_fade_stage"), fade_stage);
    if(fade_out_completed())
    {
        grid_reset(); pipes_reset();fade_effect_reset(diffuse_shader);
    }
}

CGL_coord coord_next_from_direction(CGL_coord coord_current, Pipe_Direction direction_current)
{
    switch(direction_current)
    {
        case PIPE_LEFT:
        return (CGL_coord){coord_current.x - 1, coord_current.y, coord_current.z};
        break;
        case PIPE_RIGHT:
        return (CGL_coord){coord_current.x + 1, coord_current.y, coord_current.z};
        break;
        case PIPE_DOWN:
        return (CGL_coord){coord_current.x, coord_current.y - 1, coord_current.z};
        break;
        case PIPE_UP:
        return (CGL_coord){coord_current.x, coord_current.y + 1, coord_current.z};
        break;
        case PIPE_BACK:
        return (CGL_coord){coord_current.x, coord_current.y, coord_current.z - 1};
        break;
        case PIPE_FRONT:
        return (CGL_coord){coord_current.x, coord_current.y, coord_current.z + 1};
        break;
        default:
        assert(1==0);
        return (CGL_coord){0, 0, 0};
        break;
    }
}

CGL_bool coord_is_valid(CGL_coord coord)
{
    return (coord.x >= 0 && coord.x < PIPE_BOX_LENGTH) &&
        (coord.y >= 0 && coord.y < PIPE_BOX_LENGTH) &&
        (coord.z >= 0 && coord.z < PIPE_BOX_LENGTH);
}

Pipe_Direction pipe_extend_direction(Pipe* pipe)
{
    if(pipe->length == PIPE_MAX_LENGTH)
        return PIPE_NONE;
    
    // if the pipe is starting, we only need a random direction, 
    // as the starting point is set to be extensible in any direction (eg, not on the border)
    if(pipe->length == 1)
    {
        return (Pipe_Direction)(CGL_utils_random_int(1,6));
    }
    
    CGL_coord coord_current = pipe->coord_end;
    Pipe_Direction direction_current = pipe->direction_current;
    
    // the next insertion position was already implicitely determined valid last iteration
    // ie an extension is valid only if its next grid coord is available
    // so check if we can go further
    CGL_coord coord_next = coord_next_from_direction(coord_current, direction_current);
    
    CGL_int available_directions_count = 6;
    Pipe_Direction pipe_available_directions[7];
    
    for(CGL_int i = 1; i < 7; i ++)
    {
        pipe_available_directions[i] = true;
    }
    
    // Check for neighbour collisions
    for(CGL_int i = 1; i < 7; i ++)
    {
        CGL_coord extension_candidate = coord_next_from_direction(coord_next, (Pipe_Direction)i);
        
        if(!coord_is_valid(extension_candidate) || grid[extension_candidate.x][extension_candidate.y][extension_candidate.z])
        {
            available_directions_count--; pipe_available_directions[i] = false;
        }
    }
    
    if(available_directions_count == 0)
        return PIPE_NONE;
    
    // if we can keep going forward, do so with probability 1-PIPE_ELBOW_PROBABILITY
    if(pipe_available_directions[direction_current])
    {
        CGL_float f = CGL_utils_random_float();
        
        if(f > PIPE_ELBOW_PROBABILITY)
        {
            return direction_current;
        }
        
        // otherwise, only go for elbows
        pipe_available_directions[direction_current] = false;
        available_directions_count--;
    }
    
    // if we could have kept going forward but decided to turn and no turn direction is valid, end the pipe
    if(available_directions_count == 0)
        return PIPE_NONE;
    
    // Select a random direction from the available ones
    CGL_int selected_neighbour = CGL_utils_random_int(1, available_directions_count);
    
    for(CGL_int i = 1 ; i < 7; i ++)
    {
        if(pipe_available_directions[i])
            selected_neighbour--;
        
        if(selected_neighbour == 0)
        {
            return (Pipe_Direction)i;
        }
    }
    
    assert(1==0);
    return PIPE_NONE;
}

CGL_mat4 rotation_from_direction(Pipe_Direction direction_current, Pipe_Direction direction_next)
{
    //CGL_info("Directions : \n");
    //log_direction(direction_current);
    //log_direction(direction_next);
    
    CGL_mat4 rotate = CGL_mat4_identity();
    CGL_mat4 rotate2 = CGL_mat4_identity();
    
    if(direction_current == PIPE_NONE || direction_current == direction_next)
    {
        // Cylinder : default is up/down
        if(direction_next == PIPE_LEFT || direction_next == PIPE_RIGHT)
            rotate = CGL_mat4_rotate_z(CGL_PI_2);
        if(direction_next == PIPE_FRONT || direction_next == PIPE_BACK)
            rotate = CGL_mat4_rotate_x(-CGL_PI_2);
    }
    else
    {
        // Torus : default is front to left (also equivalent to : right to back)
        switch(direction_current)
        {
            case PIPE_LEFT:
            {
                rotate = CGL_mat4_rotate_y(CGL_PI);
                switch(direction_next)
                {
                    case PIPE_BACK:rotate2 = CGL_mat4_rotate_x(CGL_PI);break;
                    case PIPE_DOWN:rotate2 = CGL_mat4_rotate_x(-CGL_PI_2);break;
                    case PIPE_UP:rotate2 = CGL_mat4_rotate_x(CGL_PI_2);break;
                }
            }break;
            case PIPE_RIGHT:
            {
                switch(direction_next)
                {
                    case PIPE_FRONT:rotate2 = CGL_mat4_rotate_x(CGL_PI);break;
                    case PIPE_DOWN:rotate2 = CGL_mat4_rotate_x(CGL_PI_2);break;
                    case PIPE_UP:rotate2 = CGL_mat4_rotate_x(-CGL_PI_2);break;
                }
            }break;
            case PIPE_UP:
            {
                rotate = CGL_mat4_rotate_z(-CGL_PI_2);
                switch(direction_next)
                {
                    case PIPE_FRONT:rotate2 = CGL_mat4_rotate_y(CGL_PI);break;
                    case PIPE_LEFT:rotate2 = CGL_mat4_rotate_y(-CGL_PI_2);break;
                    case PIPE_RIGHT:rotate2 = CGL_mat4_rotate_y(CGL_PI_2);break;
                }
            }break;
            case PIPE_DOWN:
            {
                rotate = CGL_mat4_rotate_z(CGL_PI_2);
                switch(direction_next)
                {
                    case PIPE_FRONT:rotate2 = CGL_mat4_rotate_y(CGL_PI);break;
                    case PIPE_LEFT:rotate2 = CGL_mat4_rotate_y(-CGL_PI_2);break;
                    case PIPE_RIGHT:rotate2 = CGL_mat4_rotate_y(CGL_PI_2);break;
                }
            }break;
            case PIPE_BACK:
            {
                rotate = CGL_mat4_rotate_x(CGL_PI);
                switch(direction_next)
                {
                    case PIPE_RIGHT:rotate2 = CGL_mat4_rotate_z(CGL_PI);break;
                    case PIPE_DOWN:rotate2 = CGL_mat4_rotate_z(-CGL_PI_2);break;
                    case PIPE_UP:rotate2 = CGL_mat4_rotate_z(CGL_PI_2);break;
                }
            }break;
            case PIPE_FRONT:
            {
                switch(direction_next)
                {
                    case PIPE_RIGHT:rotate2 = CGL_mat4_rotate_z(CGL_PI);break;
                    case PIPE_DOWN:rotate2 = CGL_mat4_rotate_z(-CGL_PI_2);break;
                    case PIPE_UP:rotate2 = CGL_mat4_rotate_z(CGL_PI_2);break;
                }
            }break;
        }
    }
    
    rotate = CGL_mat4_mul(rotate, rotate2);
    return rotate;
}


void pipe_append_extension(Pipe* pipe, CGL_mesh_cpu* pipe_extension, CGL_coord coord_next, Pipe_Direction direction_next)
{
    assert(pipe->mesh_cpu->vertex_count_used + pipe_extension->vertex_count_used < pipe->mesh_cpu->vertex_count);
    assert(pipe->mesh_cpu->index_count_used + pipe_extension->index_count_used < pipe->mesh_cpu->index_count);
    //CGL_info("%d, %d, %d\n", pipe->coord_end.x, pipe->coord_end.y, pipe->coord_end.z);
    //CGL_info("%d, %d, %d\n", coord_next.x, coord_next.y, coord_next.z);
    
    // Set the primitive transform to the new position and add it to the current pipe
    CGL_vec3 offset = (CGL_vec3){(CGL_float)coord_next.x, (CGL_float)coord_next.y, (CGL_float)coord_next.z};
    CGL_mat4 rotate = rotation_from_direction(pipe->direction_current, direction_next);
    
    CGL_mesh_cpu_transform_vertices(pipe_extension, rotate);
    CGL_mesh_cpu_offset_vertices(pipe_extension, offset);
    CGL_mesh_cpu_recalculate_normals(pipe_extension);
    //CGL_mesh_cpu_flip_normals(pipe_extension);
    CGL_mesh_cpu_add_mesh(pipe->mesh_cpu, pipe_extension);
    
    // revert the primitive back, we're keeping it for later
    CGL_mesh_cpu_offset_vertices(pipe_extension, (CGL_vec3){-offset.x, -offset.y, -offset.z});
    CGL_mat4 rotate_back = CGL_mat4_transpose(rotate);
    CGL_mesh_cpu_transform_vertices(pipe_extension, rotate_back);
    
    pipe->coord_end = coord_next;
    pipe->direction_current = direction_next;
    pipe->length++;
    grid[coord_next.x][coord_next.y][coord_next.z] = true;
}

CGL_bool pipe_3d_update()
{
    // if pipes are done, do the pixel fade out effect
    if(pipe_current_active == PIPE_MAX_COUNT)
        return CGL_FALSE;
    
    Pipe* pipe = &pipes[pipe_current_active];
    
    // if we're starting a new pipe, draw a sphere at a random location
    if(pipe->length == 0)
    {
        //  set random starting color and valid starting coordinate
        
        CGL_int color = pipe_colors[CGL_utils_random_int(0,17)];
        
        pipe->color = (CGL_vec4){(color & 0xFF)/255.0f, ((color & 0xFF00)>> 8)/255.0f, ((color & 0x00FF0000)>> 16)/255.0f, ((color & 0xFF000000)>> 24 )/255.0f};
        
        assert(PIPE_BOX_LENGTH - 3 >= 2);
        
        CGL_coord pipe_start_coord = {
            CGL_utils_random_int(2, PIPE_BOX_LENGTH - 3),
            CGL_utils_random_int(2, PIPE_BOX_LENGTH - 3),
            CGL_utils_random_int(2, PIPE_BOX_LENGTH - 3)};
        
        // NOTE : if pipe max length were unbounded, this would need to be changed as it could cause performance issues
        while(grid[pipe_start_coord.x][pipe_start_coord.y][pipe_start_coord.z])
        {
            pipe_start_coord = (CGL_coord){
                .x=CGL_utils_random_int(2, PIPE_BOX_LENGTH - 3),
                .y=CGL_utils_random_int(2, PIPE_BOX_LENGTH - 3),
                .z=CGL_utils_random_int(2, PIPE_BOX_LENGTH - 3)};
            
        }
        
        pipe->coord_end = pipe_start_coord;
        
        // draw sphere
        pipe_append_extension(pipe, sphere, pipe_start_coord, PIPE_NONE);
        CGL_mesh_gpu_upload(pipe->mesh_gpu, pipe->mesh_cpu, false);
        return CGL_TRUE;
    }
    
    Pipe_Direction direction_next = pipe_extend_direction(pipe);
    
    CGL_coord coord_next = (CGL_coord){0,0,0};
    
    // Only a sphere is present, no initial direction given, so set it
    if(pipes[pipe_current_active].length == 1)
    {
        pipe->direction_current = direction_next;
    }
    
    coord_next =  coord_next_from_direction(pipe->coord_end, pipe->direction_current);
    
    // if the pipe can't extend, complete it and move to the next one
    if(direction_next == PIPE_NONE)
    {
        // draw sphere
        pipe_append_extension(pipe, sphere, coord_next, PIPE_NONE);
        pipe_current_active++;
        return CGL_TRUE;
    }
    
    // the pipe can extend, so extend the pipe
    if (direction_next == pipe->direction_current)
    {
        // keep going forward, add a cylinder
        pipe_append_extension(pipe, cylinder, coord_next, direction_next);
    }
    else
    {
        // time to turn, add an elbow, update to new direction
        pipe_append_extension(pipe, torus, coord_next, direction_next);
        pipe->direction_current = direction_next;
    }
    CGL_mesh_gpu_upload(pipe->mesh_gpu, pipe->mesh_cpu, false);
    return CGL_TRUE;
}

/*#define DEBUG_MESH_COUNT 125
static CGL_mesh_cpu* debug_meshes[DEBUG_MESH_COUNT];
static CGL_mesh_gpu* debug_meshes_gpu[DEBUG_MESH_COUNT];

void debug_mesh()
{
    for(CGL_int i = 0; i < DEBUG_MESH_COUNT; i++)
    {
        debug_meshes[i] = CGL_mesh_cpu_sphere(5, 5);
        debug_meshes[i] = CGL_mesh_cpu_scale_vertices(debug_meshes[i], 0.02f);
        
        CGL_mesh_cpu_recalculate_normals(debug_meshes[i]);
    }
    
    for(CGL_int x = -2; x < 3; x++)
    {
        for(CGL_int y = -2; y < 3; y++)
        {
            for(CGL_int z = -2; z < 3; z++)
            {
                CGL_mesh_cpu_offset_vertices(debug_meshes[(x+2)+5*(y+2)+25*(z+2)], CGL_vec3_init(x/2.0f, y/2.0f,  z/2.0f));
            }
        }
    }
    for(CGL_int i = 0; i < DEBUG_MESH_COUNT; i++)
    {
        debug_meshes_gpu[i] = CGL_mesh_gpu_create();
        CGL_mesh_gpu_upload(debug_meshes_gpu[i], debug_meshes[i], false);
    }
}*/
struct {
    CGL_window* window;
    CGL_framebuffer* framebuffer;
    CGL_shader* diffuse_shader;
    CGL_texture* texture;

    CGL_mat4 perspective;
    CGL_vec3 eye;
    CGL_mat4 view;
} g_State;

void pipes_render(CGL_shader* diffuse_shader)
{
    // TODO fade out
    
    for(CGL_int i = 0; i < PIPE_MAX_COUNT; i++)
    {
        CGL_shader_set_uniform_vec4(diffuse_shader, CGL_shader_get_uniform_location(diffuse_shader, "pipe_color"), (CGL_vec4*)&(pipes[i].color));
        CGL_mesh_gpu_render(pipes[i].mesh_gpu);
    }
    /*for(CGL_int i = 0; i < DEBUG_MESH_COUNT; i++)
    {
        CGL_mesh_gpu_render(debug_meshes_gpu[i]);
    }*/
}



CGL_bool init() {
    if(!CGL_init()) return CGL_FALSE;
    
    g_State.window = CGL_window_create(WINDOW_WIDTH, WINDOW_HEIGHT, "CGL 3D Pipes - Jaysmito Mukherjee");
    
    CGL_window_make_context_current(g_State.window);
    CGL_gl_init();
    CGL_widgets_init();
    g_State.framebuffer = CGL_framebuffer_create_from_default(g_State.window);
    
    //debug_mesh();
    
    g_State.diffuse_shader = CGL_shader_create(DIFFUSE_VERTEX_SHADER_SOURCE, DIFFUSE_FRAGMENT_SHADER_SOURCE, NULL);
    CGL_shader_bind(g_State.diffuse_shader);
    
    pipes_init();
    grid_init();
    g_State.texture = fade_effect_init(WINDOW_WIDTH, WINDOW_HEIGHT);
    CGL_shader_set_uniform_int(g_State.diffuse_shader, CGL_shader_get_uniform_location(g_State.diffuse_shader, "u_TexFade"), CGL_texture_bind(g_State.texture, 0));
    
    
    CGL_shader_set_uniform_float(g_State.diffuse_shader, CGL_shader_get_uniform_location(g_State.diffuse_shader, "u_WindowWidth"), (float)(WINDOW_WIDTH));
    CGL_shader_set_uniform_float(g_State.diffuse_shader, CGL_shader_get_uniform_location(g_State.diffuse_shader, "u_WindowHeight"), (float)(WINDOW_HEIGHT));
    
    
    // Primitive creation
    CGL_vec3 start = CGL_vec3_init(0.0f, -0.5f, 0.0f);
    CGL_vec3 end = CGL_vec3_init(0.0f, 0.5f, 0.0f);
    cylinder = CGL_mesh_cpu_create_cylinder(start, end, PIPE_3D_RADIUS, PIPE_3D_RADIUS, PIPE_3D_CIRCULAR_RESOLUTION);
    
    sphere = CGL_mesh_cpu_sphere(PIPE_3D_CIRCULAR_RESOLUTION, PIPE_3D_CIRCULAR_RESOLUTION);
    sphere = CGL_mesh_cpu_scale_vertices(sphere, PIPE_3D_RADIUS*2);
    
    CGL_float radius0 = 0.5f;
    CGL_float radius1 = PIPE_3D_RADIUS;
    CGL_vec3 center = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    torus = CGL_mesh_cpu_create_torus_elbow(center, radius0, radius1, PIPE_3D_CIRCULAR_RESOLUTION, PIPE_3D_CIRCULAR_RESOLUTION, CGL_PI_2);
    CGL_float offset = CGL_SQRT2*sqrtf(radius0)/2.0f;
    torus = CGL_mesh_cpu_offset_vertices(torus, CGL_vec3_init(-offset, 0.0f,  -offset));
    // 3D scene setup
    g_State.perspective = CGL_mat4_perspective(CGL_PI_2 * 0.5f, 1.0f, 0.01f, 100.0f);
    g_State.eye = CGL_vec3_init(PIPE_BOX_LENGTH/2.0f, 2.0f+PIPE_BOX_LENGTH/2.0f, 2.5f*PIPE_BOX_LENGTH);
    g_State.view = CGL_mat4_look_at(g_State.eye, CGL_vec3_init(PIPE_BOX_LENGTH/2.0f, PIPE_BOX_LENGTH/2.0f, PIPE_BOX_LENGTH/2.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));
    
    glEnable(GL_DEPTH_TEST);

    return CGL_TRUE;
}


void cleanup() {
    CGL_texture_destroy(g_State.texture);
    CGL_shader_destroy(g_State.diffuse_shader);
    CGL_framebuffer_destroy(g_State.framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(g_State.window);
    CGL_shutdown();
}

EM_BOOL loop(double time, void* userData) {
    (void)time;
    (void)userData;

    if(CGL_window_get_key(g_State.window, CGL_KEY_UP) == CGL_PRESS)
        g_State.eye.y += 1.0f;
        
    if(CGL_window_get_key(g_State.window, CGL_KEY_DOWN) == CGL_PRESS)
        g_State.eye.y -= 1.0f;
        
    if(CGL_window_get_key(g_State.window, CGL_KEY_LEFT) == CGL_PRESS)
        g_State.eye.x -= 1.0f;
        
    if(CGL_window_get_key(g_State.window, CGL_KEY_RIGHT) == CGL_PRESS)
        g_State.eye.x += 1.0f;
        
    if(CGL_window_get_key(g_State.window, CGL_KEY_O) == CGL_PRESS)
        g_State.eye.z -= 1.0f;
        
    if(CGL_window_get_key(g_State.window, CGL_KEY_P) == CGL_PRESS)
        g_State.eye.z += 1.0f;
        
        
    CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        
    CGL_mat4 view_proj = CGL_mat4_mul(g_State.perspective, g_State.view);
    g_State.view = CGL_mat4_look_at(g_State.eye, CGL_vec3_init(2.0f, 2.0f, 2.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));
        
        
        
    CGL_framebuffer_bind(g_State.framebuffer);
        
    CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
        
    int rx = 0; int ry = 0;
    CGL_framebuffer_get_size(g_State.framebuffer, &rx, &ry);
        
    CGL_shader_bind(g_State.diffuse_shader);
    CGL_shader_set_uniform_mat4(g_State.diffuse_shader, CGL_shader_get_uniform_location(g_State.diffuse_shader, "view_proj"), &view_proj);
    CGL_shader_set_uniform_vec3v(g_State.diffuse_shader, CGL_shader_get_uniform_location(g_State.diffuse_shader, "u_light_pos"), PIPE_BOX_LENGTH/2.0f, PIPE_BOX_LENGTH/2.0f, 2*PIPE_BOX_LENGTH);
    CGL_shader_set_uniform_vec3v(g_State.diffuse_shader, CGL_shader_get_uniform_location(g_State.diffuse_shader, "u_eye_pos"), g_State.eye.x, g_State.eye.y, g_State.eye.z);
        
        
    if(CGL_window_get_key(g_State.window, CGL_KEY_L) == CGL_PRESS)
    {
        if(!pipe_3d_update())
        {
            fade_out(g_State.diffuse_shader);
        }
    }
        
    pipes_render(g_State.diffuse_shader);
        
    CGL_widgets_begin();
    CGL_widgets_add_string("Press L", -0.9f, 0.8f, 0.5f, 0.1f);
    CGL_widgets_end();
        
        
    CGL_window_swap_buffers(g_State.window);
    glfwWaitEvents();
    //CGL_window_poll_events(window);
    if(CGL_window_get_key(g_State.window, CGL_KEY_ESCAPE) == CGL_PRESS) return CGL_FALSE;

    return CGL_TRUE;
}

int main()
{   
    if (!init()) return EXIT_FAILURE;

#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while(!CGL_window_should_close(g_State.window))
    { 
        if(!loop(0.0, NULL)) break;
    }
    cleanup();    
#endif

    return EXIT_SUCCESS;
}
