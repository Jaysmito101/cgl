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

#define CGL_EXCLUDE_NETWORKING
#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_EXCLUDE_AUDIO
#define CGL_IMPLEMENTATION
#include "cgl.h"


#define EPSILON 0.001f
#define POINT_RADIUS 0.02f
#define CURVE_SAMPLE_POINTS 1000

static struct
{
	CGL_window* window; // window object
	CGL_framebuffer* framebuffer; // framebuffer object
	CGL_float eq_corfficients[12];
	CGL_bool is_solving;
	CGL_float past_sample_points[CURVE_SAMPLE_POINTS][2];
	CGL_int past_sample_points_count;
	CGL_float current_point;
	CGL_float current_point_percent;
	CGL_float next_point;
} g_context; // global context

CGL_float plot_function(float x)
{
	CGL_float y = 0.0f;
	for(CGL_int i = 0; i < 4; i++) y += g_context.eq_corfficients[i] * powf(x * 1.0f, (CGL_float)i);
	return y * 0.2f;
}

CGL_void calculate_next_point()
{
	CGL_float x = g_context.current_point;
	CGL_float y = plot_function(x);
	CGL_float y0 = plot_function(x - EPSILON);
	CGL_float y1 = plot_function(x + EPSILON);
	CGL_float slope = (y1 - y0) / (2.0f * EPSILON);
	g_context.next_point = x - y / slope;
}

CGL_int main()
{
	srand((uint32_t)time(NULL)); // seed the random number generator
	if(!CGL_init()) return EXIT_FAILURE; // initialize CGL (required for setting up internals of CGL)
	g_context.window = CGL_window_create(700, 700, "Newton's Approximate Solver - Jaysmito Mukherjee"); // create the window
	CGL_window_make_context_current(g_context.window); // make the opengl context for the window current
	if(!CGL_gl_init()) return EXIT_FAILURE; // initialize cgl opengl module    
	g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window); // load the default framebuffer (0) into CGL_framebuffer object
	g_context.is_solving = false; g_context.past_sample_points_count = 0; g_context.current_point = 0.0f; g_context.current_point_percent = 0.0f;
	CGL_widgets_init(); // initialize the widgets module
	CGL_float curr_time = CGL_utils_get_time(); // get the current time
	CGL_int frame_count = 0, current_player_sprite = 0, closest_pipe_index = 0, score = 0, last_scored_pipe = -1; // initialize the frame count, current player sprite, closest pipe index, score and last scored pipe index
	while(!CGL_window_should_close(g_context.window)) // run till the close button is clicked
	{
			frame_count++; curr_time = CGL_utils_get_time();
			CGL_window_set_size(g_context.window, 700, 700); // set window size
			CGL_framebuffer_bind(g_context.framebuffer); // bind default framebuffer and also adjust viewport size and offset
			CGL_gl_clear(0.0f, 0.0f, 0.0f, 1.0f); // clear screen with a dark gray color        
			CGL_widgets_begin(); // begin drawing widgets
			CGL_widgets_set_stroke_thicnkess(0.01f);
			CGL_widgets_set_stroke_colorf(0.5f, 0.7f, 0.5f, 1.0f);
			CGL_widgets_add_line2f(-1.0f, 0.0f, 1.0f, 0.0f); // x axis
			CGL_widgets_add_line2f(0.0f, -1.0f, 0.0f, 1.0f); // y axis
			if(g_context.is_solving)
			{
				g_context.current_point_percent += 0.01f;
				// draw the past sample points and tangent lines
				for (CGL_int i = 0 ; i < g_context.past_sample_points_count ; i++)
				{
					CGL_widgets_set_fill_colorf(0.8f, 0.5f, 0.6f, 1.0f);  
					CGL_widgets_add_circle2fr(g_context.past_sample_points[i][0], plot_function(g_context.past_sample_points[i][0]), POINT_RADIUS * 0.7f, 8);
					CGL_widgets_add_circle2fr(g_context.past_sample_points[i][0], 0.0f, POINT_RADIUS * 0.7f, 8);
					CGL_widgets_set_stroke_colorf(0.5f, 0.9f, 0.9f, 1.0f); CGL_widgets_set_stroke_thicnkess(0.004f);
					CGL_widgets_add_line2f(g_context.past_sample_points[i][0], plot_function(g_context.past_sample_points[i][0]), g_context.past_sample_points[i][0], 0.0f);
					CGL_widgets_set_stroke_colorf(0.8f, 0.8f, 0.5f, 1.0f); CGL_widgets_set_stroke_thicnkess(0.005f);
					CGL_widgets_add_line2f(g_context.past_sample_points[i][0], plot_function(g_context.past_sample_points[i][0]), g_context.past_sample_points[i][1], 0.0f);
				}
				// draw the current point
				CGL_widgets_set_fill_colorf(0.5f, 0.5f, 0.8f, 1.0f);
				CGL_widgets_add_circle2fr(g_context.current_point, plot_function(g_context.current_point), POINT_RADIUS, 8);
				CGL_widgets_add_circle2fr(g_context.current_point, 0.0f, POINT_RADIUS, 8);
				CGL_widgets_set_stroke_colorf(0.8f, 0.8f, 0.8f, 1.0f); CGL_widgets_set_stroke_thicnkess(0.006f);
				CGL_widgets_add_line2f(g_context.current_point, 0.0f, g_context.current_point, plot_function(g_context.current_point));
				// animate the current tangent line
				CGL_widgets_set_stroke_colorf(0.5f, 0.8f, 0.8f, 1.0f); CGL_widgets_set_stroke_thicnkess(0.01f); 
				CGL_vec3 start = CGL_vec3_init(g_context.current_point, plot_function(g_context.current_point), 0.0f);
				CGL_vec3 end_original = CGL_vec3_init(g_context.next_point, 0.0f, 0.0f);
				CGL_vec3 end = CGL_vec3_lerp(start, end_original, g_context.current_point_percent);
				CGL_widgets_add_line(start, end);
				if(g_context.current_point_percent >= 1.0f)
				{
					g_context.past_sample_points[g_context.past_sample_points_count][0] = g_context.current_point;
					g_context.past_sample_points[g_context.past_sample_points_count++][1] = g_context.next_point;
					g_context.current_point = g_context.next_point;
					g_context.current_point_percent = 0.0f;
					calculate_next_point();
				}
			}
			CGL_widgets_set_fill_colorf(0.8f, 0.8f, 0.8f, 1.0f); // set the fill color to light gray
			CGL_widgets_set_stroke_colorf(0.7f, 0.5f, 0.5f, 1.0f); // set the stroke color to slight red
			CGL_widgets_set_stroke_thicnkess(0.005f); // set the stroke thickness to 0.005 to draw the curve
			CGL_vec2 prev_point = CGL_vec2_init(0.0f, 0.0f);  // initialize the previous point to 0, 0
			for(CGL_int i = 0; i < CURVE_SAMPLE_POINTS + 10 ; i++) // draw the curve +10 points to make sure the curve is drawn completely
			{
					CGL_float x = -1.0f + (2.0f / (CGL_float)CURVE_SAMPLE_POINTS) * (CGL_float)i; // calculate the x value for the point
					CGL_float y = plot_function(x); // evaluate the plot function at the x value
					//CGL_float y = CGL_linear_regression_evaluate(g_context.lr_context, &x, NULL); // evaluate the linear regression model at the x value
					if(i % (CGL_int)(CURVE_SAMPLE_POINTS * 0.1f) == 0) CGL_widgets_add_circle2fr(x, y, POINT_RADIUS * 0.5f, 4); // draw a small circle at every 10% of the curve (to prevent clutter)
					if(i > 0) CGL_widgets_add_line2f(prev_point.x, prev_point.y, x, y); // draw a line from the previous point to the current point to draw the curve
					prev_point = CGL_vec2_init(x, y); // set the previous point to the current point
			}
			CGL_widgets_end(); // end the widgets frame
			if(CGL_window_is_key_pressed(g_context.window, CGL_KEY_R)) { g_context.is_solving = false; for (CGL_int i = 0; i < 12; i++) g_context.eq_corfficients[i] = CGL_utils_random_float_in_range(-4.0f, 4.0f); CGL_utils_sleep(100); }
			if(CGL_window_is_key_pressed(g_context.window, CGL_KEY_S)) 
			{
				g_context.past_sample_points_count = 0;
				g_context.current_point = CGL_utils_random_float_in_range(-1.0f, 1.0f);
				g_context.current_point_percent = 0.0f;
				calculate_next_point();
				g_context.is_solving = true;
				CGL_utils_sleep(100);
			}
			if(CGL_window_is_key_pressed(g_context.window, CGL_KEY_ESCAPE)) break;
			CGL_window_swap_buffers(g_context.window); // swap framebuffers
			CGL_window_poll_events(g_context.window); // poll events (if this is not called every frame window will stop responding)
	}
	CGL_widgets_shutdown();
	CGL_framebuffer_destroy(g_context.framebuffer); // destory framebuffer object
	CGL_gl_shutdown(); // shutdown cgl opengl module
	CGL_window_destroy(g_context.window); // destroy window
	CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
	return EXIT_SUCCESS;
}
