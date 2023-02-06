/*
MIT License

Copyright (c) 2022 Jaysmito Mukherjee (jaysmito101@gmail.com)

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
