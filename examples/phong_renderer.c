#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#include "cgl.h"


#define MAIN_FRAME_BUFFER_WIDTH  800
#define MAIN_FRAME_BUFFER_HEIGHT (int)(MAIN_FRAME_BUFFER_WIDTH * (9.0f / 16.0f))


CGL_camera* camera;
CGL_vec3 cam_pos;
CGL_vec3 cam_rot;

static struct {
    float delta_x;
    float delta_y;
    float prev_x;
    float prev_y;
} mouse_input;


void input_scroll_callback(CGL_window* window, double x, double y)
{
    CGL_vec3 delta = CGL_camera_get_front(camera);
    delta = CGL_vec3_scale(delta, (float)y);
    cam_pos = CGL_vec3_add(cam_pos, delta);
    CGL_camera_set_position(camera, cam_pos);
}

void input_mouse_pos_callback(CGL_window* window, double x, double y)
{
    mouse_input.delta_x = ((float)x - mouse_input.prev_x);
    mouse_input.delta_y = ((float)y - mouse_input.prev_y);
    mouse_input.prev_x = (float)x;
    mouse_input.prev_y = (float)y;
}

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return -1;
    CGL_window* main_window = CGL_window_create(640, 480, "Hello World");
    if(!main_window) return -1;
    CGL_window_make_context_current(main_window);
    if(!CGL_gl_init()) return -1;
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(main_window);   
    CGL_mesh_cpu* cpu_mesh = CGL_mesh_cpu_load_obj("a.obj");
    CGL_mesh_gpu* mesh = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(mesh, cpu_mesh, true);
    CGL_mesh_cpu_destroy(cpu_mesh);
    mouse_input.delta_x = 0.0f;
    mouse_input.delta_y = 0.0f;
    mouse_input.prev_x = 0.0f;
    mouse_input.prev_y = 0.0f;
    camera = CGL_camera_create();
    CGL_window_set_mouse_scroll_callback(main_window, input_scroll_callback);
    CGL_window_set_mouse_position_callback(main_window, input_mouse_pos_callback);

    CGL_phong_pipeline* pipeline = CGL_phong_pipeline_create();
    CGL_phong_pipeline_add_light(pipeline, CGL_phong_light_directional(CGL_vec3_init(0, -1, 0), CGL_vec3_init(0.8f, 0.8f, 0.8f), 1.0f));

    CGL_phong_mat* material = CGL_phong_mat_create();
    CGL_phong_mat_set_diffuse_color(material, CGL_vec3_init(0.3f, 0.5f, 0.7f));
    CGL_phong_mat_set_specular_color(material, CGL_vec3_init(0.5f, 0.7f, 0.9f));
    CGL_mat4 model_matrix;
    model_matrix = CGL_mat4_translate(0.0f, 0.0f, 0.0f);

    CGL_window_resecure_callbacks(main_window);

    cam_pos = CGL_camera_get_position(camera);
    cam_rot = CGL_camera_get_rotation(camera);
    cam_pos.z += 3.0f;
    CGL_camera_set_position(camera, cam_pos);

    while(!CGL_window_should_close(main_window))
    { 
        model_matrix = CGL_mat4_translate(0, 0, 0);
        {
            CGL_framebuffer_bind(default_framebuffer);
            CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);
            int rx = 0, ry = 0;
            CGL_framebuffer_get_size(default_framebuffer, &rx, &ry);
            CGL_camera_set_aspect_ratio(camera, ((float)rx / ry));
            CGL_camera_recalculate_mat(camera);
            CGL_phong_render_begin(pipeline, camera);
            CGL_phong_render(mesh, &model_matrix, material, pipeline, camera);
            CGL_phong_render_end(pipeline, camera);
        }

        CGL_window_poll_events(main_window);

        CGL_window_swap_buffers(main_window);
        if(CGL_window_get_mouse_button(main_window, CGL_MOUSE_BUTTON_MIDDLE) == CGL_PRESS)
        {            
            CGL_vec3 right = CGL_camera_get_right(camera);
            right = CGL_vec3_scale(right, (0.05f * mouse_input.delta_x));
            CGL_vec3 up = CGL_camera_get_up(camera);
            up = CGL_vec3_scale(up, (-0.05f * mouse_input.delta_y));
            cam_pos = CGL_vec3_add(cam_pos, up);
            cam_pos = CGL_vec3_add(cam_pos, right);
            CGL_camera_set_position(camera, cam_pos);
        }
        mouse_input.delta_x = mouse_input.delta_y = 0.0f;
        if(CGL_window_get_key(main_window, CGL_KEY_ESCAPE) == CGL_PRESS) break;
    }

    CGL_phong_pipeline_destroy(pipeline);
    CGL_phong_mat_destroy(material);

    CGL_camera_destroy(camera);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_mesh_gpu_destroy(mesh);
    CGL_gl_shutdown();
    CGL_window_destroy(main_window);
    CGL_shutdown();
    return 0;
}
