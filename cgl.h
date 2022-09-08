#ifndef CGL_H
#define CGL_H

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

// CGL common

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

#define CGL_free(ptr) free(ptr)
#define CGL_exit(code) exit(code)

#if 1 // Just to use code folding

// data structures

struct CGL_list;
typedef struct CGL_list CGL_list;

CGL_list* CGL_list_create(size_t item_size);
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
    a.m[3] * b.m[0] + a.m[7] * b.m[1] + a.m[118] * b.m[2] + a.m[15] * b.m[3], \
\
    a.m[0] * b.m[4] + a.m[4] * b.m[5] + a.m[8] * b.m[6] + a.m[12] * b.m[7], \
    a.m[1] * b.m[4] + a.m[5] * b.m[5] + a.m[9] * b.m[6] + a.m[13] * b.m[7], \
    a.m[2] * b.m[4] + a.m[6] * b.m[5] + a.m[10] * b.m[6] + a.m[14] * b.m[7], \
    a.m[3] * b.m[4] + a.m[7] * b.m[5] + a.m[118] * b.m[6] + a.m[15] * b.m[7], \
\
    a.m[0] * b.m[8] + a.m[4] * b.m[9] + a.m[8] * b.m[10] + a.m[12] * b.m[11], \
    a.m[1] * b.m[8] + a.m[5] * b.m[9] + a.m[9] * b.m[10] + a.m[13] * b.m[11], \
    a.m[2] * b.m[8] + a.m[6] * b.m[9] + a.m[10] * b.m[10] + a.m[14] * b.m[11], \
    a.m[3] * b.m[8] + a.m[7] * b.m[9] + a.m[11] * b.m[10] + a.m[15] * b.m[11], \
\
    a.m[0] * b.m[12] + a.m[4] * b.m[13] + a.m[8] * b.m[14] + a.m[12] * b.m[15], \
    a.m[1] * b.m[12] + a.m[5] * b.m[13] + a.m[9] * b.m[14] + a.m[13] * b.m[15], \
    a.m[2] * b.m[12] + a.m[6] * b.m[13] + a.m[10] * b.m[14] + a.m[14] * b.m[15], \
    a.m[3] * b.m[12] + a.m[7] * b.m[13] + a.m[118] * b.m[14] + a.m[15] * b.m[15] \
}
#define CGL_mat4_perspective(aspect, fov, znear, zfar) (CGL_mat4){1.0f / (aspect * tanf(fov / 2.0f)), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f / tanf(fov / 2.0f), 0.0f, 0.0f, 0.0f, 0.0f, -1.0f * ( (zfar + znear) / (zfar - znear) ), -1.0f, 0.0f, 0.0f, -2.0f * znear * zfar / (zfar - znear), 1.0f}
CGL_mat4 CGL_mat4_look_at(CGL_vec3 eye, CGL_vec3 target, CGL_vec3 up);

#endif

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

// structs
struct CGL_window;
typedef struct CGL_window CGL_window;

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
void CGL_window_make_context_current(CGL_window* window); // make opengl context current

// inputs
int CGL_window_get_key(CGL_window* window, int key); // get key state
int CGL_window_get_mouse_button(CGL_window* window, int button); // get mouse button state
void CGL_window_get_mouse_position(CGL_window* window, double* xpos, double* ypos); // get mouse position

#endif

#if 1 // Just to use code folding

#include <glad/glad.h>

#define CGL_OPENGL_VERSION_MAJOR 4
#define CGL_OPENGL_VERSION_MINOR 6

#define CGL_OPENGL_VERSION_MAJOR_STR "4"
#define CGL_OPENGL_VERSION_MINOR_STR "6"

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
CGL_texture* CGL_texture_create(int width, int height, GLenum format, GLenum internal_format, GLenum type); // create texture
void CGL_texture_destroy(CGL_texture* texture); // destroy texture
void CGL_texture_bind(CGL_texture* texture, int unit); // bind texture to unit
void CGL_texture_set_data(CGL_texture* texture, void* data); // set texture data
void CGL_texture_set_user_data(CGL_texture* texture, void* user_data); // set texture user data
void* CGL_texture_get_user_data(CGL_texture* texture); // get texture user data
void CGL_texture_get_size(CGL_texture* texture, int* width, int* height); // get texture size


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

// Implementation of CGL

#ifdef CGL_IMPLEMENTATION

// include windows headers for windows builds
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif
#endif

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

CGL_list* CGL_list_create(size_t item_size)
{
    CGL_list* list = (CGL_list*)malloc(sizeof(CGL_list));
    list->size = 0;
    list->capacity = 5;
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
    if(index > list->size) return NULL;
    if(data) memcpy( data, ((char*)list->data + index * list->item_size), list->item_size );
    return ((char*)list->data + index * list->item_size);
}

void* CGL_list_set(CGL_list* list, size_t index, void* data)
{
    if(index > list->size) return NULL;
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


#endif

// common lib

#if 1 // Just to use code folding

CGL_mat4 CGL_mat4_look_at(CGL_vec3 eye, CGL_vec3 target, CGL_vec3 up)
{
    CGL_vec3 z_axis = CGL_vec3_sub(eye, target);
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
    mat.m[7] = -1.0f * CGL_vec3_dot(x_axis, eye);
    mat.m[8] = z_axis.x;
    mat.m[9] = z_axis.y;
    mat.m[10] = z_axis.z;
    mat.m[11] = -1.0f * CGL_vec3_dot(x_axis, eye);
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


#if 1 // just to use code folding in ide

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
};

// callbacks
void __CGL_window_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    if(cgl_window->key_callback != NULL)
        cgl_window->key_callback(cgl_window, key, scancode, action, mods);
}

void __CGL_window_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    if(cgl_window->mouse_button_callback != NULL)
        cgl_window->mouse_button_callback(cgl_window, button, action, mods);
}

void __CGL_window_mouse_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    if(cgl_window->mouse_position_callback != NULL)
        cgl_window->mouse_position_callback(cgl_window, xpos, ypos);
}

void __CGL_window_mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    if(cgl_window->mouse_scroll_callback != NULL)
        cgl_window->mouse_scroll_callback(cgl_window, xoffset, yoffset);
}

void __CGL_window_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    if(cgl_window->framebuffer_size_callback != NULL)
        cgl_window->framebuffer_size_callback(cgl_window, width, height);
}

void __CGL_window_close_callback(GLFWwindow* window)
{
    CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    if(cgl_window->close_callback != NULL)
        cgl_window->close_callback(cgl_window);
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
    glfwSetWindowUserPointer(window->handle, window);
    glfwSwapInterval(1); // vsync
    window->is_hidden = true;
    window->key_callback = NULL;
    window->mouse_button_callback = NULL;
    window->mouse_position_callback = NULL;
    window->mouse_scroll_callback = NULL;
    window->framebuffer_size_callback = NULL;
    window->close_callback = NULL;
    window->user_data = NULL;
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
        glfwSetKeyCallback(window->handle, __CGL_window_key_callback);
    window->key_callback = callback;
}

// set mouse button callback
void CGL_window_set_mouse_button_callback(CGL_window* window, CGL_window_mouse_button_callback callback)
{
    if(window->mouse_button_callback == NULL)
        glfwSetMouseButtonCallback(window->handle, __CGL_window_mouse_button_callback);
    window->mouse_button_callback = callback;
}

// set mouse position callback
void CGL_window_set_mouse_position_callback(CGL_window* window, CGL_window_mouse_position_callback callback)
{
    if(window->mouse_position_callback == NULL)
        glfwSetCursorPosCallback(window->handle, __CGL_window_mouse_position_callback);
    window->mouse_position_callback = callback;
}

// set mouse scroll callback
void CGL_window_set_mouse_scroll_callback(CGL_window* window, CGL_window_mouse_scroll_callback callback)
{
    if(window->mouse_scroll_callback == NULL)
        glfwSetScrollCallback(window->handle, __CGL_window_mouse_scroll_callback);
    window->mouse_scroll_callback = callback;
}

// set framebuffer size callback
void CGL_window_set_framebuffer_size_callback(CGL_window* window, CGL_window_framebuffer_size_callback callback)
{
    if(window->framebuffer_size_callback == NULL)
        glfwSetFramebufferSizeCallback(window->handle, __CGL_window_framebuffer_size_callback);
    window->framebuffer_size_callback = callback;
}

// set close callback
void CGL_window_set_close_callback(CGL_window* window, CGL_window_close_callback callback)
{
    if(window->close_callback == NULL)
        glfwSetWindowCloseCallback(window->handle, __CGL_window_close_callback);
    window->close_callback = callback;
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

#endif

#if 1 // Just to use code folding

// CGL opengl 

// texture

struct CGL_texture
{
    GLuint handle;
    int width;
    int height;
    GLenum format;
    GLenum internal_format;
    GLenum type;
    void* user_data;
};

// create texture
CGL_texture* CGL_texture_create(int width, int height, GLenum format, GLenum internal_format, GLenum type)
{
    CGL_texture* texture = malloc(sizeof(CGL_texture));
    texture->width = width;
    texture->height = height;
    texture->format = format;
    texture->internal_format = internal_format;
    texture->type = type;
    texture->user_data = NULL;
    glGenTextures(1, &texture->handle);
    glBindTexture(GL_TEXTURE_2D, texture->handle);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
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
    glBindTexture(GL_TEXTURE_2D, texture->handle);
}

// set texture data
void CGL_texture_set_data(CGL_texture* texture, void* data)
{
    glBindTexture(GL_TEXTURE_2D, texture->handle);
    glTexImage2D(GL_TEXTURE_2D, 0, texture->internal_format, texture->width, texture->height, 0, texture->format, texture->type, data);
    glBindTexture(GL_TEXTURE_2D, 0);
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
    framebuffer->color_texture = CGL_texture_create(width, height, GL_RGBA, GL_RGBA32F, GL_FLOAT);    
    if(!framebuffer->color_texture)
    {
        free(framebuffer);
        return NULL;
    }

    
    framebuffer->depth_texture = CGL_texture_create(width, height, GL_DEPTH_COMPONENT, GL_DEPTH24_STENCIL8, GL_FLOAT);
    if(!framebuffer->depth_texture)
    {
        CGL_texture_destroy(framebuffer->color_texture);
        free(framebuffer);
        return NULL;
    }
        
    for(int i = 0; i < 3; i++)
    {
        framebuffer->mousepick_texture[i] = CGL_texture_create(width, height, GL_RED_INTEGER, GL_R32I, GL_INT);
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
CGL_mesh_cpu* __CGL_mesh_cpu_create(size_t vertex_count, size_t index_count)
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

// generate triangle mesh
CGL_mesh_cpu* CGL_mesh_cpu_triangle(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
{
    CGL_mesh_cpu* mesh = __CGL_mesh_cpu_create(3, 3);
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
    CGL_mesh_cpu* mesh = __CGL_mesh_cpu_create(4, 6);
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

#endif // CGL_IMPLEMENTATION

#endif // CGL_H