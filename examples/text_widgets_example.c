#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_IMPLEMENTATION
#include "cgl.h"


int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(640, 320, "CGL Text Widgets - Jaysmito Mukherjee");
    CGL_window_make_context_current(window); 
    CGL_gl_init();
    CGL_widgets_init();
    if(window == NULL) return false; 
    CGL_framebuffer* framebuffer = CGL_framebuffer_create_from_default(window);


    while(!CGL_window_should_close(window)) 
    {
        CGL_framebuffer_bind(framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);

        CGL_widgets_begin();
        CGL_widgets_add_string("C G L", -0.5f, -0.5f, 1.0f, 1.0f);
        CGL_widgets_end();

        CGL_window_swap_buffers(window);
        CGL_window_poll_events(window);
    }

    
    CGL_framebuffer_destroy(framebuffer);
    CGL_widgets_shutdown();
    CGL_gl_shutdown();
    CGL_window_destroy(window);
    CGL_shutdown();
    return EXIT_SUCCESS;
}
