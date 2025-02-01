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

#define CGL_TRAIL_MAX_POINTS 1024 * 1024
#define CGL_EXCLUDE_NETWORKING
#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_AUDIO
#define CGL_EXCLUDE_TEXT_RENDER 
#define CGL_IMPLEMENTATION
#include "cgl.h"

static const char* TRAIL_VERTEX_SHADER =
#ifdef CGL_WASM
"#version 300 es\n"
"precision highp float;\n"
"precision highp int;\n"
"\n"
"in vec4 position;\n" // w is lifespan
"in vec4 normal;\n" // w is distance
"in vec4 texcoord;\n" // zw is reserved for future use
#else
"#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n" // w is lifespan
"layout (location = 1) in vec4 normal;\n" // w is distance
"layout (location = 2) in vec4 texcoord;\n" // zw is reserved for future use
#endif
"\n"
"out vec3 Position;\n"
"out vec2 TexCoord;\n"
"out float Disturbance;\n"
"\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * vec4(position.xyz, 1.0f);\n"
"	Position = position.xyz;\n"
"	TexCoord = texcoord.xy;\n"
"   Disturbance = length(position.xz);\n"
"}\n";

static const char* TRAIL_FRAGMENT_SHADER = 
#ifdef CGL_WASM
"#version 300 es\n"
"precision highp float;\n"
"precision highp int;\n"
#else  
"#version 430 core\n"
#endif
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Position;\n"
"in vec2 TexCoord;\n"
"in float Disturbance;\n"
"\n"
"void main()\n"
"{\n"
"   vec3 color = vec3(0.0f);\n"
"   float d = Disturbance - 2.0f;\n"
"   // disturnabce is in range [-1, 1] -> map it to colour blue to red\n" 
"   if (d < 0.0f) color = mix(vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f), -d);\n"
"   else color = mix(vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), d);\n"
"	FragColor = vec4(color, 1.0f);\n"
"}\n";

#ifdef CGL_WASM
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else 
#define EM_BOOL int
#endif

struct {
    CGL_window* window;
    CGL_framebuffer* framebuffer;
    CGL_shader* trail_shader;
    CGL_trail* trail;
    CGL_csv* weather_data;
    CGL_mat4 projection;
    CGL_mat4 view;
    CGL_mat4 rotate_y_pi_4;
    CGL_vec3 trail_tip;
    CGL_vec3 trail_color;
    CGL_bool is_playing;
    CGL_int frame_number;
    CGL_int current_year;
    CGL_int current_month;
    CGL_int next_month;
    CGL_int next_month_year;
    CGL_float current_theta;
    CGL_float delta_time;
    CGL_float base_radius;
    CGL_float actual_radius;
    CGL_float tip_height;
    CGL_float temp_f;
    CGL_float current_month_temp;
    CGL_float next_month_temp;
    CGL_float current_temp;
    CGL_float last_frame_time;   // Add this new field
} g_State;

CGL_bool init() {
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return CGL_FALSE;

    // Initialize window and OpenGL context
    g_State.window = CGL_window_create(700, 700, "Climate Spiral - Jaysmito Mukherjee");
    CGL_window_make_context_current(g_State.window);
    if(!CGL_gl_init()) return CGL_FALSE;
    CGL_widgets_init();
    
    // Create framebuffer and resources
    g_State.framebuffer = CGL_framebuffer_create_from_default(g_State.window);
    g_State.trail_shader = CGL_shader_create(TRAIL_VERTEX_SHADER, TRAIL_FRAGMENT_SHADER, NULL);
    g_State.trail = CGL_trail_create();
    CGL_trail_set_min_points_distance(g_State.trail, 0.01f);
    CGL_trail_set_resolution(g_State.trail, 32);

    // Load weather data
    g_State.weather_data = CGL_csv_create(8);
    if (!CGL_csv_load(g_State.weather_data, "./assets/GLB.Ts+dSST.csv", ",")) {
        CGL_error("Failed to load weather data");
        return CGL_FALSE;
    }

    // Initialize matrices and other state variables
    g_State.projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    g_State.view = CGL_mat4_identity();
    g_State.rotate_y_pi_4 = CGL_mat4_rotate_y(CGL_PI_2);
    g_State.trail_tip = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    g_State.trail_color = CGL_vec3_init(1.0f, 0.0f, 0.0f);
    g_State.is_playing = CGL_FALSE;
    g_State.frame_number = 0;
    g_State.current_year = 0;
    g_State.current_month = 0;
    g_State.next_month = 1;
    g_State.current_theta = 0.0f;
    g_State.delta_time = 0.15f;
    g_State.base_radius = 2.0f;
    g_State.actual_radius = 1.0f;
    g_State.last_frame_time = CGL_utils_get_time();
    g_State.delta_time = 0.0f;

    return CGL_TRUE;
}

void cleanup() {
    CGL_csv_destroy(g_State.weather_data);
    CGL_trail_destroy(g_State.trail);
    CGL_shader_destroy(g_State.trail_shader);
    CGL_framebuffer_destroy(g_State.framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(g_State.window);
    CGL_shutdown();
}

EM_BOOL loop(double time, void* userData) {
    (void)time;
    (void)userData;

    // Calculate delta time
    CGL_float current_time = CGL_utils_get_time();
    g_State.delta_time = current_time - g_State.last_frame_time;
    g_State.last_frame_time = current_time;

    g_State.frame_number++;
    CGL_window_set_size(g_State.window, 700, 700);

    CGL_framebuffer_bind(g_State.framebuffer);
    CGL_gl_clear(0.002f, 0.002f, 0.002f, 1.0f);

    glDisable(GL_DEPTH_TEST);
    CGL_widgets_begin();
    static CGL_byte tmp_buffer[256];

    CGL_widgets_set_fill_mode(false); 
    CGL_widgets_set_stroke_colorf(1.0f, 0.7f, 0.0f, 1.0f); 
    CGL_widgets_set_stroke_thickness(0.01f);

    // the 0 degree circle
    CGL_widgets_add_circle2fr(0, 0, 0.485f, 64);

    // the +1 degree circle
    CGL_widgets_add_circle2fr(0, 0, 0.485f + 0.245f, 64);

    // the -1 degree circle
    CGL_widgets_add_circle2fr(0, 0, 0.485f - 0.245f, 64);
    
    CGL_widgets_set_fill_mode(true); 
    CGL_widgets_set_fill_colorf(1.0f, 0.7f, 0.0f, 1.0f); 
    CGL_widgets_set_stroke_thickness(0.01f);
    CGL_widgets_add_string("0\'C", -0.1f, 0.5f, 0.2f, 0.05f);
    CGL_widgets_add_string("+1\'C", -0.1f, 0.5f + 0.245f, 0.2f, 0.05f);
    CGL_widgets_add_string("-1\'C", -0.1f, 0.5f - 0.245f, 0.2f, 0.05f);

    // the months
    CGL_float delta_out = 0.15f;
    for (CGL_int i = 0 ; i < 12 ; i++)
    {
        sprintf(tmp_buffer, "%s", CGL_csv_get_item(g_State.weather_data, 0, i + 1, NULL));
        CGL_widgets_add_string(tmp_buffer, cosf(CGL_deg_to_rad(90.0f - i * 30.0f)) * (0.485f + 0.245f + delta_out) - 0.05f, sinf(CGL_deg_to_rad(90.0f - i * 30.0f)) * (0.485f + 0.245f + delta_out) - 0.025f, 0.2f, 0.1f);
    }

    // the current year
    if (g_State.current_temp < 0.0f) g_State.trail_color = CGL_vec3_lerp(CGL_vec3_init(1.0f, 1.0f, 1.0f), CGL_vec3_init(0.0f, 0.0f, 1.0f), -g_State.current_temp);
    else g_State.trail_color = CGL_vec3_lerp(CGL_vec3_init(1.0f, 1.0f, 1.0f), CGL_vec3_init(1.0f, 0.0f, 0.0f), g_State.current_temp);
    CGL_widgets_set_fill_colorf(g_State.trail_color.x, g_State.trail_color.y, g_State.trail_color.z, 1.0f);
    CGL_widgets_add_string(CGL_csv_get_item(g_State.weather_data, g_State.current_year + 1, 0, NULL), -0.185f, -0.08f, 0.4f, 0.16f);

    CGL_widgets_end();

    // NOTE: here the 0.01f is there to aboud the angle between camera front and up being 0.0f and causing a division by zero
    g_State.view = CGL_mat4_look_at(CGL_vec3_init(0.0f, 10.0f, 0.001f), CGL_vec3_init(0.0f, 0.0f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));
    g_State.view = CGL_mat4_mul(g_State.view, g_State.rotate_y_pi_4); // rotate the camera by 45 degrees around the y axis
    
    // get the ccurrent month index that is the current month index is the floor of the current theta
    // divided by 2PI multiplied by 12 as there are 12 months mapped to 2PI 
    g_State.current_month = (CGL_int)(floorf((g_State.current_theta / CGL_2PI) * 12.0f)) % 12;

    // get next month index
    g_State.next_month = (g_State.current_month + 1); 
    
    // if the next month is greater than 11 then the next month is in the next year 
    // so choose the next year as the current year + 1 else the next year is the current year
    if (g_State.next_month > 11) g_State.next_month_year = g_State.current_year + 1; else g_State.next_month_year = g_State.current_year; 

    // map the next month index to the range 0 to 11
    g_State.next_month = g_State.next_month % 12;

    // the data is from January 1880 to March 2023 so if the next month is April 2023 then stop playing
    if (g_State.next_month == 3 && g_State.next_month_year == 2023 - 1880) g_State.is_playing = false;

    // get the current month and next month temperature
    if(g_State.is_playing)
    {
        g_State.current_month_temp = (CGL_float)atof(CGL_csv_get_item(g_State.weather_data, g_State.current_year + 1, g_State.current_month + 1, NULL));
        g_State.next_month_temp = (CGL_float)atof(CGL_csv_get_item(g_State.weather_data, g_State.next_month_year + 1, g_State.next_month + 1, NULL));
    }
    
    // get the current temperature by lerping between the current month and next month temperature
    g_State.current_temp = CGL_utils_lerp(g_State.current_month_temp, g_State.next_month_temp, modff((g_State.current_theta / CGL_2PI) * 12.0f, &g_State.temp_f));

    // calculate the actual radius of the trail tip by adding the base radius to the current temperature
    g_State.actual_radius = g_State.base_radius + CGL_utils_map(g_State.current_temp, -1.0f, 1.0f, -1.0f, 1.0f);
    
    // calculate the trail tip position
    g_State.trail_tip = CGL_vec3_init(g_State.actual_radius * cosf(g_State.current_theta), g_State.tip_height, g_State.actual_radius * sinf(g_State.current_theta));
    
    // update the current theta and tip height
    // Adjust the speed multiplier (0.5f) to control animation speed
    if(g_State.is_playing) {
        g_State.current_theta += 5.0f * g_State.delta_time;
    }
    if (g_State.current_theta > CGL_2PI) { g_State.current_theta = 0.0f;  g_State.current_year++; }

    // add the trail tip to the trail and update the trail and bake the trail mesh        
    if(g_State.is_playing)
    {
        CGL_trail_add_point(g_State.trail, g_State.trail_tip, 6000.0f, 0.02f);
        CGL_trail_update(g_State.trail, g_State.delta_time);
        CGL_trail_bake_mesh(g_State.trail);
    }

    if (CGL_window_is_key_pressed(g_State.window, CGL_KEY_SPACE)) 
    {
        g_State.current_theta = 0.0f;
        g_State.current_year = 0;
        CGL_trail_clear(g_State.trail);
        g_State.is_playing = CGL_TRUE;
    }

    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    CGL_shader_bind(g_State.trail_shader);
    CGL_trail_render(g_State.trail, &g_State.view, &g_State.projection, g_State.trail_shader);

    CGL_window_swap_buffers(g_State.window);
    CGL_window_poll_events(g_State.window);

    return !CGL_window_should_close(g_State.window);
}

int main() {
    if(!init()) return 1;

#ifdef CGL_WASM
    CGL_info("Running in WASM mode");
    emscripten_request_animation_frame_loop(loop, NULL);
#else
    while (!CGL_window_should_close(g_State.window)) {
        loop(0, NULL);
    }
    cleanup();
#endif
    return 0;
}
