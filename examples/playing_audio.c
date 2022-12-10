#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#include "cgl.h"

static CGL_wav_file g_wav_file = {0};
static CGL_bool g_is_playing = false;
static CGL_bool g_is_loaded = false;

void on_drop_file(CGL_window* window, const CGL_byte** paths, CGL_int count)
{
    if(g_is_loaded) CGL_wav_file_destroy(&g_wav_file);
    g_is_loaded = false;
    if(!CGL_wav_file_load(&g_wav_file, paths[0]))
    {
        CGL_error("Failed to load wav file: %s", paths[0]);
        return;
    }
    else CGL_info("Loaded wav file: %s", paths[0]);
    g_is_loaded = true;
    g_is_playing = false;
}

int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_window* window = CGL_window_create(600, 600, "Audio - Jaysmito Mukherjee");
    if(!window) return 1;
    CGL_window_make_context_current(window);
    CGL_gl_init();
    CGL_window_set_drag_n_drop_callback(window, on_drop_file);
    CGL_framebuffer* default_framebuffer = CGL_framebuffer_create_from_default(window);
    
    /*
    const CGL_byte* available_devices = CGL_audio_get_device_list();
    const CGL_byte* next_device = available_devices + 1;
    CGL_info("Available devices:");
    CGL_int ct = 1, len = 0;
    while(available_devices && *available_devices != '\0' && next_device && *next_device != '\0')
    {
        CGL_info("%d. %s", ct++, available_devices);
        len = strlen(available_devices);
        available_devices += len + 1;
        next_device += len + 2;
    }

   CGL_info("Default device : %s", CGL_audio_get_default_device());
    */

   CGL_audio_context* audio_context = CGL_audio_context_create(NULL);
   CGL_audio_make_context_current(audio_context);

   CGL_audio_source* audio_source = CGL_audio_source_create();
   CGL_audio_buffer* audio_buffer = CGL_audio_buffer_create();
    

    while(!CGL_window_should_close(window))
    {
        CGL_window_set_size(window, 600, 600); // force window size to be 600x600

        CGL_framebuffer_bind(default_framebuffer);
        CGL_gl_clear(0.2f, 0.2f, 0.2f, 1.0f);

        if(g_is_loaded && !g_is_playing)
        {
            if(CGL_audio_source_is_playing(audio_source)) CGL_audio_source_stop(audio_source);
            CGL_audio_buffer_set_data_from_wav_file(audio_buffer, &g_wav_file);
            CGL_audio_source_set_buffer(audio_source, audio_buffer);
            CGL_audio_source_play(audio_source);
            g_is_playing = true;
            CGL_info("Started Playing");
        }

        if(g_is_loaded && g_is_playing)
        {
            if(!CGL_audio_source_is_playing(audio_source))
            {
                CGL_info("Finished playing");
                CGL_wav_file_destroy(&g_wav_file);
                g_is_loaded = false;
                g_is_playing = false;
            }
        }

        CGL_window_poll_events(window);
        CGL_window_swap_buffers(window);
    }


    CGL_audio_buffer_destroy(audio_buffer);
    CGL_audio_source_destroy(audio_source);
    CGL_audio_make_context_current(NULL);
    CGL_audio_context_destroy(audio_context);
    if(g_is_loaded) CGL_wav_file_destroy(&g_wav_file);
    CGL_framebuffer_destroy(default_framebuffer);
    CGL_gl_shutdown();
    CGL_window_destroy(window);    
    CGL_shutdown();
    return 0;
}
