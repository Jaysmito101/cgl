/*
MIT License

Copyright (c) 2022 Jaysmito Mukherjee (jaysmito101@gmail.com)

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

const CGL_byte* DIFFUSE_VERTEX_SHADER_SOURCE = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"out vec3 Position;\n"
"out vec3 Normal;\n"
"out vec3 Color;\n"
"out vec2 TexCoord;\n"
"\n"
"uniform mat4 view_proj;\n"
"uniform float u_scale;\n"
"\n"
"void main()\n"
"{\n"
"   vec3 pos = position.xyz * u_scale;\n"
"	gl_Position = view_proj * vec4(pos, 1.0f);\n"
"	Position = position.xyz;\n"
"	Normal = normal.xyz;\n"
"	TexCoord = texcoord.xy;\n"
"   Color = vec3(0.4f);\n"
"   Color = Normal * 0.5f + vec3(0.5f);"
"   float scale_factor = u_scale;\n"
"   if(pos.x >= scale_factor || pos.y >= scale_factor || pos.z >= scale_factor || pos.x <= -scale_factor || pos.y <= -scale_factor || pos.z <= -scale_factor)\n"
"   {\n"
"       Color = vec3(1.0f);\n"
"   }\n"
"   Color *= 0.6f;\n"
"}";

const CGL_byte* DIFFUSE_FRAGMENT_SHADER_SOURCE = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Position;\n"
"in vec3 Normal;\n"
"in vec3 Color;\n"
"in vec2 TexCoord;\n"
"\n"
"uniform vec3 u_light_pos;\n"
"\n"
"void main()\n"
"{\n"
"	vec3 object_color = Color;\n"
"	vec3 light_color = vec3(1.0f, 1.0f, 1.0f);\n"
"	vec3 light_dir = normalize(u_light_pos - Position);\n"
"	float diffuse = max(dot(Normal, light_dir), 0.0f) + 0.18f;\n"
"	vec3 result = (diffuse * light_color) * object_color;\n"
"	FragColor = vec4(result, 1.0f);\n"
"}";


static CGL_framebuffer *default_framebuffer, *bloom_framebuffer;
static CGL_shader *present_shader, *diffuse_shader;
static CGL_mesh_gpu *mesh;

CGL_mesh_cpu* generate_menger_unit(CGL_mesh_cpu* base_mesh, CGL_bool invert)
{
    CGL_mesh_cpu* mesh_cpu = CGL_mesh_cpu_create(base_mesh->vertex_count_used * 27, base_mesh->index_count_used * 27);
    for(int x = 0 ; x < 3 ; x++)
    {
        for(int y = 0 ; y < 3 ; y++)
        {
            for(CGL_int z = 0; z < 3; z++)
            {
                CGL_bool cond = abs(x - 1) +  abs(y - 1) + abs(z - 1) > 1;
                if(invert ? !cond : cond) CGL_mesh_cpu_add_mesh(mesh_cpu, base_mesh);
                CGL_mesh_cpu_offset_vertices(mesh_cpu, CGL_vec3_init(0.0f, 0.0f, 2.0f));
            }
            CGL_mesh_cpu_offset_vertices(mesh_cpu, CGL_vec3_init(0.0f, 2.0f, -3.0f * 2.0f));
        }
        CGL_mesh_cpu_offset_vertices(mesh_cpu, CGL_vec3_init(2.0f, -3.0f * 2.0f, 0.0f));
    }
    CGL_mesh_cpu_offset_vertices(mesh_cpu, CGL_vec3_init(-4.0f, 2.0f, 2.0f));
    return mesh_cpu;
}

CGL_mesh_gpu* generate_menger_item(CGL_int sub_div, CGL_bool invert)
{
    CGL_info("Generating menger sponge with %d subdivisions ...", sub_div);
    //CGL_mesh_cpu* base_mesh = CGL_mesh_cpu_cube(false);
    CGL_mesh_cpu* base_mesh = CGL_mesh_cpu_cube(false);
    for(int i = 0 ; i < sub_div ; i++)
    {
        CGL_info("Subdividing %d/%d", i + 1, sub_div);
        CGL_mesh_cpu* mesh_cpu = generate_menger_unit(base_mesh, invert);
        CGL_mesh_cpu_destroy(base_mesh);
        base_mesh = mesh_cpu;
        CGL_mesh_cpu_scale_vertices(base_mesh, 1.0f / 3.0f);
    }
    CGL_info("Recalculating normals ...");
    CGL_mesh_cpu_recalculate_normals(base_mesh);
    CGL_info("Uploading to GPU ...");
    CGL_mesh_gpu* mesh_gpu = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(mesh_gpu, base_mesh, false);
    CGL_mesh_cpu_destroy(base_mesh);
    CGL_info("Done");
    return mesh_gpu;    
}


int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(700, 700, "Meneger Sponge Fractal - Jaysmito Mukherjee");
    if(!window) return 1;
    CGL_window_make_context_current(window);
    CGL_gl_init();
    CGL_widgets_init();

    default_framebuffer = CGL_framebuffer_create_from_default(window);
    bloom_framebuffer = CGL_framebuffer_create(700, 700);

    present_shader = CGL_shader_create(PASS_THROUGH_VERTEX_SHADER_SOURCE, PASS_THROUGH_FRAGMENT_SHADER_SOURCE, NULL);
    diffuse_shader = CGL_shader_create(DIFFUSE_VERTEX_SHADER_SOURCE, DIFFUSE_FRAGMENT_SHADER_SOURCE, NULL);

    mesh = generate_menger_item(0, false);

    CGL_float curr_time = CGL_utils_get_time();
    CGL_float prev_time = CGL_utils_get_time();
    CGL_float delta_time = 0.0f;
    CGL_float time = 0.0f, frame_time = 0.0f;
    CGL_float obj_scale = 3.0f;
    CGL_int frames = 0, fps = 0;
    CGL_int subdiv_level = 0;
    CGL_bool inverted_menger = false;
    
    CGL_mat4 projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    CGL_mat4 view = CGL_mat4_identity();

    CGL_bloom* bloom = CGL_bloom_create(700, 700, 3);
    CGL_bloom_set_threshold(bloom, 1.0f);
    CGL_bloom_set_offset(bloom, 3.0f, 3.0f);

    while(!CGL_window_should_close(window))
    {
        CGL_window_set_size(window, 700, 700); // force window size to be 600x600

        CGL_vec3 camera_position = CGL_vec3_init(sinf(time * 0.3f) * 20.0f, (cosf(time * 0.3f) + sinf(time * 0.3f)) / 1.414f * 20.0f, cosf(time * 0.3f) * 20.0f);
        view = CGL_mat4_look_at(camera_position, CGL_vec3_init(0.0f, 0.0f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));
        view = CGL_mat4_mul(projection, view);

        curr_time = CGL_utils_get_time();
        delta_time = curr_time - prev_time;
        prev_time = curr_time;
        time += delta_time; frame_time += delta_time;
        frames++;
        if(frame_time >= 1.0f) { fps = frames; frames = 0; frame_time = 0.0f; }

        static CGL_bool prev_inv = false;
        static CGL_int prev_subdiv = 0;
        if(inverted_menger != prev_inv || subdiv_level != prev_subdiv)
        {
            CGL_mesh_gpu_destroy(mesh);
            mesh = generate_menger_item(subdiv_level, inverted_menger);
            prev_inv = inverted_menger;
            prev_subdiv = subdiv_level;
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        CGL_framebuffer_bind(bloom_framebuffer);
        CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
        CGL_shader_bind(diffuse_shader);
        CGL_shader_set_uniform_mat4(diffuse_shader, CGL_shader_get_uniform_location(diffuse_shader, "view_proj"), &view);
        CGL_shader_set_uniform_vec3v(diffuse_shader, CGL_shader_get_uniform_location(diffuse_shader, "u_light_pos"), camera_position.x, camera_position.y, camera_position.z);
        CGL_shader_set_uniform_float(diffuse_shader, CGL_shader_get_uniform_location(diffuse_shader, "u_scale"), obj_scale);
        CGL_mesh_gpu_render(mesh);

        CGL_bloom_apply(bloom, CGL_framebuffer_get_color_texture(bloom_framebuffer));

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shader_bind(present_shader);
        CGL_texture_bind(CGL_framebuffer_get_color_texture(bloom_framebuffer), 0);
        CGL_shader_set_uniform_int(present_shader, CGL_shader_get_uniform_location(present_shader, "u_tex"), 0);
        CGL_gl_render_screen_quad();


        glDisable(GL_DEPTH_TEST);

        CGL_widgets_begin();

        static CGL_byte buffer[1024];
        sprintf(buffer, "Framerate: %d", fps);
        CGL_widgets_add_string(buffer, -1.0f, 0.95f, 1.0f, 0.05f);
        sprintf(buffer, "Frametime: %f", delta_time);
        CGL_widgets_add_string(buffer, -1.0f, 0.90f, 1.0f, 0.05f);
        sprintf(buffer, "Subdivision: %d", subdiv_level);
        CGL_widgets_add_string(buffer, -1.0f, 0.85f, 1.0f, 0.05f);
        sprintf(buffer, "Inverted: %s", inverted_menger ? "True" : "False");
        CGL_widgets_add_string(buffer, -1.0f, 0.80f, 1.0f, 0.05f);
        sprintf(buffer, "Scale: %f", obj_scale);
        CGL_widgets_add_string(buffer, -1.0f, 0.75f, 1.0f, 0.05f);

        CGL_widgets_end();

        CGL_window_poll_events(window);
        CGL_window_swap_buffers(window);

        if(CGL_window_is_key_pressed(window, CGL_KEY_UP)) { subdiv_level++; CGL_utils_sleep(300); }
        if(CGL_window_is_key_pressed(window, CGL_KEY_DOWN)) { subdiv_level--; CGL_utils_sleep(300); }
        subdiv_level = CGL_utils_clamp(subdiv_level, 0, 8);

        if(CGL_window_is_key_pressed(window, CGL_KEY_SPACE)) { inverted_menger = !inverted_menger; CGL_utils_sleep(100); }

        if(CGL_window_is_key_pressed(window, CGL_KEY_RIGHT)) { obj_scale += 0.01f;}
        if(CGL_window_is_key_pressed(window, CGL_KEY_LEFT)) { obj_scale -= 0.01f;}
        obj_scale = CGL_utils_clamp(obj_scale, 0.01f, 10.0f);
    }

    CGL_bloom_destroy(bloom);
    CGL_shader_destroy(diffuse_shader);
    CGL_shader_destroy(present_shader);
    CGL_mesh_gpu_destroy(mesh);
    CGL_framebuffer_destroy(bloom_framebuffer);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);    
    CGL_shutdown();
    return 0;
}
