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
"\n"
"uniform sampler2D texture_diffuse;\n"
"\n"
"void main()\n"
"{\n"
"    //FragColor = vec4(1.0);return;\n"
"    vec3 lightColor = vec3(1.0, 1.0, 1.0);\n"
"    vec3 objectColor = pow(texture(texture_diffuse, TexCoords).rgb, vec3(2.2f));\n"
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
"    gl_Position = vec4(aPos.xy, 0.0f, 1.0f);\n"
"}\n";

static const char* FS_presentation = "#version 430 core\n"
"in vec2 TexCoords;\n"
"\n"
"// Filmic Tonemapping Operators http://filmicworlds.com/blog/filmic-tonemapping-operators/\n"
"vec3 filmic(vec3 x) {\n"
"  vec3 X = max(vec3(0.0), x - 0.004);\n"
"  vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);\n"
"  return pow(result, vec3(2.2));\n"
"}\n"
"out vec4 FragColor;\n"
"\n"
"uniform sampler2D texture_scene;\n"
"\n"
"void main()\n"
"{\n"
"    vec2 uv = vec2(TexCoords.y, 1.0f - TexCoords.x);\n"
"    vec3 scene = texture(texture_scene, uv).rgb;\n"
"    scene = filmic(scene);\n"
"    FragColor = vec4(scene, 1.0);\n"
"}\n";

static struct
{
    CGL_texture* tex[3];
    CGL_mesh_gpu* model[3];
    CGL_int selected_model;
    CGL_shader* diffuse_shader;
    CGL_shader* presentation_shader;
} g_resources;

CGL_texture* load_texture(const char* filename)
{
    CGL_image img = {0};
    img.bytes_per_channel = 8;
    stbi_set_flip_vertically_on_load(true);
    img.data = stbi_load(filename, &img.width, &img.height, &img.channels, 0);
    if (!img.data)
    {
        CGL_error("Failed to load image: %s", filename);
        return NULL;
    }
    CGL_texture* tex = CGL_texture_create(&img);
    stbi_image_free(img.data);
    return tex;
}

void load_resources()
{
    // NOTE: I dont own the models so I cant redistribute them so if you are trying to run this example you will have to find your own models and textures to use

    // load the bulbasaur model
    g_resources.tex[0] = load_texture("./assets/bulba.jpg");
    CGL_mesh_cpu* bulba_model_cpu = CGL_mesh_cpu_load_obj("./assets/bulba.obj");
    g_resources.model[0] = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(g_resources.model[0], bulba_model_cpu, true);
    CGL_mesh_cpu_destroy(bulba_model_cpu);

    // load the charmander model
    g_resources.tex[1] = load_texture("./assets/charm.jpg");
    CGL_mesh_cpu* charm_model_cpu = CGL_mesh_cpu_load_obj("./assets/charm.obj");
    g_resources.model[1] = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(g_resources.model[1], charm_model_cpu, true);
    CGL_mesh_cpu_destroy(charm_model_cpu);

    // load the squirtle model
    g_resources.tex[2] = load_texture("./assets/squirt.jpg");
    CGL_mesh_cpu* squirt_model_cpu = CGL_mesh_cpu_load_obj("./assets/squirt.obj");
    g_resources.model[2] = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(g_resources.model[2], squirt_model_cpu, true);
    CGL_mesh_cpu_destroy(squirt_model_cpu);

    // load the shaders
    g_resources.diffuse_shader = CGL_shader_create(VS_diffuse, FS_diffuse, NULL);
    g_resources.presentation_shader = CGL_shader_create(VS_presentation, FS_presentation, NULL);

    g_resources.selected_model = 0;

}

void free_resources()
{
    // free the models and textures
    for (int i = 0; i < 3; ++i)
    {
        CGL_texture_destroy(g_resources.tex[i]);
        CGL_mesh_gpu_destroy(g_resources.model[i]);
    }

    // free the shaders
    CGL_shader_destroy(g_resources.diffuse_shader);
    CGL_shader_destroy(g_resources.presentation_shader);
}

void print_mat4(const char* name, float* m)
{
    printf("%s = \n\t", name);
    for (int i = 0; i < 16; i++)
    {
        printf("%f ", m[i]);
        if ((i+1) % 4 == 0) printf("\n\t");
    }
    printf("\n");
}

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return -1;
    CGL_window* main_window = CGL_window_create(512, 512, "Toon Shading - Jaysmito Mukherjee");
    if(!main_window) return -1;
    CGL_window_make_context_current(main_window);
    if(!CGL_gl_init()) return -1;
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(main_window);   
    CGL_framebuffer* g_buffer = CGL_framebuffer_create_basic(512, 512);
    CGL_framebuffer_add_color_attachment(g_buffer, CGL_texture_create_blank(512, 512, GL_RGBA, GL_RGBA, GL_FLOAT)); // for albedo
    CGL_framebuffer_add_color_attachment(g_buffer, CGL_texture_create_blank(512, 512, GL_RGBA, GL_RGBA, GL_FLOAT)); // for normal
    CGL_toon_post_processor_init();

    CGL_bool toon_shading = false;
    CGL_float outline_width = 1.0f;
    CGL_int shades = 4;
    CGL_mat4 projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    CGL_mat4 view = CGL_mat4_look_at(CGL_vec3_init(0.0f, 5.0f, 10.0f), CGL_vec3_init(0.0f, 1.5f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));
    CGL_mat4 model = CGL_mat4_identity();

    load_resources();

    while(!CGL_window_should_close(main_window))
    { 
        CGL_window_set_size(main_window, 512, 512); // force window size to be 512x512

        
        CGL_mat4 temp;
        model = CGL_mat4_identity();
        temp = CGL_mat4_translate(0.3f, 0.1f, 0.0f);
        model = CGL_mat4_mul(temp, model);
        temp = CGL_mat4_rotate_y( (CGL_PI * (CGL_utils_get_time() * -0.5f)) );
        model = CGL_mat4_mul(temp, model);
        temp = CGL_mat4_scale(0.1f, 0.1f, 0.1f);
        model = CGL_mat4_mul(temp, model);

        // scene render pass
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
            CGL_framebuffer_bind(g_buffer);
            CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
            CGL_shader_bind(g_resources.diffuse_shader);
            CGL_shader_set_uniform_mat4(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "projection"), &projection);
            CGL_shader_set_uniform_mat4(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "view"), &view);
            CGL_shader_set_uniform_mat4(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "model"), &model);
            CGL_shader_set_uniform_vec3v(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "lightPos"), 0.0f, 3.0f, 5.0f);
            CGL_texture_bind(g_resources.tex[g_resources.selected_model], 0);
            CGL_shader_set_uniform_int(g_resources.diffuse_shader, CGL_shader_get_uniform_location(g_resources.diffuse_shader, "texture_diffuse"), 0);
            CGL_mesh_gpu_render(g_resources.model[g_resources.selected_model]);           
        }

        // toon shading pass
        if(toon_shading)
        {
            CGL_toon_post_processor_process_shades(CGL_framebuffer_get_color_attachment(g_buffer, 0), CGL_framebuffer_get_color_attachment(g_buffer, 0), CGL_framebuffer_get_color_attachment(g_buffer, 1), shades);
            CGL_toon_post_processor_process_outline(CGL_framebuffer_get_color_attachment(g_buffer, 0), CGL_framebuffer_get_color_attachment(g_buffer, 0), CGL_framebuffer_get_color_attachment(g_buffer, 2), CGL_framebuffer_get_depth_texture(g_buffer), outline_width);
        }

        // presentation pass
        {
            CGL_framebuffer_bind(default_framebuffer);
            CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
            CGL_shader_bind(g_resources.presentation_shader);
            CGL_texture_bind(CGL_framebuffer_get_color_attachment(g_buffer, 0), 0);
            CGL_shader_set_uniform_int(g_resources.presentation_shader, CGL_shader_get_uniform_location(g_resources.presentation_shader, "texture_scene"), 0);
            CGL_gl_render_screen_quad();
        }

        CGL_window_poll_events(main_window);
        CGL_window_swap_buffers(main_window);
        
        if(CGL_window_get_key(main_window, CGL_KEY_1) == CGL_PRESS) g_resources.selected_model = 0;
        if(CGL_window_get_key(main_window, CGL_KEY_2) == CGL_PRESS) g_resources.selected_model = 1;
        if(CGL_window_get_key(main_window, CGL_KEY_3) == CGL_PRESS) g_resources.selected_model = 2;
        if(CGL_window_get_key(main_window, CGL_KEY_T) == CGL_PRESS) toon_shading = true;
        if(CGL_window_get_key(main_window, CGL_KEY_D) == CGL_PRESS) toon_shading = false;
        if(CGL_window_get_key(main_window, CGL_KEY_UP) == CGL_PRESS) outline_width += 0.01f;
        if(CGL_window_get_key(main_window, CGL_KEY_DOWN) == CGL_PRESS) outline_width -= 0.01f;
        for(int i = 0; i < 10; i++) if(CGL_window_get_key(main_window, CGL_KEY_KP_0 + i) == CGL_PRESS) shades = i;
        if(CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }

    free_resources();

    CGL_toon_post_processor_shutdown();
    CGL_framebuffer_destroy(g_buffer);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(main_window);
    CGL_shutdown();
    return 0;
}
