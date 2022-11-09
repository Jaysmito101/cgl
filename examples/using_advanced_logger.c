#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_IMPLEMENTATION
#include "cgl.h"


int main()
{
    srand((uint32_t)time(NULL));
    CGL_init();
    CGL_info("This is a info!");
    CGL_warn("This is a warning!");
    CGL_error("This is a error!");
    CGL_trace("This is a trace!");

    CGL_logger_attach_log_file("./log.txt");
    CGL_logger_attach_log_file("./log2.txt");

    CGL_info("This is a info!");
    CGL_warn("This is a warning!");
    CGL_error("This is a error!");
    CGL_trace("This is a trace!");

    CGL_shutdown();
    return EXIT_SUCCESS;
}
