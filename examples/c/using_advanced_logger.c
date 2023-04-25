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
