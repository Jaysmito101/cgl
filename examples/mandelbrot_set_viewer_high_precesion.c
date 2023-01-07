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

static const char* JULIA_COMPUTE_SHADER = "#version 430 core\n"
"\n"
"layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;\n"
"\n"
"layout(rgba32f, binding = 0) uniform image2D output_tex;\n"
"\n"
"uniform double sc_size;\n"
"uniform double sc_res;\n"
"uniform double sc_ofx;\n"
"uniform double sc_ofy;\n"
"uniform int mode;\n"
"\n"
"#define MAX_ITER 1000\n"
"#define BAIL_RADIUS 2.0\n"
"\n"
"void main()\n"
"{\n"
"   ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);\n"
"   dvec2 psc = vec2(pixel) / sc_res * sc_size + dvec2(sc_ofx, sc_ofy);\n"
"   dvec2 c = psc;\n"
"   dvec2 z = dvec2(0.0, 0.0);\n"
"   int i = 0;\n"
"   for (; i < MAX_ITER; i++)\n"
"   {\n"
"       z = dvec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;\n"
"       if (z.x * z.x + z.y * z.y > BAIL_RADIUS)\n"
"           break;\n"
"   }\n"
"   float log_zn = float(log(float(z.x * z.x + z.y * z.y)) / 2.0);\n"
"   float nu = log(log_zn / log(float(BAIL_RADIUS))) / log(2.0f);\n"
"   float iter = float(i) + 1.0 - nu;\n"
"   float r = sqrt(iter / float(MAX_ITER));\n"
"   float g = iter / float(MAX_ITER);\n"
"   float b = sqrt(sqrt(iter / float(MAX_ITER)));\n"
"   imageStore(output_tex, pixel, vec4(r, g, b, 1.0f));\n"
"}\n";


#define MAP_RESOLUTION 700

static CGL_framebuffer* default_framebuffer = NULL;
static CGL_texture *output_tex = NULL;
static CGL_shader *output_tex_compute_shader = NULL, *present_shader = NULL;
static CGL_bloom *bloom = NULL;
static CGL_double scale, offset_x, offset_y;


void update_simulation()
{
    glBindImageTexture(0, output_tex->handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    CGL_shader_bind(output_tex_compute_shader);
    CGL_shader_set_uniform_double(output_tex_compute_shader, CGL_shader_get_uniform_location(output_tex_compute_shader, "sc_size"), 1.0 / scale);
    CGL_shader_set_uniform_double(output_tex_compute_shader, CGL_shader_get_uniform_location(output_tex_compute_shader, "sc_res"), (CGL_double)MAP_RESOLUTION);
    CGL_shader_set_uniform_double(output_tex_compute_shader, CGL_shader_get_uniform_location(output_tex_compute_shader, "sc_ofx"), offset_x);
    CGL_shader_set_uniform_double(output_tex_compute_shader, CGL_shader_get_uniform_location(output_tex_compute_shader, "sc_ofy"), offset_y);
    CGL_shader_compute_dispatch(output_tex_compute_shader, MAP_RESOLUTION / 16 + 1, MAP_RESOLUTION / 16 + 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
}

int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(700, 700, "Julia Set - Jaysmito Mukherjee");
    if(!window) return 1;
    CGL_window_make_context_current(window);
    CGL_gl_init();
    CGL_widgets_init();

    default_framebuffer = CGL_framebuffer_create_from_default(window);
    output_tex = CGL_texture_create_blank(MAP_RESOLUTION, MAP_RESOLUTION, GL_RGBA, GL_RGBA32F, GL_FLOAT);

    present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    output_tex_compute_shader = CGL_shader_compute_create(JULIA_COMPUTE_SHADER, NULL);

    bloom = CGL_bloom_create(MAP_RESOLUTION, MAP_RESOLUTION, 3);

    scale = 1.0; offset_x = 0.0; offset_y = 0.0;

    CGL_float curr_time = CGL_utils_get_time();
    CGL_float prev_time = CGL_utils_get_time();
    CGL_float time = 0.0f, frame_time = 0.0f, delta_time = 0.0f;
    CGL_int frames = 0, fps = 0;

    while(!CGL_window_should_close(window))
    {
        CGL_window_set_size(window, 700, 700); // force window size to be 600x600

        curr_time = CGL_utils_get_time();
        delta_time = curr_time - prev_time;
        prev_time = curr_time;
        time += delta_time; frame_time += delta_time;
        frames++; if(frame_time >= 1.0f) { fps = frames; frames = 0; frame_time = 0.0f; }
        
        update_simulation();

        //CGL_bloom_apply(bloom, output_tex);

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shader_bind(present_shader);
        CGL_texture_bind(output_tex, 0);
        CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 0);
        CGL_gl_render_screen_quad();

        glDisable(GL_DEPTH_TEST);

        CGL_widgets_begin();
        
        CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.0f, 1.0f);

        static CGL_byte buffer[256];
        sprintf(buffer, "Frame Time: %f", delta_time);
        CGL_widgets_add_string(buffer, -1.0f, 0.975f, 0.5f, 0.025f);
        sprintf(buffer, "Framerate : %d", fps);
        CGL_widgets_add_string(buffer, -1.0f, 0.950f, 0.5f, 0.025f);
        sprintf(buffer, "Scale : %lf", scale);
        CGL_widgets_add_string(buffer, -1.0f, 0.925f, 0.5f, 0.025f);  
        sprintf(buffer, "Resolution: %dx%d", MAP_RESOLUTION, MAP_RESOLUTION);
        CGL_widgets_add_string(buffer, -1.0f, 0.900f, 0.5f, 0.025f);
        sprintf(buffer, "Offset X: %lf", offset_x);
        CGL_widgets_add_string(buffer, -1.0f, 0.875f, 0.5f, 0.025f);
        sprintf(buffer, "Offset Y: %lf", offset_y);
        CGL_widgets_add_string(buffer, -1.0f, 0.850f, 0.5f, 0.025f);


        CGL_widgets_end();

        CGL_window_poll_events(window);
        CGL_window_swap_buffers(window);


        if(CGL_window_get_key(window, CGL_KEY_R) == CGL_PRESS)
        {
//            if(CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) dir_randomness += 0.01f;
//            if(CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) dir_randomness -= 0.01f;
        }
        else if(CGL_window_get_key(window, CGL_KEY_S) == CGL_PRESS)
        {
            if(CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) scale *= 1.005f;
            if(CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) scale *= 0.995f;
        }
        else
        {
            if(CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) offset_y += 0.01f / scale;
            if(CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) offset_y -= 0.01f / scale;
            if(CGL_window_get_key(window, CGL_KEY_LEFT) == CGL_PRESS) offset_x += 0.01f / scale;
            if(CGL_window_get_key(window, CGL_KEY_RIGHT) == CGL_PRESS) offset_x -= 0.01f / scale;
        }


    }


    CGL_texture_destroy(output_tex);
    CGL_shader_destroy(output_tex_compute_shader);
    CGL_shader_destroy(present_shader);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_bloom_destroy(bloom);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);    
    CGL_shutdown();
    return 0;
}
