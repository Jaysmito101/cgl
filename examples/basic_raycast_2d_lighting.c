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

const char* __VS_S = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"uniform vec2 tri_pos[3];\n"
"\n"
"out vec2 Position;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(tri_pos[gl_VertexID], 0.0f, 1.0f);\n"
"	Position = tri_pos[gl_VertexID];\n"
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
"   float alpha = 1.0f - pow((Position.x - c_pos.x)*1.2f, 2.0f) - pow((Position.y - c_pos.y)*1.2f, 2.0f);\n"
"   alpha *= exp(-2.0f * (pow(Position.x - c_pos.x, 2.0f) + pow(Position.y - c_pos.y, 2.0f)));\n"
"	vec4 color = vec4(1.0f, 1.0f, 1.0f , clamp(alpha, 0.0f, 0.7f));\n"
"	FragColor = color;\n"
"}";

static struct {
    CGL_vec2 a;
    CGL_vec2 b;
    CGL_vec2 c;
    CGL_vec2 position;
    float rotation;
    float total_rotation;
} triangle;

static CGL_vec4 walls[1024];
static int wall_count = 0;

static struct 
{
    bool started;
    bool released;
    CGL_vec2 start;
    CGL_vec2 end;
} current_line;

int main(int argc, char** argv, char** envp)
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(700, 700, "CGL Ray Caster - Jaysmito Mukherjee");
    if(window == NULL) return false; 
    CGL_window_make_context_current(window); 
    CGL_gl_init();
    CGL_widgets_init();
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);

    CGL_window_swap_buffers(window);
    CGL_window_poll_events(window);

    triangle.a = CGL_vec2_init(-0.025f, 0.0f);
    triangle.b = CGL_vec2_init(0.025f, 0.0f);
    triangle.c = CGL_vec2_init(0.0f, 0.08f);
    triangle.position = CGL_vec2_init(0.0f, 0.0f);
    triangle.rotation = 0.0f;
    triangle.total_rotation = 0.0f;

    current_line.started = false;
    current_line.released = true;

    CGL_ray_caster* caster = CGL_ray_caster_create();
    float fov = 3.14159265358979323846f / 2.0f;
    
    CGL_shader* shd = CGL_shader_create(__VS_S, __FS_S, NULL);

    while(!CGL_window_should_close(window)) 
    {
        static int frame_count = 0;
        frame_count ++;
        int wx, wy;
        double mxp, myp;
        CGL_window_get_framebuffer_size(window, &wx, &wy);
        CGL_window_get_mouse_position(window, &mxp, &myp);
        float aspect_ratio = (float)wx / wy;
        float mouse_pos_x = (float)mxp / wx;
        float mouse_pos_y = 1.0f - (float)myp / wy;
        mouse_pos_x = mouse_pos_x * 2.0f - 1.0f;
        mouse_pos_y = mouse_pos_y * 2.0f - 1.0f;    

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);

        CGL_widgets_begin();

        CGL_widgets_set_fill_colorf(0.3f, 0.7f, 0.4f, 1.0f);
        CGL_widgets_add_triangle(
            CGL_vec3_init(triangle.a.x - triangle.position.x, triangle.a.y - triangle.position.y, 0.0f),
            CGL_vec3_init(triangle.b.x - triangle.position.x, triangle.b.y - triangle.position.y, 0.0f),
            CGL_vec3_init(triangle.c.x - triangle.position.x, triangle.c.y - triangle.position.y, 0.0f));
        
        CGL_widgets_set_stroke_thicnkess(0.02f);
        CGL_widgets_set_stroke_colorf(0.1f, 0.1f, 0.1f, 1.0f);
        for(int i = 0; i < wall_count; i++)
        {
            CGL_widgets_add_line(
                CGL_vec3_init(walls[i].x, walls[i].y, 0.0f),
                CGL_vec3_init(walls[i].z, walls[i].w, 0.0f));
        }

        if(current_line.started)
        {
            CGL_widgets_set_stroke_colorf(0.7f, 0.7f, 0.3f, 1.0f);
            CGL_widgets_add_line(
                CGL_vec3_init(current_line.start.x, current_line.start.y, 0.0f),
                CGL_vec3_init(mouse_pos_x, mouse_pos_y, 0.0f));
        }        

        if(CGL_window_get_key(window, CGL_KEY_R) == CGL_PRESS)
        {
            CGL_vec2 centroid = CGL_vec2_centroid_of_triangle(triangle.a, triangle.b, triangle.c);
            CGL_vec2 dir = CGL_vec2_sub(triangle.c, centroid);
            CGL_vec2_normalize(dir);
            CGL_vec2 inter_pt;
            CGL_widgets_set_stroke_thicnkess(0.01f);
            CGL_widgets_set_stroke_colorf(0.7f, 0.3f, 0.3f, 1.0f);
            if(CGL_ray_caster_get_intersection_point_for_walls(CGL_vec2_sub(triangle.c, triangle.position), dir, walls, wall_count, &inter_pt, NULL, 1.0f) > 0.0f)
            {
                CGL_widgets_add_line(
                    CGL_vec3_init(triangle.c.x - triangle.position.x, triangle.c.y - triangle.position.y, 0.0f),
                    CGL_vec3_init(inter_pt.x, inter_pt.y, 0.0f));
            }
        }

        CGL_ray_caster_set_angle_limits(caster, 3.14f / 2.0f - fov / 2.0f, 3.14f / 2.0f + fov / 2.0f); // for future
        CGL_ray_caster_calculate(caster, CGL_vec2_sub(triangle.c, triangle.position), -triangle.total_rotation, (CGL_window_get_key(window, CGL_KEY_T) == CGL_PRESS));
        
        glDisable(GL_BLEND);
        
        CGL_widgets_end();    

        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        CGL_shader_bind(shd);
        int tri_count = 0;
        CGL_shape_triangle* triangles = CGL_ray_caster_get_triangles(caster, &tri_count);
        CGL_vec2 c_pos = CGL_vec2_sub(triangle.c, triangle.position);
        CGL_shader_set_uniform_vec2v(shd, CGL_shader_get_uniform_location(shd, "c_pos"), c_pos.x, c_pos.y);
        for(int i = 0 ; i < tri_count ; i++)
        {
            CGL_shader_set_uniform_vec2v(shd, CGL_shader_get_uniform_location(shd, "tri_pos[0]"), triangles[i].a.x, triangles[i].a.y);
            CGL_shader_set_uniform_vec2v(shd, CGL_shader_get_uniform_location(shd, "tri_pos[1]"), triangles[i].b.x, triangles[i].b.y);
            CGL_shader_set_uniform_vec2v(shd, CGL_shader_get_uniform_location(shd, "tri_pos[2]"), triangles[i].c.x, triangles[i].c.y);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        
        CGL_window_swap_buffers(window);
        CGL_window_poll_events(window);



        bool rotChanged = false;
        if(CGL_window_get_key(window, CGL_KEY_E) == CGL_PRESS) { triangle.rotation = -0.003f; rotChanged = true; }
        if(CGL_window_get_key(window, CGL_KEY_Q) == CGL_PRESS) { triangle.rotation = 0.003f; rotChanged = true; }
        if(CGL_window_get_key(window, CGL_KEY_A) == CGL_PRESS) { fov += 0.003f; }
        if(CGL_window_get_key(window, CGL_KEY_D) == CGL_PRESS) { fov -= 0.003f;  }
        if(CGL_window_get_key(window, CGL_KEY_W) == CGL_PRESS) { CGL_vec2 centroid = CGL_vec2_centroid_of_triangle(triangle.a, triangle.b, triangle.c); centroid = CGL_vec2_sub(triangle.c, centroid); centroid = CGL_vec2_scale(centroid, 0.009f); triangle.position = CGL_vec2_sub(triangle.position, centroid); }
        if(CGL_window_get_key(window, CGL_KEY_S) == CGL_PRESS) { CGL_vec2 centroid = CGL_vec2_centroid_of_triangle(triangle.a, triangle.b, triangle.c); centroid = CGL_vec2_sub(triangle.c, centroid); centroid = CGL_vec2_scale(centroid, 0.009f); triangle.position = CGL_vec2_add(triangle.position, centroid); }
        if(rotChanged)
        {
            triangle.total_rotation -= triangle.rotation;
            CGL_vec2 centroid = CGL_vec2_centroid_of_triangle(triangle.a, triangle.b, triangle.c);
            triangle.a = CGL_vec2_rotate_about_point(triangle.a, centroid, triangle.rotation);
            triangle.b = CGL_vec2_rotate_about_point(triangle.b, centroid, triangle.rotation);
            triangle.c = CGL_vec2_rotate_about_point(triangle.c, centroid, triangle.rotation);
        }
        if(CGL_window_get_mouse_button(window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS) 
        {
            if(!current_line.started)
            {
                current_line.started = true;
                current_line.start = CGL_vec2_init(mouse_pos_x, mouse_pos_y);
            }
        }
        else if(CGL_window_get_mouse_button(window, CGL_MOUSE_BUTTON_LEFT) == CGL_RELEASE) 
        {
            if(current_line.started)
            {
                current_line.started = false;
                current_line.end = CGL_vec2_init(mouse_pos_x, mouse_pos_y);
                walls[wall_count++] = CGL_vec4_init(current_line.start.x, current_line.start.y, current_line.end.x, current_line.end.y);
                CGL_ray_caster_add_wall(caster, walls[wall_count - 1]);
            }
        }
        if(CGL_window_get_key(window, CGL_KEY_C) == CGL_PRESS) { wall_count = 0; CGL_ray_caster_clear_walls(caster); }
    }

    CGL_ray_caster_destroy(caster);
    CGL_shader_destroy(shd);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
    return EXIT_SUCCESS;
}
