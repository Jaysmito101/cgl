#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_IMPLEMENTATION
#include "cgl.h"

static CGL_window* window;
static CGL_shape shapes[1000];
static int shape_count;

int main(int argc, char** argv, char** envp)
{
    srand((uint32_t)time(NULL));
    CGL_init();
    window = CGL_window_create(700, 700, "CGL 2D Collision Detection - Jaysmito Mukherjee");
    if(window == NULL) return false; 
    CGL_window_make_context_current(window); 
    CGL_gl_init();
    CGL_widgets_init();
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);

    CGL_window_swap_buffers(window);
    CGL_window_poll_events(window);

    bool mode_is_3d = false;

    shape_count = CGL_utils_random_int(20, 35);
    float xo = -0.8f, yo = -0.8f;
    for(int i = 0, j = 3 ; i < shape_count + 1 ; i++)
    {
        int vertex_count = j;
        if(i == shape_count) vertex_count = 5;
        CGL_shape_init(&shapes[i], vertex_count);
        shapes[i].position.z = 0.0f;
        shapes[i].position.x = xo;
        shapes[i].position.y = yo;
        shapes[i].rotation.z = CGL_utils_random_float() * CGL_2PI;
        for(int j = 0 ; j < vertex_count ; j++) shapes[i].vertices[j] = CGL_vec3_init(cosf((CGL_2PI / vertex_count * j)) * 0.15f, sinf((CGL_2PI / vertex_count * j)) * 0.15f, 0.0f);
        xo += 0.4f; if(xo > 0.8f) {xo = -0.8f; yo += 0.5f;} if(i % 4 != 0) j++;
    }
    shapes[shape_count].scale = CGL_vec3_init(0.5f, 0.5f, 0.5f);
    shapes[shape_count].position = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    CGL_bool use_gjk = true;
    CGL_float last_time = CGL_utils_get_time();
    CGL_float curr_time = CGL_utils_get_time();
    CGL_float delta_time = 0.0f;

    while(!CGL_window_should_close(window)) 
    {
        curr_time = CGL_utils_get_time();
        delta_time = curr_time - last_time;
        last_time = curr_time;

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
        CGL_shape* cursor = &shapes[shape_count];
        CGL_widgets_begin();

        CGL_vec2 normal_vector = CGL_vec2_init(0.0f, 0.0f);
        CGL_vec2 normal_vector2 = CGL_vec2_init(0.0f, 0.0f);

        // draw shapes
        for(int i = 0 ; i < shape_count ; i++)
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
            if(use_gjk)
            {
                static CGL_vec3 simplex[3];
                result = CGL_gjk_check_collision_2d(cursor, &shapes[i], simplex);
                if(result)
                {
                    CGL_vec3 tmp = CGL_gjk_epa_2d(cursor, &shapes[i], simplex);
                    normal_vector = CGL_vec2_init(tmp.x, tmp.y);
                }
            }
            else
            {
                result = CGL_sat_collision_detect(cursor, &shapes[i], &normal_vector);
            }

            if(result) normal_vector2 = CGL_vec2_add(normal_vector2, normal_vector);

            if(result) CGL_widgets_set_stroke_colorf(0.8f, 0.1f, 0.1f, 1.0f);
            else CGL_widgets_set_stroke_colorf(0.7f, 0.7f, 0.7f, 1.0f);
            CGL_widgets_set_stroke_thicnkess(0.005f);
            CGL_widgets_add_shape_out_line(&shapes[i]);

            if(result)
            {
                CGL_widgets_set_stroke_thicnkess(0.002f);
                CGL_widgets_set_stroke_colorf(0.5f, 1.0f, 0.5f, 1.0f);
                CGL_widgets_add_line2f(cursor->position.x, cursor->position.y, shapes[i].position.x, shapes[i].position.y);

                CGL_widgets_set_stroke_thicnkess(0.02f);
                CGL_widgets_set_stroke_colorf(1.0f, 0.5f, 0.5f, 1.0f);
                CGL_widgets_add_line2f(cursor->position.x, cursor->position.y, cursor->position.x + normal_vector.x, cursor->position.y + normal_vector.y);
            }


        }

        if(CGL_window_get_key(window, CGL_KEY_R) == CGL_PRESS) 
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
        sprintf(buffer, "Mode: %s", use_gjk ? "GJK" : "SAT");
        CGL_widgets_add_string(buffer, -1.0f, 0.9f, 0.5f, 0.1f);
        sprintf(buffer, "Frame Time: %f", delta_time);
        CGL_widgets_add_string(buffer, -1.0f, 0.8f, 1.0f, 0.1f);


        CGL_widgets_end();

        if(CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) cursor->position.y += 0.001f;
        if(CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) cursor->position.y -= 0.001f;
        if(CGL_window_get_key(window, CGL_KEY_LEFT) == CGL_PRESS) cursor->position.x -= 0.001f;
        if(CGL_window_get_key(window, CGL_KEY_RIGHT) == CGL_PRESS) cursor->position.x += 0.001f;
        if(CGL_window_get_key(window, CGL_KEY_Z) == CGL_PRESS) cursor->rotation.z -= 0.0025f;
        if(CGL_window_get_key(window, CGL_KEY_W) == CGL_PRESS) {cursor->scale.x += 0.0025f;cursor->scale.y += 0.0025f;}
        if(CGL_window_get_key(window, CGL_KEY_S) == CGL_PRESS) {cursor->scale.x -= 0.0025f;cursor->scale.y -= 0.0025f;}
        if(CGL_window_get_key(window, CGL_KEY_G) == CGL_PRESS) use_gjk = true;
        if(CGL_window_get_key(window, CGL_KEY_T) == CGL_PRESS) use_gjk = false;


        if(CGL_window_get_key(window, CGL_KEY_ESCAPE) == CGL_PRESS) break;

        CGL_window_swap_buffers(window);
        CGL_window_poll_events(window);
    }

    for(int i = 0; i <= shape_count; i++) CGL_shape_destroy(&shapes[i]);

    CGL_framebuffer_destroy(default_framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
    return EXIT_SUCCESS;
}
