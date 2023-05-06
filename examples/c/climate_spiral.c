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
#define CGL_IMPLEMENTATION
#include "cgl.h"

static const char* TRAIL_VERTEX_SHADER = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n" // w is lifespan
"layout (location = 1) in vec4 normal;\n" // w is distance
"layout (location = 2) in vec4 texcoord;\n" // zw is reserved for future use
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

static const char* TRAIL_FRAGMENT_SHADER = "#version 430 core\n"
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


int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(700, 700, "Climate Spiral - Jaysmito Mukherjee");
    CGL_window_make_context_current(window); 
    CGL_gl_init();
    CGL_widgets_init();
    if(window == NULL) return false; 
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);

    CGL_shader* trail_shader = CGL_shader_create(TRAIL_VERTEX_SHADER, TRAIL_FRAGMENT_SHADER, NULL);
    CGL_trail* trail = CGL_trail_create();
    CGL_trail_set_min_points_distance(trail, 0.01f);
    CGL_trail_set_resolution(trail, 32);

    CGL_csv* weather_data = CGL_csv_create(8);
    // data source: https://data.giss.nasa.gov/gistemp/tabledata_v4/GLB.Ts+dSST.csv
    if (!CGL_csv_load(weather_data, "GLB.Ts+dSST.csv", ","))
    {
		CGL_error("Failed to load weather data");
		return 1;
	}

    CGL_mat4 projection = CGL_mat4_perspective(CGL_deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    CGL_mat4 view = CGL_mat4_identity();
    CGL_mat4 rotate_y_pi_4 = CGL_mat4_rotate_y(CGL_PI_2);

    CGL_vec3 trail_tip = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    CGL_vec3 trail_color = CGL_vec3_init(1.0f, 0.0f, 0.0f);
    CGL_bool is_playing = CGL_FALSE;    
    CGL_int frame_number = 0, current_year = 0, current_month = 0, next_month = 1, next_month_year = 0, temp = 0;
    CGL_float current_theta = 0.0f, delta_time = 0.15f, base_radius = 2.0f, actual_radius = 1.0f, tip_height = 0.0f, temp_f = 0.0f;
    CGL_float current_month_temp = 0.0f, next_month_temp = 0.0f, current_temp = 0.0f;
    while(!CGL_window_should_close(window)) 
    {
        frame_number++;
        CGL_window_set_size(window, 700, 700);

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.002f, 0.002f, 0.002f, 1.0f);

        
        glDisable(GL_DEPTH_TEST);
        CGL_widgets_begin();
        static CGL_byte tmp_buffer[256];
        // sprintf(tmp_buffer, "Curr Year: %d", current_year); CGL_widgets_add_string(tmp_buffer, -1.0f, 0.9f, 0.6f, 0.05f);
        // sprintf(tmp_buffer, "Next Year: %d", next_month_year); CGL_widgets_add_string(tmp_buffer, -1.0f, 0.85f, 0.6f, 0.05f);
        // sprintf(tmp_buffer, "Curr Month: %s", CGL_csv_get_item(weather_data, 0, current_month + 1, NULL)); CGL_widgets_add_string(tmp_buffer, -1.0f, 0.80f, 0.6f, 0.05f);
        // sprintf(tmp_buffer, "Next Month: %s", CGL_csv_get_item(weather_data, 0, next_month + 1, NULL)); CGL_widgets_add_string(tmp_buffer, -1.0f, 0.75f, 0.6f, 0.05f);
        // sprintf(tmp_buffer, "Temperature: %.2f", current_temp); CGL_widgets_add_string(tmp_buffer, -1.0f, 0.70f, 0.6f, 0.05f);

        CGL_widgets_set_fill_mode(false); CGL_widgets_set_stroke_colorf(1.0f, 0.7f, 0.0f, 1.0f); CGL_widgets_set_stroke_thickness(0.01f);

        // the 0 degree circle
        CGL_widgets_add_circle2fr(0, 0, 0.485f, 64);

        // the +1 degree circle
        CGL_widgets_add_circle2fr(0, 0, 0.485f + 0.245f, 64);

        // the -1 degree circle
        CGL_widgets_add_circle2fr(0, 0, 0.485f - 0.245f, 64);
        
        CGL_widgets_set_fill_mode(true); CGL_widgets_set_fill_colorf(1.0f, 0.7f, 0.0f, 1.0f); CGL_widgets_set_stroke_thickness(0.01f);
        CGL_widgets_add_string("0\'C", -0.1f, 0.5f, 0.2f, 0.05f);
        CGL_widgets_add_string("+1\'C", -0.1f, 0.5f + 0.245f, 0.2f, 0.05f);
        CGL_widgets_add_string("-1\'C", -0.1f, 0.5f - 0.245f, 0.2f, 0.05f);

        // the months
        CGL_float delta_out = 0.15f;
        for (CGL_int i = 0 ; i < 12 ; i++)
        {
            sprintf(tmp_buffer, "%s", CGL_csv_get_item(weather_data, 0, i + 1, NULL));
            CGL_widgets_add_string(tmp_buffer, cosf(CGL_deg_to_rad(90.0f - i * 30.0f)) * (0.485f + 0.245f + delta_out) - 0.05f, sinf(CGL_deg_to_rad(90.0f - i * 30.0f)) * (0.485f + 0.245f + delta_out) - 0.025f, 0.2f, 0.1f);
        }

        // the current year
        if (current_temp < 0.0f) trail_color = CGL_vec3_lerp(CGL_vec3_init(1.0f, 1.0f, 1.0f), CGL_vec3_init(0.0f, 0.0f, 1.0f), -current_temp);
        else trail_color = CGL_vec3_lerp(CGL_vec3_init(1.0f, 1.0f, 1.0f), CGL_vec3_init(1.0f, 0.0f, 0.0f), current_temp);
        CGL_widgets_set_fill_colorf(trail_color.x, trail_color.y, trail_color.z, 1.0f);
        CGL_widgets_add_string(CGL_csv_get_item(weather_data, current_year + 1, 0, NULL), -0.185f, -0.08f, 0.4f, 0.16f);

        CGL_widgets_end();

        // view = CGL_mat4_look_at(CGL_vec3_init(5.0f, 5.0f, 5.0f), CGL_vec3_init(0.0f, 0.0f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));
        // NOTE: here the 0.01f is there to aboud the angle between camera front and up being 0.0f and causing a division by zero
        view = CGL_mat4_look_at(CGL_vec3_init(0.0f, 10.0f, 0.001f), CGL_vec3_init(0.0f, 0.0f, 0.0f), CGL_vec3_init(0.0f, 1.0f, 0.0f));
        view = CGL_mat4_mul(view, rotate_y_pi_4); // rotate the camera by 45 degrees around the y axis
        
        // get the ccurrent month index that is the current month index is the floor of the current theta
        // divided by 2PI multiplied by 12 as there are 12 months mapped to 2PI 
        current_month = (CGL_int)(floorf((current_theta / CGL_2PI) * 12.0f)) % 12;


        // get next month index
        next_month = (current_month + 1); 
        
        // if the next month is greater than 11 then the next month is in the next year 
        // so choose the next year as the current year + 1 else the next year is the current year
        if (next_month > 11) next_month_year = current_year + 1; else next_month_year = current_year; 

        // map the next month index to the range 0 to 11
        next_month = next_month % 12;

        // the data is from January 1880 to March 2023 so if the next month is April 2023 then stop playing
        if (next_month == 3 && next_month_year == 2023 - 1880) is_playing = false;

        // get the current month and next month temperature
        if(is_playing)
        {
            current_month_temp = (CGL_float)atof(CGL_csv_get_item(weather_data, current_year + 1, current_month + 1, NULL));
            next_month_temp = (CGL_float)atof(CGL_csv_get_item(weather_data, next_month_year + 1, next_month + 1, NULL));
        }
        
        // get the current temperature by lerping between the current month and next month temperature
        current_temp = CGL_utils_lerp(current_month_temp, next_month_temp, modff((current_theta / CGL_2PI) * 12.0f, &temp_f));

        // calculate the actual radius of the trail tip by adding the base radius to the current temperature
        actual_radius = base_radius + CGL_utils_map(current_temp, -1.0f, 1.0f, -1.0f, 1.0f);
        
        // calculate the trail tip position
        trail_tip = CGL_vec3_init(actual_radius * cosf(current_theta), tip_height, actual_radius * sinf(current_theta));
        
        // update the current theta and tip height
        if(is_playing) current_theta += delta_time;
        // tip_height += delta_time * 0.01f;
        if (current_theta > CGL_2PI) { current_theta = 0.0f;  current_year++; }

        // add the trail tip to the trail and update the trail and bake the trail mesh        
        if(is_playing)
        {
            CGL_trail_add_point(trail, trail_tip, 6000.0f, 0.02f);
            CGL_trail_update(trail, delta_time);
            CGL_trail_bake_mesh(trail);
        }

        if (CGL_window_is_key_pressed(window, CGL_KEY_SPACE)) 
        {
            current_theta = 0.0f;
            current_year = 0;
            CGL_trail_clear(trail);
            is_playing = CGL_TRUE;
        }

        glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
        CGL_shader_bind(trail_shader);
        CGL_trail_render(trail, &view, &projection, trail_shader);

        CGL_window_swap_buffers(window);
        CGL_window_poll_events(window);

    }
    CGL_csv_destroy(weather_data);
    CGL_trail_destroy(trail);
    CGL_shader_destroy(trail_shader);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
    return EXIT_SUCCESS;
}
