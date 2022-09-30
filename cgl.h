/*

MIT License

Copyright (c) 2022 Jaysmito Mukherjee

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

#ifndef CGL_H
#define CGL_H

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

// common
#if 1 // Just to use code folding

// std includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>
#include <errno.h>

#ifdef CGL_LOGGING_ENABLED
#define CGL_LOG(...) printf(__VA_ARGS__)
#else
#define CGL_LOG(...)
#endif // CGL_LOG_ENABLED

struct CGL_context;
typedef struct CGL_context CGL_context;

bool CGL_init(); // initialize CGL
void CGL_shutdown(); // shutdown CGL

#endif

// utils
#if 1
// CGL utils

char* CGL_utils_read_file(const char* path, size_t* size); // read file into memory
bool CGL_utils_write_file(const char* path, const char* data, size_t size); // write data to file

#define CGL_utils_random_float() ((float)rand() / (float)RAND_MAX)
#define CGL_utils_random_int(min, max) (rand() % (max - min + 1) + min)
#define CGL_utils_random_bool() (rand() % 2)
#define CGL_utils_random_vec2(min, max) ((CGL_vec2){CGL_utils_random_float() * (max.x - min.x) + min.x, CGL_utils_random_float() * (max.y - min.y) + min.y})
#define CGL_utils_random_vec3(min, max) ((CGL_vec3){CGL_utils_random_float() * (max.x - min.x) + min.x, CGL_utils_random_float() * (max.y - min.y) + min.y, CGL_utils_random_float() * (max.z - min.z) + min.z})
#define CGL_utils_random_vec4(min, max) ((CGL_vec4){CGL_utils_random_float() * (max.x - min.x) + min.x, CGL_utils_random_float() * (max.y - min.y) + min.y, CGL_utils_random_float() * (max.z - min.z) + min.z, CGL_utils_random_float() * (max.w - min.w) + min.w})
#define CGL_utils_random_color() ((CGL_color){CGL_utils_random_float(), CGL_utils_random_float(), CGL_utils_random_float(), 1.0f})
#define CGL_utils_clamp(x, minl, maxl) (x < minl ? minl : (x > maxl ? maxl : x))
#define CGL_utils_array_size(array) (sizeof(array) / sizeof(array[0]))

#define CGL_free(ptr) free(ptr)
#define CGL_exit(code) exit(code)

#endif

// math and data structures
#if 1 // Just to use code folding

// data structures

struct CGL_list;
typedef struct CGL_list CGL_list;

CGL_list* CGL_list_create(size_t item_size, size_t initial_capacity);
void CGL_list_destroy(CGL_list* list);
void CGL_list_set_increase_factor(CGL_list* list, float increase_factor);
float CGL_list_get_increase_factor(CGL_list* list);
size_t CGL_list_get_item_size(CGL_list* list);
size_t CGL_list_get_size(CGL_list* list);
size_t CGL_list_get_capacity(CGL_list* list);
size_t CGL_list_push(CGL_list* list, void* data);
size_t CGL_list_pop(CGL_list* list, void* data);
void* CGL_list_get(CGL_list* list, size_t index, void* data);
void* CGL_list_set(CGL_list* list, size_t index, void* data);
bool CGL_list_is_empty(CGL_list* list);
size_t CGL_list_find(CGL_list* list, void* data);
void CGL_list_reserve(CGL_list* list, size_t size);
void CGL_list_fill(CGL_list* list, size_t size);


// algorithms
uint32_t CGL_utils_crc32(const void* data, size_t size);
uint64_t CGL_utils_crc64(const void* data, size_t size);
void CGL_utils_rot13(const char* data_in, char* data_out);

// threads
#ifndef CGL_EXCLUDES_THREADS
struct CGL_thread;
typedef struct CGL_thread CGL_thread;

typedef void (*CGL_thread_function)(void*);

struct CGL_mutex;
typedef struct CGL_mutex CGL_mutex;

CGL_thread* CGL_thread_create();
bool CGL_thread_start(CGL_thread* thread, CGL_thread_function function, void* argument);
void CGL_thread_destroy(CGL_thread* thread);
bool CGL_thread_join(CGL_thread* thread);
bool CGL_thread_joinable(CGL_thread* thread);
bool CGL_thread_is_running(CGL_thread* thread);

uintptr_t CGL_thread_get_id(CGL_thread* thread);

CGL_mutex* CGL_mutex_create(bool set);
void CGL_mutex_destroy(CGL_mutex* mutex);
int CGL_mutex_lock(CGL_mutex* mutex, uint64_t timeout);
void CGL_mutex_release(CGL_mutex* mutex);
#endif

// math

struct CGL_vec2
{
    float x;
    float y;
};
typedef struct CGL_vec2 CGL_vec2;

struct CGL_vec3
{
    float x;
    float y;
    float z;
};
typedef struct CGL_vec3 CGL_vec3;

struct CGL_vec4
{
    float x;
    float y;
    float z;
    float w;
};
typedef struct CGL_vec4 CGL_vec4;
typedef struct CGL_vec4 CGL_color;

struct CGL_ivec4
{
    int x;
    int y;
    int z;
    int w;
};
typedef struct CGL_ivec4 CGL_ivec4;

struct CGL_mat3
{
    float m[3][3];
};
typedef struct CGL_mat3 CGL_mat3;

struct CGL_mat4
{
    float m[16];
};
typedef struct CGL_mat4 CGL_mat4;

// math functions with macros

#define CGL_vec2_init(x, y) ((CGL_vec2){x, y})
#define CGL_vec2_add(a, b) (CGL_vec2){a.x + b.x, a.y + b.y}
#define CGL_vec2_sub(a, b) (CGL_vec2){a.x - b.x, a.y - b.y}
#define CGL_vec2_mul(a, b) (CGL_vec2){a.x * b.x, a.y * b.y}
#define CGL_vec2_div(a, b) (CGL_vec2){a.x / b.x, a.y / b.y}
#define CGL_vec2_scale(a, s) (CGL_vec2){a.x * s, a.y * s}
#define CGL_vec2_dot(a, b) (a.x * b.x + a.y * b.y)
#define CGL_vec2_length(a) sqrtf(a.x * a.x + a.y * a.y)
#define CGL_vec2_normalize(a) { float __CGL_vector_length##__LINE__ = 1.0f / CGL_vec2_length(a); a.x *= __CGL_vector_length##__LINE__; a.y *= __CGL_vector_length##__LINE__; }
#define CGL_vec2_lerp(a, b, t) (CGL_vec2){a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t}
#define CGL_vec2_min(a, b) (CGL_vec2){a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y}
#define CGL_vec2_max(a, b) (CGL_vec2){a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y}
#define CGL_vec2_equal(a, b) (a.x == b.x && a.y == b.y)

#define CGL_vec3_init(x, y, z) ((CGL_vec3){x, y, z})
#define CGL_vec3_add(a, b) (CGL_vec3){a.x + b.x, a.y + b.y, a.z + b.z}
#define CGL_vec3_sub(a, b) (CGL_vec3){a.x - b.x, a.y - b.y, a.z - b.z}
#define CGL_vec3_mul(a, b) (CGL_vec3){a.x * b.x, a.y * b.y, a.z * b.z}
#define CGL_vec3_div(a, b) (CGL_vec3){a.x / b.x, a.y / b.y, a.z / b.z}
#define CGL_vec3_scale(a, s) (CGL_vec3){a.x * s, a.y * s, a.z * s}
#define CGL_vec3_dot(a, b) (a.x * b.x + a.y * b.y + a.z * b.z)
#define CGL_vec3_cross(a, b) (CGL_vec3){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}
#define CGL_vec3_length(a) sqrtf(a.x * a.x + a.y * a.y + a.z * a.z)
#define CGL_vec3_normalize(a) { float __CGL_vector_length##__LINE__ = 1.0f / CGL_vec3_length(a); a.x *= __CGL_vector_length##__LINE__; a.y *= __CGL_vector_length##__LINE__; a.z *= __CGL_vector_length##__LINE__; }
#define CGL_vec3_lerp(a, b, t) (CGL_vec3){a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t}
#define CGL_vec3_min(a, b) (CGL_vec3){a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z}
#define CGL_vec3_max(a, b) (CGL_vec3){a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z}
#define CGL_vec3_equal(a, b) (a.x == b.x && a.y == b.y && a.z == b.z)
#define CGL_vec3_rotate_x(v, theta) (CGL_vec3){v.x * cosf(theta) - v.y * sinf(theta), v.x * sinf(theta) + v.y * cosf(theta), v.z}
#define CGL_vec3_rotate_y(v, theta) (CGL_vec3){v.x * cosf(theta) + v.z * sinf(theta), v.y, -1.0f * v.x * sinf(theta) + v.z * cosf(theta)}
#define CGL_vec3_rotate_z(v, theta) (CGL_vec3){v.x, v.y * cosf(theta) - v.z * sinf(theta), v.y * sinf(theta) + v.z * cosf(theta)}

#define CGL_vec4_init(x, y, z, w) ((CGL_vec4){x, y, z, w})
#define CGL_vec4_add(a, b) (CGL_vec4){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}
#define CGL_vec4_sub(a, b) (CGL_vec4){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}
#define CGL_vec4_mul(a, b) (CGL_vec4){a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w}
#define CGL_vec4_div(a, b) (CGL_vec4){a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w}
#define CGL_vec4_scale(a, s) (CGL_vec4){a.x * s, a.y * s, a.z * s, a.w * s}
#define CGL_vec4_dot(a, b) (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w)
#define CGL_vec4_lerp(a, b, t) (CGL_vec4){a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t}
#define CGL_vec4_min(a, b) (CGL_vec4){a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z, a.w < b.w ? a.w : b.w}
#define CGL_vec4_max(a, b) (CGL_vec4){a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z, a.w > b.w ? a.w : b.w}
#define CGL_vec4_equal(a, b) (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w)

#define CGL_mat4_identity() (CGL_mat4){1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
#define CGL_mat4_scale(x, y, z) (CGL_mat4){x, 0.0f, 0.0f, 0.0f, 0.0f, y, 0.0f, 0.0f, 0.0f, 0.0f, z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
#define CGL_mat4_translate(x, y, z) (CGL_mat4){1.0f, 0.0f, 0.0f, x, 0.0f, 1.0f, 0.0f, y, 0.0f, 0.0f, 1.0f, z, 0.0f, 0.0f, 0.0f, 1.0f}
#define CGL_mat4_rotate_x(x) (CGL_mat4){1.0f, 0.0f, 0.0f, 0.0f, 0.0f, cosf(x), sinf(x), 0.0f, 0.0f, -sinf(x), cosf(x), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
#define CGL_mat4_rotate_y(x) (CGL_mat4){cosf(x), 0.0f, -sinf(x), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, sinf(x), 0.0f, cosf(x), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
#define CGL_mat4_rotate_z(x) (CGL_mat4){cosf(x), sinf(x), 0.0f, 0.0f, -sinf(x), cosf(x), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}
#define CGL_mat4_add(a, b) (CGL_mat4){a.m[0] + b.m[0], a.m[1] + b.m[1], a.m[2] + b.m[2], a.m[3] + b.m[3], a.m[4] + b.m[4], a.m[5] + b.m[5], a.m[6] + b.m[6], a.m[7] + b.m[7], a.m[8] + b.m[8], a.m[9] + b.m[9], a.m[10] + b.m[10], a.m[11] + b.m[11], a.m[12] + b.m[12], a.m[13] + b.m[13], a.m[14] + b.m[14], a.m[15] + b.m[15]}
#define CGL_mat4_sub(a, b) (CGL_mat4){a.m[0] - b.m[0], a.m[1] - b.m[1], a.m[2] - b.m[2], a.m[3] - b.m[3], a.m[4] - b.m[4], a.m[5] - b.m[5], a.m[6] - b.m[6], a.m[7] - b.m[7], a.m[8] - b.m[8], a.m[9] - b.m[9], a.m[10] - b.m[10], a.m[11] - b.m[11], a.m[12] - b.m[12], a.m[13] - b.m[13], a.m[14] - b.m[14], a.m[15] - b.m[15]}
#define CGL_mat4_mul(a, b) (CGL_mat4){ \
    a.m[0] * b.m[0] + a.m[4] * b.m[1] + a.m[8] * b.m[2] + a.m[12] * b.m[3], \
    a.m[1] * b.m[0] + a.m[5] * b.m[1] + a.m[9] * b.m[2] + a.m[13] * b.m[3], \
    a.m[2] * b.m[0] + a.m[6] * b.m[1] + a.m[10] * b.m[2] + a.m[14] * b.m[3], \
    a.m[3] * b.m[0] + a.m[7] * b.m[1] + a.m[11] * b.m[2] + a.m[15] * b.m[3], \
\
    a.m[0] * b.m[4] + a.m[4] * b.m[5] + a.m[8] * b.m[6] + a.m[12] * b.m[7], \
    a.m[1] * b.m[4] + a.m[5] * b.m[5] + a.m[9] * b.m[6] + a.m[13] * b.m[7], \
    a.m[2] * b.m[4] + a.m[6] * b.m[5] + a.m[10] * b.m[6] + a.m[14] * b.m[7], \
    a.m[3] * b.m[4] + a.m[7] * b.m[5] + a.m[11] * b.m[6] + a.m[15] * b.m[7], \
\
    a.m[0] * b.m[8] + a.m[4] * b.m[9] + a.m[8] * b.m[10] + a.m[12] * b.m[11], \
    a.m[1] * b.m[8] + a.m[5] * b.m[9] + a.m[9] * b.m[10] + a.m[13] * b.m[11], \
    a.m[2] * b.m[8] + a.m[6] * b.m[9] + a.m[10] * b.m[10] + a.m[14] * b.m[11], \
    a.m[3] * b.m[8] + a.m[7] * b.m[9] + a.m[11] * b.m[10] + a.m[15] * b.m[11], \
\
    a.m[0] * b.m[12] + a.m[4] * b.m[13] + a.m[8] * b.m[14] + a.m[12] * b.m[15], \
    a.m[1] * b.m[12] + a.m[5] * b.m[13] + a.m[9] * b.m[14] + a.m[13] * b.m[15], \
    a.m[2] * b.m[12] + a.m[6] * b.m[13] + a.m[10] * b.m[14] + a.m[14] * b.m[15], \
    a.m[3] * b.m[12] + a.m[7] * b.m[13] + a.m[11] * b.m[14] + a.m[15] * b.m[15] \
}
#define CGL_mat4_perspective(aspect, fov, znear, zfar) (CGL_mat4){1.0f / (aspect * tanf(fov / 2.0f)), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / tanf(fov / 2.0f), 0.0f, 0.0f, 0.0f, 0.0f, -1.0f * ( (zfar + znear) / (zfar - znear) ), -1.0f, 0.0f, 0.0f, -2.0f * znear * zfar / (zfar - znear), 1.0f}
#define CGL_mat4_transpose(a) (CGL_mat4){a.m[0], a.m[4], a.m[12], a.m[1], a.m[5], a.m[9], a.m[13], a.m[2], a.m[6], a.m[10], a.m[14], a.m[3], a.m[7], a.m[11], a.m[15]}
CGL_mat4 CGL_mat4_look_at(CGL_vec3 eye, CGL_vec3 target, CGL_vec3 up);

#endif

// window
#if 1 // Just to use code folding

// CGL window management library (usinfg GLFW)

#ifdef CGL_EXPOSE_GLFW_API
#include <GLFW/glfw3.h> // GLFW
#else
// forrward declare GLFW data types
struct GLFWwindow;
typedef struct GLFWwindow GLFWwindow;
#endif // EXPOSE_GLFW_API

/* The unknown key */
#define GLFW_KEY_UNKNOWN            -1

/* Printable keys */
#define CGL_KEY_SPACE              32
#define CGL_KEY_APOSTROPHE         39  /* ' */
#define CGL_KEY_COMMA              44  /* , */
#define CGL_KEY_MINUS              45  /* - */
#define CGL_KEY_PERIOD             46  /* . */
#define CGL_KEY_SLASH              47  /* / */
#define CGL_KEY_0                  48
#define CGL_KEY_1                  49
#define CGL_KEY_2                  50
#define CGL_KEY_3                  51
#define CGL_KEY_4                  52
#define CGL_KEY_5                  53
#define CGL_KEY_6                  54
#define CGL_KEY_7                  55
#define CGL_KEY_8                  56
#define CGL_KEY_9                  57
#define CGL_KEY_SEMICOLON          59  /* ; */
#define CGL_KEY_EQUAL              61  /* = */
#define CGL_KEY_A                  65
#define CGL_KEY_B                  66
#define CGL_KEY_C                  67
#define CGL_KEY_D                  68
#define CGL_KEY_E                  69
#define CGL_KEY_F                  70
#define CGL_KEY_G                  71
#define CGL_KEY_H                  72
#define CGL_KEY_I                  73
#define CGL_KEY_J                  74
#define CGL_KEY_K                  75
#define CGL_KEY_L                  76
#define CGL_KEY_M                  77
#define CGL_KEY_N                  78
#define CGL_KEY_O                  79
#define CGL_KEY_P                  80
#define CGL_KEY_Q                  81
#define CGL_KEY_R                  82
#define CGL_KEY_S                  83
#define CGL_KEY_T                  84
#define CGL_KEY_U                  85
#define CGL_KEY_V                  86
#define CGL_KEY_W                  87
#define CGL_KEY_X                  88
#define CGL_KEY_Y                  89
#define CGL_KEY_Z                  90
#define CGL_KEY_LEFT_BRACKET       91  /* [ */
#define CGL_KEY_BACKSLASH          92  /* \ */
#define CGL_KEY_RIGHT_BRACKET      93  /* ] */
#define CGL_KEY_GRAVE_ACCENT       96  /* ` */
#define CGL_KEY_WORLD_1            161 /* non-US #1 */
#define CGL_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define CGL_KEY_ESCAPE             256
#define CGL_KEY_ENTER              257
#define CGL_KEY_TAB                258
#define CGL_KEY_BACKSPACE          259
#define CGL_KEY_INSERT             260
#define CGL_KEY_DELETE             261
#define CGL_KEY_RIGHT              262
#define CGL_KEY_LEFT               263
#define CGL_KEY_DOWN               264
#define CGL_KEY_UP                 265
#define CGL_KEY_PAGE_UP            266
#define CGL_KEY_PAGE_DOWN          267
#define CGL_KEY_HOME               268
#define CGL_KEY_END                269
#define CGL_KEY_CAPS_LOCK          280
#define CGL_KEY_SCROLL_LOCK        281
#define CGL_KEY_NUM_LOCK           282
#define CGL_KEY_PRINT_SCREEN       283
#define CGL_KEY_PAUSE              284
#define CGL_KEY_F1                 290
#define CGL_KEY_F2                 291
#define CGL_KEY_F3                 292
#define CGL_KEY_F4                 293
#define CGL_KEY_F5                 294
#define CGL_KEY_F6                 295
#define CGL_KEY_F7                 296
#define CGL_KEY_F8                 297
#define CGL_KEY_F9                 298
#define CGL_KEY_F10                299
#define CGL_KEY_F11                300
#define CGL_KEY_F12                301
#define CGL_KEY_F13                302
#define CGL_KEY_F14                303
#define CGL_KEY_F15                304
#define CGL_KEY_F16                305
#define CGL_KEY_F17                306
#define CGL_KEY_F18                307
#define CGL_KEY_F19                308
#define CGL_KEY_F20                309
#define CGL_KEY_F21                310
#define CGL_KEY_F22                311
#define CGL_KEY_F23                312
#define CGL_KEY_F24                313
#define CGL_KEY_F25                314
#define CGL_KEY_KP_0               320
#define CGL_KEY_KP_1               321
#define CGL_KEY_KP_2               322
#define CGL_KEY_KP_3               323
#define CGL_KEY_KP_4               324
#define CGL_KEY_KP_5               325
#define CGL_KEY_KP_6               326
#define CGL_KEY_KP_7               327
#define CGL_KEY_KP_8               328
#define CGL_KEY_KP_9               329
#define CGL_KEY_KP_DECIMAL         330
#define CGL_KEY_KP_DIVIDE          331
#define CGL_KEY_KP_MULTIPLY        332
#define CGL_KEY_KP_SUBTRACT        333
#define CGL_KEY_KP_ADD             334
#define CGL_KEY_KP_ENTER           335
#define CGL_KEY_KP_EQUAL           336
#define CGL_KEY_LEFT_SHIFT         340
#define CGL_KEY_LEFT_CONTROL       341
#define CGL_KEY_LEFT_ALT           342
#define CGL_KEY_LEFT_SUPER         343
#define CGL_KEY_RIGHT_SHIFT        344
#define CGL_KEY_RIGHT_CONTROL      345
#define CGL_KEY_RIGHT_ALT          346
#define CGL_KEY_RIGHT_SUPER        347
#define CGL_KEY_MENU               348

#define CGL_MOUSE_BUTTON_1         0
#define CGL_MOUSE_BUTTON_2         1
#define CGL_MOUSE_BUTTON_3         2
#define CGL_MOUSE_BUTTON_4         3
#define CGL_MOUSE_BUTTON_5         4
#define CGL_MOUSE_BUTTON_6         5
#define CGL_MOUSE_BUTTON_7         6
#define CGL_MOUSE_BUTTON_8         7
#define CGL_MOUSE_BUTTON_LAST      CGL_MOUSE_BUTTON_8
#define CGL_MOUSE_BUTTON_LEFT      CGL_MOUSE_BUTTON_1
#define CGL_MOUSE_BUTTON_RIGHT     CGL_MOUSE_BUTTON_2
#define CGL_MOUSE_BUTTON_MIDDLE    CGL_MOUSE_BUTTON_3

#define CGL_RELEASE                0
#define CGL_PRESS                  1
#define CGL_REPEAT                 2

#define CGL_WINDOW_TABLE_SIZE      10000

// structs
struct CGL_window;
typedef struct CGL_window CGL_window;

struct GLFWwindow;
typedef struct GLFWwindow GLFWwindow;

typedef void(*CGL_window_key_callback)(CGL_window* window, int key, int scancode, int action, int mods);
typedef void(*CGL_window_mouse_button_callback)(CGL_window* window, int button, int action, int mods);
typedef void(*CGL_window_mouse_position_callback)(CGL_window* window, double xpos, double ypos);
typedef void(*CGL_window_mouse_scroll_callback)(CGL_window* window, double xoffset, double yoffset);
typedef void(*CGL_window_framebuffer_size_callback)(CGL_window* window, int width, int height);
typedef void(*CGL_window_close_callback)(CGL_window* window);

CGL_window* CGL_window_create(int width, int height, const char* title); // create window and initialize GLFW
void CGL_window_destroy(CGL_window* window); // destroy window and terminate GLFW
void CGL_window_poll_events(CGL_window* window); // poll events from GLFW
void CGL_window_swap_buffers(CGL_window* window); // swap buffers
bool CGL_window_should_close(CGL_window* window); // check if window should close
void CGL_window_set_title(CGL_window* window, const char* title); // set window title
void CGL_window_set_size(CGL_window* window, int width, int height); // set window size
void CGL_window_set_position(CGL_window* window, int x, int y); // set window position
void CGL_window_set_hidden(CGL_window* window, bool hidden); // set window hidden
void CGL_window_set_user_data(CGL_window* window, void* user_data); // set window user data
void* CGL_window_get_user_data(CGL_window* window); // get window user data
void CGL_window_get_size(CGL_window* window, int* width, int* height); // get window size
void CGL_window_get_position(CGL_window* window, int* x, int* y); // get window position
void CGL_window_get_framebuffer_size(CGL_window* window, int* width, int* height); // get window framebuffer size
void CGL_window_set_key_callback(CGL_window* window, CGL_window_key_callback callback); // set key callback
void CGL_window_set_mouse_button_callback(CGL_window* window, CGL_window_mouse_button_callback callback); // set mouse button callback
void CGL_window_set_mouse_position_callback(CGL_window* window, CGL_window_mouse_position_callback callback); // set mouse position callback
void CGL_window_set_mouse_scroll_callback(CGL_window* window, CGL_window_mouse_scroll_callback callback); // set mouse scroll callback
void CGL_window_set_framebuffer_size_callback(CGL_window* window, CGL_window_framebuffer_size_callback callback); // set framebuffer size callback
void CGL_window_set_close_callback(CGL_window* window, CGL_window_close_callback callback); // set close callback
void CGL_window_resecure_callbacks(CGL_window* window);
void CGL_window_make_context_current(CGL_window* window); // make opengl context current
GLFWwindow* CGL_window_get_glfw_handle(CGL_window* window);

// inputs
int CGL_window_get_key(CGL_window* window, int key); // get key state
int CGL_window_get_mouse_button(CGL_window* window, int button); // get mouse button state
void CGL_window_get_mouse_position(CGL_window* window, double* xpos, double* ypos); // get mouse position

#endif

// opengl
#if 1 // Just to use code folding

#include <glad/glad.h>

#define CGL_OPENGL_VERSION_MAJOR 4
#define CGL_OPENGL_VERSION_MINOR 6

#define CGL_OPENGL_VERSION_MAJOR_STR "4"
#define CGL_OPENGL_VERSION_MINOR_STR "6"

struct CGL_image
{
    void* data;
    uint32_t height;
    uint32_t width;
    uint32_t bytes_per_channel;
    uint32_t channels;
};
typedef struct CGL_image CGL_image;

// forward declarations
struct CGL_texture;
typedef struct CGL_texture CGL_texture;

struct CGL_framebuffer;
typedef struct CGL_framebuffer CGL_framebuffer;

struct CGL_shader;
typedef struct CGL_shader CGL_shader;

struct CGL_mesh_gpu;
typedef struct CGL_mesh_gpu CGL_mesh_gpu;

struct CGL_mesh_vertex
{
    CGL_vec4 position;
    CGL_vec4 normal;
    CGL_vec4 texture_coordinates;
};
typedef struct CGL_mesh_vertex CGL_mesh_vertex;

struct CGL_mesh_cpu
{
    size_t index_count;
    uint32_t* indices;
    size_t vertex_count;
    CGL_mesh_vertex* vertices;
};
typedef struct CGL_mesh_cpu CGL_mesh_cpu;


struct CGL_ssbo;
typedef struct CGL_ssbo CGL_ssbo;

// texture
CGL_texture* CGL_texture_create(CGL_image* image);
CGL_texture* CGL_texture_create_blank(int width, int height, GLenum format, GLenum internal_format, GLenum type); // create texture
CGL_texture* CGL_texture_create_array(int width, int height, int layers, GLenum format, GLenum internal_format, GLenum type);
void CGL_texture_array_set_layer_data(CGL_texture* texture, int layer, void* data);
void CGL_texture_destroy(CGL_texture* texture); // destroy texture
void CGL_texture_bind(CGL_texture* texture, int unit); // bind texture to unit
void CGL_texture_set_data(CGL_texture* texture, void* data); // set texture data
void CGL_texture_set_user_data(CGL_texture* texture, void* user_data); // set texture user data
void* CGL_texture_get_user_data(CGL_texture* texture); // get texture user data
void CGL_texture_get_size(CGL_texture* texture, int* width, int* height); // get texture size
void CGL_texture_set_scaling_method(CGL_texture* texture, GLint method);
void CGL_texture_set_wrapping_method(CGL_texture* texture, GLint method);

// framebuffer
CGL_framebuffer* CGL_framebuffer_create(int width, int height); // create framebuffer (32 bit)
CGL_framebuffer* CGL_framebuffer_create_from_default(CGL_window* window); // create framebuffer from default framebuffer
void CGL_framebuffer_destroy(CGL_framebuffer* framebuffer); // destroy framebuffer
void CGL_framebuffer_bind(CGL_framebuffer* framebuffer); // bind framebuffer
void CGL_framebuffer_get_size(CGL_framebuffer* framebuffer, int* width, int* height); // get framebuffer size
void CGL_framebuffer_set_user_data(CGL_framebuffer* framebuffer, void* user_data); // set framebuffer user data
void* CGL_framebuffer_get_user_data(CGL_framebuffer* framebuffer); // get framebuffer user data
void CGL_framebuffer_read_pixels(CGL_framebuffer* framebuffer, int x, int y, int width, int height, void* pixels); // read pixels from framebuffer
int CGL_framebuffer_get_mouse_pick_id(CGL_framebuffer* framebuffer, int x, int y, int index); // get mouse pick id
CGL_texture* CGL_framebuffer_get_color_texture(CGL_framebuffer* framebuffer); // get color texture
CGL_texture* CGL_framebuffer_get_depth_texture(CGL_framebuffer* framebuffer); // get depth texture

// gl

void CGL_gl_clear(float r, float g, float b, float a); // clear framebuffer
bool CGL_gl_init(); // load opengl functions
void CGL_gl_shutdown(); // clean up

// mesh

CGL_mesh_gpu* CGL_mesh_gpu_create(); // create mesh (gpu)
void CGL_mesh_gpu_destroy(CGL_mesh_gpu* mesh); // destroy mesh (gpu)
void CGL_mesh_gpu_render(CGL_mesh_gpu* mesh); // render mesh (gpu)
void CGL_mesh_gpu_render_instanced(CGL_mesh_gpu* mesh, uint32_t count); // render mesh instanfced (gpu)
void CGL_mesh_gpu_set_user_data(CGL_mesh_gpu* mesh, void* user_data); // set mesh user data
void* CGL_mesh_gpu_get_user_data(CGL_mesh_gpu* mesh); // get mesh user data
void CGL_mesh_gpu_upload(CGL_mesh_gpu* mesh, CGL_mesh_cpu* mesh_cpu, bool static_draw); // upload mesh from (cpu) to (gpu)

// mesh generation

CGL_mesh_cpu* CGL_mesh_cpu_create(size_t vertex_count, size_t index_count);
CGL_mesh_cpu* CGL_mesh_cpu_load_obj(const char* path);
CGL_mesh_cpu* CGL_mesh_cpu_triangle(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c); // generate triangle mesh
CGL_mesh_cpu* CGL_mesh_cpu_quad(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d); // generate quad mesh
void CGL_mesh_cpu_destroy(CGL_mesh_cpu* mesh); // destroy mesh (cpu)

// shader

CGL_shader* CGL_shader_create(const char* vertex_shader_source, const char* fragment_shader_source, char** error); // create shader
CGL_shader* CGL_shader_compute_create(const char* compute_shader_source, char** error); // create compute shader
CGL_shader* CGL_shader_create_from_files(const char* vertex_shader_file, const char* fragment_shader_file, char** error); // create shader from files
CGL_shader* CGL_shader_compute_create_from_files(const char* compute_shader_file, char** error); // create compute shader from files
void CGL_shader_destroy(CGL_shader* shader); // destroy shader
void CGL_shader_bind(CGL_shader* shader); // bind shader
int CGL_shader_get_uniform_location(CGL_shader* shader, const char* name); // get uniform location
void CGL_shader_set_uniform_mat4(CGL_shader* shader, int location, CGL_mat4* matrix); // set uniform matrix
void CGL_shader_set_uniform_vec4(CGL_shader* shader, int location, CGL_vec4* vector); // set uniform vector
void CGL_shader_set_uniform_vec3(CGL_shader* shader, int location, CGL_vec3* vector); // set uniform vector
void CGL_shader_set_uniform_vec2(CGL_shader* shader, int location, CGL_vec2* vector); // set uniform vector
void CGL_shader_set_uniform_int(CGL_shader* shader, int location, int value); // set uniform int
void CGL_shader_set_uniform_bool(CGL_shader* shader, int location, bool value); // set uniform bool
void CGL_shader_set_uniform_float(CGL_shader* shader, int location, float value); // set uniform float
void CGL_shader_set_uniform_vec2v(CGL_shader* shader, int location, float x, float y); // set uniform vector
void CGL_shader_set_uniform_vec3v(CGL_shader* shader, int location, float x, float y, float z); // set uniform vector
void CGL_shader_set_uniform_vec4v(CGL_shader* shader, int location, float x, float y, float z, float w); // set uniform vector
void CGL_shader_set_uniform_ivec2v(CGL_shader* shader, int location, int x, int y); // set uniform vector
void CGL_shader_set_uniform_ivec3v(CGL_shader* shader, int location, int x, int y, int z); // set uniform vector
void CGL_shader_set_uniform_ivec4v(CGL_shader* shader, int location, int x, int y, int z, int w); // set uniform vector
void CGL_shader_set_user_data(CGL_shader* shader, void* user_data); // set shader user data
void* CGL_shader_get_user_data(CGL_shader* shader); // get shader user data
void CGL_shader_compute_dispatch(CGL_shader* shader, int x, int y, int z); // dispatch compute shader

// ssbo
CGL_ssbo* CGL_ssbo_create(uint32_t binding); // create ssbo
void CGL_ssbo_destroy(CGL_ssbo* ssbo); // destroy ssbo
void CGL_ssbo_bind(CGL_ssbo* ssbo); // bind ssbo
void CGL_ssbo_set_data(CGL_ssbo* ssbo, size_t size, void* data, bool static_draw); // set ssbo data
void CGL_ssbo_set_sub_data(CGL_ssbo* ssbo, size_t offset, size_t size, void* data, bool static_draw); // set ssbo sub data
void CGL_ssbo_get_data(CGL_ssbo* ssbo, size_t* size, void* data); // get ssbo data
void CGL_ssbo_get_sub_data(CGL_ssbo* ssbo, size_t offset, size_t size, void* data); // get ssbo sub data
void CGL_ssbo_set_user_data(CGL_ssbo* ssbo, void* user_data); // set ssbo user data
void* CGL_ssbo_get_user_data(CGL_ssbo* ssbo); // get ssbo user data
size_t CGL_ssbo_get_size(CGL_ssbo* ssbo); // get ssbo size
void CGL_ssbo_copy(CGL_ssbo* dst, CGL_ssbo* src, size_t src_offset, size_t dst_offset, size_t size); // copy ssbo


#endif

// camera
#if 1

struct CGL_camera;
typedef struct CGL_camera CGL_camera;

CGL_camera* CGL_camera_create();
void CGL_camera_destroy(CGL_camera* camera);
bool CGL_camera_is_perspective(CGL_camera* camera);
bool CGL_camera_is_orthographic(CGL_camera* camera);
float CGL_camera_get_fov(CGL_camera* camera);
float CGL_camera_get_aspect_ratio(CGL_camera* camera);
float CGL_camera_get_z_near(CGL_camera* camera);
float CGL_camera_get_z_far(CGL_camera* camera);
CGL_vec4 CGL_camera_get_orthographic_limits(CGL_camera* camera);
void CGL_camera_set_fov(CGL_camera* camera, float fov);
void CGL_camera_set_aspect_ratio(CGL_camera* camera, float aspect_ratio);
void CGL_camera_set_z_near(CGL_camera* camera, float z_near);
void CGL_camera_set_z_far(CGL_camera* camera, float z_far);
void CGL_camera_set_orthographic_limits(CGL_camera* camera, CGL_vec4 ortho_limits);
void CGL_camera_set_perspective(CGL_camera* camera);
void CGL_camera_set_orthographic(CGL_camera* camera);
CGL_mat4 CGL_camera_get_projection_mat(CGL_camera* camera);
CGL_mat4* CGL_camera_get_projection_mat_ptr(CGL_camera* camera);
CGL_mat4 CGL_camera_get_view_mat(CGL_camera* camera);
CGL_mat4* CGL_camera_get_view_mat_ptr(CGL_camera* camera);
CGL_mat4 CGL_camera_get_pv_mat(CGL_camera* camera);
CGL_mat4* CGL_camera_get_pv_mat_ptr(CGL_camera* camera);
void CGL_camera_set_position(CGL_camera* camera, CGL_vec3 position);
void CGL_camera_set_rotation(CGL_camera* camera, CGL_vec3 ratation);
CGL_vec3 CGL_camera_get_position(CGL_camera* camera);
CGL_vec3 CGL_camera_get_rotation(CGL_camera* camera);
CGL_vec3* CGL_camera_get_position_ptr(CGL_camera* camera);
CGL_vec3* CGL_camera_get_rotation_ptr(CGL_camera* camera);
CGL_vec3 CGL_camera_get_front(CGL_camera* camera);
CGL_vec3 CGL_camera_get_right(CGL_camera* camera);
CGL_vec3 CGL_camera_get_up(CGL_camera* camera);
void CGL_camera_recalculate_mat(CGL_camera* camera);

#endif

// The phong renderer
#if 1

#ifndef CGL_EXCLUDE_PHONG_RENDERER

#define CGL_PHONG_MAX_LIGHTS            16

#define CGL_PHONG_LIGHT_DIRECTIONAL     0
#define CGL_PHONG_LIGHT_POINT           1
#define CGL_PHONG_LIGHT_SPOT            2

struct CGL_phong_mat;
typedef struct CGL_phong_mat CGL_phong_mat;

struct CGL_phong_pipeline;
typedef struct CGL_phong_pipeline CGL_phong_pipeline;

struct CGL_phong_light;
typedef struct CGL_phong_light CGL_phong_light;

CGL_phong_pipeline* CGL_phong_pipeline_create();
void CGL_phong_pipeline_destroy(CGL_phong_pipeline* pipeline);
void CGL_phong_pipeline_set_user_data(CGL_phong_pipeline* pipeline, void* data);
void* CGL_phong_pipeline_get_user_data(CGL_phong_pipeline* pipeline);
bool CGL_phong_pipeline_is_using_blinn(CGL_phong_pipeline* pipeline);
void CGL_phong_pipeline_enable_blinn(CGL_phong_pipeline* pipeline);
void CGL_phong_pipeline_disable_blinn(CGL_phong_pipeline* pipeline);
void CGL_phong_pipeline_enable_gamma_correction(CGL_phong_pipeline* pipeline);
void CGL_phong_pipeline_disable_gamma_correction(CGL_phong_pipeline* pipeline);
void CGL_phong_pipeline_set_ambient_light_color(CGL_phong_pipeline* pipeline, CGL_vec3 color);
CGL_vec3 CGL_phong_pipeline_get_ambient_light_color(CGL_phong_pipeline* pipeline);
void CGL_phong_pipeline_set_ambient_light_strength(CGL_phong_pipeline* pipeline, float strength);
float CGL_phong_pipeline_get_ambient_light_strength(CGL_phong_pipeline* pipeline);
uint32_t CGL_phong_pipeline_add_light(CGL_phong_pipeline* pipeline, CGL_phong_light* light);
CGL_phong_light* CGL_phong_pipeline_remove_light(CGL_phong_pipeline* pipeline, uint32_t light_id);

CGL_phong_mat* CGL_phong_mat_create();
void CGL_phong_mat_destroy(CGL_phong_mat* mat);
void CGL_phong_mat_set_diffuse_color(CGL_phong_mat* mat, CGL_vec3 color);
void CGL_phong_mat_set_diffuse_texture(CGL_phong_mat* mat, CGL_image* image);
void CGL_phong_mat_set_specular_color(CGL_phong_mat* mat, CGL_vec3 color);
void CGL_phong_mat_set_specular_texture(CGL_phong_mat* mat, CGL_image* image);
void CGL_phong_mat_set_normal_texture(CGL_phong_mat* mat, CGL_image* image);
void CGL_phong_mat_set_shininess(CGL_phong_mat* mat, float shininess);
void CGL_phong_mat_set_user_data(CGL_phong_mat* mat, void* data);
void* CGL_phong_mat_get_user_data(CGL_phong_mat* mat);
void CGL_phong_mat_disable_normal_map(CGL_phong_mat* mat);
void CGL_phong_mat_enable_wireframe(CGL_phong_mat* mat);
void CGL_phong_mat_disable_wireframe(CGL_phong_mat* mat);
float CGL_phong_mat_get_shininess(CGL_phong_mat* mat);

CGL_phong_light* CGL_phong_light_directional(CGL_vec3 direction, CGL_vec3 color, float itensity);
CGL_phong_light* CGL_phong_light_point(CGL_vec3 position, CGL_vec3 color, float itensity, float constant, float linear, float quadratic);
void CGL_phong_light_destroy(CGL_phong_light* light);
void CGL_phong_light_set_intensity(CGL_phong_light* light, float intensity);
void CGL_phong_light_set_color(CGL_phong_light* light, CGL_vec3 color);
float CGL_phong_light_get_intensity(CGL_phong_light* light);
CGL_vec3 CGL_phong_light_get_color(CGL_phong_light* light);
uint32_t CGL_phong_light_get_type(CGL_phong_light* light);

void CGL_phong_render_begin(CGL_phong_pipeline* pipeline, CGL_camera* camera);
void CGL_phong_render(CGL_mesh_gpu* mesh, CGL_mat4* model_matrix, CGL_phong_mat* material, CGL_phong_pipeline* pipeline, CGL_camera* camera);
void CGL_phong_render_end(CGL_phong_pipeline* pipeline, CGL_camera* camera);

#endif

#endif

// tilemap renderer
#if 1

#ifndef CGL_EXCLUDE_TILEMAP_RENDERER

struct CGL_tilemap;
typedef struct CGL_tilemap CGL_tilemap;

struct CGL_tile;
typedef struct CGL_tile CGL_tile;

CGL_tilemap* CGL_tilemap_create(uint32_t tile_count_x, uint32_t tile_count_y, uint32_t tile_size_x, uint32_t tile_size_y, uint32_t ssbo_binding);
void CGL_tilemap_destroy(CGL_tilemap* tilemap);
void CGL_tilemap_set_tile_color(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, float r, float g, float b);
void CGL_tilemap_set_tile_texture_from_array(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, uint32_t texture_index);
void CGL_tilemap_set_tile_texture_from_tileset(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, float texture_x_min, float texture_y_min, float texture_x_max, float texture_y_max);
void CGL_tilemap_clear_tile(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y);
void CGL_tilemap_render(CGL_tilemap* tilemap, float scale_x, float scale_y, float offset_x, float offset_y, CGL_texture* texture);
void CGL_tilemap_reset(CGL_tilemap* tilemap);

#endif

#endif

// Implementation of CGL
#ifdef CGL_IMPLEMENTATION

// include windows headers for windows builds
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)
#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif
#endif

// list
#if 1

// data structures
struct CGL_list
{
    size_t size;
    size_t capacity;
    size_t item_size;
    float increase_factor;
    void* data;
};

CGL_list* CGL_list_create(size_t item_size, size_t initial_capacity)
{
    CGL_list* list = (CGL_list*)malloc(sizeof(CGL_list));
    list->size = 0;
    list->capacity = initial_capacity;
    list->item_size = item_size;
    list->increase_factor = 1.5f;
    list->data = malloc(list->capacity * item_size);
    return list;
}

void CGL_list_destroy(CGL_list* list)
{
    free(list->data);
    free(list);
}

void CGL_list_set_increase_factor(CGL_list* list, float increase_factor)
{
    list->increase_factor = increase_factor;
}

float CGL_list_get_increase_factor(CGL_list* list)
{
    return list->increase_factor;
}

size_t CGL_list_get_item_size(CGL_list* list)
{
    return list->item_size;
}

size_t CGL_list_get_size(CGL_list* list)
{
    return list->size;
}

size_t CGL_list_get_capacity(CGL_list* list)
{
    return list->capacity;
}

size_t CGL_list_push(CGL_list* list, void* data)
{
    if(list->size == list->capacity)
    {
        size_t new_capacity = (size_t)(list->capacity * list->increase_factor);
        list->data = realloc(list->data, new_capacity * list->item_size);
        list->capacity = new_capacity;
    }
    memcpy( ((char*)list->data + list->size * list->item_size), data, list->item_size );
    list->size += 1;
    return list->size - 1;
}

size_t CGL_list_pop(CGL_list* list, void* data)
{
    if(list->size == 0) return 0;
    list->size -= 1;
    if(data) memcpy( data, ((char*)list->data + list->size * list->item_size), list->item_size );
    return list->size; 
}

void* CGL_list_get(CGL_list* list, size_t index, void* data)
{
    if(index >= list->size || index < 0) return NULL;
    if(data) memcpy( data, ((char*)list->data + index * list->item_size), list->item_size );
    return ((char*)list->data + index * list->item_size);
}

void* CGL_list_set(CGL_list* list, size_t index, void* data)
{
    if (index >= list->size || index < 0) return NULL;
    if(data) memcpy( ((char*)list->data + index * list->item_size), data, list->item_size );
    return ((char*)list->data + index * list->item_size);
}

bool CGL_list_is_empty(CGL_list* list)
{
    return list->size == 0;
}

size_t CGL_list_find(CGL_list* list, void* data)
{
    for(size_t i = 0 ; i < list->size ; i++)
        if(memcmp(((char*)list->data + i * list->item_size), data, list->item_size) == 0)
            return i;
    return UINT64_MAX;
}

void CGL_list_reserve(CGL_list* list, size_t size)
{
    if(list->capacity > size) return;
    size_t new_capacity = size;
    list->data = realloc(list->data, new_capacity * list->item_size);
    list->capacity = new_capacity;
}

void CGL_list_fill(CGL_list* list, size_t size)
{
    CGL_list_reserve(list, size);
    list->size = max(size, list->size);
}


#ifndef CGL_EXCLUDES_THREADS

#ifdef _WIN32

#include <process.h>

struct CGL_thread
{
    uintptr_t id;
    HANDLE handle;
    CGL_thread_function function;
    bool running;
};

struct CGL_mutex
{
    HANDLE handle;
};

CGL_thread* CGL_thread_create()
{
    CGL_thread* thread = (CGL_thread*)malloc(sizeof(CGL_thread));
    thread->function = NULL;
    thread->handle = NULL;
    thread->id = 0;
    thread->running = false;
    return thread;
}

bool CGL_thread_start(CGL_thread* thread, CGL_thread_function function, void* argument)
{
    if(thread->running) CGL_thread_join(thread);
    if(thread->handle) CloseHandle(thread->handle);
    thread->function = function;
    thread->handle = (HANDLE)_beginthread(function, 0, argument);
    thread->id = (uintptr_t)GetThreadId(thread->handle);
    thread->running = true;
    return thread->handle != NULL;
}

void CGL_thread_destroy(CGL_thread* thread)
{
    if(thread->running) CGL_thread_join(thread);
    if(thread->handle) CloseHandle(thread->handle);
    CGL_free(thread);
}

uintptr_t CGL_thread_get_id(CGL_thread* thread)
{
    return thread->id;
}

bool CGL_thread_is_running(CGL_thread* thread)
{
    if(!thread->handle) return false;
    DWORD exit_code;
    if(!GetExitCodeThread(thread->handle, &exit_code)) return false;
    if (exit_code == STILL_ACTIVE)
        thread->running = true;
    else
    {
        thread->running = false;
        CloseHandle(thread->handle);
    }
    return thread->running;
}

bool CGL_thread_join(CGL_thread* thread)
{
    if(!thread->handle) return true;
    bool result = (WaitForSingleObject(thread->handle, INFINITE) != WAIT_FAILED);
    thread->running = false;
    return result;
}

bool CGL_thread_joinable(CGL_thread* thread)
{
    return true; // Temporary
}


CGL_mutex* CGL_mutex_create(bool set)
{
    CGL_mutex* mutex = (CGL_mutex*)malloc(sizeof(CGL_mutex));
    mutex->handle = CreateMutex(NULL, (BOOL)set, NULL);
    return mutex;
}

void CGL_mutex_destroy(CGL_mutex* mutex)
{
    if(mutex->handle) CloseHandle(mutex->handle);
    CGL_free(mutex);
}

int CGL_mutex_lock(CGL_mutex* mutex, uint64_t timeout)
{
    return (int)WaitForSingleObject(mutex->handle, (DWORD)timeout);
}

void CGL_mutex_release(CGL_mutex* mutex)
{
    ReleaseMutex(mutex->handle);
}

#else // for posix (using pthread)

#include <pthread.h>

struct CGL_thread
{
    uintptr_t id;
    pthread_t handle;
    CGL_thread_function function;
    bool running;
};

struct CGL_mutex
{
    pthread_mutex_t handle;
};

CGL_thread* CGL_thread_create()
{
    CGL_thread* thread = (CGL_thread*)malloc(sizeof(CGL_thread));
    thread->function = NULL;
    thread->handle = NULL;
    thread->id = 0;
    thread->running = false;
    return thread;
}

bool CGL_thread_start(CGL_thread* thread, CGL_thread_function function, void* argument)
{
    if(thread->running) CGL_thread_join(thread);
    thread->function = function;
    pthread_create(&thread->handle, 0, (void*)function, argument);
    thread->id = (uintptr_t)thread->handle; // Temporary
    thread->running = true;
    return thread->handle != NULL;
}

void CGL_thread_destroy(CGL_thread* thread)
{
    if(thread->running) CGL_thread_join(thread);
    CGL_free(thread);
}

uintptr_t CGL_thread_get_id(CGL_thread* thread)
{
    return thread->id;
}

bool CGL_thread_is_running(CGL_thread* thread)
{
    if(!thread->handle) return false;

    //  thread->running = (pthread_kill(thread->handle, 0) == 0);
    if(pthread_kill(thread->handle, 0) == 0)
        thread->running = true;        
    else
        thread->running = false;
    return thread->running;
}

bool CGL_thread_join(CGL_thread* thread)
{
    if(!thread->handle) return true;
    return pthread_join(thread->handle, NULL);
}

bool CGL_thread_joinable(CGL_thread* thread)
{
    return true; // Temporary
}


CGL_mutex* CGL_mutex_create(bool set)
{
    CGL_mutex* mutex = (CGL_mutex*)malloc(sizeof(CGL_mutex));
    pthread_mutex_init(&mutex->handle, NULL);
    return mutex;
}

void CGL_mutex_destroy(CGL_mutex* mutex)
{
    if(mutex->handle)  pthread_mutex_destroy(&mutex->handle);
    CGL_free(mutex);
}

int CGL_mutex_lock(CGL_mutex* mutex, uint64_t timeout)
{
    return pthread_mutex_lock(&mutex->handle);
}

void CGL_mutex_release(CGL_mutex* mutex)
{
    pthread_mutex_unlock(&mutex->handle);  
}


#endif

#endif

#endif

// utils
#if 1
/*
uint32_t CGL_utils_crc32(const void* dat, size_t size)
{
    const uint8_t* data = (const uint8_t*)dat;
	uint32_t crc=0xFFFFFFFF;	
	for(size_t i = 0 ; i < size ; i++)
	{
		char ch = data[i];
		for(size_t j=0;j<8;j++) {
			uint32_t b=(ch^crc)&1;
			crc>>=1;
			if(b) crc=crc^0xEDB88320;
			ch>>=1;
		}
	}	
	return ~crc;
}
*/

// From : https://chromium.googlesource.com/chromiumos/platform/punybench/+/refs/heads/stabilize-6909.B/libpuny.b/crc32.c
static const uint32_t __CGL_CRC32_TABLE[256] =
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

uint32_t CGL_utils_crc32(const void* dat, size_t size)
{
    const uint8_t* b = (const uint8_t*)dat;
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < size; i++)
		crc = __CGL_CRC32_TABLE[(crc ^ *b++) & 0xff] ^ (crc >> 8);
	return ~crc;
}


// From : https://chromium.googlesource.com/chromiumos/platform/punybench/+/refs/heads/stabilize-6909.B/libpuny.b/crc64.c
#define CGL_CONST64(x) x##ull

static const uint64_t __CGL_CRC64_TABLE[256] = {
	CGL_CONST64(0x0000000000000000), CGL_CONST64(0x42f0e1eba9ea3693),
	CGL_CONST64(0x85e1c3d753d46d26), CGL_CONST64(0xc711223cfa3e5bb5),
	CGL_CONST64(0x493366450e42ecdf), CGL_CONST64(0x0bc387aea7a8da4c),
	CGL_CONST64(0xccd2a5925d9681f9), CGL_CONST64(0x8e224479f47cb76a),
	CGL_CONST64(0x9266cc8a1c85d9be), CGL_CONST64(0xd0962d61b56fef2d),
	CGL_CONST64(0x17870f5d4f51b498), CGL_CONST64(0x5577eeb6e6bb820b),
	CGL_CONST64(0xdb55aacf12c73561), CGL_CONST64(0x99a54b24bb2d03f2),
	CGL_CONST64(0x5eb4691841135847), CGL_CONST64(0x1c4488f3e8f96ed4),
	CGL_CONST64(0x663d78ff90e185ef), CGL_CONST64(0x24cd9914390bb37c),
	CGL_CONST64(0xe3dcbb28c335e8c9), CGL_CONST64(0xa12c5ac36adfde5a),
	CGL_CONST64(0x2f0e1eba9ea36930), CGL_CONST64(0x6dfeff5137495fa3),
	CGL_CONST64(0xaaefdd6dcd770416), CGL_CONST64(0xe81f3c86649d3285),
	CGL_CONST64(0xf45bb4758c645c51), CGL_CONST64(0xb6ab559e258e6ac2),
	CGL_CONST64(0x71ba77a2dfb03177), CGL_CONST64(0x334a9649765a07e4),
	CGL_CONST64(0xbd68d2308226b08e), CGL_CONST64(0xff9833db2bcc861d),
	CGL_CONST64(0x388911e7d1f2dda8), CGL_CONST64(0x7a79f00c7818eb3b),
	CGL_CONST64(0xcc7af1ff21c30bde), CGL_CONST64(0x8e8a101488293d4d),
	CGL_CONST64(0x499b3228721766f8), CGL_CONST64(0x0b6bd3c3dbfd506b),
	CGL_CONST64(0x854997ba2f81e701), CGL_CONST64(0xc7b97651866bd192),
	CGL_CONST64(0x00a8546d7c558a27), CGL_CONST64(0x4258b586d5bfbcb4),
	CGL_CONST64(0x5e1c3d753d46d260), CGL_CONST64(0x1cecdc9e94ace4f3),
	CGL_CONST64(0xdbfdfea26e92bf46), CGL_CONST64(0x990d1f49c77889d5),
	CGL_CONST64(0x172f5b3033043ebf), CGL_CONST64(0x55dfbadb9aee082c),
	CGL_CONST64(0x92ce98e760d05399), CGL_CONST64(0xd03e790cc93a650a),
	CGL_CONST64(0xaa478900b1228e31), CGL_CONST64(0xe8b768eb18c8b8a2),
	CGL_CONST64(0x2fa64ad7e2f6e317), CGL_CONST64(0x6d56ab3c4b1cd584),
	CGL_CONST64(0xe374ef45bf6062ee), CGL_CONST64(0xa1840eae168a547d),
	CGL_CONST64(0x66952c92ecb40fc8), CGL_CONST64(0x2465cd79455e395b),
	CGL_CONST64(0x3821458aada7578f), CGL_CONST64(0x7ad1a461044d611c),
	CGL_CONST64(0xbdc0865dfe733aa9), CGL_CONST64(0xff3067b657990c3a),
	CGL_CONST64(0x711223cfa3e5bb50), CGL_CONST64(0x33e2c2240a0f8dc3),
	CGL_CONST64(0xf4f3e018f031d676), CGL_CONST64(0xb60301f359dbe0e5),
	CGL_CONST64(0xda050215ea6c212f), CGL_CONST64(0x98f5e3fe438617bc),
	CGL_CONST64(0x5fe4c1c2b9b84c09), CGL_CONST64(0x1d14202910527a9a),
	CGL_CONST64(0x93366450e42ecdf0), CGL_CONST64(0xd1c685bb4dc4fb63),
	CGL_CONST64(0x16d7a787b7faa0d6), CGL_CONST64(0x5427466c1e109645),
	CGL_CONST64(0x4863ce9ff6e9f891), CGL_CONST64(0x0a932f745f03ce02),
	CGL_CONST64(0xcd820d48a53d95b7), CGL_CONST64(0x8f72eca30cd7a324),
	CGL_CONST64(0x0150a8daf8ab144e), CGL_CONST64(0x43a04931514122dd),
	CGL_CONST64(0x84b16b0dab7f7968), CGL_CONST64(0xc6418ae602954ffb),
	CGL_CONST64(0xbc387aea7a8da4c0), CGL_CONST64(0xfec89b01d3679253),
	CGL_CONST64(0x39d9b93d2959c9e6), CGL_CONST64(0x7b2958d680b3ff75),
	CGL_CONST64(0xf50b1caf74cf481f), CGL_CONST64(0xb7fbfd44dd257e8c),
	CGL_CONST64(0x70eadf78271b2539), CGL_CONST64(0x321a3e938ef113aa),
	CGL_CONST64(0x2e5eb66066087d7e), CGL_CONST64(0x6cae578bcfe24bed),
	CGL_CONST64(0xabbf75b735dc1058), CGL_CONST64(0xe94f945c9c3626cb),
	CGL_CONST64(0x676dd025684a91a1), CGL_CONST64(0x259d31cec1a0a732),
	CGL_CONST64(0xe28c13f23b9efc87), CGL_CONST64(0xa07cf2199274ca14),
	CGL_CONST64(0x167ff3eacbaf2af1), CGL_CONST64(0x548f120162451c62),
	CGL_CONST64(0x939e303d987b47d7), CGL_CONST64(0xd16ed1d631917144),
	CGL_CONST64(0x5f4c95afc5edc62e), CGL_CONST64(0x1dbc74446c07f0bd),
	CGL_CONST64(0xdaad56789639ab08), CGL_CONST64(0x985db7933fd39d9b),
	CGL_CONST64(0x84193f60d72af34f), CGL_CONST64(0xc6e9de8b7ec0c5dc),
	CGL_CONST64(0x01f8fcb784fe9e69), CGL_CONST64(0x43081d5c2d14a8fa),
	CGL_CONST64(0xcd2a5925d9681f90), CGL_CONST64(0x8fdab8ce70822903),
	CGL_CONST64(0x48cb9af28abc72b6), CGL_CONST64(0x0a3b7b1923564425),
	CGL_CONST64(0x70428b155b4eaf1e), CGL_CONST64(0x32b26afef2a4998d),
	CGL_CONST64(0xf5a348c2089ac238), CGL_CONST64(0xb753a929a170f4ab),
	CGL_CONST64(0x3971ed50550c43c1), CGL_CONST64(0x7b810cbbfce67552),
	CGL_CONST64(0xbc902e8706d82ee7), CGL_CONST64(0xfe60cf6caf321874),
	CGL_CONST64(0xe224479f47cb76a0), CGL_CONST64(0xa0d4a674ee214033),
	CGL_CONST64(0x67c58448141f1b86), CGL_CONST64(0x253565a3bdf52d15),
	CGL_CONST64(0xab1721da49899a7f), CGL_CONST64(0xe9e7c031e063acec),
	CGL_CONST64(0x2ef6e20d1a5df759), CGL_CONST64(0x6c0603e6b3b7c1ca),
	CGL_CONST64(0xf6fae5c07d3274cd), CGL_CONST64(0xb40a042bd4d8425e),
	CGL_CONST64(0x731b26172ee619eb), CGL_CONST64(0x31ebc7fc870c2f78),
	CGL_CONST64(0xbfc9838573709812), CGL_CONST64(0xfd39626eda9aae81),
	CGL_CONST64(0x3a28405220a4f534), CGL_CONST64(0x78d8a1b9894ec3a7),
	CGL_CONST64(0x649c294a61b7ad73), CGL_CONST64(0x266cc8a1c85d9be0),
	CGL_CONST64(0xe17dea9d3263c055), CGL_CONST64(0xa38d0b769b89f6c6),
	CGL_CONST64(0x2daf4f0f6ff541ac), CGL_CONST64(0x6f5faee4c61f773f),
	CGL_CONST64(0xa84e8cd83c212c8a), CGL_CONST64(0xeabe6d3395cb1a19),
	CGL_CONST64(0x90c79d3fedd3f122), CGL_CONST64(0xd2377cd44439c7b1),
	CGL_CONST64(0x15265ee8be079c04), CGL_CONST64(0x57d6bf0317edaa97),
	CGL_CONST64(0xd9f4fb7ae3911dfd), CGL_CONST64(0x9b041a914a7b2b6e),
	CGL_CONST64(0x5c1538adb04570db), CGL_CONST64(0x1ee5d94619af4648),
	CGL_CONST64(0x02a151b5f156289c), CGL_CONST64(0x4051b05e58bc1e0f),
	CGL_CONST64(0x87409262a28245ba), CGL_CONST64(0xc5b073890b687329),
	CGL_CONST64(0x4b9237f0ff14c443), CGL_CONST64(0x0962d61b56fef2d0),
	CGL_CONST64(0xce73f427acc0a965), CGL_CONST64(0x8c8315cc052a9ff6),
	CGL_CONST64(0x3a80143f5cf17f13), CGL_CONST64(0x7870f5d4f51b4980),
	CGL_CONST64(0xbf61d7e80f251235), CGL_CONST64(0xfd913603a6cf24a6),
	CGL_CONST64(0x73b3727a52b393cc), CGL_CONST64(0x31439391fb59a55f),
	CGL_CONST64(0xf652b1ad0167feea), CGL_CONST64(0xb4a25046a88dc879),
	CGL_CONST64(0xa8e6d8b54074a6ad), CGL_CONST64(0xea16395ee99e903e),
	CGL_CONST64(0x2d071b6213a0cb8b), CGL_CONST64(0x6ff7fa89ba4afd18),
	CGL_CONST64(0xe1d5bef04e364a72), CGL_CONST64(0xa3255f1be7dc7ce1),
	CGL_CONST64(0x64347d271de22754), CGL_CONST64(0x26c49cccb40811c7),
	CGL_CONST64(0x5cbd6cc0cc10fafc), CGL_CONST64(0x1e4d8d2b65facc6f),
	CGL_CONST64(0xd95caf179fc497da), CGL_CONST64(0x9bac4efc362ea149),
	CGL_CONST64(0x158e0a85c2521623), CGL_CONST64(0x577eeb6e6bb820b0),
	CGL_CONST64(0x906fc95291867b05), CGL_CONST64(0xd29f28b9386c4d96),
	CGL_CONST64(0xcedba04ad0952342), CGL_CONST64(0x8c2b41a1797f15d1),
	CGL_CONST64(0x4b3a639d83414e64), CGL_CONST64(0x09ca82762aab78f7),
	CGL_CONST64(0x87e8c60fded7cf9d), CGL_CONST64(0xc51827e4773df90e),
	CGL_CONST64(0x020905d88d03a2bb), CGL_CONST64(0x40f9e43324e99428),
	CGL_CONST64(0x2cffe7d5975e55e2), CGL_CONST64(0x6e0f063e3eb46371),
	CGL_CONST64(0xa91e2402c48a38c4), CGL_CONST64(0xebeec5e96d600e57),
	CGL_CONST64(0x65cc8190991cb93d), CGL_CONST64(0x273c607b30f68fae),
	CGL_CONST64(0xe02d4247cac8d41b), CGL_CONST64(0xa2dda3ac6322e288),
	CGL_CONST64(0xbe992b5f8bdb8c5c), CGL_CONST64(0xfc69cab42231bacf),
	CGL_CONST64(0x3b78e888d80fe17a), CGL_CONST64(0x7988096371e5d7e9),
	CGL_CONST64(0xf7aa4d1a85996083), CGL_CONST64(0xb55aacf12c735610),
	CGL_CONST64(0x724b8ecdd64d0da5), CGL_CONST64(0x30bb6f267fa73b36),
	CGL_CONST64(0x4ac29f2a07bfd00d), CGL_CONST64(0x08327ec1ae55e69e),
	CGL_CONST64(0xcf235cfd546bbd2b), CGL_CONST64(0x8dd3bd16fd818bb8),
	CGL_CONST64(0x03f1f96f09fd3cd2), CGL_CONST64(0x41011884a0170a41),
	CGL_CONST64(0x86103ab85a2951f4), CGL_CONST64(0xc4e0db53f3c36767),
	CGL_CONST64(0xd8a453a01b3a09b3), CGL_CONST64(0x9a54b24bb2d03f20),
	CGL_CONST64(0x5d45907748ee6495), CGL_CONST64(0x1fb5719ce1045206),
	CGL_CONST64(0x919735e51578e56c), CGL_CONST64(0xd367d40ebc92d3ff),
	CGL_CONST64(0x1476f63246ac884a), CGL_CONST64(0x568617d9ef46bed9),
	CGL_CONST64(0xe085162ab69d5e3c), CGL_CONST64(0xa275f7c11f7768af),
	CGL_CONST64(0x6564d5fde549331a), CGL_CONST64(0x279434164ca30589),
	CGL_CONST64(0xa9b6706fb8dfb2e3), CGL_CONST64(0xeb46918411358470),
	CGL_CONST64(0x2c57b3b8eb0bdfc5), CGL_CONST64(0x6ea7525342e1e956),
	CGL_CONST64(0x72e3daa0aa188782), CGL_CONST64(0x30133b4b03f2b111),
	CGL_CONST64(0xf7021977f9cceaa4), CGL_CONST64(0xb5f2f89c5026dc37),
	CGL_CONST64(0x3bd0bce5a45a6b5d), CGL_CONST64(0x79205d0e0db05dce),
	CGL_CONST64(0xbe317f32f78e067b), CGL_CONST64(0xfcc19ed95e6430e8),
	CGL_CONST64(0x86b86ed5267cdbd3), CGL_CONST64(0xc4488f3e8f96ed40),
	CGL_CONST64(0x0359ad0275a8b6f5), CGL_CONST64(0x41a94ce9dc428066),
	CGL_CONST64(0xcf8b0890283e370c), CGL_CONST64(0x8d7be97b81d4019f),
	CGL_CONST64(0x4a6acb477bea5a2a), CGL_CONST64(0x089a2aacd2006cb9),
	CGL_CONST64(0x14dea25f3af9026d), CGL_CONST64(0x562e43b4931334fe),
	CGL_CONST64(0x913f6188692d6f4b), CGL_CONST64(0xd3cf8063c0c759d8),
	CGL_CONST64(0x5dedc41a34bbeeb2), CGL_CONST64(0x1f1d25f19d51d821),
	CGL_CONST64(0xd80c07cd676f8394), CGL_CONST64(0x9afce626ce85b507)
};

uint64_t CGL_utils_crc64(const void* data, size_t size)
{
    uint64_t crc = 0xffffffffffffffffULL;
	const uint8_t *b = (const uint8_t*)data;
	for (size_t i = 0; i < size; i++)
		crc = __CGL_CRC64_TABLE[(uint8_t)(crc >> 56) ^ *b++] ^ (crc << 8);
	return ~crc;
}


// From : https://github.com/B-Con/crypto-algorithms/blob/master/rot-13.c
void CGL_utils_rot13(const char* data_in, char* str)
{
   int case_type, idx, len;
   len = (int)strlen(data_in);
   strcpy(str, data_in);
   for (idx = 0; idx < len; idx++) {
      // Only process alphabetic characters.
      if (str[idx] < 'A' || (str[idx] > 'Z' && str[idx] < 'a') || str[idx] > 'z')
         continue;
      // Determine if the char is upper or lower case.
      if (str[idx] >= 'a')
         case_type = 'a';
      else
         case_type = 'A';
      // Rotate the char's value, ensuring it doesn't accidentally "fall off" the end.
      str[idx] = (str[idx] + 13) % (case_type + 26);
      if (str[idx] < 26)
         str[idx] += case_type;
   }
}

#endif

// common lib and mat
#if 1 // Just to use code folding

CGL_mat4 CGL_mat4_look_at(CGL_vec3 eye, CGL_vec3 target, CGL_vec3 up)
{
    CGL_vec3 z_axis = CGL_vec3_sub(target, eye);
    CGL_vec3_normalize(z_axis);
    CGL_vec3 x_axis = CGL_vec3_cross(up, z_axis);
    CGL_vec3_normalize(x_axis);
    CGL_vec3 y_axis = CGL_vec3_cross(z_axis, x_axis);
    CGL_mat4 mat;
    mat.m[0] = x_axis.x;
    mat.m[1] = x_axis.y;
    mat.m[2] = x_axis.z;
    mat.m[3] = -1.0f * CGL_vec3_dot(x_axis, eye);
    mat.m[4] = y_axis.x;
    mat.m[5] = y_axis.y;
    mat.m[6] = y_axis.z;
    mat.m[7] = -1.0f * CGL_vec3_dot(y_axis, eye);
    mat.m[8] = z_axis.x;
    mat.m[9] = z_axis.y;
    mat.m[10] = z_axis.z;
    mat.m[11] = -1.0f * CGL_vec3_dot(z_axis, eye);
    mat.m[12] = 0.0f;
    mat.m[13] = 0.0f;
    mat.m[14] = 0.0f;
    mat.m[15] = 1.0f;
    return mat;
}

struct CGL_context
{
    bool is_initialized;
    int window_count;
    
    CGL_window* window_table[CGL_WINDOW_TABLE_SIZE];
};

CGL_context* __CGL_context = NULL;

bool CGL_init()
{
    if(__CGL_context != NULL)
        return true;
    __CGL_context = (CGL_context*)malloc(sizeof(CGL_context));
    if(__CGL_context == NULL)
        return false;

    __CGL_context->is_initialized = true;


    __CGL_context->window_count = 0;


    return true;
}

void CGL_shutdown()
{
    if(__CGL_context == NULL)
        return;
    free(__CGL_context);
    __CGL_context = NULL;
}

#endif


#if 1 // just to use code folding in ide

// read file into memory
char* CGL_utils_read_file(const char* path, size_t* size_ptr)
{
    FILE* file = fopen(path, "rb");
    if(file == NULL)
        return NULL;
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = (char*)malloc(size + 1);
    if(data == NULL)
    {
        fclose(file);
        return NULL;
    }
    fread(data, 1, size, file);
    data[size] = '\0';
    fclose(file);
    if(size_ptr)
        *size_ptr = size;
    return data;
}

// write data to file
bool CGL_utils_write_file(const char* path, const char* data, size_t size)
{
    FILE* file = fopen(path, "wb");
    if(file == NULL)
        return false;
    fwrite(data, 1, size, file);
    fclose(file);
    return true;
}



#ifndef CGL_EXPOSE_GLFW_API
#include <GLFW/glfw3.h> // GLFW
#endif

struct CGL_window
{
    GLFWwindow* handle;
    bool is_hidden;
    void* user_data;
    // callbacks
    CGL_window_key_callback key_callback;
    CGL_window_mouse_button_callback mouse_button_callback;
    CGL_window_mouse_position_callback mouse_position_callback;
    CGL_window_mouse_scroll_callback mouse_scroll_callback;
    CGL_window_framebuffer_size_callback framebuffer_size_callback;
    CGL_window_close_callback close_callback;

    GLFWkeyfun previous_key_callback;
    GLFWmousebuttonfun previous_mouse_button_callback;
    GLFWcursorposfun previous_mouse_position_callback;
    GLFWscrollfun previous_mouse_scroll_callback;
    GLFWframebuffersizefun previous_framebuffer_size_callback;
    GLFWwindowclosefun previous_close_callback;
};

// callbacks
void __CGL_window_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    CGL_window* cgl_window = __CGL_context->window_table[(uintptr_t)window % CGL_WINDOW_TABLE_SIZE];
    if(cgl_window->key_callback != NULL)
        cgl_window->key_callback(cgl_window, key, scancode, action, mods);
    if(cgl_window->previous_key_callback)
        cgl_window->previous_key_callback(window, key, scancode, action, mods);
}

void __CGL_window_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    CGL_window* cgl_window = __CGL_context->window_table[(uintptr_t)window % CGL_WINDOW_TABLE_SIZE];
    if(cgl_window->mouse_button_callback != NULL)
        cgl_window->mouse_button_callback(cgl_window, button, action, mods);
    if(cgl_window->previous_mouse_button_callback)
        cgl_window->previous_mouse_button_callback(window, button, action, mods);
}

void __CGL_window_mouse_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    // CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    CGL_window* cgl_window = __CGL_context->window_table[(uintptr_t)window % CGL_WINDOW_TABLE_SIZE];
    if(cgl_window->mouse_position_callback != NULL)
        cgl_window->mouse_position_callback(cgl_window, xpos, ypos);
    if(cgl_window->previous_mouse_position_callback)
        cgl_window->previous_mouse_position_callback(window, xpos, ypos);
}

void __CGL_window_mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    CGL_window* cgl_window = __CGL_context->window_table[(uintptr_t)window % CGL_WINDOW_TABLE_SIZE];
    if(cgl_window->mouse_scroll_callback != NULL)
        cgl_window->mouse_scroll_callback(cgl_window, xoffset, yoffset);
    if(cgl_window->previous_mouse_scroll_callback)
        cgl_window->previous_mouse_scroll_callback(window, xoffset, yoffset);
}

void __CGL_window_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    CGL_window* cgl_window = __CGL_context->window_table[(uintptr_t)window % CGL_WINDOW_TABLE_SIZE];
    if(cgl_window->framebuffer_size_callback != NULL)
        cgl_window->framebuffer_size_callback(cgl_window, width, height);
    if(cgl_window->previous_framebuffer_size_callback)
        cgl_window->previous_framebuffer_size_callback(window, width, height);
}

void __CGL_window_close_callback(GLFWwindow* window)
{
    // CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    CGL_window* cgl_window = __CGL_context->window_table[(uintptr_t)window % CGL_WINDOW_TABLE_SIZE];
    if(cgl_window->close_callback != NULL)
        cgl_window->close_callback(cgl_window);
    if(cgl_window->previous_close_callback)
        cgl_window->previous_close_callback(window);
}

// create window
CGL_window* CGL_window_create(int width, int height, const char* title)
{
    if (__CGL_context->window_count == 0)
    {
        if (!glfwInit())
        {
            CGL_LOG("Failed to initialize GLFW\n");
            return false;
        }
    }
    // tell glfw we are going to use opengl api
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	// tell glfw to initialize opengl context for opengl version 4.6 (latest)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// tell glfw to use the opengl core profile and not the compatibility profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// disabling resinfg gets rid of managing things like aspect ration and stuff
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    CGL_window* window = (CGL_window*)malloc(sizeof(CGL_window));
    if(window == NULL)
    {
        CGL_LOG("Failed to allocate memory for window\n");
        return NULL;
    }
    window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window->handle == NULL)
    {
        CGL_LOG("Failed to create GLFW window\n");
        return false;
    }
    glfwSwapInterval(1); // vsync
    window->is_hidden = true;
    window->key_callback = NULL;
    window->mouse_button_callback = NULL;
    window->mouse_position_callback = NULL;
    window->mouse_scroll_callback = NULL;
    window->framebuffer_size_callback = NULL;
    window->close_callback = NULL;
    window->user_data = NULL;
    window->previous_close_callback = NULL;
    window->previous_framebuffer_size_callback = NULL;
    window->previous_key_callback = NULL;
    window->previous_mouse_button_callback = NULL;
    window->previous_mouse_position_callback = NULL;
    window->previous_mouse_scroll_callback = NULL;
    __CGL_context->window_table[(uintptr_t)window->handle % CGL_WINDOW_TABLE_SIZE] = window; // Temporary
    __CGL_context->window_count++;
    return window;
}


// destroy window
void CGL_window_destroy(CGL_window* window)
{
    glfwDestroyWindow(window->handle);
    free(window);
    __CGL_context->window_count--;
    if (__CGL_context->window_count == 0)
    {
        glfwTerminate();
    }
}

// poll events
void CGL_window_poll_events(CGL_window* window)
{
    glfwPollEvents();
}

// swap buffers
void CGL_window_swap_buffers(CGL_window* window)
{
    glfwSwapBuffers(window->handle);
}

// check if window should close
bool CGL_window_should_close(CGL_window* window)
{
    return glfwWindowShouldClose(window->handle);
}

// set window title
void CGL_window_set_title(CGL_window* window, const char* title)
{
    glfwSetWindowTitle(window->handle, title);
}

// set window size
void CGL_window_set_size(CGL_window* window, int width, int height)
{
    glfwSetWindowSize(window->handle, width, height);
}

// set window position
void CGL_window_set_position(CGL_window* window, int x, int y)
{
    glfwSetWindowPos(window->handle, x, y);
}

// set window hidden
void CGL_window_set_hidden(CGL_window* window, bool hidden)
{
    if (hidden)
        glfwHideWindow(window->handle);
    else
        glfwShowWindow(window->handle);
    window->is_hidden = hidden;
}

// set window user data
void CGL_window_set_user_data(CGL_window* window, void* user_data)
{
    window->user_data = user_data;
}

// get window user data
void* CGL_window_get_user_data(CGL_window* window)
{
    return window->user_data;
}

// get window size
void CGL_window_get_size(CGL_window* window, int* width, int* height)
{
    glfwGetWindowSize(window->handle, width, height);
}

// get window position
void CGL_window_get_position(CGL_window* window, int* x, int* y)
{
    glfwGetWindowPos(window->handle, x, y);
}

// get window framebuffer size
void CGL_window_get_framebuffer_size(CGL_window* window, int* width, int* height)
{
    glfwGetFramebufferSize(window->handle, width, height);
}

// set key callback
void CGL_window_set_key_callback(CGL_window* window, CGL_window_key_callback callback)
{
    if(window->key_callback == NULL)
        window->previous_key_callback = glfwSetKeyCallback(window->handle, __CGL_window_key_callback);
    window->key_callback = callback;
}

// set mouse button callback
void CGL_window_set_mouse_button_callback(CGL_window* window, CGL_window_mouse_button_callback callback)
{
    if(window->mouse_button_callback == NULL)
        window->previous_mouse_button_callback = glfwSetMouseButtonCallback(window->handle, __CGL_window_mouse_button_callback);
    window->mouse_button_callback = callback;
}

// set mouse position callback
void CGL_window_set_mouse_position_callback(CGL_window* window, CGL_window_mouse_position_callback callback)
{
    if(window->mouse_position_callback == NULL)
        window->previous_mouse_position_callback = glfwSetCursorPosCallback(window->handle, __CGL_window_mouse_position_callback);
    window->mouse_position_callback = callback;
}

// set mouse scroll callback
void CGL_window_set_mouse_scroll_callback(CGL_window* window, CGL_window_mouse_scroll_callback callback)
{
    if(window->mouse_scroll_callback == NULL)
        window->previous_mouse_scroll_callback = glfwSetScrollCallback(window->handle, __CGL_window_mouse_scroll_callback);
    window->mouse_scroll_callback = callback;
}

// set framebuffer size callback
void CGL_window_set_framebuffer_size_callback(CGL_window* window, CGL_window_framebuffer_size_callback callback)
{
    if(window->framebuffer_size_callback == NULL)
        window->previous_framebuffer_size_callback = glfwSetFramebufferSizeCallback(window->handle, __CGL_window_framebuffer_size_callback);
    window->framebuffer_size_callback = callback;
}

// set close callback
void CGL_window_set_close_callback(CGL_window* window, CGL_window_close_callback callback)
{
    if(window->close_callback == NULL)
        window->previous_close_callback = glfwSetWindowCloseCallback(window->handle, __CGL_window_close_callback);
    window->close_callback = callback;
}

void CGL_window_resecure_callbacks(CGL_window* window)
{
    if(window->key_callback != NULL)
    {
        window->previous_key_callback = glfwSetKeyCallback(window->handle, __CGL_window_key_callback);
        if(window->previous_key_callback == __CGL_window_key_callback) window->previous_key_callback = NULL;
    }

    if(window->mouse_button_callback != NULL)
    {
        window->previous_mouse_button_callback = glfwSetMouseButtonCallback(window->handle, __CGL_window_mouse_button_callback);
        if(window->previous_mouse_button_callback == __CGL_window_mouse_button_callback) window->previous_mouse_button_callback = NULL;
    }

    if(window->mouse_position_callback != NULL)
    {
        window->previous_mouse_position_callback = glfwSetCursorPosCallback(window->handle, __CGL_window_mouse_position_callback);
        if(window->previous_mouse_position_callback == __CGL_window_mouse_position_callback) window->previous_mouse_position_callback = NULL;
    }

    if(window->mouse_scroll_callback != NULL)
    {
        window->previous_mouse_scroll_callback = glfwSetScrollCallback(window->handle, __CGL_window_mouse_scroll_callback);
        if(window->previous_mouse_scroll_callback == __CGL_window_mouse_scroll_callback) window->previous_mouse_scroll_callback = NULL;
    }

    if(window->framebuffer_size_callback != NULL)
    {
        window->previous_framebuffer_size_callback = glfwSetFramebufferSizeCallback(window->handle, __CGL_window_framebuffer_size_callback);
        if(window->previous_framebuffer_size_callback == __CGL_window_framebuffer_size_callback) window->previous_framebuffer_size_callback = NULL;
    }

    if(window->close_callback != NULL)
    {
        window->previous_close_callback = glfwSetWindowCloseCallback(window->handle, __CGL_window_close_callback);
        if(window->previous_close_callback == __CGL_window_close_callback) window->previous_close_callback = NULL;
    }
}

// get key state
int CGL_window_get_key(CGL_window* window, int key)
{
    return glfwGetKey(window->handle, key);
}

// get mouse button state
int CGL_window_get_mouse_button(CGL_window* window, int button)
{
    return glfwGetMouseButton(window->handle, button);
}

// get mouse position
void CGL_window_get_mouse_position(CGL_window* window, double* xpos, double* ypos)
{
    glfwGetCursorPos(window->handle, xpos, ypos);
}

// make opengl context current
void CGL_window_make_context_current(CGL_window* window)
{
    glfwMakeContextCurrent(window->handle);
}

GLFWwindow* CGL_window_get_glfw_handle(CGL_window* window)
{
    return window->handle;
}

#endif


// opengl 
#if 1 // Just to use code folding


// texture

struct CGL_texture
{
    GLuint handle;
    int width;
    int height;
    int depth;
    GLenum format;
    GLenum internal_format;
    GLenum type;
    GLenum target;
    void* user_data;
};

// create texture
CGL_texture* CGL_texture_create(CGL_image* image)
{
    GLenum format, internal_format, type;
    if(image->channels == 3)
        format = internal_format = GL_RGB;
    else if(image->channels == 4)
        format = internal_format = GL_RGBA;
    else
    {printf("Invalid channel count for image\n");return NULL;}
    if(image->bytes_per_channel == 8)
        type = GL_UNSIGNED_BYTE;
    else if(image->bytes_per_channel == 16)
        type = GL_UNSIGNED_SHORT;
    else if(image->bytes_per_channel == 32)
        type = GL_FLOAT;
    else
    {printf("Invalid bit depth for image\n");return NULL;}        
    CGL_texture* texture = malloc(sizeof(CGL_texture));
    texture->width = image->width;
    texture->height = image->height;
    texture->depth = 0;
    texture->format = format;
    texture->internal_format = internal_format;
    texture->type = type;
    texture->target = GL_TEXTURE_2D;
    texture->user_data = NULL;
    glGenTextures(1, &texture->handle);
    glBindTexture(texture->target, texture->handle);
    glTexImage2D(texture->target, 0, internal_format, image->width, image->height, 0, format, type, image->data);
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(texture->target, 0);
    return texture;
}

// create texture
CGL_texture* CGL_texture_create_blank(int width, int height, GLenum format, GLenum internal_format, GLenum type)
{
    CGL_texture* texture = malloc(sizeof(CGL_texture));
    texture->width = width;
    texture->height = height;
    texture->depth = 0;
    texture->format = format;
    texture->internal_format = internal_format;
    texture->type = type;
    texture->target = GL_TEXTURE_2D;
    texture->user_data = NULL;
    glGenTextures(1, &texture->handle);
    glBindTexture(texture->target, texture->handle);
    glTexImage2D(texture->target, 0, internal_format, width, height, 0, format, type, NULL);
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(texture->target, 0);
    return texture;
}

CGL_texture* CGL_texture_create_array(int width, int height, int layers, GLenum format, GLenum internal_format, GLenum type)
{
    CGL_texture* texture = malloc(sizeof(CGL_texture));
    texture->width = width;
    texture->height = height;
    texture->depth = layers;
    texture->format = format;
    texture->internal_format = internal_format;
    texture->type = type;
    texture->target = GL_TEXTURE_2D_ARRAY;
    texture->user_data = NULL;
    glGenTextures(1, &texture->handle);
    glBindTexture(texture->target, texture->handle);
    glTexImage2D(texture->target, 0, internal_format, width, height, 0, format, type, NULL);
    glTexImage3D(texture->target, 0, texture->internal_format, width, height, layers, 0, texture->format, texture->type, NULL);
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(texture->target, 0);
    return texture;
}

void CGL_texture_array_set_layer_data(CGL_texture* texture, int layer, void* data)
{
    glBindTexture(texture->target, texture->handle);
	glTexSubImage3D(texture->target, 0, 0, 0, layer, texture->width, texture->height, 1, texture->format, texture->type, data);
	glBindTexture(texture->target, 0);
}

void CGL_texture_set_scaling_method(CGL_texture* texture, GLint method)
{
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, method);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, method);
}

void CGL_texture_set_wrapping_method(CGL_texture* texture, GLint method)
{
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

// destroy texture
void CGL_texture_destroy(CGL_texture* texture)
{
    glDeleteTextures(1, &texture->handle);
    free(texture);
}

// bind texture to unit
void CGL_texture_bind(CGL_texture* texture, int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(texture->target, texture->handle);
}

// set texture data
void CGL_texture_set_data(CGL_texture* texture, void* data)
{
    glBindTexture(texture->target, texture->handle);
    glTexImage2D(texture->target, 0, texture->internal_format, texture->width, texture->height, 0, texture->format, texture->type, data);
    glBindTexture(texture->target, 0);
}

// set texture user data
void CGL_texture_set_user_data(CGL_texture* texture, void* user_data)
{
    texture->user_data = user_data;
}

// get texture user data
void* CGL_texture_get_user_data(CGL_texture* texture)
{
    return texture->user_data;
}


// get texture size
void CGL_texture_get_size(CGL_texture* texture, int* width, int* height)
{
    if(width)   *width = texture->width;
    if(height)  *height = texture->height;
}

// framebuffer

struct CGL_framebuffer
{
    GLuint handle;
    CGL_texture* color_texture;
    CGL_texture* depth_texture;
    CGL_texture* mousepick_texture[3];
    bool is_default;
    int width;
    int height;
    void* user_data;
};

// create framebuffer (32 bit)
CGL_framebuffer* CGL_framebuffer_create(int width, int height)
{
    CGL_framebuffer* framebuffer = (CGL_framebuffer*)malloc(sizeof(CGL_framebuffer));
    if(framebuffer == NULL)
        return NULL;
    
    glGenFramebuffers(1, &framebuffer->handle);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle);
    
    // create the textures
    framebuffer->color_texture = CGL_texture_create_blank(width, height, GL_RGBA, GL_RGBA32F, GL_FLOAT);    
    if(!framebuffer->color_texture)
    {
        free(framebuffer);
        return NULL;
    }

    
    framebuffer->depth_texture = CGL_texture_create_blank(width, height, GL_DEPTH_COMPONENT, GL_DEPTH24_STENCIL8, GL_FLOAT);
    if(!framebuffer->depth_texture)
    {
        CGL_texture_destroy(framebuffer->color_texture);
        free(framebuffer);
        return NULL;
    }
        
    for(int i = 0; i < 3; i++)
    {
        framebuffer->mousepick_texture[i] = CGL_texture_create_blank(width, height, GL_RED_INTEGER, GL_R32I, GL_INT);
        if(!framebuffer->mousepick_texture[i])
        {
            CGL_texture_destroy(framebuffer->color_texture);
            CGL_texture_destroy(framebuffer->depth_texture);
            for(int j = 0; j < i; j++)
                CGL_texture_destroy(framebuffer->mousepick_texture[j]);
            free(framebuffer);
            return NULL;
        }
    }

    
    // attach the textures
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->color_texture->handle, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer->depth_texture->handle, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, framebuffer->mousepick_texture[0]->handle, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, framebuffer->mousepick_texture[1]->handle, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, framebuffer->mousepick_texture[2]->handle, 0);

    GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, buffers);

    
    // check if framebuffer is complete
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        CGL_texture_destroy(framebuffer->color_texture);
        CGL_texture_destroy(framebuffer->depth_texture);
        for(int i = 0; i < 3; i++)
            CGL_texture_destroy(framebuffer->mousepick_texture[i]);
        free(framebuffer);
        CGL_LOG("Framebuffer is not complete\n");
        // get and print opengl error
        GLenum error = glGetError();
        if(error != GL_NO_ERROR)
            CGL_LOG("OpenGL error: %d\n", (error));
        return NULL;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->is_default = false;
    framebuffer->user_data = NULL;

    return framebuffer;
}

// create framebuffer from default framebuffer
CGL_framebuffer* CGL_framebuffer_create_from_default(CGL_window* window)
{
    CGL_framebuffer* framebuffer = (CGL_framebuffer*)malloc(sizeof(CGL_framebuffer));
    if(framebuffer == NULL)
        return NULL;
    framebuffer->handle = 0;
    framebuffer->is_default = true;
    framebuffer->user_data = window;
    return framebuffer;
}

// destroy framebuffer
void CGL_framebuffer_destroy(CGL_framebuffer* framebuffer)
{
    if(!framebuffer->is_default)
    {
        CGL_texture_destroy(framebuffer->color_texture);
        CGL_texture_destroy(framebuffer->depth_texture);
        for(int i = 0; i < 3; i++)
            CGL_texture_destroy(framebuffer->mousepick_texture[i]);
        glDeleteFramebuffers(1, &framebuffer->handle);
    }
    free(framebuffer);
}

// bind framebuffer
void CGL_framebuffer_bind(CGL_framebuffer* framebuffer)
{
    if(framebuffer->is_default)
    {
        int width = 0, height = 0;
        CGL_window_get_size((CGL_window*)framebuffer->user_data, &width, &height);
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle);
        glViewport(0, 0, framebuffer->width, framebuffer->height);
    }
}

// get framebuffer size
void CGL_framebuffer_get_size(CGL_framebuffer* framebuffer, int* width, int* height)
{
    if(framebuffer->is_default)
    {
        int w = 0, h = 0;
        CGL_window_get_framebuffer_size((CGL_window*)framebuffer->user_data, &w, &h);
        if(width)   *width = w;
        if(height)  *height = h;
    }
    else    
    {
        if(width)   *width = framebuffer->width;
        if(height)  *height = framebuffer->height;
    }
}

// set framebuffer user data
void CGL_framebuffer_set_user_data(CGL_framebuffer* framebuffer, void* user_data)
{
    if(!framebuffer->is_default)
        framebuffer->user_data = user_data;
}

// get framebuffer user data
void* CGL_framebuffer_get_user_data(CGL_framebuffer* framebuffer)
{
    if(!framebuffer->is_default)
        return framebuffer->user_data;
    return NULL;
}

// read pixels from framebuffer
void CGL_framebuffer_read_pixels(CGL_framebuffer* framebuffer, int x, int y, int width, int height, void* pixels)
{    
    if(framebuffer->is_default)
        return;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle);
    glReadPixels(x, y, width, height, framebuffer->color_texture->format, framebuffer->color_texture->type, pixels);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);    
}
 
// get mouse pick id
int CGL_framebuffer_get_mouse_pick_id(CGL_framebuffer* framebuffer, int x, int y, int index)
{
    if(index >= 3)
        return -1;
    if(framebuffer->is_default)
        return 0;
    int id = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + 1 + index);
    glReadPixels(x, framebuffer->height - y, 1, 1, GL_RED_INTEGER, GL_INT, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return id;
}

// get color texture
CGL_texture* CGL_framebuffer_get_color_texture(CGL_framebuffer* framebuffer)
{
    if(framebuffer->is_default)
        return NULL;
    return framebuffer->color_texture;
}

// get depth texture
CGL_texture* CGL_framebuffer_get_depth_texture(CGL_framebuffer* framebuffer)
{
    if(framebuffer->is_default)
        return NULL;
    return framebuffer->depth_texture;
}

// ssbo

struct CGL_ssbo
{
    GLuint handle;
    size_t size;
    int binding;
    void* user_data;
};

// create ssbo
CGL_ssbo* CGL_ssbo_create(uint32_t binding)
{
    CGL_ssbo* ssbo = (CGL_ssbo*)malloc(sizeof(CGL_ssbo));
    if(ssbo == NULL)
        return NULL;    
    glGenBuffers(1, &ssbo->handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->handle);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo->handle);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    ssbo->binding = binding;
    ssbo->user_data = NULL;
    ssbo->size = 0;
    return ssbo;
}

// destroy ssbo
void CGL_ssbo_destroy(CGL_ssbo* ssbo)
{
    glDeleteBuffers(1, &ssbo->handle);
    free(ssbo);
}

// bind ssbo
void CGL_ssbo_bind(CGL_ssbo* ssbo)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->handle);
}

// set ssbo data
void CGL_ssbo_set_data(CGL_ssbo* ssbo, size_t size, void* data, bool static_draw)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->handle);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, static_draw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    ssbo->size = size;
}

// set ssbo sub data
void CGL_ssbo_set_sub_data(CGL_ssbo* ssbo, size_t offset, size_t size, void* data, bool static_draw)
{
    if(offset + size > ssbo->size)
    {
        CGL_LOG("CGL_ssbo_set_sub_data: offset + size > ssbo->size");
        return;
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->handle);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// get ssbo data
void CGL_ssbo_get_data(CGL_ssbo* ssbo, size_t* size, void* data)
{
    if(size)
        *size = ssbo->size;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->handle);
    glGetNamedBufferSubData(ssbo->handle, 0, ssbo->size, data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// get ssbo sub data
void CGL_ssbo_get_sub_data(CGL_ssbo* ssbo, size_t offset, size_t size, void* data)
{
    if(offset + size > ssbo->size)
    {
        CGL_LOG("CGL_ssbo_get_sub_data: offset + size > ssbo->size");
        return;
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->handle);
    glGetNamedBufferSubData(ssbo->handle, offset, size, data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// set ssbo user data
void CGL_ssbo_set_user_data(CGL_ssbo* ssbo, void* user_data)
{
    ssbo->user_data = user_data;
}

// get ssbo user data
void* CGL_ssbo_get_user_data(CGL_ssbo* ssbo)
{
    return ssbo->user_data;
}

// get ssbo size
size_t CGL_ssbo_get_size(CGL_ssbo* ssbo)
{
    return ssbo->size;
}

// copy ssbo
void CGL_ssbo_copy(CGL_ssbo* dst, CGL_ssbo* src, size_t src_offset, size_t dst_offset, size_t size)
{
    if(dst_offset + size > dst->size)
    {
        CGL_LOG("CGL_ssbo_copy: dst_offset + size > dst->size");
        return;
    }
    if(src_offset + size > src->size)
    {
        CGL_LOG("CGL_ssbo_copy: src_offset + size > src->size");
        return;
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dst->handle);
    glCopyNamedBufferSubData(src->handle, dst->handle, src_offset, dst_offset, size);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// gl

// clear 
void CGL_gl_clear(float r, float g, float b, float a)
{    
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// load opengl functions
bool CGL_gl_init()
{
    bool result = gladLoadGL();
    if(!result)
    {
        CGL_LOG("Failed to load OpenGL functions");
    }
    return result;
}

// clean up
void CGL_gl_shutdown()
{

}

// mesh

struct CGL_mesh_gpu
{
    GLuint vertex_array;
    GLuint vertex_buffer;
    GLuint index_buffer;
    size_t index_count;
    size_t vertex_count;
    void* user_data;
};

// create mesh (gpu)
CGL_mesh_gpu* CGL_mesh_gpu_create()
{
    CGL_mesh_gpu* mesh = (CGL_mesh_gpu*)malloc(sizeof(CGL_mesh_gpu));
    if(mesh == NULL)
        return NULL;
    glGenVertexArrays(1, &mesh->vertex_array);
    glBindVertexArray(mesh->vertex_array);
    glGenBuffers(1, &mesh->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer);
    glGenBuffers(1, &mesh->index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer);
    mesh->index_count = 0;
    mesh->vertex_count = 0;
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(CGL_mesh_vertex), (void*)offsetof(CGL_mesh_vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(CGL_mesh_vertex), (void*)offsetof(CGL_mesh_vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(CGL_mesh_vertex), (void*)offsetof(CGL_mesh_vertex, texture_coordinates));
    glEnableVertexAttribArray(2);
    mesh->user_data = NULL;    
    return mesh;
}

// destroy mesh (gpu)
void CGL_mesh_gpu_destroy(CGL_mesh_gpu* mesh)
{
    glDeleteBuffers(1, &mesh->index_buffer);
    glDeleteBuffers(1, &mesh->vertex_buffer);
    glDeleteVertexArrays(1, &mesh->vertex_array);
    free(mesh);
}

// bind mesh (gpu)
void CGL_mesh_gpu_render(CGL_mesh_gpu* mesh)
{
    if(mesh->index_count <= 0)
        return;
    glBindVertexArray(mesh->vertex_array);
    glDrawElements(GL_TRIANGLES, (GLsizei)mesh->index_count, GL_UNSIGNED_INT, 0);
}

// render mesh instanfced (gpu)
void CGL_mesh_gpu_render_instanced(CGL_mesh_gpu* mesh, uint32_t count)
{
    if(mesh->index_count <= 0)
        return;
    glBindVertexArray(mesh->vertex_array);
    glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)mesh->index_count, GL_UNSIGNED_INT, 0, count);
}

// upload mesh from (cpu) to (gpu)
void CGL_mesh_gpu_upload(CGL_mesh_gpu* mesh, CGL_mesh_cpu* mesh_cpu, bool static_draw)
{
    if(mesh_cpu->index_count <= 0)
        return;
    mesh->index_count = mesh_cpu->index_count;
    mesh->vertex_count = mesh_cpu->vertex_count;
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh_cpu->vertex_count * sizeof(CGL_mesh_vertex), mesh_cpu->vertices, static_draw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_cpu->index_count * sizeof(unsigned int), mesh_cpu->indices, static_draw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
}

// set mesh user data
void CGL_mesh_gpu_set_user_data(CGL_mesh_gpu* mesh, void* user_data)
{
    mesh->user_data = user_data;
}

// get mesh user data
void* CGL_mesh_gpu_get_user_data(CGL_mesh_gpu* mesh)
{
    return mesh->user_data;
}

// destroy mesh (cpu)
void CGL_mesh_cpu_destroy(CGL_mesh_cpu* mesh)
{
    if(mesh->vertices)
        free(mesh->vertices);
    if(mesh->indices)
        free(mesh->indices);
    free(mesh);
}

// create mesh (cpu)
CGL_mesh_cpu* CGL_mesh_cpu_create(size_t vertex_count, size_t index_count)
{
    CGL_mesh_cpu* mesh = (CGL_mesh_cpu*)malloc(sizeof(CGL_mesh_cpu));
    if(mesh == NULL)
        return NULL;
    mesh->vertex_count = vertex_count;
    mesh->index_count = index_count;
    mesh->vertices = (CGL_mesh_vertex*)malloc(mesh->vertex_count * sizeof(CGL_mesh_vertex));
    if(mesh->vertices == NULL)
    {
        free(mesh);
        return NULL;
    }
    mesh->indices = (uint32_t*)malloc(mesh->index_count * sizeof(uint32_t));
    if(mesh->indices == NULL)
    {
        free(mesh->vertices);
        free(mesh);
        return NULL;
    }
    return mesh;
}

void __CGL_mesh_cpu_load_obj_helper_parse_obj_line(char* line, float* items, int count)
{
    char* begin = line;
    char* end = line + strlen(line);
    int i = 0, j = 0;
    while ((begin < end) && (i < count))
    {
        if (line[j++] == ' ')
        {
            line[j - 1] = '\0';
            items[i++] = (float)atof(begin);
            begin = line + j;
        }
    }
}

CGL_mesh_cpu* CGL_mesh_cpu_load_obj(const char* path)
{
    char temp_buffer[1024];
    CGL_list* vertex_positions = CGL_list_create(sizeof(float) * 4, 1000);
    CGL_list* vertex_normals = CGL_list_create(sizeof(float) * 4, 1000);
    CGL_list* vertex_texture_coordinates = CGL_list_create(sizeof(float) * 4, 1000);
    CGL_list* vertices = CGL_list_create(sizeof(CGL_mesh_vertex), 1000);
    CGL_list* indices = CGL_list_create(sizeof(uint32_t), 1000);
    size_t file_size = 0;
    char* file_data = CGL_utils_read_file(path, &file_size);
    if(file_size == 0) return NULL;
    char* line = strtok(file_data, "\n");
    int object_count = 0;
    float item_data[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    uint32_t index = 0;
    while(line != NULL)
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            sprintf(temp_buffer, "%s ", (line + 2));
            __CGL_mesh_cpu_load_obj_helper_parse_obj_line(temp_buffer, item_data, 3);
            CGL_list_push(vertex_positions, item_data);
        }
        else if (line[0] == 'v' && line[1] == 'n')
        {
            sprintf(temp_buffer, "%s ", (line + 3));
            __CGL_mesh_cpu_load_obj_helper_parse_obj_line(temp_buffer, item_data, 3);
            CGL_list_push(vertex_normals, item_data);
        }
        else if (line[0] == 'v' && line[1] == 't')
        {
            sprintf(temp_buffer, "%s ", (line + 3));
            __CGL_mesh_cpu_load_obj_helper_parse_obj_line(temp_buffer, item_data, 2);
            CGL_list_push(vertex_texture_coordinates, item_data);
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            sprintf(temp_buffer, "%s ", (line + 2));
            uint32_t index_v_vt_vn[4][3];
            memset(index_v_vt_vn, 0, sizeof(index_v_vt_vn));
            char* begin = temp_buffer;
            char* end = temp_buffer + strlen(temp_buffer);
            int i = 0, j = 0, k = 0;
            while ((begin < end) && (i < 3))
            {
                if (temp_buffer[j] == '/' || temp_buffer[j] == ' ')
                {
                    char c = temp_buffer[j];
                    temp_buffer[j] = '\0';
                    index_v_vt_vn[i][k++] = (uint32_t)atoll(begin);
                    begin = temp_buffer + j + 1;
                    if (c == ' ') { k = 0; i++; }
                }
                j++;
            }            
            CGL_mesh_vertex current_vertex;
            for (int i = 0; i < 3; i++)
            {
                if (!CGL_list_get(vertex_positions, index_v_vt_vn[i][0] - 1, &current_vertex.position))
                    current_vertex.position = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
                if (!CGL_list_get(vertex_texture_coordinates, index_v_vt_vn[i][1] - 1, &current_vertex.texture_coordinates))
                    current_vertex.texture_coordinates= CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
                if (!CGL_list_get(vertex_normals, index_v_vt_vn[i][2] - 1, &current_vertex.normal))
                    current_vertex.normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
                CGL_list_push(vertices, &current_vertex);
            }

            CGL_list_push(indices, &index); index++;
            CGL_list_push(indices, &index); index++;
            CGL_list_push(indices, &index); index++;            
        }
        else if (line[0] == 'o' && line[1] == ' ')
            object_count++;
        line = strtok(NULL, "\n");
        if(object_count == 2) break; // only one object is parsed as of now
    }

    size_t index_count = CGL_list_get_size(indices);
    size_t vertex_count = CGL_list_get_size(vertices);
    CGL_mesh_cpu* mesh = CGL_mesh_cpu_create(vertex_count, index_count);
    memcpy(mesh->indices, CGL_list_get(indices, 0, NULL), sizeof(uint32_t) * index_count);
    memcpy(mesh->vertices, CGL_list_get(vertices, 0, NULL), sizeof(CGL_mesh_vertex) * vertex_count);
    CGL_free(file_data);
    CGL_list_destroy(vertices);
    CGL_list_destroy(indices);
    CGL_list_destroy(vertex_positions);
    CGL_list_destroy(vertex_normals);
    CGL_list_destroy(vertex_texture_coordinates);
    return mesh;
}

// generate triangle mesh
CGL_mesh_cpu* CGL_mesh_cpu_triangle(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
{
    CGL_mesh_cpu* mesh = CGL_mesh_cpu_create(3, 3);
    if(mesh == NULL)
        return NULL;
    CGL_vec3 ab = CGL_vec3_sub(b, a);
    CGL_vec3 ac = CGL_vec3_sub(c, a);
    CGL_vec3 normal = CGL_vec3_cross(ab, ac);
    CGL_vec4 normal_4 = CGL_vec4_init(normal.x, normal.y, normal.z, 0.0f);
    mesh->vertices[0].position = CGL_vec4_init(a.x, a.y, a.z, 1.0f);
    mesh->vertices[0].normal = normal_4;
    mesh->vertices[0].texture_coordinates = CGL_vec4_init(0.0f, 0.0f, 0.0f, 1.0f);
    mesh->vertices[1].position = CGL_vec4_init(b.x, b.y, b.z, 1.0f);
    mesh->vertices[1].normal = normal_4;
    mesh->vertices[1].texture_coordinates = CGL_vec4_init(0.0f, 0.0f, 0.0f, 1.0f);
    mesh->vertices[2].position = CGL_vec4_init(c.x, c.y, c.z, 1.0f);
    mesh->vertices[2].normal = normal_4;
    mesh->vertices[2].texture_coordinates = CGL_vec4_init(0.0f, 0.0f, 0.0f, 1.0f);
    mesh->indices[0] = 0;
    mesh->indices[1] = 1;
    mesh->indices[2] = 2;
    return mesh;
}

// generate quad mesh
CGL_mesh_cpu* CGL_mesh_cpu_quad(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d)
{
    CGL_mesh_cpu* mesh = CGL_mesh_cpu_create(4, 6);
    if(mesh == NULL)
        return NULL;
    CGL_vec3 ab = CGL_vec3_sub(b, a);
    CGL_vec3 ac = CGL_vec3_sub(c, a);
    // CGL_vec3 ad = CGL_vec3_sub(d, a);
    CGL_vec3 normal_abc = CGL_vec3_cross(ab, ac);
    CGL_vec4 normal_abc_4 = CGL_vec4_init(normal_abc.x, normal_abc.y, normal_abc.z, 0.0f);


    mesh->vertices[0].position = CGL_vec4_init(a.x, a.y, a.z, 1.0f);
    mesh->vertices[0].normal = normal_abc_4;
    mesh->vertices[0].texture_coordinates = CGL_vec4_init(1.0f, 1.0f, 0.0f, 1.0f);
    mesh->vertices[1].position = CGL_vec4_init(b.x, b.y, b.z, 1.0f);
    mesh->vertices[1].normal = normal_abc_4;
    mesh->vertices[1].texture_coordinates = CGL_vec4_init(1.0f, 0.0f, 0.0f, 1.0f);
    mesh->vertices[2].position = CGL_vec4_init(c.x, c.y, c.z, 1.0f);
    mesh->vertices[2].normal = normal_abc_4;
    mesh->vertices[2].texture_coordinates = CGL_vec4_init(0.0f, 0.0f, 0.0f, 1.0f);
    mesh->vertices[3].position = CGL_vec4_init(d.x, d.y, d.z, 1.0f);
    mesh->vertices[3].normal = normal_abc_4;
    mesh->vertices[3].texture_coordinates = CGL_vec4_init(0.0f, 1.0f, 0.0f, 1.0f);
    mesh->indices[0] = 0;
    mesh->indices[1] = 1;
    mesh->indices[2] = 3;
    mesh->indices[3] = 1;
    mesh->indices[4] = 2;
    mesh->indices[5] = 3;
    return mesh;
}

// shader

struct CGL_shader
{
    GLuint handle;
    void* user_data;
};

bool __CGL_shader_compile(const char* source, GLenum type, GLuint* handle, char** error)
{    
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)(&source), NULL);
    glCompileShader(shader);
    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE)
    {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        char* log = (char*)malloc(log_length);
        glGetShaderInfoLog(shader, log_length, NULL, log);
        CGL_LOG("%s\n", log);
        if(error)
            *error = log;
        else
            free(log);
        glDeleteShader(shader);
        return false;
    }
    *handle = shader;
    return true;
}

// create compute shader
CGL_shader* CGL_shader_compute_create(const char* compute_shader_source, char** error)
{
    if(error)
        *error = NULL;

    CGL_shader* shader = (CGL_shader*)malloc(sizeof(CGL_shader));
    if(shader == NULL)
        return NULL;
    shader->user_data = NULL;
    GLuint compute_shader = 0;
    if(!__CGL_shader_compile(compute_shader_source, GL_COMPUTE_SHADER, &compute_shader, error))
    {
        free(shader);
        return NULL;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, compute_shader);
    glLinkProgram(program);
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_FALSE)
    {
        GLint log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        char* log = (char*)malloc(log_length);
        glGetProgramInfoLog(program, log_length, NULL, log);
        CGL_LOG("%s\n", log);
        if(error)
            *error = log;
        else
            free(log);
        glDeleteProgram(program);
        glDeleteShader(compute_shader);
        free(shader);
        return NULL;
    }
    glDetachShader(program, compute_shader);
    glDeleteShader(compute_shader);
    shader->handle = program;
    return shader;
}

// create compute shader from files
CGL_shader* CGL_shader_compute_create_from_files(const char* compute_shader_file, char** error)
{
    char* compute_shader_source = CGL_utils_read_file(compute_shader_file, NULL);
    if(compute_shader_source == NULL)
    {
        CGL_LOG("Failed to read compute shader file %s\n", compute_shader_file);
        return NULL;
    }
    CGL_shader* shader = CGL_shader_compute_create(compute_shader_source, error);
    free(compute_shader_source);
    return shader;
}

// create shader
CGL_shader* CGL_shader_create(const char* vertex_shader_source, const char* fragment_shader_source, char** error)
{
    if(error)
        *error = NULL;
    CGL_shader* shader = (CGL_shader*)malloc(sizeof(CGL_shader));
    if(shader == NULL)
        return NULL;
    shader->user_data = NULL;

    GLuint vertex_shader = 0;
    GLuint fragment_shader = 0;
    if(!__CGL_shader_compile(vertex_shader_source, GL_VERTEX_SHADER, &vertex_shader, error))
    {
        free(shader);
        return NULL;
    }

    if(!__CGL_shader_compile(fragment_shader_source, GL_FRAGMENT_SHADER, &fragment_shader, error))
    {
        glDeleteShader(vertex_shader);
        free(shader);
        return NULL;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if(status == GL_FALSE)
    {
        GLint log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        char* log = (char*)malloc(log_length);
        glGetProgramInfoLog(program, log_length, NULL, log);
        CGL_LOG("%s\n", log);
        if(error)
            *error = log;
        else
            free(log);
        glDeleteProgram(program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        free(shader);
        return NULL;
    }

    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);    

    shader->handle = program;

    if(error)
        *error = NULL;
    return shader;
}

// create shader from files
CGL_shader* CGL_shader_create_from_files(const char* vertex_shader_file, const char* fragment_shader_file, char** error)
{
    char* vertex_shader_source = CGL_utils_read_file(vertex_shader_file, NULL);
    if(vertex_shader_source == NULL)
    {
        CGL_LOG("Failed to read vertex shader file %s\n", vertex_shader_file);
        return NULL;
    }
    char* fragment_shader_source = CGL_utils_read_file(fragment_shader_file, NULL);
    if(fragment_shader_source == NULL)
    {
        CGL_LOG("Failed to read fragment shader file %s\n", fragment_shader_file);
        free(vertex_shader_source);
        return NULL;
    }
    CGL_shader* shader = CGL_shader_create(vertex_shader_source, fragment_shader_source, error);
    free(vertex_shader_source);
    free(fragment_shader_source);
    return shader;
}

// destroy shader
void CGL_shader_destroy(CGL_shader* shader)
{
    glDeleteProgram(shader->handle);
    free(shader);
}

// dispatch compute shader
void CGL_shader_compute_dispatch(CGL_shader* shader, int x, int y, int z)
{
    glUseProgram(shader->handle);
    glDispatchCompute(x, y, z);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

// bind shader
void CGL_shader_bind(CGL_shader* shader)
{
    glUseProgram(shader->handle);
}

// get uniform location
int CGL_shader_get_uniform_location(CGL_shader* shader, const char* name)
{
    return glGetUniformLocation(shader->handle, name);
}

// set uniform bool
void CGL_shader_set_uniform_bool(CGL_shader* shader, int location, bool value)
{
    glUniform1i(location, value);
}

// set uniform matrix
void CGL_shader_set_uniform_mat4(CGL_shader* shader, int location, CGL_mat4* matrix)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat*)matrix);
}

// set uniform vector
void CGL_shader_set_uniform_vec4(CGL_shader* shader, int location, CGL_vec4* vector)
{
    glUniform4fv(location, 1, (GLfloat*)vector);
}

// set uniform vector
void CGL_shader_set_uniform_vec3(CGL_shader* shader, int location, CGL_vec3* vector)
{
    glUniform3fv(location, 1, (GLfloat*)vector);
}

// set uniform vector
void CGL_shader_set_uniform_vec2(CGL_shader* shader, int location, CGL_vec2* vector)
{
    glUniform2fv(location, 1, (GLfloat*)vector);
}

// set uniform int
void CGL_shader_set_uniform_int(CGL_shader* shader, int location, int value)
{
    glUniform1i(location, value);
}

// set uniform float
void CGL_shader_set_uniform_float(CGL_shader* shader, int location, float value)
{
    glUniform1f(location, value);
}

// set uniform vector
void CGL_shader_set_uniform_vec2v(CGL_shader* shader, int location, float x, float y)
{
    glUniform2f(location, x, y);
}

// set uniform vector
void CGL_shader_set_uniform_vec3v(CGL_shader* shader, int location, float x, float y, float z)
{
    glUniform3f(location, x, y, z);
}

// set uniform vector
void CGL_shader_set_uniform_vec4v(CGL_shader* shader, int location, float x, float y, float z, float w)
{
    glUniform4f(location, x, y, z, w);
}

void CGL_shader_set_uniform_ivec2v(CGL_shader* shader, int location, int x, int y)
{
    glUniform2i(location, x, y);
}

// set uniform vector
void CGL_shader_set_uniform_ivec3v(CGL_shader* shader, int location, int x, int y, int z)
{
    glUniform3i(location, x, y, z);
}

// set uniform vector
void CGL_shader_set_uniform_ivec4v(CGL_shader* shader, int location, int x, int y, int z, int w)
{
    glUniform4i(location, x, y, z, w);
}

// set shader user data
void CGL_shader_set_user_data(CGL_shader* shader, void* user_data)
{
    shader->user_data = user_data;
}

// get shader user data
void* CGL_shader_get_user_data(CGL_shader* shader)
{
    return shader->user_data;
}

#endif

// camera
#if 1

struct CGL_camera
{
    bool is_perspective;
    CGL_vec4 ortho_limits;
    CGL_vec3 position;
    CGL_vec3 rotation;
    CGL_vec3 front;
    CGL_vec3 right;
    CGL_vec3 up;
    float fov;
    float aspect;
    float z_near;
    float z_far;
    CGL_mat4 projection;
    CGL_mat4 view;
    CGL_mat4 pv;
};

CGL_camera* CGL_camera_create()
{
    CGL_camera* camera = (CGL_camera*)malloc(sizeof(CGL_camera));
    if(!camera) return NULL;
    camera->is_perspective = true;
    camera->ortho_limits = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    camera->position = CGL_vec3_init(0.0f, 0.0f, -1.0f);
    camera->rotation = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    camera->fov = 45.0f;
    camera->aspect = 1.0f;
    camera->z_near = 0.001f;
    camera->z_far = 1000.0f;
    return camera;
}

void CGL_camera_destroy(CGL_camera* camera)
{
    CGL_free(camera);
}

bool CGL_camera_is_perspective(CGL_camera* camera)
{
    return camera->is_perspective;
}

bool CGL_camera_is_orthographic(CGL_camera* camera)
{
    return !camera->is_perspective;
}

float CGL_camera_get_fov(CGL_camera* camera)
{
    return camera->fov;
}

float CGL_camera_get_aspect_ratio(CGL_camera* camera)
{
    return camera->aspect;
}

float CGL_camera_get_z_near(CGL_camera* camera)
{
    return camera->z_near;
}

float CGL_camera_get_z_far(CGL_camera* camera)
{
    return camera->z_far;
}

CGL_vec4 CGL_camera_get_orthographic_limits(CGL_camera* camera)
{
    return camera->ortho_limits;
}

void CGL_camera_set_fov(CGL_camera* camera, float fov)
{
    camera->fov = fov;
}

void CGL_camera_set_aspect_ratio(CGL_camera* camera, float aspect_ratio)
{
    if(fabs(aspect_ratio) < 0.001f || fabs(aspect_ratio) > 100.0f )aspect_ratio = 1.0f;
    camera->aspect = aspect_ratio;
}

void CGL_camera_set_z_near(CGL_camera* camera, float z_near)
{
    camera->z_near = z_near;
}

void CGL_camera_set_z_far(CGL_camera* camera, float z_far)
{
    camera->z_far = z_far;
}

void CGL_camera_set_orthographic_limits(CGL_camera* camera, CGL_vec4 ortho_limits)
{
    camera->ortho_limits = ortho_limits;
}

void CGL_camera_set_perspective(CGL_camera* camera)
{
    camera->is_perspective = true;
}

void CGL_camera_set_orthographic(CGL_camera* camera)
{
    camera->is_perspective = false;
}

CGL_mat4 CGL_camera_get_projection_mat(CGL_camera* camera)
{
    return camera->projection;
}

CGL_mat4* CGL_camera_get_projection_mat_ptr(CGL_camera* camera)
{
    return &camera->projection;
}

CGL_mat4 CGL_camera_get_view_mat(CGL_camera* camera)
{
    return camera->view;
}

CGL_mat4* CGL_camera_get_view_mat_ptr(CGL_camera* camera)
{
    return &camera->view;
}

CGL_mat4 CGL_camera_get_pv_mat(CGL_camera* camera)
{
    return camera->pv;
}

CGL_mat4* CGL_camera_get_pv_mat_ptr(CGL_camera* camera)
{
    return &camera->pv;
}

void CGL_camera_set_position(CGL_camera* camera, CGL_vec3 position)
{
    camera->position = position;
}

void CGL_camera_set_rotation(CGL_camera* camera, CGL_vec3 rotation)
{
    camera->rotation = rotation;
}

CGL_vec3 CGL_camera_get_position(CGL_camera* camera)
{
    return camera->position;
}

CGL_vec3 CGL_camera_get_rotation(CGL_camera* camera)
{
    return camera->rotation;
}

CGL_vec3* CGL_camera_get_position_ptr(CGL_camera* camera)
{
    return &camera->position;
}

CGL_vec3* CGL_camera_get_rotation_ptr(CGL_camera* camera)
{
    return &camera->rotation;
}

CGL_vec3 CGL_camera_get_front(CGL_camera* camera)
{
    return camera->front;    
}

CGL_vec3 CGL_camera_get_right(CGL_camera* camera)
{
    return camera->right;    
}

CGL_vec3 CGL_camera_get_up(CGL_camera* camera)
{
    return camera->up;    
}

void CGL_camera_recalculate_mat(CGL_camera* camera)
{
    if(camera->is_perspective)
        camera->projection = CGL_mat4_perspective(camera->aspect, camera->fov, camera->z_near, camera->z_far);
    else
    {printf("Ortho graphic projections not yet supported!\n");CGL_exit(EXIT_FAILURE);}
    
    CGL_vec3 z_axis = CGL_vec3_init(0.0f, 0.0f, 1.0f);
    z_axis = CGL_vec3_rotate_x(z_axis, camera->rotation.x);
    z_axis = CGL_vec3_rotate_y(z_axis, camera->rotation.y);
    z_axis = CGL_vec3_rotate_z(z_axis, camera->rotation.z);
    camera->front = z_axis;

    CGL_vec3 x_axis = CGL_vec3_init(1.0f, 0.0f, 0.0f);
    x_axis = CGL_vec3_rotate_x(x_axis, camera->rotation.x);
    x_axis = CGL_vec3_rotate_y(x_axis, camera->rotation.y);
    x_axis = CGL_vec3_rotate_z(x_axis, camera->rotation.z);
    camera->right = x_axis;

    CGL_vec3 y_axis = CGL_vec3_cross(z_axis, x_axis);
    camera->up = y_axis;

    camera->view.m[0] = x_axis.x;
    camera->view.m[1] = x_axis.y;
    camera->view.m[2] = x_axis.z;
    camera->view.m[3] = -1.0f * CGL_vec3_dot(camera->position, x_axis);
    camera->view.m[4] = y_axis.x;
    camera->view.m[5] = y_axis.y;
    camera->view.m[6] = y_axis.z;
    camera->view.m[7] = -1.0f * CGL_vec3_dot(camera->position, y_axis);
    camera->view.m[8] = z_axis.x;
    camera->view.m[9] = z_axis.y;
    camera->view.m[10] = z_axis.z;
    camera->view.m[11] = -1.0f * CGL_vec3_dot(camera->position, z_axis);
    camera->view.m[12] = 0.0f;
    camera->view.m[13] = 0.0f;
    camera->view.m[14] = 0.0f;
    camera->view.m[15] = 1.0f;

    camera->pv = CGL_mat4_mul(camera->projection, camera->view);
}

#endif


// The phong renderer
#if 1
#ifndef CGL_EXCLUDE_PHONG_RENDERER


struct CGL_phong_mat
{
    bool use_diffuse_texture;
    CGL_vec3 diffuse_color;
    CGL_texture* diffuse_texture;
    CGL_image* diffuse_image;    
    bool use_specular_texture;
    CGL_vec3 specular_color;
    CGL_texture* specular_texture;
    CGL_image* specular_image;
    bool use_normal_map;
    CGL_texture* normal_map_texture;
    CGL_image* normal_map_image;
    float shininess;    
    bool wireframe;
    void* user_data;
};

struct CGL_phong_pipeline
{
    bool use_blinn;
    CGL_shader* shader;
    CGL_phong_light* lights[CGL_PHONG_MAX_LIGHTS];
    CGL_vec3 ambient_light_color;
    float ambient_light_strength;
    uint32_t light_count;
    bool use_gamma_correction;
    bool depth_testing;
    void* user_data;
    // uniform locations
    int u_lights_v4[4][CGL_PHONG_MAX_LIGHTS];
    int u_projection;
    int u_view;
    int u_pv;
    int u_model_matrix;
    int u_light_count;
    int u_use_diffuse_texture;
    int u_diffuse_color;
    int u_diffuse_texture;
    int u_use_specular_texture;
    int u_specular_color;
    int u_specular_texture;
    int u_use_normal_map;
    int u_normal_map_texture;
    int u_shininess;
    int u_camera_position;
    int u_use_blinn;
    int u_ambient_light_color;
    int u_ambient_light_strength;
    int u_use_gamma_correction;
};

struct CGL_phong_light
{
    uint32_t light_type;
    CGL_vec3 vector;
    CGL_vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
    void* user_data;
};

static const char* __CGL_PHONG_VERTEX_SHADER = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"out vec3 Position;\n"
"out vec3 Normal;\n"
"out vec2 TexCoord; \n"
"\n"
"uniform mat4 u_projection;\n"
"uniform mat4 u_view;\n"
"uniform mat4 u_pv;\n"
"uniform mat4 u_model_matrix;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = u_projection * transpose(u_view * u_model_matrix) * vec4(position.xyz, 1.0f);\n"
"	Position = (transpose(u_model_matrix) * position).xyz;\n"
"	Normal = normal.xyz;\n"
"	TexCoord = texcoord.xy;		\n"
"}";


static const char* __CGL_PHONG_FRAGMENT_SHADER = "#version 430 core\n"
"\n"
"#define MAX_LIGHTS             16\n"
"\n"
"#define LIGHT_TYPE_DIRECTIONAL      0.5f\n"
"#define LIGHT_TYPE_POINT            1.5f\n"
"#define LIGHT_TYPE_SPOT             2.5f\n"
"#define LIGHT_TYPE(index)      u_lights_data_1[index].x\n"
"#define LIGHT_CONSTANT(index ) u_lights_data_1[index].y\n"
"#define LIGHT_LINEAR(index)    u_lights_data_1[index].z\n"
"#define LIGHT_QUADRATIC(index) u_lights_data_1[index].w\n"
"#define LIGHT_COLOR(index)     vec4(u_lights_data_0[index].xyz, 1.0f)\n"
"#define LIGHT_INTENSITY(index) u_lights_data_0[index].w\n"
"#define LIGHT_VECTOR(index)    u_lights_data_2[index].xyz\n"
"\n"
"out vec4 FragColor;\n"
"//out int MousePick0;\n"
"//out int MousePick1;\n"
"//out int MousePick2;\n"
"in vec3 Position;\n"
"in vec3 Normal;\n"
"in vec2 TexCoord;\n"
"\n"
"\n"
"// unifroms\n"
"uniform vec4 u_lights_data_0[MAX_LIGHTS];\n"
"uniform vec4 u_lights_data_1[MAX_LIGHTS];\n"
"uniform vec4 u_lights_data_2[MAX_LIGHTS];\n"
"uniform vec4 u_lights_data_3[MAX_LIGHTS];\n"
"\n"
"uniform mat4 u_projection;\n"
"uniform mat4 u_view;\n"
"uniform mat4 u_pv;\n"
"uniform mat4 u_model_matrix;\n"
"uniform int u_light_count;\n"
"uniform bool u_use_diffuse_texture;\n"
"uniform vec3 u_diffuse_color;\n"
"uniform sampler2D u_diffuse_texture;\n"
"uniform bool u_use_specular_texture;\n"
"uniform vec3 u_specular_color;\n"
"uniform sampler2D u_specular_texture;\n"
"uniform bool u_use_normal_map;\n"
"uniform sampler2D u_normal_map_texture;\n"
"uniform float u_shininess;\n"
"uniform vec3 u_camera_position;\n"
"uniform bool u_use_blinn;\n"
"uniform vec3 u_ambient_light_color;\n"
"uniform float u_ambient_light_strength;\n"
"uniform bool u_use_gamma_correction;\n"
"\n"
"vec4 get_material_diffuse_color()\n"
"{\n"
"    if(u_use_diffuse_texture)\n"
"        return texture(u_diffuse_texture, TexCoord);\n"
"    return vec4(u_diffuse_color, 1.0f);\n"
"}\n"
"\n"
"vec4 get_material_specular_color()\n"
"{\n"
"    if(u_use_specular_texture)\n"
"        return texture(u_specular_texture, TexCoord);\n"
"    return vec4(u_specular_color, 1.0f);\n"
"}\n"
"\n"
"vec4 calculate_directional_light(int index)\n"
"{\n"
"    vec3 light_direcion = normalize(-LIGHT_VECTOR(index));\n"
"    // diffuse shading\n"
"    float diff = max(dot(Normal, light_direcion), 0.0f);\n"
"    // specular shading\n"
"    float spec = 0.0f;\n"
"    if(u_use_blinn)\n"
"    {\n"
"        vec3 halfway_direction = normalize(light_direcion + u_camera_position);\n"
"        spec = pow(max(dot(u_camera_position, halfway_direction), 0.0f), u_shininess);\n"
"    }\n"
"    else\n"
"    {\n"
"        vec3 reflect_direction = reflect(-light_direcion, Normal);\n"
"        spec = pow(max(dot(u_camera_position, reflect_direction), 0.0f), u_shininess);\n"
"    }\n"
"    vec4 material_diffuse = get_material_diffuse_color();\n"
"    vec4 material_specular = get_material_specular_color();\n"
"    vec4 ambient_lighting = vec4(u_ambient_light_color * u_ambient_light_strength, 1.0f);\n"
"    vec4 diffuse_lighting = LIGHT_COLOR(index) * diff * material_diffuse;\n"
"    vec4 specular_lighting = LIGHT_COLOR(index) * spec * material_specular;\n"
"    return (ambient_lighting + diffuse_lighting + specular_lighting) * LIGHT_INTENSITY(index);\n"
"}\n"
"\n"
"vec4 calculate_point_light(int index)\n"
"{\n"
"    vec3 light_direcion = normalize(LIGHT_VECTOR(index) - Position);\n"
"    // diffuse shading\n"
"    float diff = max(dot(Normal, light_direcion), 0.0f);\n"
"    // specular shading\n"
"    float spec = 0.0f;\n"
"    if(u_use_blinn)\n"
"    {\n"
"        vec3 halfway_direction = normalize(light_direcion + u_camera_position);\n"
"        spec = pow(max(dot(u_camera_position, halfway_direction), 0.0f), u_shininess);\n"
"    }\n"
"    else\n"
"    {\n"
"        vec3 reflect_direction = reflect(-light_direcion, Normal);\n"
"        spec = pow(max(dot(u_camera_position, reflect_direction), 0.0f), u_shininess);\n"
"    }\n"
"\n"
"    vec4 material_diffuse = get_material_diffuse_color();\n"
"    vec4 material_specular = get_material_specular_color();\n"
"    // attrnuation\n"
"    float distance = length(LIGHT_VECTOR(index) - Position);\n"
"    float attenuation = 1.0f / ( LIGHT_CONSTANT(index) + LIGHT_LINEAR(index) * distance + LIGHT_QUADRATIC(index) * (distance * distance));\n"
"    vec4 ambient_lighting = vec4(u_ambient_light_color * u_ambient_light_strength, 1.0f);\n"
"    vec4 diffuse_lighting = LIGHT_COLOR(index) * diff * material_diffuse;\n"
"    vec4 specular_lighting = LIGHT_COLOR(index) * spec * material_specular;\n"
"    return (ambient_lighting + diffuse_lighting + specular_lighting) * attenuation * LIGHT_INTENSITY(index);\n"
"}\n"
"\n"
"vec4 calculate_spot_light(int index)\n"
"{\n"
"    return vec4(0.0f);\n"
"}\n"
"\n"
"\n"
"void main()\n"
"{\n"
"    vec4 color = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
"    vec4 light_output = vec4(0.0f);\n"
"    for ( int i = 0 ; i < u_light_count ; i++)\n"
"    {\n"
"        if(LIGHT_TYPE(i) < LIGHT_TYPE_DIRECTIONAL)\n"
"            light_output += calculate_directional_light(i);\n"
"        else if(LIGHT_TYPE(i) < LIGHT_TYPE_POINT)\n"
"            light_output += calculate_point_light(i);\n"
"        else if(LIGHT_TYPE(i) < LIGHT_TYPE_SPOT)        \n"
"            light_output += calculate_spot_light(i);\n"
"    }\n"
"\n"
"    if(u_use_gamma_correction)\n"
"        color = vec4(pow(light_output.xyz, vec3(0.4545f)), light_output.w);\n"
"    else\n"
"        color = light_output;\n"
"\n"
"    FragColor = color;\n"
"    //MousePick0 = InstanceID;\n"
"    //MousePick1 = 0;\n"
"    //MousePick2 = 1;\n"
"}";

// pipeline_create
CGL_phong_pipeline* CGL_phong_pipeline_create()
{
    CGL_phong_pipeline* pipeline = (CGL_phong_pipeline*)malloc(sizeof(CGL_phong_pipeline));
    pipeline->use_blinn = false;
    memset(pipeline->lights, 0, sizeof(pipeline->lights));
    pipeline->light_count = 0;
    pipeline->ambient_light_color = CGL_vec3_init(0.18f, 0.18f, 0.18f);
    pipeline->ambient_light_strength = 1.0f;
    pipeline->use_gamma_correction = true;
    pipeline->depth_testing = true;
    pipeline->user_data = NULL;
    pipeline->shader = CGL_shader_create(__CGL_PHONG_VERTEX_SHADER, __CGL_PHONG_FRAGMENT_SHADER, NULL);
    // Load uniforms
    pipeline->u_projection = CGL_shader_get_uniform_location(pipeline->shader, "u_projection");
    pipeline->u_view = CGL_shader_get_uniform_location(pipeline->shader, "u_view");
    pipeline->u_pv = CGL_shader_get_uniform_location(pipeline->shader, "u_pv");
    pipeline->u_light_count = CGL_shader_get_uniform_location(pipeline->shader, "u_light_count");
    pipeline->u_use_diffuse_texture = CGL_shader_get_uniform_location(pipeline->shader, "u_use_diffuse_texture");
    pipeline->u_diffuse_color = CGL_shader_get_uniform_location(pipeline->shader, "u_diffuse_color");
    pipeline->u_diffuse_texture = CGL_shader_get_uniform_location(pipeline->shader, "u_diffuse_texture");
    pipeline->u_use_specular_texture = CGL_shader_get_uniform_location(pipeline->shader, "u_use_specular_texture");
    pipeline->u_specular_color = CGL_shader_get_uniform_location(pipeline->shader, "u_specular_color");
    pipeline->u_specular_texture = CGL_shader_get_uniform_location(pipeline->shader, "u_specular_texture");
    pipeline->u_use_normal_map = CGL_shader_get_uniform_location(pipeline->shader, "u_use_normal_map");
    pipeline->u_normal_map_texture = CGL_shader_get_uniform_location(pipeline->shader, "u_normal_map_texture");
    pipeline->u_shininess = CGL_shader_get_uniform_location(pipeline->shader, "u_shininess");
    pipeline->u_camera_position = CGL_shader_get_uniform_location(pipeline->shader, "u_camera_position");
    pipeline->u_use_blinn = CGL_shader_get_uniform_location(pipeline->shader, "u_use_blinn");
    pipeline->u_model_matrix = CGL_shader_get_uniform_location(pipeline->shader, "u_model_matrix");
    pipeline->u_ambient_light_color = CGL_shader_get_uniform_location(pipeline->shader, "u_ambient_light_color");
    pipeline->u_ambient_light_strength = CGL_shader_get_uniform_location(pipeline->shader, "u_ambient_light_strength");
    pipeline->u_use_gamma_correction = CGL_shader_get_uniform_location(pipeline->shader, "u_use_gamma_correction");
    static char temp_buffer[256];
    for(int i = 0 ; i < 4 ; i ++)
    {
        for(int j = 0 ; j < CGL_PHONG_MAX_LIGHTS ; j ++)
        {
            sprintf(temp_buffer, "u_lights_data_%d[%d]", i, j);
            pipeline->u_lights_v4[i][j] = CGL_shader_get_uniform_location(pipeline->shader, temp_buffer);
        }
    }
    return pipeline;
}

// pipeline_destroy
void CGL_phong_pipeline_destroy(CGL_phong_pipeline* pipeline)
{
    CGL_shader_destroy(pipeline->shader);
    for(int i = 0 ; i < CGL_utils_array_size(pipeline->lights); i++)
        if(pipeline->lights[i])
            CGL_phong_light_destroy(pipeline->lights[i]);
    CGL_free(pipeline);
}

void CGL_phong_pipeline_set_ambient_light_color(CGL_phong_pipeline* pipeline, CGL_vec3 color)
{
    pipeline->ambient_light_color = color;
}

CGL_vec3 CGL_phong_pipeline_get_ambient_light_color(CGL_phong_pipeline* pipeline)
{
    return pipeline->ambient_light_color;
}

void CGL_phong_pipeline_enable_gamma_correction(CGL_phong_pipeline* pipeline)
{
    pipeline->use_gamma_correction = true;
}

void CGL_phong_pipeline_disable_gamma_correction(CGL_phong_pipeline* pipeline)
{
    pipeline->use_gamma_correction = false;
}

void CGL_phong_pipeline_set_ambient_light_strength(CGL_phong_pipeline* pipeline, float strength)
{
    pipeline->ambient_light_strength = CGL_utils_clamp(strength, 0.0f, 1000.0f);
}

float CGL_phong_pipeline_get_ambient_light_strength(CGL_phong_pipeline* pipeline)
{
    return pipeline->ambient_light_strength;
}

// pipeline_set_user_data
void CGL_phong_pipeline_set_user_data(CGL_phong_pipeline* pipeline, void* data)
{
    pipeline->user_data = data;
}

// pipeline_get_user_data
void* CGL_phong_pipeline_get_user_data(CGL_phong_pipeline* pipeline)
{
    return pipeline->user_data;
}

// pipeline_is_using_blinn
bool CGL_phong_pipeline_is_using_blinn(CGL_phong_pipeline* pipeline)
{
    return pipeline->use_blinn;
}

// pipeline_enable_blinn
void CGL_phong_pipeline_enable_blinn(CGL_phong_pipeline* pipeline)
{
    pipeline->use_blinn = true;
}

// pipeline_disable_blinn
void CGL_phong_pipeline_disable_blinn(CGL_phong_pipeline* pipeline)
{
    pipeline->use_blinn = false;
}

// pipeline_add_light
uint32_t CGL_phong_pipeline_add_light(CGL_phong_pipeline* pipeline, CGL_phong_light* light)
{
    for(int i = 0 ; i < CGL_utils_array_size(pipeline->lights);i++)
    {
        if(pipeline->lights[i] == NULL)
        {
            pipeline->light_count += 1;
            pipeline->lights[i] = light;
            return i;
        }
    }
    return UINT32_MAX;
}

// pipeline_remove_light
CGL_phong_light* CGL_phong_pipeline_remove_light(CGL_phong_pipeline* pipeline, uint32_t light_id)
{
    if(light_id < 0 || light_id >= CGL_utils_array_size(pipeline->lights))
        return NULL;
    pipeline->light_count -= 1;
    CGL_phong_light* light = pipeline->lights[light_id];
    pipeline->lights[light_id] = NULL;
    return light;
}

// mat_create
CGL_phong_mat* CGL_phong_mat_create()
{
    CGL_phong_mat* mat = (CGL_phong_mat*)malloc(sizeof(CGL_phong_mat));
    mat->use_diffuse_texture = false;
    mat->diffuse_color = CGL_vec3_init(0.8f, 0.8f, 0.8f);
    mat->diffuse_image = NULL;
    mat->diffuse_texture = NULL;
    mat->use_specular_texture = false;
    mat->specular_color = CGL_vec3_init(0.8f, 0.8f, 0.8f);
    mat->specular_image = NULL;
    mat->specular_texture = NULL;
    mat->use_normal_map = false;
    mat->normal_map_image = NULL;
    mat->normal_map_texture = NULL;
    mat->shininess = 64.0f;
    mat->wireframe = false;
    mat->user_data = NULL;
    return mat;
}

// mat_destroy
void CGL_phong_mat_destroy(CGL_phong_mat* mat)
{
    if(mat->use_diffuse_texture)
        CGL_texture_destroy(mat->diffuse_texture);
    if(mat->use_specular_texture)
        CGL_texture_destroy(mat->specular_texture);
    if(mat->use_normal_map)
        CGL_texture_destroy(mat->normal_map_texture);
    CGL_free(mat);
}

// mat_set_diffuse_color
void CGL_phong_mat_set_diffuse_color(CGL_phong_mat* mat, CGL_vec3 color)
{
    if(mat->use_diffuse_texture)
        CGL_texture_destroy(mat->diffuse_texture);
    mat->diffuse_image = NULL;
    mat->diffuse_texture = NULL;
    mat->use_diffuse_texture = false;
    mat->diffuse_color = color;
}

// mat_set_diffuse_texture
void CGL_phong_mat_set_diffuse_texture(CGL_phong_mat* mat, CGL_image* image)
{
    if(mat->use_diffuse_texture)
        CGL_texture_destroy(mat->diffuse_texture);
    mat->use_diffuse_texture = true;
    mat->diffuse_image = image;
    mat->diffuse_texture = CGL_texture_create(image);
}

// mat_set_specular_color
void CGL_phong_mat_set_specular_color(CGL_phong_mat* mat, CGL_vec3 color)
{
    if(mat->use_specular_texture)
        CGL_texture_destroy(mat->specular_texture);
    mat->specular_image = NULL;
    mat->specular_texture = NULL;
    mat->use_specular_texture = false;
    mat->specular_color = color;
}

// mat_set_specular_texture
void CGL_phong_mat_set_specular_texture(CGL_phong_mat* mat, CGL_image* image)
{
    if(mat->use_specular_texture)
        CGL_texture_destroy(mat->specular_texture);
    mat->use_specular_texture = true;
    mat->specular_image = image;
    mat->specular_texture = CGL_texture_create(image);
}

// mat_set_normal_texture
void CGL_phong_mat_set_normal_texture(CGL_phong_mat* mat, CGL_image* image)
{
    if(mat->use_normal_map)
        CGL_texture_destroy(mat->normal_map_texture);
    mat->use_normal_map = true;
    mat->normal_map_image = image;
    mat->normal_map_texture = CGL_texture_create(image);
}

// mat_set_shininess
void CGL_phong_mat_set_shininess(CGL_phong_mat* mat, float shininess)
{
    mat->shininess = shininess;
}

// mat_set_user_data
void CGL_phong_mat_set_user_data(CGL_phong_mat* mat, void* data)
{
    mat->user_data = data;
}

// mat_get_user_data
void* CGL_phong_mat_get_user_data(CGL_phong_mat* mat)
{
    return mat->user_data;
}

// mat_disable_normal_map
void CGL_phong_mat_disable_normal_map(CGL_phong_mat* mat)
{
    if(mat->use_normal_map)
        CGL_texture_destroy(mat->normal_map_texture);
    mat->normal_map_image = NULL;
    mat->normal_map_texture = NULL;
    mat->use_normal_map = false;
}

// mat_enable_wireframe
void CGL_phong_mat_enable_wireframe(CGL_phong_mat* mat)
{
    mat->wireframe = true;
}

// mat_disable_wireframe
void CGL_phong_mat_disable_wireframe(CGL_phong_mat* mat)
{
    mat->wireframe = false;
}

// mat_get_shininess
float CGL_phong_mat_get_shininess(CGL_phong_mat* mat)
{
    return mat->shininess;
}

// light_directional
CGL_phong_light* CGL_phong_light_directional(CGL_vec3 direction, CGL_vec3 color, float intensity)
{
    CGL_phong_light* light = (CGL_phong_light*)malloc(sizeof(CGL_phong_light));
    light->light_type = CGL_PHONG_LIGHT_DIRECTIONAL;
    light->color = color;
    light->intensity = intensity;
    light->vector = direction;
    return light;
}

// light_point
CGL_phong_light* CGL_phong_light_point(CGL_vec3 position, CGL_vec3 color, float intensity, float constant, float linear, float quadratic)
{
    CGL_phong_light* light = (CGL_phong_light*)malloc(sizeof(CGL_phong_light));
    light->light_type = CGL_PHONG_LIGHT_POINT;
    light->color = color;
    light->intensity = intensity;
    light->vector = position;
    light->constant = constant;
    light->linear = linear;
    light->quadratic = quadratic;
    return light;
}

// light_destroy
void CGL_phong_light_destroy(CGL_phong_light* light)
{
    CGL_free(light);
}

// light_set_intensity
void CGL_phong_light_set_intensity(CGL_phong_light* light, float intensity)
{
    light->intensity = intensity;
}

// light_set_color
void CGL_phong_light_set_color(CGL_phong_light* light, CGL_vec3 color)
{
    light->color = color;
}

// light_get_intensity
float CGL_phong_light_get_intensity(CGL_phong_light* light)
{
    return light->intensity;
}

// light_get_color
CGL_vec3 CGL_phong_light_get_color(CGL_phong_light* light)
{
    return light->color;
}

// light_get_type
uint32_t CGL_phong_light_get_type(CGL_phong_light* light)
{
    return light->light_type;
}


void CGL_phong_render_begin(CGL_phong_pipeline* pipeline, CGL_camera* camera)
{
    if(pipeline->depth_testing) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);

    CGL_shader_bind(pipeline->shader);
    // set uniforms
    CGL_shader_set_uniform_mat4(pipeline->shader, pipeline->u_projection, CGL_camera_get_projection_mat_ptr(camera));// camera
    CGL_shader_set_uniform_mat4(pipeline->shader, pipeline->u_view, CGL_camera_get_view_mat_ptr(camera));// camera
    CGL_shader_set_uniform_mat4(pipeline->shader, pipeline->u_pv, CGL_camera_get_pv_mat_ptr(camera));// camera
    CGL_vec3 camera_position = CGL_camera_get_position(camera);
    CGL_shader_set_uniform_vec3v(pipeline->shader, pipeline->u_camera_position, camera_position.x, camera_position.y, camera_position.z);
    CGL_shader_set_uniform_bool(pipeline->shader, pipeline->u_use_blinn, pipeline->use_blinn);
    CGL_shader_set_uniform_float(pipeline->shader, pipeline->u_ambient_light_strength, pipeline->ambient_light_strength);
    CGL_shader_set_uniform_vec3v(pipeline->shader, pipeline->u_ambient_light_color, pipeline->ambient_light_color.x, pipeline->ambient_light_color.y, pipeline->ambient_light_color.z);
    CGL_shader_set_uniform_bool(pipeline->shader, pipeline->u_use_gamma_correction, pipeline->use_gamma_correction);
    // set lights
    CGL_shader_set_uniform_int(pipeline->shader, pipeline->u_light_count, pipeline->light_count);
    for(uint32_t i = 0, j = 0 ; i < pipeline->light_count ; i++)
    {
        CGL_phong_light* light = pipeline->lights[i];
        if(light == NULL) continue;
        // data_0 is color and strength
        CGL_shader_set_uniform_vec4v(pipeline->shader, pipeline->u_lights_v4[0][j], light->color.x, light->color.y, light->color.z, light->intensity);
        // data_1 is type, constant, linear, quadratic
        CGL_shader_set_uniform_vec4v(pipeline->shader, pipeline->u_lights_v4[1][j], (float)light->light_type, light->constant, light->linear, light->quadratic);
        // data_2 is vector
        CGL_shader_set_uniform_vec4v(pipeline->shader, pipeline->u_lights_v4[2][j], light->vector.x, light->vector.y, light->vector.z, 0.0f);
        // data_3 is reserved for future use
        j++;
    }
}

void CGL_phong_render(CGL_mesh_gpu* mesh, CGL_mat4* model_matrix, CGL_phong_mat* material, CGL_phong_pipeline* pipeline, CGL_camera* camera)
{
    CGL_shader_set_uniform_mat4(pipeline->shader, pipeline->u_model_matrix, model_matrix);
    CGL_shader_set_uniform_bool(pipeline->shader, pipeline->u_use_diffuse_texture, material->use_diffuse_texture);
    CGL_shader_set_uniform_vec3v(pipeline->shader, pipeline->u_diffuse_color, material->diffuse_color.x, material->diffuse_color.y, material->diffuse_color.z);
    if(material->use_diffuse_texture)
    {
        CGL_texture_bind(material->diffuse_texture, 0);
        CGL_shader_set_uniform_int(pipeline->shader, pipeline->u_diffuse_texture, 0);
    }
    CGL_shader_set_uniform_bool(pipeline->shader, pipeline->u_use_specular_texture, material->use_specular_texture);
    CGL_shader_set_uniform_vec3v(pipeline->shader, pipeline->u_specular_color, material->specular_color.x, material->specular_color.y, material->specular_color.z);
    if(material->use_specular_texture)
    {
        CGL_texture_bind(material->specular_texture, 1);
        CGL_shader_set_uniform_int(pipeline->shader, pipeline->u_specular_texture, 1);
    }
    CGL_shader_set_uniform_bool(pipeline->shader, pipeline->u_use_normal_map, material->use_normal_map);
    if(material->use_normal_map)
    {
        CGL_texture_bind(material->normal_map_texture, 3);
        CGL_shader_set_uniform_int(pipeline->shader, pipeline->u_use_normal_map, 3);
    }    
    CGL_shader_set_uniform_float(pipeline->shader, pipeline->u_shininess, material->shininess);
    if(material->wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
    CGL_mesh_gpu_render(mesh);
    if(material->wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CGL_phong_render_end(CGL_phong_pipeline* pipeline, CGL_camera* camera)
{
    
}


#endif

#endif


// tilemap renderer
#if 1

#ifndef CGL_EXCLUDE_TILEMAP_RENDERER

static const char* __CGL_TILEMAP_VERTEX_SHADER = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(position.xyz, 1.0f);\n"
"}";

static const char* __CGL_TILEMAP_FRAGENT_SHADER = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"//out int MousePick0;\n"
"//out int MousePick1;\n"
"//out int MousePick2;\n"
"in vec3 Position;\n"
"in vec3 Normal;\n"
"in vec2 TexCoord;\n"
"\n"
"\n"
"// unifroms\n"
"uniform vec2 u_offset;\n"
"uniform vec2 u_scale;\n"
"uniform vec2 u_tile_count;\n"
"uniform vec2 u_tile_size;\n"
"\n"
"uniform sampler2D u_texture_tileset;\n"
"uniform sampler2DArray u_texture_array;\n"
"\n"
"struct cglTile\n"
"{\n"
"    vec4 color;\n"
"};\n"
"\n"
"layout (std430, binding = %d)  buffer tiles_buffer\n"
"{\n"
"    cglTile tiles[];\n"
"};\n"
"\n"
"void main()\n"
"{\n"
"    vec4 color = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
"    vec2 frag_coord = gl_FragCoord.xy;\n"
"    vec2 coord = vec2(\n"
"        (frag_coord.x + u_offset.x) * u_scale.x,\n"
"        (frag_coord.y + u_offset.y) * u_scale.y\n"
"    );\n"
"    \n"
"    vec2 tile_index_f = vec2(\n"
"        coord.x / u_tile_size.x,\n"
"        coord.y / u_tile_size.y\n"
"    );\n"
"    ivec2 tile_index = ivec2(tile_index_f);\n"
"\n"
"    vec2 tile_tex_coord = vec2(\n"
"        mod(coord.x, u_tile_size.x) / u_tile_size.x,\n"
"        mod(coord.y, u_tile_size.y) / u_tile_size.y\n"
"    );\n"
"\n"
"\n"
"    if(tile_index_f.x < 0 || tile_index_f.x >= u_tile_count.x\n"
"    || tile_index_f.y < 0 || tile_index_f.y >= u_tile_count.y)\n"
"    {\n"
"        // discard; // Using discard might not be the best idea\n"
"        FragColor = vec4(0.0f);\n"
"        return;\n"
"    }\n"
"\n"
"    cglTile current_tile = tiles[tile_index.y * int(u_tile_count.x) + tile_index.x];\n"
"\n"
"    if (current_tile.color.w > 3.0f)  // case where tile is empty\n"
"    {\n"
"        // discard; // Using discard might not be the best idea\n"
"        FragColor = vec4(0.0f);\n"
"        return;\n"
"    }\n"
"    else if(current_tile.color.w > 2.0f ) // case where tile is a solid color\n"
"    {\n"
"        FragColor = vec4(current_tile.color.xyz, 1.0f);\n"
"        return;\n"
"    }\n"
"    else if(current_tile.color.w > 1.0f ) // case where tile is a texture from texture array\n"
"    {\n"
"        FragColor = texture(u_texture_array, vec3(tile_tex_coord, current_tile.color.x));\n"
"        return;\n"
"    }\n"
"    else // case where tile is a texture from tileset \n"
"    {\n"
"        vec2 final_tex_scale = current_tile.color.zw - current_tile.color.xy;\n"
"        vec2 final_tex_coord = vec2(\n"
"            tile_tex_coord.x / final_tex_scale.x + current_tile.color.x,\n"
"            tile_tex_coord.y / final_tex_scale.y + current_tile.color.y\n"
"        );\n"
"        FragColor = texture(u_texture_tileset, final_tex_coord);\n"
"        return;\n"
"    }\n"
"\n"
"    FragColor = vec4(tile_tex_coord, 0.0f, 1.0f);\n"
"    //MousePick0 = InstanceID;\n"
"    //MousePick1 = 0;\n"
"    //MousePick2 = 1;\n"
"}";

struct CGL_tile
{
    CGL_vec4 color;
};

struct CGL_tilemap
{
    // tiles data
    CGL_tile* tile_data;
    uint32_t tile_count_x;
    uint32_t tile_count_y;
    uint32_t tile_size_x;
    uint32_t tile_size_y;
    // renderer data
    CGL_mesh_gpu* mesh;
    CGL_shader* shader;
    CGL_ssbo* ssbo;
    // unifrom locations
    int u_tile_count;
    int u_tile_size;
    int u_offset;
    int u_scale;
    int u_texture_tileset;
    int u_texture_array;
};


CGL_tilemap* CGL_tilemap_create(uint32_t tile_count_x, uint32_t tile_count_y, uint32_t tile_size_x, uint32_t tile_size_y, uint32_t ssbo_binding)
{
    /*
    assert(tile_count_x > 0);
    assert(tile_count_y > 0);
    assert(tile_size_x > 0);
    assert(tile_size_y > 0);
    */
    CGL_tilemap* tilemap = (CGL_tilemap*)malloc(sizeof(CGL_tilemap));
    tilemap->tile_data = (CGL_tile*)malloc(sizeof(CGL_tile) * tile_count_x * tile_count_y);
    memset(tilemap->tile_data, 0, (sizeof(CGL_tile) * tile_count_x * tile_count_y));
    tilemap->tile_count_x = tile_count_x;
    tilemap->tile_count_y = tile_count_y;
    tilemap->tile_size_x = tile_size_x;
    tilemap->tile_size_y = tile_size_y;
    tilemap->ssbo = CGL_ssbo_create(ssbo_binding);
    CGL_mesh_cpu* screen_quad_mesh_cpu = CGL_mesh_cpu_quad((CGL_vec3){ 1.0,  1.0, 0.0},
                                                           (CGL_vec3){ 1.0, -1.0, 0.0},
                                                           (CGL_vec3){-1.0, -1.0, 0.0},
                                                           (CGL_vec3){-1.0,  1.0, 0.0});
    CGL_mesh_gpu* screen_quad_mesh_gpu = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(screen_quad_mesh_gpu, screen_quad_mesh_cpu, true);
    CGL_mesh_cpu_destroy(screen_quad_mesh_cpu);
    tilemap->mesh = screen_quad_mesh_gpu;
    static char shader_source_buffer[1024 * 64];
    sprintf(shader_source_buffer, __CGL_TILEMAP_FRAGENT_SHADER, ssbo_binding);
    tilemap->shader = CGL_shader_create(__CGL_TILEMAP_VERTEX_SHADER, shader_source_buffer, NULL);
    tilemap->u_offset = CGL_shader_get_uniform_location(tilemap->shader, "u_offset");
    tilemap->u_scale = CGL_shader_get_uniform_location(tilemap->shader, "u_scale");
    tilemap->u_tile_count = CGL_shader_get_uniform_location(tilemap->shader, "u_tile_count");
    tilemap->u_tile_size = CGL_shader_get_uniform_location(tilemap->shader, "u_tile_size");
    tilemap->u_texture_tileset = CGL_shader_get_uniform_location(tilemap->shader, "u_texture_tileset");
    tilemap->u_texture_array = CGL_shader_get_uniform_location(tilemap->shader, "u_texture_array");
    CGL_tilemap_reset(tilemap);
    return tilemap;
}

void CGL_tilemap_destroy(CGL_tilemap* tilemap)
{
    CGL_ssbo_destroy(tilemap->ssbo);
    CGL_shader_destroy(tilemap->shader);
    CGL_mesh_gpu_destroy(tilemap->mesh);
    free(tilemap->tile_data);
    free(tilemap);
}

void CGL_tilemap_set_tile_color(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, float r, float g, float b)
{
    CGL_tile* tile = &tilemap->tile_data[tile_y * tilemap->tile_count_x + tile_x];
    tile->color = CGL_vec4_init(r, g, b, 2.5f);
    CGL_ssbo_set_sub_data(tilemap->ssbo, (tile_y * tilemap->tile_count_x + tile_x) * sizeof(CGL_tile), sizeof(CGL_tile), tile, false);
}

void CGL_tilemap_set_tile_texture_from_array(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, uint32_t texture_index)
{
    CGL_tile* tile = &tilemap->tile_data[tile_y * tilemap->tile_count_x + tile_x];
    tile->color = CGL_vec4_init((float)texture_index, 0.0f, 0.0f, 1.5f);
    CGL_ssbo_set_sub_data(tilemap->ssbo, (tile_y * tilemap->tile_count_x + tile_x) * sizeof(CGL_tile), sizeof(CGL_tile), tile, false);
}

void CGL_tilemap_set_tile_texture_from_tileset(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, float texture_x_min, float texture_y_min, float texture_x_max, float texture_y_max)
{
    CGL_tile* tile = &tilemap->tile_data[tile_y * tilemap->tile_count_x + tile_x];
    tile->color = CGL_vec4_init(texture_x_min, texture_y_min, texture_x_max, texture_y_max);
    CGL_ssbo_set_sub_data(tilemap->ssbo, (tile_y * tilemap->tile_count_x + tile_x) * sizeof(CGL_tile), sizeof(CGL_tile), tile, false);
}

void CGL_tilemap_clear_tile(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y)
{
    CGL_tile* tile = &tilemap->tile_data[tile_y * tilemap->tile_count_x + tile_x];
    tile->color = CGL_vec4_init(0.0f, 0.0f, 0.0f, 3.5f);
    CGL_ssbo_set_sub_data(tilemap->ssbo, (tile_y * tilemap->tile_count_x + tile_x) * sizeof(CGL_tile), sizeof(CGL_tile), tile, false);
}

void CGL_tilemap_reset(CGL_tilemap* tilemap)
{
    for(uint32_t tile_x = 0 ; tile_x < tilemap->tile_count_x ; tile_x++)
    {
        for(uint32_t tile_y = 0 ; tile_y < tilemap->tile_count_y ; tile_y++)
        {
            CGL_tile* tile = &tilemap->tile_data[tile_y * tilemap->tile_count_x + tile_x];
            tile->color = CGL_vec4_init(0.0f, 0.0f, 0.0f, 3.5f);
        }
    }
    CGL_ssbo_set_data(tilemap->ssbo, (sizeof(CGL_tile) * tilemap->tile_count_x * tilemap->tile_count_y), tilemap->tile_data, false);
}

void CGL_tilemap_render(CGL_tilemap* tilemap, float scale_x, float scale_y, float offset_x, float offset_y, CGL_texture* texture)
{
    CGL_shader_bind(tilemap->shader);
    if(texture)
        CGL_texture_bind(texture, 0);
    CGL_shader_set_uniform_vec2v(tilemap->shader, tilemap->u_offset, -offset_x, -offset_y);
    CGL_shader_set_uniform_vec2v(tilemap->shader, tilemap->u_scale, 1.0f / scale_x, 1.0f / scale_y);
    CGL_shader_set_uniform_vec2v(tilemap->shader, tilemap->u_tile_count, (float)tilemap->tile_count_x, (float)tilemap->tile_count_y);
    CGL_shader_set_uniform_vec2v(tilemap->shader, tilemap->u_tile_size, (float)tilemap->tile_size_x, (float)tilemap->tile_size_y);
    CGL_shader_set_uniform_int(tilemap->shader, tilemap->u_texture_tileset, 0);
    CGL_shader_set_uniform_int(tilemap->shader, tilemap->u_texture_array, 0);
    CGL_mesh_gpu_render(tilemap->mesh);
}

#endif

#endif

#endif // CGL_IMPLEMENTATION

#endif // CGL_H