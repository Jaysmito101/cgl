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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

static const char* VS_diffuse = "#version 430 core\n"
"layout(location = 0) in vec4 aPos;\n"
"layout(location = 1) in vec4 aNormal;\n"
"layout(location = 2) in vec4 aTexCoords;\n"
"\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"out vec2 TexCoords;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"    mat4 model_view = view * model;\n"
"    FragPos = vec3(model * aPos);\n"
"    Normal = mat3(transpose(inverse(model_view))) * aNormal.xyz;\n"
"    TexCoords = aTexCoords.xy;\n"
"    gl_Position = projection * model_view * vec4(aPos.xyz, 1.0f);\n"
"}\n";

static const char* FS_diffuse = "#version 430 core\n"
"in vec3 FragPos;\n"
"in vec3 Normal;\n"
"in vec2 TexCoords;\n"
"\n"
"layout(location = 0) out vec4 FragColor;\n"
"layout(location = 1) out vec4 AlbedoOut;\n"
"layout(location = 2) out vec4 NormalOut;\n"
"\n"
"uniform vec3 lightPos;\n"
"uniform vec3 objectColor;\n"
"\n"
"void main()\n"
"{\n"
"    //FragColor = vec4(1.0);return;\n"
"    vec3 lightColor = vec3(1.0, 1.0, 1.0);\n"
"    vec3 objectColor = pow(objectColor, vec3(2.2f));\n"
"\n"
"    vec3 ambient = 0.1f * lightColor;\n"
"\n"
"    vec3 norm = normalize(Normal);\n"
"    vec3 lightDir = normalize(lightPos - FragPos);\n"
"    float diff = max(dot(norm, lightDir), 0.0);\n"
"    vec3 diffuse = diff * lightColor;\n"
"\n"
"    vec3 result = (ambient + diffuse) * objectColor;\n"
"    FragColor = vec4(result, 1.0);\n"
"    AlbedoOut = vec4(objectColor, 1.0);\n"
"    NormalOut = vec4(norm, 1.0);\n"
"}\n";

static const char* VS_presentation = "#version 430 core\n"
"layout(location = 0) in vec4 aPos;\n"
"layout(location = 1) in vec4 aNormal;\n"
"layout(location = 2) in vec4 aTexCoords;\n"
"\n"
"out vec2 TexCoords;\n"
"\n"
"void main()\n"
"{\n"
"    TexCoords = aTexCoords.xy;\n"
"    gl_Position = vec4(aPos.xy, 0.1f, 1.0f);\n"
"}\n";

static const char* FS_presentation = "#version 430 core\n"
"in vec2 TexCoords;\n"
"\n"
"vec3 aces_tonemap(vec3 x){	\n"
"	const float a = 2.51f;\n"
"    const float b = 0.03f;\n"
"    const float c = 2.43f;\n"
"    const float d = 0.59f;\n"
"    const float e = 0.14f;\n"
"    return clamp((x * (a * x + b)) / (x * (c * x + d ) + e), 0.0f, 1.0f);\n"
"}\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"uniform sampler2D texture_scene;\n"
"\n"
"void main()\n"
"{\n"
"    vec2 uv = vec2(TexCoords.y, 1.0f - TexCoords.x);\n"
"    vec3 scene = texture(texture_scene, uv).rgb;\n"
"    scene = aces_tonemap(scene);\n"
"    FragColor = vec4(scene, 1.0);\n"
"}\n";

#define OBJ_TYPE_ENEMY 1
#define OBJ_TYPE_SCORE 2

typedef struct
{
    CGL_vec3 position;
    CGL_vec3 color;
    CGL_int type;
    CGL_bool set;
}  interactable_object;

static struct
{
    interactable_object objects[100];
    CGL_mesh_gpu* model;
    CGL_shader* diffuse_shader;
    CGL_shader* presentation_shader;
} g_resources;
static CGL_float distance_traveled = 0.0f;
static CGL_float game_speed = 0.05f;
static CGL_int frame_number = 0;

void load_resources()
{
    for(int i = 0 ; i < 100 ; i++) g_resources.objects[i].set = false;

    CGL_mesh_cpu* model_cpu = CGL_mesh_cpu_cube(false);
    CGL_mesh_cpu_recalculate_normals(model_cpu);
    g_resources.model = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(g_resources.model, model_cpu, true);
    CGL_mesh_cpu_destroy(model_cpu);

    // load the shaders
    g_resources.diffuse_shader = CGL_shader_create(VS_diffuse, FS_diffuse, NULL);
    g_resources.presentation_shader = CGL_shader_create(VS_presentation, FS_presentation, NULL);
}

void free_resources()
{
    CGL_mesh_gpu_destroy(g_resources.model);
    // free the shaders
    CGL_shader_destroy(g_resources.diffuse_shader);
    CGL_shader_destroy(g_resources.presentation_shader);
}

void add_object()
{
    int type = CGL_utils_random_int(0, 100);
    for(int i = 0 ; i < 100 ; i++)
    {
        if(!g_resources.objects[i].set)
        {
            g_resources.objects[i].set = true;
            g_resources.objects[i].position = CGL_vec3_init(CGL_utils_random_float() * 6.0f - 3.0f, 0.5f, -100.0f);
            if(type % 5 == 0) g_resources.objects[i].color = CGL_vec3_init(1.2f, CGL_utils_random_float() * 0.3f + 0.3f,  CGL_utils_random_float() * 0.3f + 0.3f); // enemy
            else g_resources.objects[i].color = CGL_vec3_init(CGL_utils_random_float() * 0.3f + 0.3f,  CGL_utils_random_float() * 0.3f + 0.3f, 1.2f); // score
            g_resources.objects[i].type = type;
            return;
        }
    }
}

CGL_int update_objects(float player_x)
{
    for(int i = 0 ; i < 100 ; i++)
    {
        g_resources.objects[i].position.z += game_speed;
        if(g_resources.objects[i].set) if(g_resources.objects[i].position.z > 10) g_resources.objects[i].set = false;
        if(!g_resources.objects[i].set) continue;
        if(g_resources.objects[i].position.z > -1.0f && fabsf(g_resources.objects[i].position.x - player_x) < 1.9f)
        {
            g_resources.objects[i].set = false;
            return g_resources.objects[i].type;
        }
    }
    return -1;
}

void render_objects()
{    
    CGL_mat4 model = CGL_mat4_identity();
    CGL_mat4 temp = CGL_mat4_identity();
    for(int i = 0 ; i < 100 ; i++)
    {
        if(!g_resources.objects[i].set) continue;
        model = CGL_mat4_translate(g_resources.objects[i].position.x, g_resources.objects[i].position.y, g_resources.objects[i].position.z);
        temp = CGL_mat4_scale(0.5f, 0.5f, 0.5f);
        model = CGL_mat4_mul(model, temp);
        CGL_shader_set_uniform_mat4(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "model"), &model);
        CGL_shader_set_uniform_vec3v(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "objectColor"), g_resources.objects[i].color.x, g_resources.objects[i].color.y, g_resources.objects[i].color.z);
        CGL_mesh_gpu_render(g_resources.model);
    }
}

void clear_all()
{
    for(int i = 0 ; i < 100 ; i++) g_resources.objects[i].set = false;    
}

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return -1;
    CGL_window* main_window = CGL_window_create(700, 700, "Neopeed - Jaysmito Mukherjee");
    if(!main_window) return -1;
    CGL_window_make_context_current(main_window);
    if(!CGL_gl_init()) return -1;
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(main_window);   
    CGL_framebuffer* g_buffer = CGL_framebuffer_create_basic(700, 700);
    CGL_framebuffer_add_color_attachment(g_buffer, CGL_texture_create_blank(700, 700, GL_RGBA, GL_RGBA, GL_FLOAT)); // for albedo
    CGL_framebuffer_add_color_attachment(g_buffer, CGL_texture_create_blank(700, 700, GL_RGBA, GL_RGBA, GL_FLOAT)); // for normal
    CGL_toon_post_processor_init();
    CGL_widgets_init();
    CGL_bloom* bloom = CGL_bloom_create(700, 700, 3);
    CGL_bloom_set_offset(bloom, 10.0f, 8.0f);

    CGL_mat4 projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    CGL_mat4 view = CGL_mat4_look_at(CGL_vec3_init(0.0f, 8.0f, 12.0f), CGL_vec3_init(0.0f, 0.5f, -3.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));
    CGL_mat4 model = CGL_mat4_identity();
    CGL_mat4 temp = CGL_mat4_identity();
    CGL_float intensity = 1.15f;
    CGL_float player_x = 0.0f;
    CGL_float player_y = 0.0f;
    CGL_float prev_time = CGL_utils_get_time();
    CGL_float curr_time = CGL_utils_get_time();
    CGL_float start_time = curr_time;
    CGL_float delta_time = curr_time - prev_time;
    CGL_float last_add_time = curr_time;
    CGL_int score = 0;

    load_resources();

    while(!CGL_window_should_close(main_window))
    { 

        curr_time = CGL_utils_get_time();
        delta_time = curr_time - prev_time;
        prev_time = curr_time;
        frame_number++;
        //CGL_window_set_size(main_window, 700, 700); // force window size to be 700x700
        CGL_int wx, wy;
        CGL_window_get_framebuffer_size(main_window, &wx, &wy);
        projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), (CGL_float)wx/wy, 0.01f, 100.0f);
        view = CGL_mat4_look_at(CGL_vec3_init(0.0f, 8.0f, 12.0f), CGL_vec3_init(player_x, 0.5f, -3.0f + player_y), CGL_vec3_init(0.0f, 1.0f, 0.0f));
        distance_traveled += game_speed;
        CGL_float tt = (curr_time - start_time - 0.2f) / (1.38f - 0.2f);
        game_speed = powf((0.2f + (1.38f - 0.2f) * 2.0f * atanf(tt * CGL_PI * 0.5f) / CGL_PI) * 0.5f, 1.5f);
        if(score < 0) game_speed = 0.0f;
        if((curr_time - last_add_time) >= 0.5f) { add_object(); last_add_time = curr_time; }
        CGL_int obj_type = update_objects(player_x);
        if(obj_type >= 0)
        {
            if(obj_type % 5 == 0) score -= (CGL_int)(distance_traveled * 0.1f);
            else score += 10;
        }
        // scene render pass
        {
            // setup pipeline
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
            CGL_framebuffer_bind(g_buffer);
            CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f);
            CGL_shader_bind(g_resources.diffuse_shader);            
            CGL_shader_set_uniform_mat4(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "projection"), &projection);
            CGL_shader_set_uniform_mat4(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "view"), &view);
            CGL_shader_set_uniform_vec3v(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "lightPos"), 0.0f, 3.0f, 5.0f); // global light
            // render ground
            model = CGL_mat4_scale(5.0f, 1.0f, 1000.0f);
            temp = CGL_mat4_translate(0.0f, -1.0f, 0.0f);
            model = CGL_mat4_mul(model, temp);
            CGL_shader_set_uniform_mat4(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "model"), &model);
            CGL_shader_set_uniform_vec3v(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "objectColor"), 0.5f * intensity, 0.5f * intensity, 0.5f * intensity);
            CGL_mesh_gpu_render(g_resources.model);
            // render side walls
            for (int i = 0; i < 25 ;i++)
            {
                CGL_float zdist = i * 10.0f - distance_traveled;
                zdist = -(zdist - floorf(zdist / 250.0f) * 250.0f);
                zdist += 10.0f;
                CGL_float tmp = -i * 10.0f;
                CGL_float height = sinf(tmp * 0.05f) * sinf(tmp * 0.05f) * 3.0f + 1.5f;

                CGL_shader_set_uniform_vec3v(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "objectColor"), 0.5f * intensity, 0.5f * intensity, 0.5f * intensity);                
                model = CGL_mat4_translate(-5.5f, -1.0f, zdist);
                temp = CGL_mat4_scale(1.0f, height, 1.0f);
                model = CGL_mat4_mul(model, temp);
                CGL_shader_set_uniform_mat4(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "model"), &model);
                CGL_mesh_gpu_render(g_resources.model);

                model = CGL_mat4_translate(5.5f, -1.0f, zdist);
                temp = CGL_mat4_scale(1.0f, height, 1.0f);
                model = CGL_mat4_mul(model, temp);
                CGL_shader_set_uniform_mat4(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "model"), &model);
                CGL_mesh_gpu_render(g_resources.model);

                CGL_shader_set_uniform_vec3v(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "objectColor"), 1.2f * intensity, 1.0f * intensity, 0.3f * intensity);
                model =  CGL_mat4_translate(5.5f, height, zdist); 
                temp = CGL_mat4_scale(0.2f, 0.2f, 0.2f);
                model = CGL_mat4_mul(model, temp);
                CGL_shader_set_uniform_mat4(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "model"), &model);
                CGL_mesh_gpu_render(g_resources.model);

                model =  CGL_mat4_translate(-5.5f, height, zdist); 
                temp = CGL_mat4_scale(0.2f, 0.2f, 0.2f);
                model = CGL_mat4_mul(model, temp);
                CGL_shader_set_uniform_mat4(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "model"), &model);
                CGL_mesh_gpu_render(g_resources.model);

            }

            render_objects();

            // render the player
            model = CGL_mat4_translate(player_x, 0.5f, 0.0f);
            CGL_shader_set_uniform_mat4(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "model"), &model);
            CGL_shader_set_uniform_vec3v(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "objectColor"), 0.2f * intensity, 1.0f * intensity, 0.2f * intensity);
            CGL_mesh_gpu_render(g_resources.model);
        }
        // post process pass
        {
            CGL_toon_post_processor_process_shades(CGL_framebuffer_get_color_attachment(g_buffer, 0), CGL_framebuffer_get_color_attachment(g_buffer, 0), CGL_framebuffer_get_color_attachment(g_buffer, 1), 3);
            CGL_toon_post_processor_process_outline(CGL_framebuffer_get_color_attachment(g_buffer, 0), CGL_framebuffer_get_color_attachment(g_buffer, 0), CGL_framebuffer_get_color_attachment(g_buffer, 2), CGL_framebuffer_get_depth_texture(g_buffer), 0.5f);
            CGL_bloom_apply(bloom, CGL_framebuffer_get_color_attachment(g_buffer, 0));
        }
        // presentation pass
        {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            CGL_framebuffer_bind(default_framebuffer);
            CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
            CGL_shader_bind(g_resources.presentation_shader);
            CGL_texture_bind(CGL_framebuffer_get_color_attachment(g_buffer, 0), 0);
            CGL_shader_set_uniform_int(g_resources.presentation_shader, CGL_shader_get_uniform_location(g_resources.presentation_shader, "texture_scene"), 0);
            CGL_gl_render_screen_quad();
            CGL_widgets_begin();
            CGL_widgets_set_fill_colorf(0.8f, 0.8f, 0.6f, 1.0f);
            static char buffer[256];
            if(score >= 0)
            {
                sprintf(buffer, "Distance : %d", (CGL_int)(distance_traveled * 0.1f));
                CGL_widgets_add_string(buffer, -1.0f, 0.9f, 1.0f, 0.1f);
                sprintf(buffer, "Score : %d", score);
                CGL_widgets_add_string(buffer, -1.0f, 0.8f, 1.0f, 0.1f);
            }
            else
            {
                CGL_widgets_set_fill_colorf(0.9f, 0.2f, 0.2f, 1.0f);
                sprintf(buffer, "Game Over");
                CGL_widgets_add_string(buffer, -0.7f, 0.0f, 1.4f, 0.5f);
                sprintf(buffer, "Distance : %d", (CGL_int)(distance_traveled * 0.1f));
                CGL_widgets_add_string(buffer, -0.6f, -0.7f, 1.2f, 0.3f);
            }
            CGL_widgets_end();
        }
        CGL_window_poll_events(main_window);
        CGL_window_swap_buffers(main_window);        
        if(score >=0)
        {
            if(CGL_window_get_key(main_window, CGL_KEY_UP) == CGL_PRESS) player_y -= 0.3f;
            if(CGL_window_get_key(main_window, CGL_KEY_DOWN) == CGL_PRESS) player_y += 0.3f;
            player_y = CGL_utils_clamp(player_y, -3.5f, 3.5f);
            if(CGL_window_get_key(main_window, CGL_KEY_LEFT) == CGL_PRESS) player_x += 0.25f;
            if(CGL_window_get_key(main_window, CGL_KEY_RIGHT) == CGL_PRESS) player_x -= 0.25f;
            player_x = CGL_utils_clamp(player_x, -3.5f, 3.5f);
            intensity = CGL_utils_max(0.0f, intensity);
        }
        else
        {
            if(CGL_window_get_key(main_window, CGL_KEY_R) == CGL_PRESS)
            {
                clear_all();
                start_time = curr_time;
                distance_traveled = 0.0f;
                score = 0;
            }
        }
        if(CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }
    free_resources();
    CGL_bloom_destroy(bloom);
    CGL_toon_post_processor_shutdown();
    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(g_buffer);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(main_window);
    CGL_shutdown();
    return 0;
}
