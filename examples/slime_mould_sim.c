// Copyright Jaysmito Mukherjee, 2023
// I am the sole copyright owner of this Work.
// You cannot host, display, distribute or share this Work neither
// as it is or altered,  anywhere else, in any
// form including physical and digital. You cannot use this Work in any
// commercial or non-commercial product, website or project. You cannot
// sell this Work and you cannot mint an NFTs of it or train a neural
// network with it without permission. I share this Work for educational
// purposes, and you can link to it, through an URL, proper attribution
// and unmodified screenshot, as part of your educational material. If
// these conditions are too restrictive please contact me and we'll
// definitely work it out.
// email : jaysmito101@gmail.com

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
"	color = acesFilm(color);\n"
"   color = pow(color, vec3(1.0f / 2.2f));\n"
"	FragColor = vec4(color, 1.0f);\n"
"}";

static const char* PARTICLE_COMPUTE_SHADER = "#version 430 core\n"
"\n"
"layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;\n"
"\n"
"layout(rgba32f, binding = 0) uniform image2D trail_map;\n"
"\n"
"struct Particle\n"
"{\n"
"	vec4 position_dir;\n"
"	vec4 extra0;\n"
"};\n"
"\n"
"layout (std430, binding = 1) buffer Particles\n"
"{\n"
"	Particle particles[];\n"
"};\n"
"\n"
"uniform float delta_time;\n"
"uniform float velocity;\n"
"uniform float sensor_fov;\n"
"uniform float sensor_length;\n"
"uniform float sensor_size;\n"
"uniform float dir_randomness;\n"
"uniform int mode;\n"
"uniform vec3 seed = vec3(0.0f);\n"
"uniform vec2 map_size;\n"
"\n"
"\n"
"// Hash function www.cs.ubc.ca/~rbridson/docs/schechter-sca08-turbulence.pdf\n"
"uint hash(uint state)\n"
"{\n"
"    state ^= 2747636419u;\n"
"    state *= 2654435769u;\n"
"    state ^= state >> 16;\n"
"    state *= 2654435769u;\n"
"    state ^= state >> 16;\n"
"    state *= 2654435769u;\n"
"    return state;\n"
"}\n"
"\n"
"float rand(vec2 co)\n"
"{\n"
"    co += seed.xy;\n"
"    return hash(uint(co.x + co.y)) / 4294967295.0f;\n"
"}\n"
"\n"
"void initialize()\n"
"{\n"
"   uint index = gl_GlobalInvocationID.x;\n"
"   Particle particle;\n"
"   particle.position_dir = vec4(rand(vec2(index, 451.54f)) * 2.0f - 1.0f, rand(vec2(1865.54f, index)) * 2.0f - 1.0f, rand(vec2(1865.54f, index)) * 2.0f - 1.0f, rand(vec2(1865.54f, index)) * 2.0f - 1.0f);\n"
"   particle.position_dir.xy = normalize(particle.position_dir.xy) * rand(vec2(index, index)) * 0.3f;\n"
"   particle.position_dir.wz = normalize(particle.position_dir.wz);\n"
"   particle.extra0 = vec4(0.0f, 0.0f, 0.0f, 0.0f);\n"
"   float rand_f = rand(vec2(hash(index), hash(-index)));\n"
"   if (rand_f < 0.3f) particle.extra0.xyz = vec3(1.5f, 0.0f, 0.0f);\n"
"   else if (rand_f < 0.6f) particle.extra0.xyz = vec3(0.0f, 1.5f, 0.0f);\n"
"   else particle.extra0.xyz = vec3(0.0f, 0.0f, 1.5f);\n"
"   particle.extra0.xyz = vec3(rand(vec2(index, 154)), rand(vec2(465, index)), rand(vec2(index, index)));\n"
"   particles[index] = particle;\n"
"}\n"
"\n"
"float get_sensor(vec2 position, vec2 direction, vec3 mask)\n"
"{\n"
"   vec2 pos = position + direction * sensor_length;\n"
"   float sensor_data = 0.0f;\n"
"   ivec2 pos_on_map = ivec2((pos * 0.5f + vec2(0.5f)) * map_size);\n"
"   int sensor_size_i = int(sensor_size);\n"
"   mask = mask * 2.0f - vec3(1.0f);\n"
"   for(int i = -sensor_size_i; i <= sensor_size_i; ++i)\n"
"   {\n"
"       for(int j = -sensor_size_i; j <= sensor_size_i; ++j)\n"
"       {\n"
"          sensor_data = dot(imageLoad(trail_map, pos_on_map + ivec2(i, j)).rgb, mask);\n"
"       }\n"
"   }\n"
"   return sensor_data;\n"
"}\n"
"\n"
"vec2 get_new_direction(vec2 direction, vec2 position, vec3 mask)\n"
"{\n"
"   vec3 sensor_data = vec3(0.0f);\n"
"   vec2 dir1 = direction;\n"
"   vec2 dir0 = vec2(dir1.x * cos(-sensor_fov / 2.0f) - dir1.y * sin(-sensor_fov / 2.0f), dir1.x * sin(-sensor_fov / 2.0f) + dir1.y * cos(-sensor_fov / 2.0f));\n"
"   vec2 dir2 = vec2(dir1.x * cos(sensor_fov / 2.0f) - dir1.y * sin(sensor_fov / 2.0f), dir1.x * sin(sensor_fov / 2.0f) + dir1.y * cos(sensor_fov / 2.0f));\n"
"   sensor_data.x = get_sensor(position, dir0, mask);\n"
"   sensor_data.y = get_sensor(position, dir1, mask);\n"
"   sensor_data.z = get_sensor(position, dir2, mask);\n"
"   if(sensor_data.x > sensor_data.y && sensor_data.x > sensor_data.z) return dir0;\n"
"   if(sensor_data.y > sensor_data.x && sensor_data.y > sensor_data.z) return dir1;\n"
"   if(sensor_data.z > sensor_data.x && sensor_data.z > sensor_data.y) return dir2;\n"
"   return direction;\n"
"}\n"
"\n"
"void update_pt()\n"
"{\n"
"   uint index = gl_GlobalInvocationID.x;\n"
"   Particle particle = particles[index];\n"
"   vec2 position = particle.position_dir.xy;\n"
"   vec2 direction = particle.position_dir.zw;\n"
"   position += direction * delta_time;\n"
"   if (position.x > 1.0f || position.x < -1.0f) direction.x *= -1.0f;\n"
"   if (position.y > 1.0f || position.y < -1.0f) direction.y *= -1.0f;\n"
"   direction = get_new_direction(direction, position, particle.extra0.xyz);\n"
"   position.x = clamp(position.x, -1.0f, 1.0f);\n"
"   position.y = clamp(position.y, -1.0f, 1.0f);\n"
"   vec2 rand_dir = vec2(rand(vec2(index, 451.54f)) * 2.0f - 1.0f, rand(vec2(1865.54f, index)) * 2.0f - 1.0f);"
"   direction = normalize(mix(direction, rand_dir, dir_randomness));\n"
"   particle.position_dir = vec4(position, direction);\n"
"   particles[index] = particle;\n"
"   ivec2 pixel = ivec2((position.xy * 0.5f + vec2(0.5f)) * map_size);\n"
"   imageStore(trail_map, pixel, vec4(particle.extra0.xyz, 1.0f));\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"   if (mode == 0) initialize();\n"
"   else update_pt();\n"
"}\n";

static const char* TRAIL_MAP_COMPUTE_SHADER = "#version 430 core\n"
"\n"
"layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;\n"
"\n"
"layout(rgba32f, binding = 0) uniform image2D trail_map;\n"
"layout(rgba32f, binding = 1) uniform image2D trail_mapo;\n"
"\n"
"struct Particle\n"
"{\n"
"	vec4 position_dir;\n"
"	vec4 extra0;\n"
"};\n"
"\n"
"layout (std430, binding = 0) buffer Particles\n"
"{\n"
"	Particle particles[];\n"
"};\n"
"\n"
"uniform float delta_time;\n"
"uniform int mode;\n"
"uniform vec3 seed = vec3(0.0f);\n"
"\n"
"void initialize()\n"
"{\n"
"   ivec2 index = ivec2(gl_GlobalInvocationID.xy);\n"
"   imageStore(trail_map, index, vec4(0.0f, 0.0f, 0.0f, 1.0f));\n"
"}\n"
"\n"
"vec4 blur_filter(ivec2 uv)\n"
"{\n"
"   vec4 color = vec4(0.0f);\n"
"   for (int i = -2; i <= 2; i++)\n"
"       for (int j = -2; j <= 2; j++)\n"
"           color += imageLoad(trail_map, uv + ivec2(i, j));\n"
"   return color / 25.0f;\n"
"}\n"
"\n"
"void update_tm()\n"
"{\n"
"   ivec2 index = ivec2(gl_GlobalInvocationID.xy);\n"
"   vec4 color = blur_filter(index);\n"
"   color = mix(color, vec4(0.0f), delta_time * 8.0f);\n"
"   imageStore(trail_map, index, color);\n"
"   imageStore(trail_mapo, index, color);\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    if (mode == 0) initialize();\n"
"    else if (mode == 1) update_tm();\n"
"}\n";


#define PARTICLE_COUNT 10000
#define MAP_RESOLUTION 700

static CGL_framebuffer* default_framebuffer = NULL;
static CGL_texture *trail_map = NULL, *trail_mapo = NULL;
static CGL_shader *particle_compute_shader = NULL, *trail_map_compute_shader = NULL, *present_shader = NULL;
static CGL_bloom *bloom = NULL;
static CGL_ssbo *particle_ssbo = NULL;
static CGL_float delta_time = 0.0f, dir_randomness = 0.05f, sensor_length = 0.01f, sensor_angle = CGL_PI_2, sensor_size = 0.0f;

void reset_simulation()
{
    glBindImageTexture(0, trail_map->handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    CGL_shader_bind(particle_compute_shader);
    CGL_shader_set_uniform_int(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "mode"), 0);
    CGL_shader_set_uniform_vec3v(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "seed"), CGL_utils_random_float() * 100.0f, CGL_utils_random_float() * 100.0f, CGL_utils_random_float() * 100.0f);
    CGL_shader_compute_dispatch(particle_compute_shader, PARTICLE_COUNT / 256 + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

    CGL_shader_bind(trail_map_compute_shader);
    CGL_shader_set_uniform_int(trail_map_compute_shader, CGL_shader_get_uniform_location(trail_map_compute_shader, "mode"), 0);
    CGL_shader_set_uniform_vec3v(trail_map_compute_shader, CGL_shader_get_uniform_location(trail_map_compute_shader, "seed"), CGL_utils_random_float() * 100.0f, CGL_utils_random_float() * 100.0f, CGL_utils_random_float() * 100.0f);
    CGL_shader_compute_dispatch(trail_map_compute_shader, MAP_RESOLUTION / 16 + 1, MAP_RESOLUTION / 16 + 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
}

void update_simulation()
{
    glBindImageTexture(0, trail_map->handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(1, trail_mapo->handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    CGL_ssbo_bind(particle_ssbo);

    CGL_shader_bind(particle_compute_shader);
    CGL_shader_set_uniform_int(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "mode"), 1);
    CGL_shader_set_uniform_float(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "delta_time"), delta_time);
    CGL_shader_set_uniform_float(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "sensor_fov"), sensor_angle);
    CGL_shader_set_uniform_float(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "sensor_length"), sensor_length);
    CGL_shader_set_uniform_float(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "sensor_size"), sensor_size);
    CGL_shader_set_uniform_float(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "velocity"), 0.3f);
    CGL_shader_set_uniform_float(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "dir_randomness"), dir_randomness);
    CGL_shader_set_uniform_vec2v(particle_compute_shader, CGL_shader_get_uniform_location(particle_compute_shader, "map_size"), MAP_RESOLUTION, MAP_RESOLUTION);
    CGL_shader_compute_dispatch(particle_compute_shader, PARTICLE_COUNT / 256 + 1, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

    CGL_shader_bind(trail_map_compute_shader);
    CGL_shader_set_uniform_int(trail_map_compute_shader, CGL_shader_get_uniform_location(trail_map_compute_shader, "mode"), 1);
    CGL_shader_set_uniform_float(trail_map_compute_shader, CGL_shader_get_uniform_location(trail_map_compute_shader, "delta_time"), delta_time);
    CGL_shader_compute_dispatch(trail_map_compute_shader, MAP_RESOLUTION / 16 + 1, MAP_RESOLUTION / 16 + 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
}

int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(700, 700, "Slime Sim - Jaysmito Mukherjee");
    if(!window) return 1;
    CGL_window_make_context_current(window);
    CGL_gl_init();
    CGL_widgets_init();

    default_framebuffer = CGL_framebuffer_create_from_default(window);
    trail_map = CGL_texture_create_blank(MAP_RESOLUTION, MAP_RESOLUTION, GL_RGBA, GL_RGBA32F, GL_FLOAT);
    trail_mapo = CGL_texture_create_blank(MAP_RESOLUTION, MAP_RESOLUTION, GL_RGBA, GL_RGBA32F, GL_FLOAT);

    present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    particle_compute_shader = CGL_shader_compute_create(PARTICLE_COMPUTE_SHADER, NULL);
    trail_map_compute_shader = CGL_shader_compute_create(TRAIL_MAP_COMPUTE_SHADER, NULL);

    bloom = CGL_bloom_create(700, 700, 3);

    particle_ssbo = CGL_ssbo_create(1);
    CGL_ssbo_set_data(particle_ssbo, sizeof(CGL_float) * 8 * PARTICLE_COUNT, NULL, GL_DYNAMIC_DRAW);
    
    CGL_float curr_time = CGL_utils_get_time();
    CGL_float prev_time = CGL_utils_get_time();
    CGL_float time = 0.0f, frame_time = 0.0f;
    CGL_int frames = 0, fps = 0;


    reset_simulation();

    while(!CGL_window_should_close(window))
    {
        CGL_window_set_size(window, 700, 700); // force window size to be 600x600

        curr_time = CGL_utils_get_time();
        delta_time = curr_time - prev_time;
        prev_time = curr_time;
        time += delta_time; frame_time += delta_time;
        frames++;
        if(frame_time >= 1.0f) { fps = frames; frames = 0; frame_time = 0.0f; }
        
        update_simulation();

        CGL_bloom_apply(bloom, trail_mapo);

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shader_bind(present_shader);
        CGL_texture_bind(trail_mapo, 0);
        CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 0);
        CGL_gl_render_screen_quad();

        glDisable(GL_DEPTH_TEST);

        CGL_widgets_begin();
        
        CGL_widgets_set_fill_colorf(1.0f, 1.0f, 1.0f, 1.0f);
        CGL_widgets_set_stroke_colorf(1.0f, 1.0f, 1.0f, 1.0f);

        static CGL_byte buffer[256];
        sprintf(buffer, "Frame Time: %f", delta_time);
        CGL_widgets_add_string(buffer, -1.0f, 0.975f, 0.5f, 0.025f);
        sprintf(buffer, "Framerate : %d", fps);
        CGL_widgets_add_string(buffer, -1.0f, 0.950f, 0.5f, 0.025f);
        sprintf(buffer, "Particles : %d", PARTICLE_COUNT);
        CGL_widgets_add_string(buffer, -1.0f, 0.925f, 0.5f, 0.025f);  
        sprintf(buffer, "Resolution: %dx%d", MAP_RESOLUTION, MAP_RESOLUTION);
        CGL_widgets_add_string(buffer, -1.0f, 0.900f, 0.5f, 0.025f);
        sprintf(buffer, "Randomness: %f", dir_randomness);
        CGL_widgets_add_string(buffer, -1.0f, 0.875f, 0.5f, 0.025f);
        sprintf(buffer, "Sensor Length: %f", sensor_length);
        CGL_widgets_add_string(buffer, -1.0f, 0.850f, 0.5f, 0.025f);
        sprintf(buffer, "Sensor Angle: %d", (CGL_int)CGL_rad_to_deg(sensor_angle));
        CGL_widgets_add_string(buffer, -1.0f, 0.825f, 0.5f, 0.025f);
        sprintf(buffer, "Sensor Spread: %d", (CGL_int)sensor_size);
        CGL_widgets_add_string(buffer, -1.0f, 0.800f, 0.5f, 0.025f);


        CGL_widgets_end();

        CGL_window_poll_events(window);
        CGL_window_swap_buffers(window);

        if(CGL_window_get_key(window, CGL_KEY_SPACE) == CGL_PRESS) reset_simulation();

        if(CGL_window_get_key(window, CGL_KEY_R) == CGL_PRESS)
        {
            if(CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) dir_randomness += 0.01f;
            if(CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) dir_randomness -= 0.01f;
        }

        if(CGL_window_get_key(window, CGL_KEY_L) == CGL_PRESS)
        {
            if(CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) sensor_length += 0.001f;
            if(CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) sensor_length -= 0.001f;
        }

        if(CGL_window_get_key(window, CGL_KEY_A) == CGL_PRESS)
        {
            if(CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) sensor_angle += 0.01f;
            if(CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) sensor_angle -= 0.01f;
        }
        
        if(CGL_window_get_key(window, CGL_KEY_Z) == CGL_PRESS)
        {
            if(CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) sensor_size += 0.1f;
            if(CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) sensor_size -= 0.1f;
        }

    }


    CGL_texture_destroy(trail_map);
    CGL_texture_destroy(trail_mapo);
    CGL_ssbo_destroy(particle_ssbo);
    CGL_shader_destroy(particle_compute_shader);
    CGL_shader_destroy(trail_map_compute_shader);
    CGL_shader_destroy(present_shader);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_bloom_destroy(bloom);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);    
    CGL_shutdown();
    return 0;
}
