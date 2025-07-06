#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_TEXT_RENDER
#define CGL_EXCLUDE_GRAPHICS_API
#define CGL_IMPLEMENTATION
#include "cgl.h"

int main()
{
    printf("Hello!\n");
    CGL_printf_green("Gello!\n");
    return 0;
}
