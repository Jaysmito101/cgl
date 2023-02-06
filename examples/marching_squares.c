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
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_IMPLEMENTATION
#include "cgl.h"

static CGL_window* window;
static CGL_shape shapes[1000];
static int shape_count;
static CGL_vec2 mpos;
static CGL_vec2 spheres[16];

const char* __VS_S = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"out vec2 Position;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(position.xy, 0.0f, 1.0f);\n"
"	Position = position.xy;\n"
"}";

const char* __FS_S = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec2 Position;\n"
"\n"
"uniform vec2 c_pos;\n"
"\n"
"void main()\n"
"{\n"
"	vec4 color = vec4(1.0f, 1.0f, 1.0f , 1.0f);\n"
"	FragColor = color;\n"
"}";


float value_func(float x, float y, float r)
{
    //if((x * x + y * y) > (r * r)) return 0.0f;
    return r / (x * x + y * y);
}

bool sample_function(CGL_vec2 pos, float* value, void* user_data)
{
    float v = 0.0f;
    for (int i = 0; i < 32; i++)
    {
        float x = pos.x - spheres[i].x;
        float y = pos.y - spheres[i].y;
        v += value_func(x, y, 0.35f);
    }
    float sigma = 0.5f;
    if(CGL_window_get_mouse_button(window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS) sigma = 2.0f;
    v += value_func(pos.x - mpos.x, pos.y - mpos.y, sigma);
    if(value) *value = v;
    return v > 45.0f;
}

int main(int argc, char** argv, char** envp)
{
    srand((uint32_t)time(NULL));
    CGL_init();
    window = CGL_window_create(700, 700, "CGL Square Marching - Jaysmito Mukherjee");
    if(window == NULL) return false; 
    CGL_window_make_context_current(window); 
    CGL_gl_init();
    CGL_widgets_init();
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);
    CGL_shader* shd = CGL_shader_create(__VS_S, __FS_S, NULL);

    for(int i = 0; i < 32; i++)
    {
        spheres[i].x = CGL_utils_random_float() * 2.0f - 1.0f;
        spheres[i].y = CGL_utils_random_float() * 2.0f - 1.0f;
    }

    CGL_square_marcher* marcher = CGL_square_marcher_create();
    CGL_mesh_gpu* mesh = CGL_mesh_gpu_create();
    CGL_vec2 start = CGL_vec2_init(-1.0f, -1.0f);
    CGL_vec2 end = CGL_vec2_init(1.0f, 1.0f);

    while(!CGL_window_should_close(window)) 
    {
        int wx, wy;
        CGL_window_get_framebuffer_size(window, &wx, &wy);
        CGL_vec2 window_size = CGL_vec2_init((float)wx, (float)wy);
        double mx, my;
        CGL_window_get_mouse_position(window, &mx, &my);
        mpos = CGL_vec2_init((float)mx / window_size.x * 2.0f - 1.0f, (float)(1.0f - my / window_size.y) * 2.0f - 1.0f);

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);


        CGL_widgets_begin();

   
        CGL_mesh_cpu* temp = CGL_square_marcher_generate_mesh(marcher, sample_function, start, end, 256, 256);
        CGL_widgets_end();
        if(temp)
        {
            CGL_mesh_gpu_upload(mesh, temp, false);
            CGL_mesh_cpu_destroy(temp);
        }
        

        CGL_shader_bind(shd);
        CGL_mesh_gpu_render(mesh);
        
        if(CGL_window_get_key(window, CGL_KEY_I) == CGL_PRESS) CGL_square_marcher_enable_interpolation(marcher, true);
        if(CGL_window_get_key(window, CGL_KEY_J) == CGL_PRESS) CGL_square_marcher_enable_interpolation(marcher, false);
        if(CGL_window_get_key(window, CGL_KEY_SPACE) == CGL_PRESS) 
        {
            for(int i = 0; i < 32; i++)
            {
                spheres[i].x = CGL_utils_random_float() * 2.0f - 1.0f;
                spheres[i].y = CGL_utils_random_float() * 2.0f - 1.0f;
            }
        }

        if(CGL_window_get_key(window, CGL_KEY_ESCAPE) == CGL_PRESS) break;

        CGL_window_swap_buffers(window);
        CGL_window_poll_events(window);
    }


    CGL_shader_destroy(shd);
    CGL_mesh_gpu_destroy(mesh);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
    return EXIT_SUCCESS;
}
