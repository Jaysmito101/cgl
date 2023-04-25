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
#define CGL_IMPLEMENTATION
#include "cgl.h"

#define POINT_RADIUS 0.02f
#define POINT_COUNT 1000
#define CURVE_SAMPLE_POINTS 1000

static struct
{
    CGL_vec2 points[POINT_COUNT]; // points to be used for linear regression
    CGL_window* window; // window object
    CGL_framebuffer* framebuffer; // framebuffer object
    CGL_int points_count; // number of points 
    CGL_int selected_point; // selected point for dragging and deleting
    CGL_linear_regression_context* lr_context; // linear regression context
} g_context; // global context

// this function will be provided to the linear regression context to sample the data
// this will be used to train the linear regression model using gradient descent
CGL_float sample_function(CGL_void* user_data, CGL_float* input, CGL_float* output, CGL_int id)
{
    if(id < 0 || id >= g_context.points_count) return 0.0f; // invalid id
    input[0] = g_context.points[id].x;
    input[1] = input[0] * input[0];
    input[2] = input[1] * input[0];
    input[3] = input[2] * input[0];
    input[4] = input[3] * input[0];
    return g_context.points[id].y; // return the y value of the point
}

int main()
{
    srand((uint32_t)time(NULL)); // seed the random number generator
    if(!CGL_init()) return EXIT_FAILURE; // initialize CGL (required for setting up internals of CGL)
    g_context.window = CGL_window_create(700, 700, "Polynomial Regression - Jaysmito Mukherjee"); // create the window
    CGL_window_make_context_current(g_context.window); // make the opengl context for the window current
    if(!CGL_gl_init()) return EXIT_FAILURE; // initialize cgl opengl module    
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window); // load the default framebuffer (0) into CGL_framebuffer object
    g_context.points_count = 0; g_context.selected_point = -1; // initialize the number of points to 0 and selected point to -1
    CGL_widgets_init(); // initialize the widgets module
    CGL_float curr_time = CGL_utils_get_time(); // get the current time
    CGL_int frame_count = 0, current_player_sprite = 0, closest_pipe_index = 0, score = 0, last_scored_pipe = -1; // initialize the frame count, current player sprite, closest pipe index, score and last scored pipe index
    g_context.lr_context = CGL_linear_regression_context_create(5); CGL_linear_regression_randomize_coefficents(g_context.lr_context, -0.5f, 0.5f);
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
        CGL_widgets_set_fill_colorf(0.8f, 0.8f, 0.8f, 1.0f); // set the fill color to light gray
        // draw the points as circles
        for(CGL_int i = 0; i < g_context.points_count; i++) { CGL_vec2 p = g_context.points[i]; CGL_widgets_add_circle2fr(p.x, p.y, POINT_RADIUS, 8); }
        CGL_widgets_set_stroke_colorf(0.7f, 0.5f, 0.5f, 1.0f); // set the stroke color to slight red
        CGL_widgets_set_stroke_thicnkess(0.005f); // set the stroke thickness to 0.005 to draw the curve
        CGL_vec2 prev_point = CGL_vec2_init(0.0f, 0.0f);  // initialize the previous point to 0, 0
        for(CGL_int i = 0; i < CURVE_SAMPLE_POINTS + 10 ; i++) // draw the curve +10 points to make sure the curve is drawn completely
        {
            CGL_float x = -1.0f + (2.0f / (CGL_float)CURVE_SAMPLE_POINTS) * (CGL_float)i; // calculate the x value for the point
            CGL_float y = CGL_linear_regression_evaluate(g_context.lr_context, (CGL_float[]){x, x * x, x * x * x, x * x * x * x, x * x * x * x * x}, NULL); // evaluate the linear regression model at the x value
            if(i % (CGL_int)(CURVE_SAMPLE_POINTS * 0.1f) == 0) CGL_widgets_add_circle2fr(x, y, POINT_RADIUS * 0.5f, 4); // draw a small circle at every 10% of the curve (to prevent clutter)
            if(i > 0) CGL_widgets_add_line2f(prev_point.x, prev_point.y, x, y); // draw a line from the previous point to the current point to draw the curve
            prev_point = CGL_vec2_init(x, y); // set the previous point to the current point
        }
        // train the linear regression model if there are more than 0 points as 0 points will cause coeeficents to be blow up
        // NOTE : increase the max iterations to 1000 or more to get faster convergence, here it is set to 10 to make the training slower
        //        so that the user can see the training process in action as the line slowly converges to the points
        if(g_context.points_count > 0) CGL_linear_regression_train(g_context.lr_context, sample_function, NULL, g_context.points_count, 0.01f, 200); 
        CGL_widgets_end(); // end the widgets frame
        if(CGL_window_is_key_pressed(g_context.window, CGL_KEY_R)) CGL_linear_regression_randomize_coefficents(g_context.lr_context, -0.5f, 0.5f); // randomize the coeeficents if R is pressed
        if(CGL_window_is_key_pressed(g_context.window, CGL_KEY_C)) g_context.points_count = 0; // clear the points if C is pressed
        CGL_bool mslft = (CGL_window_get_mouse_button(g_context.window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS), msrht = (CGL_window_get_mouse_button(g_context.window, CGL_MOUSE_BUTTON_RIGHT) == CGL_PRESS);
        if(mslft || msrht)
        {
            CGL_double mx = 0.0, my = 0.0; CGL_bool caught = false;
            CGL_window_get_mouse_position(g_context.window, &mx, &my);
            mx = (mx / 700.0) * 2.0 - 1.0; my = (1.0 - my / 700.0) * 2.0 - 1.0;
            g_context.selected_point = -1;
            for(CGL_int i = 0; i < g_context.points_count; i++)
            {
                CGL_vec2 p = g_context.points[i]; CGL_float tx = (CGL_float)(p.x - mx), ty = (CGL_float)(p.y - my);
                if(sqrtf(tx * tx + ty * ty) < POINT_RADIUS) { g_context.selected_point = i; caught = true; break;}
            }
            if(caught)
            {
                if(mslft) {g_context.points[g_context.selected_point].x = (CGL_float)mx; g_context.points[g_context.selected_point].y = (CGL_float)my;}
                if(msrht) {for(CGL_int i = g_context.selected_point; i < g_context.points_count - 1; i++)g_context.points[i] = g_context.points[i + 1];g_context.points_count--;}
            }
            else if(mslft)
            {
                g_context.points[g_context.points_count].x = (CGL_float)mx; g_context.points[g_context.points_count].y = (CGL_float)my;
                g_context.selected_point = g_context.points_count; g_context.points_count++;
            }
        }
        CGL_window_swap_buffers(g_context.window); // swap framebuffers
        CGL_window_poll_events(g_context.window); // poll events (if this is not called every frame window will stop responding)
    }
    CGL_linear_regression_context_destroy(g_context.lr_context); // destroy the linear regression context
    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(g_context.framebuffer); // destory framebuffer object
    CGL_gl_shutdown(); // shutdown cgl opengl module
    CGL_window_destroy(g_context.window); // destroy window
    CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
    return EXIT_SUCCESS;
}
