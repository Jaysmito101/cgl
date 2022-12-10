// Copyright Jaysmito Mukherjee, 2023
// I am the sole copyright owner of this Work.
// You cannot host, display, distribute or share this Work neither
// as it is or altered, here on Shadertoy or anywhere else, in any
// form including physical and digital. You cannot use this Work in any
// commercial or non-commercial product, website or project. You cannot
// sell this Work and you cannot mint an NFTs of it or train a neural
// network with it without permission. I share this Work for educational
// purposes, and you can link to it, through an URL, proper attribution
// and unmodified screenshot, as part of your educational material. If
// these conditions are too restrictive please contact me and we'll
// definitely work it out.
// email : jaysmito101@gmail.com


#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_IMPLEMENTATION
#include "cgl.h"

int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(512, 512, "CGL Text Widgets - Jaysmito Mukherjee");
    CGL_window_make_context_current(window); 
    CGL_gl_init();
    CGL_widgets_init();
    if(window == NULL) return false; 
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);

    CGL_float aspect_ratio = 16.0f / 9.0f;
    CGL_int wx = 0, wy = 0;
    CGL_vec2 points[4];

    CGL_int selected_point = 0;
    CGL_int line_resolution = 3;
    points[0] = CGL_vec2_init(-0.8f, 0.0f);
    points[1] = CGL_vec2_init(0.8f, 0.0f);
    points[2] = CGL_vec2_init(0.0f, 0.5f);
    points[3] = CGL_vec2_init(0.0f, -0.5f);
    static CGL_byte buffer[25];
    static CGL_int frame_number = 0;
    

    while(!CGL_window_should_close(window)) 
    {
        frame_number++;
        CGL_window_set_size(window, 512, 512);
        CGL_window_get_framebuffer_size(window, &wx, &wy);
        aspect_ratio = (CGL_float)wx / (CGL_float)wy;

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);

        CGL_widgets_begin();

        for(CGL_int i = 0; i < 4; i++)
        {
            CGL_widgets_set_fill_colorf(0.9f, 0.8f, 0.2f, 1.0f);
            if(i == selected_point) CGL_widgets_add_circle2f(points[i].x, points[i].y, 0.06f);
            if(i < 2) CGL_widgets_set_fill_colorf(0.2f, 0.8f, 0.2f, 1.0f);
            else CGL_widgets_set_fill_colorf(0.8f, 0.2f, 0.2f, 1.0f);
            CGL_widgets_add_circle2f(points[i].x, points[i].y, 0.05f);
        }

        CGL_widgets_set_stroke_colorf(0.8f, 0.7f, 0.3f, 1.0f);
        CGL_widgets_set_stroke_thicnkess(0.01f);
        CGL_widgets_add_cubic_bazier2v(points[0], points[1], points[2], points[3], line_resolution);
        CGL_widgets_add_cubic_bazier_points2v(points[0], points[1], points[2], points[3], line_resolution);

        CGL_widgets_set_fill_colorf(0.8f, 0.8f, 0.8f, 1.0f);
        sprintf(buffer, "Line Resolution: %d", line_resolution);
        CGL_widgets_add_string(buffer, -1.0f, 0.9f, 1.0f, 0.1f);

        static CGL_int old_frame_number = 0;
        static CGL_int delta = 0;
        delta = 0;
        if(CGL_window_get_key(window, CGL_KEY_UP) == CGL_PRESS) delta = 1;
        if(CGL_window_get_key(window, CGL_KEY_DOWN) == CGL_PRESS) delta = -1;
        if(delta != 0 && (frame_number - old_frame_number) > 30)
        {
            old_frame_number = frame_number;
            line_resolution += delta;
            line_resolution = CGL_utils_clamp(line_resolution, 3, 100);
        }

        double mpx, mpy;
        CGL_window_get_mouse_position(window, &mpx, &mpy);
        mpx = mpx / (double)wx * 2.0 - 1.0;
        mpy = 2.0f - mpy / (double)wy * 2.0 - 1.0;
        selected_point = -1;
        for(CGL_int i = 0 ; i < 4 ; i++) if(sqrt(pow(mpx - points[i].x, 2) + pow(mpy - points[i].y, 2)) < 0.05f) selected_point = i;            

        if(selected_point != -1)
        {
            if(CGL_window_get_mouse_button(window, CGL_MOUSE_BUTTON_LEFT) == CGL_PRESS)
            {
                points[selected_point].x = (CGL_float)mpx;
                points[selected_point].y = (CGL_float)mpy;
            }
        }


        CGL_widgets_end();

        CGL_window_swap_buffers(window);
        CGL_window_poll_events(window);

    }

    CGL_framebuffer_destroy(default_framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
    return EXIT_SUCCESS;
}
