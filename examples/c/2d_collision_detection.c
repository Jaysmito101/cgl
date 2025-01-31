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
#define CGL_EXCLUDE_AUDIO
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_IMPLEMENTATION
#include "cgl.h"

#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif

#define CGL_MAX_SHAPES 1000

struct {
    CGL_window* window;
    CGL_shape shapes[CGL_MAX_SHAPES];
    CGL_sizei shape_count;
    CGL_framebuffer* framebuffer;

    CGL_bool use_gjk;
    CGL_float last_time;
    CGL_float curr_time;
    CGL_float delta_time;
} g_State;

CGL_bool init() {
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return CGL_FALSE;

    g_State.window = CGL_window_create(700, 700, "CGL 2D Collision Detection - Jaysmito Mukherjee");
    if(g_State.window == NULL) return CGL_FALSE;
    CGL_window_make_context_current(g_State.window);
    if(!CGL_gl_init()) return CGL_FALSE;
    CGL_widgets_init();

    g_State.framebuffer = CGL_framebuffer_create_from_default(g_State.window);

    CGL_window_swap_buffers(g_State.window);
    CGL_window_poll_events(g_State.window);

    g_State.shape_count = CGL_utils_random_int(20, 35);
    float xo = -0.8f, yo = -0.8f;
    for(CGL_sizei i = 0, j = 3 ; i < g_State.shape_count + 1 ; i++)
    {
        CGL_sizei vertex_count = j;
        if(i == g_State.shape_count) vertex_count = 5;
        CGL_shape_init(&g_State.shapes[i], vertex_count);
        g_State.shapes[i].position.z = 0.0f;
        g_State.shapes[i].position.x = xo;
        g_State.shapes[i].position.y = yo;
        g_State.shapes[i].rotation.z = CGL_utils_random_float() * CGL_2PI;
        for(CGL_sizei k = 0 ; k < vertex_count ; k++) g_State.shapes[i].vertices[k] = CGL_vec3_init(cosf((CGL_2PI / vertex_count * k)) * 0.15f, sinf((CGL_2PI / vertex_count * k)) * 0.15f, 0.0f);
        xo += 0.4f; if(xo > 0.8f) {xo = -0.8f; yo += 0.5f;} if(i % 4 != 0) j++;
    }
    g_State.shapes[g_State.shape_count].scale = CGL_vec3_init(0.5f, 0.5f, 0.5f);
    g_State.shapes[g_State.shape_count].position = CGL_vec3_init(0.0f, 0.0f, 0.0f);

    g_State.use_gjk = true;
    g_State.last_time = CGL_utils_get_time();
    g_State.curr_time = CGL_utils_get_time();
    g_State.delta_time = 0.0f;

    return CGL_TRUE;
}

void cleanup() {
    for(CGL_sizei i = 0; i <= g_State.shape_count; i++) CGL_shape_destroy(&g_State.shapes[i]);

    CGL_framebuffer_destroy(g_State.framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(g_State.window);
    CGL_shutdown();   
}



EM_BOOL loop(double time, void* userData) {
    (void)time;
    (void)userData;
    g_State.curr_time = CGL_utils_get_time();
    g_State.delta_time = g_State.curr_time - g_State.last_time;
    g_State.last_time = g_State.curr_time;

    CGL_framebuffer_bind(g_State.framebuffer);
    CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
    CGL_shape* cursor = &g_State.shapes[g_State.shape_count];
    CGL_widgets_begin();
    CGL_vec2 normal_vector = CGL_vec2_init(0.0f, 0.0f);
    CGL_vec2 normal_vector2 = CGL_vec2_init(0.0f, 0.0f);
    // draw shapes
    for(CGL_sizei i = 0 ; i < g_State.shape_count ; i++)
    {
        /*
        // For simple casses where the axes are known
        if(CGL_sat_collision_overlap_on_axis(cursor, &shapes[i], CGL_vec2_init(1.0f, 1.0f), NULL) && CGL_sat_collision_overlap_on_axis(cursor, &shapes[i], CGL_vec2_init(1.0f, -1.0f), NULL))
            CGL_widgets_set_stroke_colorf(0.8f, 0.1f, 0.1f, 1.0f);
        else
            CGL_widgets_set_stroke_colorf(0.1f, 0.1f, 0.1f, 1.0f);
        // you can also get sat axes by using
        CGL_vec2 axes[10000];
        int axis_count = 0;
        CGL_sat_collision_calculate_axes(&shape, axes, &axes_count);
        */
        CGL_bool result = false;
        if(g_State.use_gjk)
        {
            static CGL_vec3 simplex[3];
            result = CGL_gjk_check_collision_2d(cursor, &g_State.shapes[i], simplex);
            if(result)
            {
                CGL_vec3 tmp = CGL_gjk_epa_2d(cursor, &g_State.shapes[i], simplex);
                normal_vector = CGL_vec2_init(tmp.x, tmp.y);
            }
        }
        else
        {
            result = CGL_sat_collision_detect(cursor, &g_State.shapes[i], &normal_vector);
        }
        if(result) normal_vector2 = CGL_vec2_add(normal_vector2, normal_vector);
        if(result) CGL_widgets_set_stroke_colorf(0.8f, 0.1f, 0.1f, 1.0f);
        else CGL_widgets_set_stroke_colorf(0.7f, 0.7f, 0.7f, 1.0f);
        CGL_widgets_set_stroke_thicnkess(0.005f);
        CGL_widgets_add_shape_out_line(&g_State.shapes[i]);
        if(result)
        {
            CGL_widgets_set_stroke_thicnkess(0.002f);
            CGL_widgets_set_stroke_colorf(0.5f, 1.0f, 0.5f, 1.0f);
            CGL_widgets_add_line2f(cursor->position.x, cursor->position.y, g_State.shapes[i].position.x, g_State.shapes[i].position.y);
            CGL_widgets_set_stroke_thicnkess(0.02f);
            CGL_widgets_set_stroke_colorf(1.0f, 0.5f, 0.5f, 1.0f);
            CGL_widgets_add_line2f(cursor->position.x, cursor->position.y, cursor->position.x + normal_vector.x, cursor->position.y + normal_vector.y);
        }
    }
    if(CGL_window_get_key(g_State.window, CGL_KEY_R) == CGL_PRESS) 
    {
        cursor->position.x -= normal_vector2.x;
        cursor->position.y -= normal_vector2.y;
        CGL_utils_sleep(100); // to prevent spamming
    }

    CGL_widgets_set_stroke_thicnkess(0.005f);
    CGL_widgets_set_stroke_colorf(0.5f, 0.8f, 0.5f, 1.0f);
    CGL_widgets_add_shape_out_line(cursor);
    

    static CGL_byte buffer[1000];
    CGL_widgets_set_stroke_colorf(0.8f, 0.8f, 0.8f, 1.0f);
    sprintf(buffer, "Mode: %s", g_State.use_gjk ? "GJK" : "SAT");
    CGL_widgets_add_string(buffer, -1.0f, 0.9f, 0.5f, 0.1f);
    sprintf(buffer, "Frame Time: %f", g_State.delta_time);
    CGL_widgets_add_string(buffer, -1.0f, 0.8f, 1.0f, 0.1f);

    CGL_widgets_set_fill_colorf(0.4f, 0.8f, 0.4f, 1.0f);
    CGL_widgets_add_string("G or T for algorithm", -1.0f, -0.7f, 0.8f, 0.05f);
    CGL_widgets_add_string("W or S for changing scale", -1.0f, -0.8f, 1.0f, 0.05f);
    CGL_widgets_add_string("Z for roatation", -1.0f, -0.9f, 0.8f, 0.05f);
    CGL_widgets_add_string("Arrow keys to move", -1.0f, -1.0f, 1.0f, 0.05f);
    CGL_widgets_end();

    const CGL_float speed = 50.0;    
    if(CGL_window_get_key(g_State.window, CGL_KEY_UP) == CGL_PRESS) cursor->position.y += 0.01f * speed * g_State.delta_time;
    if(CGL_window_get_key(g_State.window, CGL_KEY_DOWN) == CGL_PRESS) cursor->position.y -= 0.01f * speed * g_State.delta_time;
    if(CGL_window_get_key(g_State.window, CGL_KEY_LEFT) == CGL_PRESS) cursor->position.x -= 0.01f * speed * g_State.delta_time;
    if(CGL_window_get_key(g_State.window, CGL_KEY_RIGHT) == CGL_PRESS) cursor->position.x += 0.01f * speed * g_State.delta_time;
    if(CGL_window_get_key(g_State.window, CGL_KEY_Z) == CGL_PRESS) cursor->rotation.z -= 0.025f * speed * g_State.delta_time;
    if(CGL_window_get_key(g_State.window, CGL_KEY_W) == CGL_PRESS) {cursor->scale.x += 0.025f * speed * g_State.delta_time; cursor->scale.y += 0.025f * speed * g_State.delta_time;}
    if(CGL_window_get_key(g_State.window, CGL_KEY_S) == CGL_PRESS) {cursor->scale.x -= 0.025f * speed * g_State.delta_time;cursor->scale.y -= 0.025f * speed * g_State.delta_time;}
    if(CGL_window_get_key(g_State.window, CGL_KEY_G) == CGL_PRESS) g_State.use_gjk = true;
    if(CGL_window_get_key(g_State.window, CGL_KEY_T) == CGL_PRESS) g_State.use_gjk = false;
    if(CGL_window_get_key(g_State.window, CGL_KEY_ESCAPE) == CGL_PRESS) return CGL_FALSE;
    
    CGL_window_swap_buffers(g_State.window);
    CGL_window_poll_events(g_State.window);

    return CGL_TRUE;
}

int main()
{
    if(!init()) return EXIT_FAILURE;

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
