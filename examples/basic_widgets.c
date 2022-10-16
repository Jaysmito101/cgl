#define CGL_EXCLUDE_NETWORKING
#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#include "cgl.h"

#define ASPECT_RATIO 1.0f // an aspect ratio on 1.0 will be needed since our sudoku game board will be a square

static struct
{
    CGL_window* window; // the main window
    int window_height;
    int window_width;

    CGL_framebuffer* framebuffer;
} g_context;

void key_callback(CGL_window* window, int key, int scancode, int action, int mods)
{
    if(action == CGL_PRESS)
    {
    }
    else if(action == CGL_RELEASE)
    {
    }
}


bool setup_window()
{
    g_context.window_height = 400;
    g_context.window_width = (int)(g_context.window_height * ASPECT_RATIO);
    g_context.window = CGL_window_create(g_context.window_width, g_context.window_height, "Tests - Jaysmito Mukherjee"); // create the window
    if(g_context.window == NULL) return false; // window creation failed
    CGL_window_make_context_current(g_context.window); // make the opengl context for the window current
    CGL_window_set_key_callback(g_context.window, key_callback);
    return true; // window was successfully created
}

bool setup_framebuffer()
{
    g_context.framebuffer = CGL_framebuffer_create_from_default(g_context.window); // load the default framebuffer (0) into CGL_framebuffer object
    if(g_context.framebuffer == NULL) return false; // failed
    return true; // success
}

int main()
{
    srand((uint32_t)time(NULL));
    if(!CGL_init()) return EXIT_FAILURE; // initialize CGL (required for setting up internals of CGL)
    if(!setup_window()) return EXIT_FAILURE; //setup window
    if(!CGL_gl_init()) return EXIT_FAILURE; // initialize cgl opengl module    
    if(!setup_framebuffer()) return EXIT_FAILURE; //setup framebuffer
    if(!CGL_widgets_init()) return EXIT_FAILURE; 

    // the main loop
    while(!CGL_window_should_close(g_context.window)) // run till the close button is clicked
    {
        // rendering
        CGL_framebuffer_bind(g_context.framebuffer); // bind default framebuffer and also adjust viewport size and offset
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f); // clear screen with a dark gray color

        CGL_widgets_begin();
        
        float offx = -0.4f;
        float offy = -0.4f;
        srand(42);
        
        for(;offx <= 0.4 ; offx += 0.1f, offy += 0.1f)
        {
            CGL_color color = CGL_utils_random_color();
            CGL_widgets_set_fill_color(color.x, color.y, color.z, 1.0f);
            CGL_widgets_add_triangle(   
                CGL_vec3_init(-0.5f + offx, -0.5f + offy, 0.0f),
                CGL_vec3_init( 0.5f + offx, -0.5f + offy, 0.0f),
                CGL_vec3_init( 0.0f + offx,  0.5f + offy, 0.0f)
                );
        }
        
        
        CGL_widgets_end();

        CGL_window_swap_buffers(g_context.window); // swap framebuffers
        CGL_window_poll_events(g_context.window); // poll events (if this is not called every frame window will stop responding)

        if(CGL_window_get_key(g_context.window, CGL_KEY_ESCAPE) == CGL_PRESS) break; // quit on pressing escape
    }

    // cleanup
    CGL_widgets_shutdown();
    CGL_framebuffer_destroy(g_context.framebuffer); // destory framebuffer object
    CGL_gl_shutdown(); // shutdown cgl opengl module
    CGL_window_destroy(g_context.window); // destroy window
    CGL_shutdown(); // shutdown cgl and clean up resources allocated by CGL internally (if any)
    return EXIT_SUCCESS;
}

