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

// Many of CGL's features are implemented using macros.
// Thus to make them available in the wrapper, here
// we create proxy functions that call the macros.

#define CGL_EXCLUDE_NETWORKING
#define CGL_LOGGING_ENABLED
#define CGL_EXCLUDE_AUDIO
#define CGL_IMPLEMENTATION
#include "cgl.h"


CGL_bool CGL_utils_is_point_in_rect_MACRO(CGL_float px, CGL_float py, CGL_float x, CGL_float y, CGL_float sx, CGL_float sy, CGL_float scx, CGL_float scy) {
    return CGL_utils_is_point_in_rect(px, py, x, y, sx, sy, scx, scy);
}

CGL_float CGL_utils_random_float_MACRO() 
{
    return CGL_utils_random_float();
}

CGL_float CGL_utils_random_float_in_range_MACRO(CGL_float min, CGL_float max) 
{
    return CGL_utils_random_float_in_range(min, max);
}

CGL_int CGL_utils_random_int_MACRO(CGL_int min, CGL_int max) 
{
    return CGL_utils_random_int(min, max);
}

CGL_bool CGL_utils_random_bool_MACRO()
{
    return CGL_utils_random_bool();
}

CGL_vec2 CGL_utils_random_vec2_MACRO(CGL_vec2 min, CGL_vec2 max) 
{
    return CGL_utils_random_vec2(min, max);
}

CGL_vec3 CGL_utils_random_vec3_MACRO(CGL_vec3 min, CGL_vec3 max) 
{
    return CGL_utils_random_vec3(min, max);
}

CGL_vec4 CGL_utils_random_vec4_MACRO(CGL_vec4 min, CGL_vec4 max) 
{
    return CGL_utils_random_vec4(min, max);
}

CGL_color CGL_utils_random_color_MACRO() 
{
    return CGL_utils_random_color();
}

CGL_mat4 CGL_mat4_zero_MACRO()
{
    return CGL_mat4_zero();
}

CGL_mat4 CGL_mat4_identity_MACRO()
{
    return CGL_mat4_identity();
}

CGL_mat4 CGL_mat4_scale_MACRO(CGL_float sx, CGL_float sy, CGL_float sz)
{
    return CGL_mat4_scale(sx, sy, sz);
}

CGL_mat4 CGL_mat4_translate_MACRO(CGL_float tx, CGL_float ty, CGL_float tz)
{
    return CGL_mat4_translate(tx, ty, tz);
}

CGL_mat4 CGL_mat4_rotate_x_MACRO(CGL_float angle)
{
    return CGL_mat4_rotate_x(angle);
}

CGL_mat4 CGL_mat4_rotate_y_MACRO(CGL_float angle)
{
    return CGL_mat4_rotate_y(angle);
}

CGL_mat4 CGL_mat4_rotate_z_MACRO(CGL_float angle)
{
    return CGL_mat4_rotate_z(angle);
}

CGL_mat4 CGL_mat4_perspective_MACRO(CGL_float fov, CGL_float aspect, CGL_float near_, CGL_float far_)
{
    return CGL_mat4_perspective(fov, aspect, near_, far_);
}

CGL_mat4 CGL_mat4_orthographic_MACRO(CGL_float left, CGL_float right, CGL_float bottom, CGL_float top, CGL_float near_, CGL_float far_)
{
    return CGL_mat4_orthographic(left, right, bottom, top, near_, far_);
}

CGL_void CGL_mat4_log_MACRO(CGL_mat4 mat)
{
    CGL_mat4_log(mat);
}