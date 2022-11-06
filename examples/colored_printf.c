#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_IMPLEMENTATION
#include "cgl.h"


int main()
{
    srand((uint32_t)time(NULL));
    printf("Hello!\n");
    CGL_printf_green("Gello!\n");
    printf("Hello!\n");
    CGL_printf_red("Rello!\n");
    printf("Hello!\n");
    CGL_printf_gray("Yello!\n");
    printf("Hello!\n");

    // custom
    CGL_console_set_color(CGL_CONSOLE_COLOR_GRAY);
    printf("Something !\n");
    printf("Another thing !\n");
    CGL_console_set_color(CGL_CONSOLE_COLOR_RESET);
    printf("Hello!\n");

    return EXIT_SUCCESS;
}
