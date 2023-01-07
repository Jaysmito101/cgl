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

#if defined(_WIN32) || defined(_WIN64)
#define CGL_WINDOWS
#else
// TODO: seperate linux and macos and android
#define CGL_UNIX
#endif

#ifdef CGL_WASM
#define CGL_EXCLUDE_NETWORKING
#define CGL_EXCLUDES_THREADS
#ifdef CGL_EXCLUDE_WINDOW_API
#undef CGL_EXCLUDE_WINDOW_API
#endif
#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
#endif
#define CGL_wasm_set_loop_func(func, data) emscripten_set_main_loop_arg((em_arg_callback_func)func, data, 0, true);
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
#include <time.h>

typedef unsigned char CGL_ubyte;
typedef unsigned short CGL_ushort;
typedef unsigned int CGL_uint;
typedef unsigned long CGL_ulong;
typedef unsigned long long CGL_ulonglong;
typedef char CGL_byte;
typedef short CGL_short;
typedef int CGL_int;
typedef long CGL_long;
typedef long long CGL_longlong;
typedef float CGL_float;
typedef double CGL_double;
typedef long double CGL_longdouble;
typedef size_t CGL_sizei;
typedef bool CGL_bool;

#ifdef CGL_LOGGING_ENABLED
#define CGL_LOG(...) CGL_log_internal(__VA_ARGS__)
#else
#define CGL_LOG(...)
#endif // CGL_LOG_ENABLED

struct CGL_context;
typedef struct CGL_context CGL_context;

bool CGL_init(); // initialize CGL
void CGL_shutdown(); // shutdown CGL

#endif

#ifndef CGL_EXCLUDE_NETWORKING

#define CGL_NET_NO_ERROR                    0xAB000
#define CGL_NET_NAME_RESOLUTION_ERROR       0xAB001
#define CGL_NET_INVALID_PARAMATER_ERROR     0xAB002
#define CGL_NET_UNSUPPORTED_ERROR           0xAB003
#define CGL_NET_MEMORY_ERROR                0xAB004
#define CGL_NET_NOT_FOUND_ERROR             0xAB005

struct CGL_net_addrinfo;
typedef struct CGL_net_addrinfo CGL_net_addrinfo;

struct CGL_net_socket;
typedef struct CGL_net_socket CGL_net_socket;


bool CGL_net_init();
void CGL_net_shutdown();

CGL_net_addrinfo* CGL_net_addrinfo_query(const char* name, const char* port, size_t* count);
void CGL_net_addrinfo_destroy(CGL_net_addrinfo* infos);

CGL_net_socket* CGL_net_socket_create();
bool CGL_net_socket_connect(CGL_net_socket* socket, CGL_net_addrinfo* target);
bool CGL_net_socket_bind(CGL_net_socket* socket, CGL_net_addrinfo* target);
bool CGL_net_socket_listen(CGL_net_socket* socket, size_t max_connections); 
CGL_net_socket* CGL_net_socket_accept(CGL_net_socket* socket, CGL_net_addrinfo* addrinfo);
void CGL_net_socket_close(CGL_net_socket* socket);
bool CGL_net_socket_send(CGL_net_socket* socket, void* buffer, size_t size, size_t* size_sent);
bool CGL_net_socket_recv(CGL_net_socket* socket, void* buffer, size_t size, size_t* size_recieved);
bool CGL_net_socket_shutdown_send(CGL_net_socket* socket);
bool CGL_net_socket_shutdown_recv(CGL_net_socket* socket);

int CGL_net_http_request(const char* method, const char* host, const char* path, void* response_buffer, size_t* size, const char* accept, const char* user_agent, const char* body);
int CGL_net_http_get(const char* host, const char* path, void* buffer, size_t* size, const char* accept, const char* user_agent);
int CGL_net_http_post(const char* host, const char* path, void* buffer, size_t* size, const char* accept, const char* user_agent, const char* body);

#ifndef CGL_EXCLUDE_SSL_SOCKET

int CGL_net_https_request(const char* method, const char* host, const char* path, void* response_buffer, size_t* size, const char* accept, const char* user_agent, const char* body);
int CGL_net_https_get(const char* host, const char* path, void* buffer, size_t* size, const char* accept, const char* user_agent);
int CGL_net_https_post(const char* host, const char* path, void* buffer, size_t* size, const char* accept, const char* user_agent, const char* body);

struct CGL_net_ssl_socket;
typedef struct CGL_net_ssl_socket CGL_net_ssl_socket;

CGL_net_ssl_socket* CGL_net_ssl_socket_create(CGL_net_socket* socket);
bool CGL_net_ssl_socket_send(CGL_net_ssl_socket* socket, void* buffer, size_t size, size_t* size_sent);
bool CGL_net_ssl_socket_recv(CGL_net_ssl_socket* socket, void* buffer, size_t size, size_t* size_recieved);
void CGL_net_ssl_socket_destroy(CGL_net_ssl_socket* soc);
void CGL_net_ssl_log_errors();

#endif

#endif

// utils
#if 1
// CGL utils

#ifndef CGL_RAND_GEN_WITH_PROBABILITY_MAX_COUNT
#define CGL_RAND_GEN_WITH_PROBABILITY_MAX_COUNT 100000
#endif

void CGL_utils_sleep(const CGL_sizei milis);
CGL_byte* CGL_utils_read_file(const CGL_byte* path, size_t* size); // read file into memory
CGL_sizei CGL_utils_get_file_size(const CGL_byte* path);
CGL_bool CGL_utils_append_file(const CGL_byte* path, const CGL_byte* data, size_t size);
CGL_bool CGL_utils_write_file(const CGL_byte* path, const CGL_byte* data, size_t size); // write data to file
CGL_float CGL_utils_get_time();
void CGL_utils_get_timestamp(char* buffer);
CGL_bool CGL_utils_is_little_endian();
CGL_sizei CGL_utils_get_random_with_probability(CGL_float* probabilities, CGL_sizei count);
void CGL_utils_reverse_bytes(void* data, size_t size);
void CGL_utils_little_endian_to_current(void* data, size_t size);
void CGL_utils_big_endian_to_current(void* data, size_t size);

#define CGL_utils_is_point_in_rect(px, py, x, y, sx, sy, scx, scy) (bool)((px) >= (x) * (scx) && (px) <= ((x) + (sx)) * (scx) && (py) >= (y) * (scy) && (py) <= ((y) + (sy)) * (scy))
#define CGL_utils_random_float() ((float)rand() / (float)RAND_MAX)
#define CGL_utils_random_float_in_range(min, max) (CGL_utils_random_float() * (max - min) + min)
#define CGL_utils_random_int(min, max) (rand() % (max - min + 1) + min)
#define CGL_utils_random_bool() (rand() % 2)
#define CGL_utils_random_vec2(min, max) (CGL_vec2_init(CGL_utils_random_float() * (max.x - min.x) + min.x, CGL_utils_random_float() * (max.y - min.y) + min.y))
#define CGL_utils_random_vec3(min, max) (CGL_vec3_init(CGL_utils_random_float() * (max.x - min.x) + min.x, CGL_utils_random_float() * (max.y - min.y) + min.y, CGL_utils_random_float() * (max.z - min.z) + min.z))
#define CGL_utils_random_vec4(min, max) (CGL_vec4_init(CGL_utils_random_float() * (max.x - min.x) + min.x, CGL_utils_random_float() * (max.y - min.y) + min.y, CGL_utils_random_float() * (max.z - min.z) + min.z, CGL_utils_random_float() * (max.w - min.w) + min.w))
#define CGL_utils_random_color() (CGL_vec4_init(CGL_utils_random_float(), CGL_utils_random_float(), CGL_utils_random_float(), 1.0f))
#define CGL_utils_clamp(x, minl, maxl) (x < minl ? minl : (x > maxl ? maxl : x))
#define CGL_utils_array_size(array) (sizeof(array) / sizeof(array[0]))
#define CGL_utils_max(a, b) ( ((a) > (b)) ? (a) : (b) )
#define CGL_utils_min(a, b) ( ((a) < (b)) ? (a) : (b) )
#define CGL_utils_mix(x, y, f) (x * f + y * (1.0f - f))

#define CGL_malloc(size) malloc(size)
#define CGL_realloc(ptr, size) realloc(ptr, size)
#define CGL_free(ptr) free(ptr)
#define CGL_exit(code) exit(code)

#define CGL_CONSOLE_COLOR_RESET  0
#define CGL_CONSOLE_COLOR_RED    1
#define CGL_CONSOLE_COLOR_GREEN  2
#define CGL_CONSOLE_COLOR_GRAY   3
#define CGL_CONSOLE_COLOR_BLUE   4

void CGL_console_set_color(uint8_t color);

void CGL_printf_red(const char* format, ...);
void CGL_printf_green(const char* format, ...);
void CGL_printf_gray(const char* format, ...);
void CGL_printf_blue(const char* format, ...);

#endif

#define CGL_LOG_LEVEL_TRACE         0
#define CGL_LOG_LEVEL_INFO          1
#define CGL_LOG_LEVEL_WARN          2
#define CGL_LOG_LEVEL_ERROR         3 
#define CGL_LOG_LEVEL_INTERNAL      4 
#define CGL_LOGGER_MAX_LOG_FILES    32
#define CGL_LOGGER_LOG_BUFFER_SIZE  (1024 * 4)

#ifndef CGL_ENABLE_CONSOLE_LOGGING
#define CGL_ENABLE_CONSOLE_LOGGING true
#endif

#ifndef CGL_DISABLE_LOGGER


struct CGL_logger_context;
typedef struct CGL_logger_context CGL_logger_context;

void CGL_logger_init(bool enable_console_logging);
void CGL_logger_shutdown();
CGL_logger_context* CGL_logger_get_context();
void CGL_logger_set_context(CGL_logger_context* context);
bool CGL_logger_attach_log_file(const char* path);
bool CGL_logger_detach_log_file(const char* path);
void CGL_logger_flush();
void CGL_logger_disable_console_logging();
void CGL_logger_enable_console_logging();
void CGL_logger_log(CGL_int level, const char* log_format, ...);

#define CGL_trace(...)        CGL_logger_log(CGL_LOG_LEVEL_TRACE, __VA_ARGS__)
#define CGL_info(...)         CGL_logger_log(CGL_LOG_LEVEL_INFO, __VA_ARGS__)
#define CGL_warn(...)         CGL_logger_log(CGL_LOG_LEVEL_WARN, __VA_ARGS__)
#define CGL_error(...)        CGL_logger_log(CGL_LOG_LEVEL_ERROR, __VA_ARGS__)
#ifndef CGL_DISABLE_INTERNAL_LOGGING
#define CGL_log_internal(...) CGL_logger_log(CGL_LOG_LEVEL_INTERNAL, __VA_ARGS__)
#endif

#else
#define CGL_logger_init(...)
#define CGL_logger_shutdown()
#define CGL_logger_get_context() NULL
#define CGL_logger_set_context(...)
#define CGL_logger_attach_log_file(path)
#define CGL_logger_dettach_log_file(path)
#define CGL_logger_flush()
#define CGL_logger_disable_console_logging()
#define CGL_logger_enable_console_logging()
#define CGL_logger_log(...)
#define CGL_info(...)
#define CGL_trace(...)
#define CGL_warn(...)
#define CGL_error(...)
#define CGL_log_internal(...)
#endif

// math and data structures
#if 1 // Just to use code folding

// data structures

struct CGL_list;
typedef struct CGL_list CGL_list;

CGL_list* CGL_list_create(size_t item_size, size_t initial_capacity);
void CGL_list_destroy(CGL_list* list);
void CGL_list_set_increase_factor(CGL_list* list, CGL_float increase_factor);
float CGL_list_get_increase_factor(CGL_list* list);
size_t CGL_list_get_item_size(CGL_list* list);
size_t CGL_list_get_size(CGL_list* list);
size_t CGL_list_get_capacity(CGL_list* list);
size_t CGL_list_push(CGL_list* list, void* data);
size_t CGL_list_pop(CGL_list* list, void* data);
void* CGL_list_get(CGL_list* list, size_t index, void* data);
void* CGL_list_get_random(CGL_list* list, void* data);
void* CGL_list_set(CGL_list* list, size_t index, void* data);
bool CGL_list_is_empty(CGL_list* list);
size_t CGL_list_find(CGL_list* list, void* data);
void CGL_list_reserve(CGL_list* list, size_t size);
void CGL_list_fill(CGL_list* list, size_t size);

#ifndef CGL_HASHTABLE_MAX_KEY_SIZE
#define CGL_HASHTABLE_MAX_KEY_SIZE 256
#endif

#ifndef CGL_HASHTABLE_ENTRY_STATIC_VALUE_SIZE
#define CGL_HASHTABLE_ENTRY_STATIC_VALUE_SIZE sizeof(uint64_t)
#endif

struct CGL_hashtable_entry;
typedef struct CGL_hashtable_entry CGL_hashtable_entry;

struct CGL_hashtable;
typedef struct CGL_hashtable CGL_hashtable;

struct CGL_hashtable_iterator;
typedef struct CGL_hashtable_iterator CGL_hashtable_iterator;

typedef uint32_t(*CGL_hash_function)(const void*, size_t);

// set key size to 0 if it is a string (it will be auto calculated using strlen)
CGL_hashtable* CGL_hashtable_create(size_t table_size, size_t key_size, size_t initial_capacity);
void CGL_hashtable_set_growth_rate(CGL_hashtable* table, CGL_float rate);
size_t CGL_hashtable_get_size(CGL_hashtable* table);
void CGL_hashtable_destroy(CGL_hashtable* table);
void CGL_hashtable_set(CGL_hashtable* table, const void* key, const void* value, size_t value_size);
size_t CGL_hashtable_get(CGL_hashtable* table, const void* key, void* value);
void* CGL_hashtable_get_ptr(CGL_hashtable* table, const void* key, size_t* value);
bool CGL_hashtable_exists(CGL_hashtable* table, const void* key);
bool CGL_hashtable_remove(CGL_hashtable* table, const void* key);
void CGL_hashtable_set_hash_function(CGL_hashtable* table, CGL_hash_function hash_function);
CGL_hashtable_iterator* CGL_hashtable_iterator_create(CGL_hashtable* table);
void CGL_hashtable_iterator_destroy(CGL_hashtable_iterator* iterator);
void CGL_hashtable_iterator_reset(CGL_hashtable_iterator* iterator);
bool CGL_hashtable_iterator_next(CGL_hashtable_iterator* iterator, void* key, void* data, size_t* size);
bool CGL_hashtable_iterator_curr(CGL_hashtable_iterator* iterator, void* key, void* data, size_t* size);
void* CGL_hashtable_iterator_curr_key(CGL_hashtable_iterator* iterator);

// getter setters for data types
#define CGL_DECLARE_HASHTABLE_GETTER_SETTER(type) \
    static type CGL_hashtable_get_##type (CGL_hashtable* table, const void* key)\
    { \
        type value; \
        CGL_hashtable_get(table, key, &value); \
        return value; \
    } \
    \
    static void CGL_hashtable_set_##type (CGL_hashtable* table, const void* key, type value) \
    { \
        CGL_hashtable_set(table, key, &value, sizeof(value)); \
    }

#ifndef CDL_DONT_DECLARE_HASHTABLE_STD_GETTER_SETTERS

static void CGL_hashtable_set_string (CGL_hashtable* table, const void* key, const char* value)
{
    CGL_hashtable_set(table, key, value, strlen(value) + 1);
}

CGL_DECLARE_HASHTABLE_GETTER_SETTER(int8_t);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(int16_t);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(int32_t);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(int64_t);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(uint8_t);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(uint16_t);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(uint32_t);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(uint64_t);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(float);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(double);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(bool);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(int);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(long);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(short);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(char);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(intptr_t);
CGL_DECLARE_HASHTABLE_GETTER_SETTER(uintptr_t);

#endif


// algorithms
uint32_t CGL_utils_crc32(const void* data, size_t size);
uint64_t CGL_utils_crc64(const void* data, size_t size);
void CGL_utils_rot13(const char* data_in, char* data_out);
uint32_t CGL_utils_super_fast_hash(const void* data, size_t size);

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
    CGL_float x;
    CGL_float y;
#ifdef __cplusplus
    CGL_vec2() : x(0), y(0) {}
    CGL_vec2(CGL_float x, CGL_float y) : x(x), y(y) {}
#endif
};
typedef struct CGL_vec2 CGL_vec2;

struct CGL_vec3
{
    CGL_float x;
    CGL_float y;
    CGL_float z;
#ifdef __cplusplus
    CGL_vec3() : x(0), y(0), z(0) {}
    CGL_vec3(CGL_float x, CGL_float y, CGL_float z) : x(x), y(y), z(z) {}
#endif
};
typedef struct CGL_vec3 CGL_vec3;

struct CGL_vec4
{
    CGL_float x;
    CGL_float y;
    CGL_float z;
    CGL_float w;
#ifdef __cplusplus
    CGL_vec4() : x(0), y(0), z(0), w(0) {}
    CGL_vec4(CGL_float x, CGL_float y, CGL_float z, CGL_float w) : x(x), y(y), z(z), w(w) {}
#endif
};
typedef struct CGL_vec4 CGL_vec4;
typedef struct CGL_vec4 CGL_color;

struct CGL_ivec4
{
    CGL_int x;
    CGL_int y;
    CGL_int z;
    CGL_int w;
#ifdef __cplusplus
    CGL_ivec4() : x(0), y(0), z(0), w(0) {}
    CGL_ivec4(CGL_int x, CGL_int y, CGL_int z, CGL_int w) : x(x), y(y), z(z), w(w) {}
#endif
};
typedef struct CGL_ivec4 CGL_ivec4;

struct CGL_mat3
{
    CGL_float m[9];
#ifdef __cplusplus
    CGL_mat3() {}
    CGL_mat3(CGL_float m0, CGL_float m1, CGL_float m2, CGL_float m3, CGL_float m4, CGL_float m5, CGL_float m6, CGL_float m7, CGL_float m8)
    {
        m[0] = m0; m[3] = m1; m[6] = m2;
        m[1] = m3; m[4] = m4; m[7] = m5;
        m[2] = m6; m[5] = m7; m[8] = m8;
    }
#endif
};
typedef struct CGL_mat3 CGL_mat3;

struct CGL_mat4
{
    CGL_float m[16];
#ifdef __cplusplus
    CGL_mat4() {}
    CGL_mat4(CGL_float m0, CGL_float m1, CGL_float m2, CGL_float m3,
             CGL_float m4, CGL_float m5, CGL_float m6, CGL_float m7,
             CGL_float m8, CGL_float m9, CGL_float m10, CGL_float m11,
             CGL_float m12, CGL_float m13, CGL_float m14, CGL_float m15)
    {
        m[0] = m0;   m[4] = m1;   m[8] = m2;     m[12] = m3;
        m[1] = m4;   m[5] = m5;   m[9] = m6;     m[13] = m7;
        m[2] = m8;   m[6] = m9;   m[10] = m10;   m[14] = m11;
        m[3] = m12;  m[7] = m13;  m[11] = m14;   m[15] = m15;
    }
#endif
};
typedef struct CGL_mat4 CGL_mat4;

struct CGL_quat
{
    CGL_vec3 vec;
    CGL_float w;
#ifdef __cplusplus
    CGL_quat() : vec(CGL_vec3(0, 0, 0)), w(0) {}
    CGL_quat(CGL_float x, CGL_float y, CGL_float z, CGL_float w) : vec(CGL_vec3(x, y, z)), w(w) {}
#endif
};
typedef struct CGL_quat CGL_quat;

// math functions with macros

#define CGL_PI (3.14159265358979323846f)
#define CGL_2PI (6.28318530717958647692f)
#define CGL_PI_2 (1.57079632679489661923f)
#define CGL_E (2.71828182845904523536f)
#define CGL_deg_to_rad(deg) ((deg) * (CGL_PI / 180.0f))
#define CGL_rad_to_deg(rad) ((rad) * (180.0f / CGL_PI))

#define CGL_float_lerp(a, b, t) (((a) * (1.0f - (t))) + ((b) * (t)))
CGL_float CGL_float_quadratic_lerp(CGL_float a, CGL_float b, CGL_float c, CGL_float t);
CGL_float CGL_float_cubic_lerp(CGL_float a, CGL_float b, CGL_float c, CGL_float d, CGL_float t);


#ifdef __cplusplus
#define CGL_vec2_init(x, y) CGL_vec2(x, y)
#else
#define CGL_vec2_init(x, y) ((CGL_vec2){(x), (y)})
#endif

#define CGL_vec2_add(a, b) CGL_vec2_init(a.x + b.x, a.y + b.y)
#define CGL_vec2_add_scaled(a, b, scale) CGL_vec2_init((a).x + (b).x * (scale), (a).y + (b).y * (scale))
#define CGL_vec2_sub(a, b) CGL_vec2_init(a.x - b.x, a.y - b.y)
#define CGL_vec2_mul(a, b) CGL_vec2_init(a.x * b.x, a.y * b.y)
#define CGL_vec2_div(a, b) CGL_vec2_init(a.x / b.x, a.y / b.y)
#define CGL_vec2_scale(a, s) CGL_vec2_init(a.x * (s), a.y * (s))
#define CGL_vec2_dot(a, b) (a.x * b.x + a.y * b.y)
#define CGL_vec2_length(a) sqrtf(a.x * a.x + a.y * a.y)
#define CGL_vec2_normalize(a) { CGL_float __CGL_vector_length##__LINE__ = 1.0f / CGL_vec2_length(a); a.x *= __CGL_vector_length##__LINE__; a.y *= __CGL_vector_length##__LINE__; }
#define CGL_vec2_lerp(a, b, t) CGL_vec2_init(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t)
#define CGL_vec2_min(a, b) CGL_vec2_init(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y)
#define CGL_vec2_max(a, b) CGL_vec2_init(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y)
#define CGL_vec2_equal(a, b) (a.x == b.x && a.y == b.y)
#define CGL_vec2_rotate_about_point(a, p, theta) CGL_vec2_init(((a.x - p.x) * cosf(theta) - (a.y - p.y) * sinf(theta) ), ((a.x - p.x) * sinf(theta) + (a.y - p.y) * cosf(theta)))
#define CGL_vec2_centroid_of_triangle(a, b, c) CGL_vec2_init((a.x + b.x + c.x) / 3.0f, (a.y + b.y + c.y) / 3.0f)
#define CGL_vec2_from_angle(theta) CGL_vec2_init(cosf(theta), sinf(theta))
#define CGL_vec2_angle_between(a, b) acosf(CGL_vec2_dot(a, b) / (CGL_vec2_length(a) * CGL_vec2_length(b)))
#define CGL_vec2_angle(a) atan2f(a.y, a.x)
#define CGL_vec2_angle_from_to(a, b) atan2f(b.y - a.y, b.x - a.x)
#define CGL_vec2_reflect(a, n) CGL_vec2_sub(a, CGL_vec2_scale(n, 2.0f * CGL_vec2_dot(a, n)))
#define CGL_vec2_perpendicular(a) CGL_vec2_init(-a.y, a.x)
#define CGL_vec2_create_from_higher_dimension(a) CGL_vec2_init(a.x, a.y)
#define CGL_vec2_elem_get(a, i) ((float*)&a)[i]
#define CGL_vec2_elem_set(a, i, v) (((float*)&a)[i] = v)
CGL_vec2 CGL_vec2_triple_product(CGL_vec2 a, CGL_vec2 b, CGL_vec2 c);

#ifdef __cplusplus
#define CGL_vec3_init(x, y, z) CGL_vec3(x, y, z)
#else
#define CGL_vec3_init(x, y, z) ((CGL_vec3){(x), (y), (z)})
#endif
#define CGL_vec3_add(a, b) CGL_vec3_init(a.x + b.x, a.y + b.y, a.z + b.z)
#define CGL_vec3_add_scaled(a, b, scale) CGL_vec3_init((a).x + (b).x * (scale), (a).y + (b).y * (scale), (a).z + (b).z * (scale))
#define CGL_vec3_sub(a, b) CGL_vec3_init(a.x - b.x, a.y - b.y, a.z - b.z)
#define CGL_vec3_mul(a, b) CGL_vec3_init(a.x * b.x, a.y * b.y, a.z * b.z)
#define CGL_vec3_div(a, b) CGL_vec3_init(a.x / b.x, a.y / b.y, a.z / b.z)
#define CGL_vec3_scale(a, s) CGL_vec3_init(a.x * (s), a.y * (s), a.z * (s))
#define CGL_vec3_dot(a, b) (a.x * b.x + a.y * b.y + a.z * b.z)
#define CGL_vec3_cross(a, b) CGL_vec3_init(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x)
#define CGL_vec3_length(a) sqrtf(a.x * a.x + a.y * a.y + a.z * a.z)
#define CGL_vec3_normalize(a) { CGL_float __CGL_vector_length##__LINE__ = 1.0f / CGL_vec3_length(a); a.x *= __CGL_vector_length##__LINE__; a.y *= __CGL_vector_length##__LINE__; a.z *= __CGL_vector_length##__LINE__; }
#define CGL_vec3_lerp(a, b, t) CGL_vec3_init(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t)
#define CGL_vec3_min(a, b) CGL_vec3_init(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z)
#define CGL_vec3_max(a, b) CGL_vec3_init(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z)
#define CGL_vec3_equal(a, b) (a.x == b.x && a.y == b.y && a.z == b.z)
#define CGL_vec3_rotate_x(a, theta) CGL_vec3_init(a.x, a.y * cosf(theta) - a.z * sinf(theta), a.y * sinf(theta) + a.z * cosf(theta))
#define CGL_vec3_rotate_y(v, theta) CGL_vec3_init(v.x * cosf(theta) + v.z * sinf(theta), v.y, -v.x * sinf(theta) + v.z * cosf(theta))
#define CGL_vec3_rotate_z(v, theta) CGL_vec3_init(v.x * cosf(theta) - v.y * sinf(theta), v.x * sinf(theta) + v.y * cosf(theta), v.z)
#define CGL_vec3_centroid_of_triangle(a, b, c) CGL_vec3_init((a.x + b.x + c.x) / 3.0f, (a.y + b.y + c.y) / 3.0f, (a.z + b.z + c.z) / 3.0f)
#define CGL_vec3_from_angle(theta) CGL_vec3_init(cosf(theta), sinf(theta), 0.0f)
#define CGL_vec3_angle_between(a, b) acosf(CGL_vec3_dot(a, b) / (CGL_vec3_length(a) * CGL_vec3_length(b)))
#define CGL_vec3_from_spherical_coordinates(r, theta, phi) CGL_vec3_init(r * sinf(theta) * cosf(phi), r * sinf(theta) * sinf(phi), r * cosf(theta))
#define CGL_vec3_to_spherical_coordinates(a) CGL_vec3_init(sqrtf(a.x * a.x + a.y * a.y + a.z * a.z), acosf(a.z / sqrtf(a.x * a.x + a.y * a.y + a.z * a.z)), atan2f(a.y, a.x))
#define CGL_vec3_from_higher_dimension(a) CGL_vec3_init(a.x, a.y, a.z)
#define CGL_vec3_from_vec2(a, z) CGL_vec3_init(a.x, a.y, z)
#define CGL_vec3_elem_get(a, i) ((float*)&a)[i]
#define CGL_vec3_elem_set(a, i, v) (((float*)&a)[i] = v)
CGL_vec3 CGL_vec3_reflect(CGL_vec3 a, CGL_vec3 n);
CGL_vec3 CGL_vec3_rotate_about_axis(CGL_vec3 v, CGL_vec3 axis, CGL_float theta);
CGL_vec3 CGL_vec3_triple_product(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c);
void CGL_vec3_calculate_orthonormal_basis_from_one_vector(CGL_vec3 a, CGL_vec3* pb, CGL_vec3* pc);

#ifdef __cplusplus
#define CGL_vec4_init(x, y, z, w) CGL_vec4(x, y, z, w)
#else
#define CGL_vec4_init(x, y, z, w) ((CGL_vec4){x, y, z, w})
#endif
#define CGL_vec4_add(a, b) CGL_vec4_init(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w)
#define CGL_vec4_sub(a, b) CGL_vec4_init(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w)
#define CGL_vec4_mul(a, b) CGL_vec4_init(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w)
#define CGL_vec4_div(a, b) CGL_vec4_init(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w)
#define CGL_vec4_scale(a, s) CGL_vec4_init(a.x * s, a.y * s, a.z * s, a.w * s)
#define CGL_vec4_dot(a, b) (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w)
#define CGL_vec4_lerp(a, b, t) CGL_vec4_init(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t)
#define CGL_vec4_min(a, b) CGL_vec4_init(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z, a.w < b.w ? a.w : b.w)
#define CGL_vec4_max(a, b) CGL_vec4_init(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z, a.w > b.w ? a.w : b.w)
#define CGL_vec4_equal(a, b) (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w)
#define CGL_vec4_normalize(a) { CGL_float __CGL_vector_length##__LINE__ = 1.0f / CGL_vec4_length(a); a.x *= __CGL_vector_length##__LINE__; a.y *= __CGL_vector_length##__LINE__; a.z *= __CGL_vector_length##__LINE__; a.w *= __CGL_vector_length##__LINE__; }
#define CGL_vec4_normalize_vec3(a) { CGL_float __CGL_vector_length##__LINE__ = 1.0f / CGL_vec3_length(a); a.x *= __CGL_vector_length##__LINE__; a.y *= __CGL_vector_length##__LINE__; a.z *= __CGL_vector_length##__LINE__; }
#define CGL_vec4_centroid_of_triangle(a, b, c) CGL_vec4_init((a.x + b.x + c.x) / 3.0f, (a.y + b.y + c.y) / 3.0f, (a.z + b.z + c.z) / 3.0f, (a.w + b.w + c.w) / 3.0f)
#define CGL_vec4_from_vec3(a, w) CGL_vec4_init(a.x, a.y, a.z, w)
#define CGL_vec4_from_vec2(a, z, w) CGL_vec4_init(a.x, a.y, z, w)
#define CGL_vec4_elem_get(a, i) ((float*)&a)[i]
#define CGL_vec4_elem_set(a, i, v) (((float*)&a)[i] = v)
CGL_vec4 CGL_vec4_triple_product(CGL_vec4 a, CGL_vec4 b, CGL_vec4 c);

#ifdef __cplusplus
#define CGL_mat3_init(a, b, c, d, e, f, g, h, i) CGL_mat3(a, b, c, d, e, f, g, h, i)
#else
#define CGL_mat3_init(m00, m01, m02, m10, m11, m12, m20, m21, m22) (CGL_mat3) \
{ \
    { \
        m00, m10, m20, \
        m01, m11, m21, \
        m02, m12, m22  \
    } \
}
#endif

#define CGL_mat3_add(a, b) (CGL_mat3){a.m[0] + b.m[0], a.m[1] + b.m[1], a.m[2] + b.m[2], a.m[3] + b.m[3], a.m[4] + b.m[4], a.m[5] + b.m[5], a.m[6] + b.m[6], a.m[7] + b.m[7], a.m[8] + b.m[8]}
#define CGL_mat3_sub(a, b) (CGL_mat3){a.m[0] - b.m[0], a.m[1] - b.m[1], a.m[2] - b.m[2], a.m[3] - b.m[3], a.m[4] - b.m[4], a.m[5] - b.m[5], a.m[6] - b.m[6], a.m[7] - b.m[7], a.m[8] - b.m[8]}
CGL_float CGL_mat3_det(CGL_mat3 a);
CGL_float CGL_mat3_trace(CGL_mat3 a);
CGL_mat3 CGL_mat3_transpose(CGL_mat3 a);

#define CGL_mat3_zero() CGL_mat3_init( \
    0.0f, 0.0f, 0.0f, \
    0.0f, 0.0f, 0.0f, \
    0.0f, 0.0f, 0.0f  \
)

#define CGL_mat3_identity() CGL_mat3_init( \
    1.0f, 0.0f, 0.0f, \
    0.0f, 1.0f, 0.0f, \
    0.0f, 0.0f, 1.0f  \
)

#define CGL_mat3_log(a) CGL_info( \
    "\n{\n" \
    "    {%f, %f, %f}\n" \
    "    {%f, %f, %f}\n" \
    "    {%f, %f, %f}\n" \
    "}\n", \
    a.m[0], a.m[3], a.m[6], \
    a.m[1], a.m[4], a.m[7], \
    a.m[2], a.m[5], a.m[8]  \
)


/*
#define CGL_mat4_init(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33) (CGL_mat4) \
{ \
    { \
        m00, m01, m02, m03, \
        m10, m11, m12, m13, \
        m20, m21, m22, m23, \
        m30, m31, m32, m33 \
    } \
}
*/

#ifdef __cplusplus
#define CGL_mat4_init(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) CGL_mat4(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p)
#else
#define CGL_mat4_init(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33) (CGL_mat4) \
{ \
    { \
        m00, m10, m20, m30, \
        m01, m11, m21, m31, \
        m02, m12, m22, m32, \
        m03, m13, m23, m33 \
    } \
}
#endif

#define CGL_mat4_add(a, b) CGL_mat_init( \
    CGL_mat4_elem_get(a, 0, 0) + CGL_mat4_elem_get(b, 0, 0), CGL_mat4_elem_get(a, 0, 1) + CGL_mat4_elem_get(b, 0, 1), CGL_mat4_elem_get(a, 0, 2) + CGL_mat4_elem_get(b, 0, 2), CGL_mat4_elem_get(a, 0, 3) + CGL_mat4_elem_get(b, 0, 3), \
    CGL_mat4_elem_get(a, 1, 0) + CGL_mat4_elem_get(b, 1, 0), CGL_mat4_elem_get(a, 1, 1) + CGL_mat4_elem_get(b, 1, 1), CGL_mat4_elem_get(a, 1, 2) + CGL_mat4_elem_get(b, 1, 2), CGL_mat4_elem_get(a, 1, 3) + CGL_mat4_elem_get(b, 1, 3), \
    CGL_mat4_elem_get(a, 2, 0) + CGL_mat4_elem_get(b, 2, 0), CGL_mat4_elem_get(a, 2, 1) + CGL_mat4_elem_get(b, 2, 1), CGL_mat4_elem_get(a, 2, 2) + CGL_mat4_elem_get(b, 2, 2), CGL_mat4_elem_get(a, 2, 3) + CGL_mat4_elem_get(b, 2, 3), \
    CGL_mat4_elem_get(a, 3, 0) + CGL_mat4_elem_get(b, 3, 0), CGL_mat4_elem_get(a, 3, 1) + CGL_mat4_elem_get(b, 3, 1), CGL_mat4_elem_get(a, 3, 2) + CGL_mat4_elem_get(b, 3, 2), CGL_mat4_elem_get(a, 3, 3) + CGL_mat4_elem_get(b, 3, 3)  \
)

#define CGL_mat4_sub(a, b) CGL_mat_init( \
    CGL_mat4_elem_get(a, 0, 0) - CGL_mat4_elem_get(b, 0, 0), CGL_mat4_elem_get(a, 0, 1) - CGL_mat4_elem_get(b, 0, 1), CGL_mat4_elem_get(a, 0, 2) - CGL_mat4_elem_get(b, 0, 2), CGL_mat4_elem_get(a, 0, 3) - CGL_mat4_elem_get(b, 0, 3), \
    CGL_mat4_elem_get(a, 1, 0) - CGL_mat4_elem_get(b, 1, 0), CGL_mat4_elem_get(a, 1, 1) - CGL_mat4_elem_get(b, 1, 1), CGL_mat4_elem_get(a, 1, 2) - CGL_mat4_elem_get(b, 1, 2), CGL_mat4_elem_get(a, 1, 3) - CGL_mat4_elem_get(b, 1, 3), \
    CGL_mat4_elem_get(a, 2, 0) - CGL_mat4_elem_get(b, 2, 0), CGL_mat4_elem_get(a, 2, 1) - CGL_mat4_elem_get(b, 2, 1), CGL_mat4_elem_get(a, 2, 2) - CGL_mat4_elem_get(b, 2, 2), CGL_mat4_elem_get(a, 2, 3) - CGL_mat4_elem_get(b, 2, 3), \
    CGL_mat4_elem_get(a, 3, 0) - CGL_mat4_elem_get(b, 3, 0), CGL_mat4_elem_get(a, 3, 1) - CGL_mat4_elem_get(b, 3, 1), CGL_mat4_elem_get(a, 3, 2) - CGL_mat4_elem_get(b, 3, 2), CGL_mat4_elem_get(a, 3, 3) - CGL_mat4_elem_get(b, 3, 3)  \
)

#define CGL_mat4_mul_scalar(a, s) CGL_mat4_init( \
    CGL_mat4_elem_get(a, 0, 0) * s, CGL_mat4_elem_get(a, 0, 1) * s, CGL_mat4_elem_get(a, 0, 2) * s, CGL_mat4_elem_get(a, 0, 3) * s, \
    CGL_mat4_elem_get(a, 1, 0) * s, CGL_mat4_elem_get(a, 1, 1) * s, CGL_mat4_elem_get(a, 1, 2) * s, CGL_mat4_elem_get(a, 1, 3) * s, \
    CGL_mat4_elem_get(a, 2, 0) * s, CGL_mat4_elem_get(a, 2, 1) * s, CGL_mat4_elem_get(a, 2, 2) * s, CGL_mat4_elem_get(a, 2, 3) * s, \
    CGL_mat4_elem_get(a, 3, 0) * s, CGL_mat4_elem_get(a, 3, 1) * s, CGL_mat4_elem_get(a, 3, 2) * s, CGL_mat4_elem_get(a, 3, 3) * s  \
)

CGL_mat4 CGL_mat4_mul(CGL_mat4 a, CGL_mat4 b);
CGL_float CGL_mat4_det(CGL_mat4 m);
CGL_float CGL_mat4_det_by_lu(CGL_mat4 m);
CGL_float CGL_mat4_det_by_gauss(CGL_mat4 m);
CGL_vec4 CGL_mat4_mul_vec4(CGL_mat4 m, CGL_vec4 v);
CGL_mat4 CGL_mat4_inverse(CGL_mat4 m);
CGL_mat4 CGL_mat4_transpose(CGL_mat4 m);
CGL_mat4 CGL_mat4_adjoint(CGL_mat4 m);
CGL_mat4 CGL_mat4_gauss_elim(CGL_mat4 m);
CGL_int CGL_mat4_rank(CGL_mat4 m);
CGL_float CGL_mat4_trace(CGL_mat4 m);
CGL_mat3 CGL_mat4_to_mat3(CGL_mat4 m);
CGL_mat4 CGL_mat4_from_mat3(CGL_mat3 m);
CGL_mat4 CGL_mat4_rotate_about_axis(CGL_vec3 axis, CGL_float angle);
CGL_mat4 CGL_mat4_look_at(CGL_vec3 eye, CGL_vec3 target, CGL_vec3 up);
void CGL_mat4_decompose_lu(CGL_mat4 m, CGL_mat4* l, CGL_mat4* u);

#define CGL_mat4_is_invertible(m) (CGL_mat4_det(m) != 0.0f)

//#define CGL_mat4_elem_get(mat, row, col) ((mat).m[row + col * 4])
//#define CGL_mat4_elem_set(mat, row, col, value) ((mat).m[row + col * 4] = value)

#define CGL_mat4_elem_get(mat, row, col) ((mat).m[row * 4 + col])
#define CGL_mat4_elem_set(mat, row, col, value) ((mat).m[row * 4 + col] = value)

#define CGL_mat3_elem_get(mat, row, col) ((mat).m[row + col * 3])
#define CGL_mat3_elem_set(mat, row, col, value) ((mat).m[row + col * 3] = value)

#define CGL_mat4_zero() CGL_mat4_init(\
    0.0f, 0.0f, 0.0f, 0.0f, \
    0.0f, 0.0f, 0.0f, 0.0f, \
    0.0f, 0.0f, 0.0f, 0.0f, \
    0.0f, 0.0f, 0.0f, 0.0f \
)

#define CGL_mat4_identity() CGL_mat4_init(\
    1.0f, 0.0f, 0.0f, 0.0f, \
    0.0f, 1.0f, 0.0f, 0.0f, \
    0.0f, 0.0f, 1.0f, 0.0f, \
    0.0f, 0.0f, 0.0f, 1.0f \
)

#define CGL_mat4_scale(x, y, z) CGL_mat4_init(\
    x, 0.0f, 0.0f, 0.0f, \
    0.0f, y, 0.0f, 0.0f, \
    0.0f, 0.0f, z, 0.0f, \
    0.0f, 0.0f, 0.0f, 1.0f \
)

#define CGL_mat4_translate(x, y, z) CGL_mat4_init(\
    1.0f, 0.0f, 0.0f, x, \
    0.0f, 1.0f, 0.0f, y, \
    0.0f, 0.0f, 1.0f, z, \
    0.0f, 0.0f, 0.0f, 1.0f \
)

#define CGL_mat4_rotate_x(angle) CGL_mat4_init(\
    1.0f, 0.0f, 0.0f, 0.0f, \
    0.0f, cosf(angle), -sinf(angle), 0.0f, \
    0.0f, sinf(angle), cosf(angle), 0.0f, \
    0.0f, 0.0f, 0.0f, 1.0f \
)

#define CGL_mat4_rotate_y(angle) CGL_mat4_init(\
    cosf(angle), 0.0f, sinf(angle), 0.0f, \
    0.0f, 1.0f, 0.0f, 0.0f, \
    -sinf(angle), 0.0f, cosf(angle), 0.0f, \
    0.0f, 0.0f, 0.0f, 1.0f \
)

#define CGL_mat4_rotate_z(angle) CGL_mat4_init(\
    cosf(angle), -sinf(angle), 0.0f, 0.0f, \
    sinf(angle), cosf(angle), 0.0f, 0.0f, \
    0.0f, 0.0f, 1.0f, 0.0f, \
    0.0f, 0.0f, 0.0f, 1.0f \
)

#define CGL_mat4_perspective(fov, aspect, near, far) CGL_mat4_init(\
    1.0f / (aspect * tanf(fov / 2.0f)), 0.0f, 0.0f, 0.0f, \
    0.0f, 1.0f / tanf(fov / 2.0f), 0.0f, 0.0f, \
    0.0f, 0.0f, (far + near) / (near - far), (2.0f * far * near) / (near - far), \
    0.0f, 0.0f, -1.0f, 0.0f \
)


/*
#define CGL_mat4_perspective(fov, aspect, fnear, ffar) CGL_mat4_init(\
    1.0f / (aspect * tanf(fov / 2.0f)), 0.0f, 0.0f, 0.0f, \
    0.0f, 1.0f / tanf(fov / 2.0f), 0.0f, 0.0f, \
    0.0f, 0.0f, (ffar + fnear) / (fnear - ffar), -1.0f, \
    0.0f, 0.0f, (2.0f * ffar * fnear) / (fnear - ffar), 0.0f \
)
*/



#define CGL_mat4_orthographic(left, right, bottom, top, fnear, ffar) CGL_mat4_init( \
    2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left), \
    0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom), \
    0.0f, 0.0f, -2.0f / (ffar - fnear), -(ffar + fnear) / (ffar - fnear), \
    0.0f, 0.0f, 0.0f, 1.0f \
)

#define CGL_mat4_log(m) CGL_info( \
    "\n{\n" \
    "    { %f, %f, %f, %f },\n" \
    "    { %f, %f, %f, %f },\n" \
    "    { %f, %f, %f, %f },\n" \
    "    { %f, %f, %f, %f }\n" \
    "}\n", \
    CGL_mat4_elem_get(m, 0, 0), CGL_mat4_elem_get(m, 0, 1), CGL_mat4_elem_get(m, 0, 2), CGL_mat4_elem_get(m, 0, 3), \
    CGL_mat4_elem_get(m, 1, 0), CGL_mat4_elem_get(m, 1, 1), CGL_mat4_elem_get(m, 1, 2), CGL_mat4_elem_get(m, 1, 3), \
    CGL_mat4_elem_get(m, 2, 0), CGL_mat4_elem_get(m, 2, 1), CGL_mat4_elem_get(m, 2, 2), CGL_mat4_elem_get(m, 2, 3), \
    CGL_mat4_elem_get(m, 3, 0), CGL_mat4_elem_get(m, 3, 1), CGL_mat4_elem_get(m, 3, 2), CGL_mat4_elem_get(m, 3, 3) \
)


typedef CGL_vec3(*CGL_parametric_function)(CGL_float, CGL_float);

#ifdef __cplusplus
#define CGL_quat_init(x, y, z, w) CGL_quat(x, y, z, w)
#else
#define CGL_quat_init(x, y, z, w) (CGL_quat){{x, y, z}, w}
#endif

#define CGL_quat_identity() CGL_quat_init(0.0f, 0.0f, 0.0f, 1.0f)
#define CGL_quat_equal(a, b) (CGL_vec3_equal(a.vec, b.vec) && (a.w == b.w))
#define CGL_quat_from_axis_angle(x, y, z, angle) CGL_quat_init(sinf(angle / 2.0f) * x, sinf(angle / 2.0f) * y, sinf(angle / 2.0f) * z, cosf(angle / 2.0f))
#define CGL_quat_from_x_rotation(angle) CGL_quat_from_axis_angle(1.0f, 0.0f, 0.0f, angle)
#define CGL_quat_from_y_rotation(angle) CGL_quat_from_axis_angle(0.0f, 1.0f, 0.0f, angle)
#define CGL_quat_from_z_rotation(angle) CGL_quat_from_axis_angle(0.0f, 0.0f, 1.0f, angle)
#define CGL_quat_from_euler_zyx(z, y, x) CGL_quat_init(cosf(x * 0.5f) * sinf(z * 0.5f) * cosf(y * 0.5f) - sinf(x * 0.5f) * cosf(z * 0.5f) * sinf(y * 0.5f), cosf(x * 0.5f) * cosf(z * 0.5f) * sinf(y * 0.5f) + sinf(x * 0.5f) * sinf(z * 0.5f) * cosf(y * 0.5f), sinf(x * 0.5f) * cosf(z * 0.5f) * cosf(y * 0.5f) - cosf(x * 0.5f) * sinf(z * 0.5f) * sinf(y * 0.5f), cosf(x * 0.5f) * cosf(z * 0.5f) * cosf(y * 0.5f) + sinf(x * 0.5f) * sinf(z * 0.5f) * sinf(y * 0.5f))
#define CGL_quat_conjuigate(q) CGL_quat_init(-q.vec.x, -q.vec.y, -q.vec.z, q.w)
#define CGL_quat_length(q) sqrtf(q.w * q.w + q.vec.x * q.vec.x + q.vec.y * q.vec.y + q.vec.z * q.vec.z)
#define CGL_quat_norm(q) CGL_quat_length(q)
#define CGL_quat_normalize(q) {CGL_float __CGL_quat_length##__LINE__ = 1.0f / CGL_quat_length(q); q.w *= __CGL_quat_length##__LINE__; q.vec.x *= __CGL_quat_length##__LINE__; q.vec.y *= __CGL_quat_length##__LINE__; q.vec.z *= __CGL_quat_length##__LINE__; }
#define CGL_quat_add(a, b) CGL_quat_init(a.vec.x + b.vec.x, a.vec.y + b.vec.y, a.vec.z + b.vec.z, a.w + b.w)
#define CGL_quat_sub(a, b) CGL_quat_init(a.vec.x - b.vec.x, a.vec.y - b.vec.y, a.vec.z - b.vec.z, a.w - b.w)
#define CGL_quat_conjugate(q) CGL_quat_init(-q.vec.x, -q.vec.y, -q.vec.z, q.w)
#define CGL_quat_mul_scalar(q, s) CGL_quat_init(q.vec.x * (s), q.vec.y * (s), q.vec.z * (s), q.w * (s))
void CGL_quat_to_euler_zyx(CGL_quat quat, CGL_float* z, CGL_float* y, CGL_float* x);
CGL_quat CGL_quat_slerp(CGL_quat a, CGL_quat b, CGL_float t);
CGL_quat CGL_quat_squad(CGL_quat a, CGL_quat b, CGL_quat c, CGL_quat d, CGL_float t);
CGL_float CGL_quat_to_axis_angle(CGL_quat quat, CGL_float* x, CGL_float* y, CGL_float* z);
CGL_quat CGL_quat_inverse(CGL_quat quat);
CGL_quat CGL_quat_mul(CGL_quat a, CGL_quat b);
void CGL_quat_rotate(CGL_quat q, CGL_float x, CGL_float y, CGL_float z, CGL_float* ox, CGL_float* oy, CGL_float* oz);
CGL_mat4 CGL_quat_to_mat4(CGL_quat quat);
CGL_vec3 CGL_vec3_apply_transformations(CGL_vec3 original, const CGL_vec3* translation, const CGL_vec3* rotation, const CGL_vec3* scale);
CGL_vec2 CGL_vec2_apply_transformations(CGL_vec2 original, const CGL_vec2* translation, const CGL_float* rotation, const CGL_vec2* scale);
CGL_vec4 CGL_quat_mul_vec4(CGL_quat q, CGL_vec4 v);

#ifndef CGL_EXCLUDE_MATH_FUNCTIONS
// vec2 
CGL_vec2 CGL_vec2_add_(CGL_vec2 a, CGL_vec2 b);
CGL_vec2 CGL_vec2_sub_(CGL_vec2 a, CGL_vec2 b);
CGL_vec2 CGL_vec2_mul_(CGL_vec2 a, CGL_vec2 b);
CGL_vec2 CGL_vec2_div_(CGL_vec2 a, CGL_vec2 b);
CGL_vec2 CGL_vec2_add_scalar_(CGL_vec2 a, CGL_float b);
CGL_vec2 CGL_vec2_sub_scalar_(CGL_vec2 a, CGL_float b);
CGL_vec2 CGL_vec2_scale_(CGL_vec2 a, CGL_float b);
CGL_vec2 CGL_vec2_normalize_(CGL_vec2 a);
CGL_float CGL_vec2_dot_(CGL_vec2 a, CGL_vec2 b);
CGL_float CGL_vec2_length_(CGL_vec2 a);
CGL_float CGL_vec2_cross_(CGL_vec2 a, CGL_vec2 b);
CGL_vec2 CGL_vec2_reflect_(CGL_vec2 a, CGL_vec2 b);
CGL_vec2 CGL_vec2_refract_(CGL_vec2 a, CGL_vec2 b, CGL_float eta);
CGL_vec2 CGL_vec2_rotate_(CGL_vec2 a, CGL_float angle);
CGL_vec2 CGL_vec2_lerp_(CGL_vec2 a, CGL_vec2 b, CGL_float t);

// vec3
CGL_vec3 CGL_vec3_add_(CGL_vec3 a, CGL_vec3 b);
CGL_vec3 CGL_vec3_add3_(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c);
CGL_vec3 CGL_vec3_sub_(CGL_vec3 a, CGL_vec3 b);
CGL_vec3 CGL_vec3_mul_(CGL_vec3 a, CGL_vec3 b);
CGL_vec3 CGL_vec3_div_(CGL_vec3 a, CGL_vec3 b);
CGL_vec3 CGL_vec3_add_scalar_(CGL_vec3 a, CGL_float b);
CGL_vec3 CGL_vec3_sub_scalar_(CGL_vec3 a, CGL_float b);
CGL_vec3 CGL_vec3_scale_(CGL_vec3 a, CGL_float b);
CGL_vec3 CGL_vec3_normalize_(CGL_vec3 a);
CGL_float CGL_vec3_dot_(CGL_vec3 a, CGL_vec3 b);
CGL_float CGL_vec3_length_(CGL_vec3 a);
CGL_vec3 CGL_vec3_cross_(CGL_vec3 a, CGL_vec3 b);
CGL_vec3 CGL_vec3_reflect_(CGL_vec3 a, CGL_vec3 b);
CGL_vec3 CGL_vec3_refract_(CGL_vec3 a, CGL_vec3 b, CGL_float eta);
CGL_vec3 CGL_vec3_lerp_(CGL_vec3 a, CGL_vec3 b, CGL_float t);

// vec4
CGL_vec4 CGL_vec4_add_(CGL_vec4 a, CGL_vec4 b);
CGL_vec4 CGL_vec4_sub_(CGL_vec4 a, CGL_vec4 b);
CGL_vec4 CGL_vec4_mul_(CGL_vec4 a, CGL_vec4 b);
CGL_vec4 CGL_vec4_div_(CGL_vec4 a, CGL_vec4 b);
CGL_vec4 CGL_vec4_add_scalar_(CGL_vec4 a, CGL_float b);
CGL_vec4 CGL_vec4_sub_scalar_(CGL_vec4 a, CGL_float b);
CGL_vec4 CGL_vec4_scale_(CGL_vec4 a, CGL_float b);
CGL_vec4 CGL_vec4_normalize_(CGL_vec4 a);
CGL_float CGL_vec4_dot_(CGL_vec4 a, CGL_vec4 b);
CGL_float CGL_vec4_length_(CGL_vec4 a);
CGL_vec4 CGL_vec4_lerp_(CGL_vec4 a, CGL_vec4 b, CGL_float t);

#endif


#endif

struct CGL_shape_triangle
{
    CGL_vec3 a;
    CGL_vec3 b;
    CGL_vec3 c;
};
typedef struct CGL_shape_triangle CGL_shape_triangle;

struct CGL_shape_quad
{
    CGL_vec3 a;
    CGL_vec3 b;
    CGL_vec3 c;
    CGL_vec3 d;
};
typedef struct CGL_shape_quad CGL_shape_quad;

struct CGL_shape
{
    CGL_vec3* vertices;
    CGL_vec3 position;
    CGL_vec3 rotation;
    CGL_vec3 scale;
    size_t vertices_count;
};
typedef struct CGL_shape CGL_shape;

void CGL_shape_init(CGL_shape* shape, size_t vertices_count);
void CGL_shape_destroy(CGL_shape* shape);

#ifndef CGL_SAT_COLLISION_MAX_COLLISIONS
#define CGL_SAT_COLLISION_MAX_COLLISIONS 4096
#endif

bool CGL_sat_collision_overlap_on_axis(CGL_shape* a, CGL_shape* b, CGL_vec2 axis, float* overlap_amount);
bool CGL_sat_collision_detect(CGL_shape* a, CGL_shape* b, CGL_vec2* n_vector);
void CGL_sat_collision_calculate_axes(CGL_shape* shape, CGL_vec2* axes, CGL_int* axes_count);
CGL_bool CGL_utils_is_point_in_triangle(CGL_vec2 p, CGL_vec2 a, CGL_vec2 b, CGL_vec2 c);


// GJK collision detection & EPA collision resolution

#ifndef CGL_GJK_EPA_MAX_POLYTOPE_VERTICES
#define CGL_GJK_EPA_MAX_POLYTOPE_VERTICES 64
#endif

#ifndef CGL_GJK_EPA_TOLERANCE
#define CGL_GJK_EPA_TOLERANCE 0.001f
#endif

CGL_vec3 CGL_gjk_shape_default_support(CGL_shape* a, CGL_vec3 d);
CGL_vec3 CGL_gjk_default_support(CGL_shape* a, CGL_shape* b, CGL_vec3 d);
CGL_bool CGL_gjk_check_collision_2d(CGL_shape* a, CGL_shape* b, CGL_vec3* simplex_out);
CGL_vec3 CGL_gjk_epa_2d(CGL_shape* a, CGL_shape* b, CGL_vec3* simplex);


// window
#if 1 // Just to use code folding

#ifndef CGL_EXCLUDE_WINDOW_API

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

typedef void(*CGL_window_key_callback)(CGL_window* window, CGL_int key, CGL_int scancode, CGL_int action, CGL_int mods);
typedef void(*CGL_window_mouse_button_callback)(CGL_window* window, CGL_int button, CGL_int action, CGL_int mods);
typedef void(*CGL_window_mouse_position_callback)(CGL_window* window, double xpos, double ypos);
typedef void(*CGL_window_mouse_scroll_callback)(CGL_window* window, double xoffset, double yoffset);
typedef void(*CGL_window_framebuffer_size_callback)(CGL_window* window, CGL_int width, CGL_int height);
typedef void(*CGL_window_close_callback)(CGL_window* window);
typedef void(*CGL_window_drag_n_drop_callback)(CGL_window* window, const CGL_byte** paths, CGL_int count);

CGL_window* CGL_window_create(CGL_int width, CGL_int height, const char* title); // create window and initialize GLFW
void CGL_window_destroy(CGL_window* window); // destroy window and terminate GLFW
void CGL_window_poll_events(CGL_window* window); // poll events from GLFW
void CGL_window_swap_buffers(CGL_window* window); // swap buffers
bool CGL_window_should_close(CGL_window* window); // check if window should close
void CGL_window_set_title(CGL_window* window, const char* title); // set window title
void CGL_window_set_size(CGL_window* window, CGL_int width, CGL_int height); // set window size
void CGL_window_set_position(CGL_window* window, CGL_int x, CGL_int y); // set window position
void CGL_window_set_hidden(CGL_window* window, bool hidden); // set window hidden
void CGL_window_set_user_data(CGL_window* window, void* user_data); // set window user data
void* CGL_window_get_user_data(CGL_window* window); // get window user data
void CGL_window_get_size(CGL_window* window, CGL_int* width, CGL_int* height); // get window size
void CGL_window_get_position(CGL_window* window, CGL_int* x, CGL_int* y); // get window position
void CGL_window_get_framebuffer_size(CGL_window* window, CGL_int* width, CGL_int* height); // get window framebuffer size
void CGL_window_set_key_callback(CGL_window* window, CGL_window_key_callback callback); // set key callback
void CGL_window_set_mouse_button_callback(CGL_window* window, CGL_window_mouse_button_callback callback); // set mouse button callback
void CGL_window_set_mouse_position_callback(CGL_window* window, CGL_window_mouse_position_callback callback); // set mouse position callback
void CGL_window_set_mouse_scroll_callback(CGL_window* window, CGL_window_mouse_scroll_callback callback); // set mouse scroll callback
void CGL_window_set_framebuffer_size_callback(CGL_window* window, CGL_window_framebuffer_size_callback callback); // set framebuffer size callback
void CGL_window_set_close_callback(CGL_window* window, CGL_window_close_callback callback); // set close callback
void CGL_window_set_drag_n_drop_callback(CGL_window* window, CGL_window_drag_n_drop_callback callback); // set drag and drop callback
void CGL_window_resecure_callbacks(CGL_window* window);
void CGL_window_make_context_current(CGL_window* window); // make opengl context current
GLFWwindow* CGL_window_get_glfw_handle(CGL_window* window);

// inputs
CGL_int CGL_window_get_key(CGL_window* window, CGL_int key); // get key state
CGL_bool CGL_window_is_key_pressed(CGL_window* window, CGL_int key); // check if key is pressed
CGL_int CGL_window_get_mouse_button(CGL_window* window, CGL_int button); // get mouse button state
void CGL_window_get_mouse_position(CGL_window* window, double* xpos, double* ypos); // get mouse position

#endif

#endif

// opengl
#if 1 // Just to use code folding

#ifndef CGL_EXCLUDE_GRAPHICS_API

//#ifndef  CGL_EXCLUDE_WINDOW_API
#include <glad/glad.h>


#ifdef CGL_WASM
#define CGL_OPENGL_VERSION_MAJOR 2
#define CGL_OPENGL_VERSION_MINOR 0
#define CGL_OPENGL_VERSION_MAJOR_STR "2"
#define CGL_OPENGL_VERSION_MINOR_STR "0"
#else
#define CGL_OPENGL_VERSION_MAJOR 4
#define CGL_OPENGL_VERSION_MINOR 3
#define CGL_OPENGL_VERSION_MAJOR_STR "4"
#define CGL_OPENGL_VERSION_MINOR_STR "3"
#endif

struct CGL_image
{
    void* data;
    CGL_int height;
    CGL_int width;
    CGL_int bytes_per_channel;
    CGL_int channels;
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
    CGL_vec4 bone_wieghts;
    CGL_ivec4 bone_ids;
};
typedef struct CGL_mesh_vertex CGL_mesh_vertex;

struct CGL_mesh_cpu
{
    size_t index_count;
    size_t index_count_used;
    uint32_t* indices;
    size_t vertex_count;
    size_t vertex_count_used;
    CGL_mesh_vertex* vertices;
};
typedef struct CGL_mesh_cpu CGL_mesh_cpu;


struct CGL_ssbo;
typedef struct CGL_ssbo CGL_ssbo;

struct CGL_ubo;
typedef struct CGL_ubo CGL_ubo;

// Taken directly from OpenGL
#define CGL_CUBEMAP_POSITIVE_X 0x8515 
#define CGL_CUBEMAP_NEGATIVE_X 0x8516 
#define CGL_CUBEMAP_POSITIVE_Y 0x8517 
#define CGL_CUBEMAP_NEGATIVE_Y 0x8518 
#define CGL_CUBEMAP_POSITIVE_Z 0x8519 
#define CGL_CUBEMAP_NEGATIVE_Z 0x851A 

// texture
CGL_texture* CGL_texture_create(CGL_image* image);
CGL_texture* CGL_texture_create_blank(CGL_int width, CGL_int height, GLenum format, GLenum internal_format, GLenum type); // create texture
CGL_texture* CGL_texture_create_array(CGL_int width, CGL_int height, CGL_int layers, GLenum format, GLenum internal_format, GLenum type);
CGL_texture* CGL_texture_create_3d(CGL_int width, CGL_int height, CGL_int depth, GLenum format, GLenum internal_format, GLenum type);
CGL_texture* CGL_texture_create_cubemap();
void CGL_texture_cubemap_set_face(CGL_texture* texture, CGL_int face, CGL_image* image);
void CGL_texture_array_set_layer_data(CGL_texture* texture, CGL_int layer, void* data);
void CGL_texture_destroy(CGL_texture* texture); // destroy texture
void CGL_texture_bind(CGL_texture* texture, CGL_int unit); // bind texture to unit
void CGL_texture_set_data(CGL_texture* texture, void* data); // set texture data
void CGL_texture_set_sub_data(CGL_texture* texture, size_t offset_x, size_t offset_y, size_t size_x, size_t size_y,  void* data); // set texture data
void CGL_texture_set_user_data(CGL_texture* texture, void* user_data); // set texture user data
void* CGL_texture_get_user_data(CGL_texture* texture); // get texture user data
CGL_uint CGL_texture_get_internal_handle(CGL_texture* texture); // get texture user data
void CGL_texture_get_size(CGL_texture* texture, CGL_int* width, CGL_int* height); // get texture size
void CGL_texture_set_scaling_method(CGL_texture* texture, GLint method);
void CGL_texture_set_wrapping_method(CGL_texture* texture, GLint method);

// framebuffer
#ifndef CGL_EXCLUDE_WINDOW_API
CGL_framebuffer* CGL_framebuffer_create_from_default(CGL_window* window); // create framebuffer from default framebuffer
#endif
CGL_framebuffer* CGL_framebuffer_create(CGL_int width, CGL_int height); // create framebuffer (32 bit)
CGL_framebuffer* CGL_framebuffer_create_basic(CGL_int width, CGL_int height);
void CGL_framebuffer_add_color_attachment(CGL_framebuffer* framebuffer, CGL_texture* texture); // add color attachment to framebuffer
void CGL_framebuffer_destroy(CGL_framebuffer* framebuffer); // destroy framebuffer
CGL_texture* CGL_framebuffer_get_color_attachment(CGL_framebuffer* framebuffer, CGL_int index); // get color attachment from framebuffer
void CGL_framebuffer_bind(CGL_framebuffer* framebuffer); // bind framebuffer
void CGL_framebuffer_get_size(CGL_framebuffer* framebuffer, CGL_int* width, CGL_int* height); // get framebuffer size
void CGL_framebuffer_set_user_data(CGL_framebuffer* framebuffer, void* user_data); // set framebuffer user data
void* CGL_framebuffer_get_user_data(CGL_framebuffer* framebuffer); // get framebuffer user data
void CGL_framebuffer_read_pixels(CGL_framebuffer* framebuffer, CGL_int x, CGL_int y, CGL_int width, CGL_int height, void* pixels); // read pixels from framebuffer
CGL_int CGL_framebuffer_get_mouse_pick_id(CGL_framebuffer* framebuffer, CGL_int x, CGL_int y, CGL_int index); // get mouse pick id
CGL_texture* CGL_framebuffer_get_color_texture(CGL_framebuffer* framebuffer); // get color texture
CGL_texture* CGL_framebuffer_get_depth_texture(CGL_framebuffer* framebuffer); // get depth texture

// gl

void CGL_gl_clear(CGL_float r, CGL_float g, CGL_float b, CGL_float a); // clear framebuffer
bool CGL_gl_init(); // load opengl functions
void CGL_gl_shutdown(); // clean up
void CGL_gl_render_screen_quad();

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
CGL_mesh_cpu* CGL_mesh_cpu_recalculate_normals(CGL_mesh_cpu* mesh);
CGL_mesh_cpu* CGL_mesh_cpu_load_obj(const char* path);
CGL_mesh_cpu* CGL_mesh_cpu_triangle(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c); // generate triangle mesh
CGL_mesh_cpu* CGL_mesh_cpu_plane(CGL_vec3 front, CGL_vec3 right, CGL_int resolution, CGL_float scale); // generate plane mesh
CGL_mesh_cpu* CGL_mesh_cpu_quad(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d); // generate quad mesh
CGL_mesh_cpu* CGL_mesh_cpu_cube(CGL_bool use_3d_tex_coords);
CGL_mesh_cpu* CGL_mesh_cpu_sphere(CGL_int res_u, CGL_int res_v);
CGL_mesh_cpu* CGL_mesh_cpu_create_from_parametric_function(CGL_int res_u, CGL_int res_v, CGL_float start_u, CGL_float start_v, CGL_float end_u, CGL_float end_v, CGL_parametric_function function);
CGL_mesh_cpu* CGL_mesh_cpu_create_cylinder(CGL_vec3 start, CGL_vec3 end, CGL_float radius0, CGL_float radius1, CGL_int resolution);

CGL_mesh_cpu* CGL_mesh_cpu_add_mesh(CGL_mesh_cpu* mesh, CGL_mesh_cpu* mesh_other);
CGL_mesh_cpu* CGL_mesh_cpu_add_cube(CGL_mesh_cpu* mesh, CGL_bool use_3d_tex_coords);
CGL_mesh_cpu* CGL_mesh_cpu_add_triangle(CGL_mesh_cpu* mesh, CGL_vec3 a, CGL_vec3 b, CGL_vec3 c); // generate triangle mesh
CGL_mesh_cpu* CGL_mesh_cpu_add_quad(CGL_mesh_cpu* mesh, CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d); // generate quad mesh
CGL_mesh_cpu* CGL_mesh_cpu_add_from_parametric_function(CGL_mesh_cpu* mesh, CGL_int res_u, CGL_int res_v, CGL_float start_u, CGL_float start_v, CGL_float end_u, CGL_float end_v, CGL_parametric_function function);
CGL_mesh_cpu* CGL_mesh_cpu_add_sphere(CGL_mesh_cpu* mesh, CGL_int res_u, CGL_int res_v);
CGL_mesh_cpu* CGL_mesh_cpu_add_cylinder(CGL_mesh_cpu* mesh, CGL_vec3 start, CGL_vec3 end, CGL_float radius0, CGL_float radius1, CGL_int resolution);



CGL_mesh_cpu* CGL_mesh_cpu_offset_vertices(CGL_mesh_cpu* mesh, CGL_vec3 offset);
CGL_mesh_cpu* CGL_mesh_cpu_scale_vertices(CGL_mesh_cpu* mesh, CGL_float scale);
CGL_mesh_cpu* CGL_mesh_cpu_rotate_vertices(CGL_mesh_cpu* mesh, CGL_quat rotation);
CGL_mesh_cpu* CGL_mesh_cpu_transform_vertices(CGL_mesh_cpu* mesh, CGL_mat4 transform);



void CGL_mesh_cpu_generate_c_initialization_code(CGL_mesh_cpu* mesh, char* buffer, const char* function_name);
void CGL_mesh_cpu_destroy(CGL_mesh_cpu* mesh); // destroy mesh (cpu)

// shader

CGL_shader* CGL_shader_create(const char* vertex_shader_source, const char* fragment_shader_source, char** error); // create shader
CGL_shader* CGL_shader_compute_create(const char* compute_shader_source, char** error); // create compute shader
CGL_shader* CGL_shader_create_from_files(const char* vertex_shader_file, const char* fragment_shader_file, char** error); // create shader from files
CGL_shader* CGL_shader_compute_create_from_files(const char* compute_shader_file, char** error); // create compute shader from files
void CGL_shader_destroy(CGL_shader* shader); // destroy shader
void CGL_shader_bind(CGL_shader* shader); // bind shader
int CGL_shader_get_uniform_location(CGL_shader* shader, const char* name); // get uniform location
void CGL_shader_set_uniform_mat4(CGL_shader* shader, CGL_int location, CGL_mat4* matrix); // set uniform matrix
void CGL_shader_set_uniform_vec4(CGL_shader* shader, CGL_int location, CGL_vec4* vector); // set uniform vector
void CGL_shader_set_uniform_vec3(CGL_shader* shader, CGL_int location, CGL_vec3* vector); // set uniform vector
void CGL_shader_set_uniform_vec2(CGL_shader* shader, CGL_int location, CGL_vec2* vector); // set uniform vector
void CGL_shader_set_uniform_int(CGL_shader* shader, CGL_int location, CGL_int value); // set uniform int
void CGL_shader_set_uniform_bool(CGL_shader* shader, CGL_int location, bool value); // set uniform bool
void CGL_shader_set_uniform_float(CGL_shader* shader, CGL_int location, CGL_float value); // set uniform float
void CGL_shader_set_uniform_vec2v(CGL_shader* shader, CGL_int location, CGL_float x, CGL_float y); // set uniform vector
void CGL_shader_set_uniform_vec3v(CGL_shader* shader, CGL_int location, CGL_float x, CGL_float y, CGL_float z); // set uniform vector
void CGL_shader_set_uniform_vec4v(CGL_shader* shader, CGL_int location, CGL_float x, CGL_float y, CGL_float z, CGL_float w); // set uniform vector
void CGL_shader_set_uniform_ivec2v(CGL_shader* shader, CGL_int location, CGL_int x, CGL_int y); // set uniform vector
void CGL_shader_set_uniform_ivec3v(CGL_shader* shader, CGL_int location, CGL_int x, CGL_int y, CGL_int z); // set uniform vector
void CGL_shader_set_uniform_ivec4v(CGL_shader* shader, CGL_int location, CGL_int x, CGL_int y, CGL_int z, CGL_int w); // set uniform vector
void CGL_shader_set_user_data(CGL_shader* shader, void* user_data); // set shader user data
void* CGL_shader_get_user_data(CGL_shader* shader); // get shader user data
void CGL_shader_compute_dispatch(CGL_shader* shader, CGL_int x, CGL_int y, CGL_int z); // dispatch compute shader

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

CGL_ubo* CGL_ubo_create(); // create ubo
void CGL_ubo_destroy(CGL_ubo* ubo); // destroy ubo
void CGL_ubo_bind(CGL_ubo* ubo, CGL_shader* shader, const CGL_byte* name, uint32_t binding); // bind ubo
void CGL_ubo_set_data(CGL_ubo* ubo, size_t size, void* data, bool static_draw); // set ubo data
void CGL_ubo_set_sub_data(CGL_ubo* ubo, size_t offset, size_t size, void* data, bool static_draw); // set ubo sub data
void CGL_ubo_get_data(CGL_ubo* ubo, size_t* size, void* data); // get ubo data
void CGL_ubo_get_sub_data(CGL_ubo* ubo, size_t offset, size_t size, void* data); // get ubo sub data
void CGL_ubo_set_user_data(CGL_ubo* ubo, void* user_data); // set ubo user data
void* CGL_ubo_get_user_data(CGL_ubo* ubo); // get ubo user data
size_t CGL_ubo_get_size(CGL_ubo* ubo); // get ubo size



#endif
#endif

// BLOOM
#ifndef CGL_EXCLUDE_BLOOM
#ifdef CGL_EXCLUDE_GRAPHICS_API
#error "CGL Bloom requires CGL Graphics API"
#endif

#define CGL_BLOOM_SHADER_MODE_PREFILTER      0
#define CGL_BLOOM_SHADER_MODE_DOWNSAMPLE     1
#define CGL_BLOOM_SHADER_MODE_UPSAMPLE       2
#define CGL_BLOOM_SHADER_MODE_COMPOSITE      3

#define CGL_BLOOM_MAX_ITERATIONS 8

struct CGL_bloom;
typedef struct CGL_bloom CGL_bloom;

CGL_bloom* CGL_bloom_create(CGL_int width, CGL_int height, CGL_int iterations);
void CGL_bloom_destroy(CGL_bloom* bloom);
void CGL_bloom_set_threshold(CGL_bloom* bloom, CGL_float val);
float CGL_bloom_get_threshold(CGL_bloom* bloom);
void CGL_bloom_set_knee(CGL_bloom* bloom, CGL_float val);
float CGL_bloom_get_knee(CGL_bloom* bloom);
void CGL_bloom_set_offset(CGL_bloom* bloom, CGL_float x, CGL_float y);
void CGL_bloom_apply(CGL_bloom* bloom, CGL_texture* tex);
void CGL_bloom_apply2(CGL_bloom* bloom, CGL_texture* tex_src, CGL_texture* tex_dst);
int CGL_bloom_get_iterations(CGL_bloom* bloom);
CGL_texture* CGL_bloom_get_lod_texture(CGL_bloom* bloom, CGL_int index);
CGL_texture* CGL_bloom_get_prefiltered_texture(CGL_bloom* bloom);

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
void CGL_camera_set_fov(CGL_camera* camera, CGL_float fov);
void CGL_camera_set_aspect_ratio(CGL_camera* camera, CGL_float aspect_ratio);
void CGL_camera_set_z_near(CGL_camera* camera, CGL_float z_near);
void CGL_camera_set_z_far(CGL_camera* camera, CGL_float z_far);
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

// phong renderer
#ifndef CGL_EXCLUDE_GRAPHICS_API
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
void CGL_phong_pipeline_set_ambient_light_strength(CGL_phong_pipeline* pipeline, CGL_float strength);
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
void CGL_phong_mat_set_shininess(CGL_phong_mat* mat, CGL_float shininess);
void CGL_phong_mat_set_user_data(CGL_phong_mat* mat, void* data);
void* CGL_phong_mat_get_user_data(CGL_phong_mat* mat);
void CGL_phong_mat_disable_normal_map(CGL_phong_mat* mat);
void CGL_phong_mat_enable_wireframe(CGL_phong_mat* mat);
void CGL_phong_mat_disable_wireframe(CGL_phong_mat* mat);
float CGL_phong_mat_get_shininess(CGL_phong_mat* mat);

CGL_phong_light* CGL_phong_light_directional(CGL_vec3 direction, CGL_vec3 color, CGL_float itensity);
CGL_phong_light* CGL_phong_light_point(CGL_vec3 position, CGL_vec3 color, CGL_float itensity, CGL_float constant, CGL_float linear, CGL_float quadratic);
void CGL_phong_light_destroy(CGL_phong_light* light);
void CGL_phong_light_set_intensity(CGL_phong_light* light, CGL_float intensity);
void CGL_phong_light_set_color(CGL_phong_light* light, CGL_vec3 color);
float CGL_phong_light_get_intensity(CGL_phong_light* light);
CGL_vec3 CGL_phong_light_get_color(CGL_phong_light* light);
uint32_t CGL_phong_light_get_type(CGL_phong_light* light);

void CGL_phong_render_begin(CGL_phong_pipeline* pipeline, CGL_camera* camera);
void CGL_phong_render(CGL_mesh_gpu* mesh, CGL_mat4* model_matrix, CGL_phong_mat* material, CGL_phong_pipeline* pipeline, CGL_camera* camera);
void CGL_phong_render_end(CGL_phong_pipeline* pipeline, CGL_camera* camera);

#endif

#endif

#endif

// tilemap renderer
#ifndef CGL_EXCLUDE_GRAPHICS_API

// tilemap renderer
#if 1

#ifndef CGL_EXCLUDE_TILEMAP_RENDERER

struct CGL_tilemap;
typedef struct CGL_tilemap CGL_tilemap;

struct CGL_tile;
typedef struct CGL_tile CGL_tile;

CGL_tilemap* CGL_tilemap_create(uint32_t tile_count_x, uint32_t tile_count_y, uint32_t tile_size_x, uint32_t tile_size_y, uint32_t ssbo_binding);
void CGL_tilemap_destroy(CGL_tilemap* tilemap);
void CGL_tilemap_set_auto_upload(CGL_tilemap* tilemap, bool value);
bool CGL_tilemap_get_auto_upload(CGL_tilemap* tilemap);
bool CGL_tilemap_upload(CGL_tilemap* tilemap);
void CGL_tilemap_set_tile_color(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, CGL_float r, CGL_float g, CGL_float b);
void CGL_tilemap_set_tile_texture_from_array(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, uint32_t texture_index);
void CGL_tilemap_set_tile_texture_from_tileset(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, CGL_float texture_x_min, CGL_float texture_y_min, CGL_float texture_x_max, CGL_float texture_y_max);
void CGL_tilemap_clear_tile(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y);
void CGL_tilemap_render(CGL_tilemap* tilemap, CGL_float scale_x, CGL_float scale_y, CGL_float offset_x, CGL_float offset_y, CGL_texture* texture);
void CGL_tilemap_reset(CGL_tilemap* tilemap);

#endif

#endif

#endif

// sky renderer
#ifndef CGL_EXCLUDE_GRAPHICS_API
#ifndef CGL_EXCLUDE_SKY_RENDERER

struct CGL_sky;
typedef struct CGL_sky CGL_sky;

CGL_sky* CGL_sky_create();
void CGL_sky_destroy(CGL_sky* sky);
void CGL_sky_use_skybox(CGL_sky* sky);
void CGL_sky_use_skysphere(CGL_sky* sky);
void CGL_sky_use_cubemap(CGL_sky* sky);
void CGL_sky_use_procedural(CGL_sky* sky);
void CGL_sky_cubemap_set_face(CGL_sky* sky, CGL_int face, CGL_image* image);
void CGL_sky_procedural_set_options(CGL_sky* sky, CGL_float cirrus, CGL_float cumulus, CGL_float upf);
void CGL_sky_procedural_set_time(CGL_sky* sky, CGL_float time);
void CGL_sky_procedural_set_sun_position(CGL_sky* sky, CGL_vec3 position);
void CGL_sky_render(CGL_sky* sky, CGL_camera* camera);

#endif // CGL_EXCLUDE_SKY_RENDERER
#endif // CGL_EXCLUDE_GRAPHICS_API

// markov chains
#ifndef CGL_EXCLUDE_MARKOV_API

struct CGL_markov;
typedef struct CGL_markov CGL_markov;

typedef bool(*CGL_markov_token_function)(void*, const void*, const size_t, void*, void*); // (void* context, const void* data, const size_t data_size, void* key, void* value)

struct CGL_markov_token_function_ngram_text_context;
typedef struct CGL_markov_token_function_ngram_text_context CGL_markov_token_function_ngram_text_context;

CGL_markov_token_function_ngram_text_context* CGL_markov_token_function_ngram_text_context_create(size_t n);
void CGL_markov_token_function_ngram_text_context_destroy(CGL_markov_token_function_ngram_text_context* context);
bool CGL_markov_token_function_ngram_text(void* context, const void* data, const size_t data_size, void* key, void* value);

CGL_markov* CGL_markov_create(const size_t key_size, const size_t value_size);
bool CGL_markov_train(CGL_markov* markov, void* context, const void* data, const size_t data_size, const CGL_markov_token_function function);
bool CGL_markov_generate(const CGL_markov* markov, const void* key, void* value);
void CGL_markov_destroy(CGL_markov* markov);

#endif

// text rendering
#ifndef CGL_EXCLUDE_TEXT_RENDER

#ifndef CGL_EXCLUDE_GRAPHICS_API

struct CGL_font_character
{
    CGL_vec2 size;
    CGL_vec2 normalized_size;
    CGL_vec2 offset;
    CGL_vec2 normalized_offset;
    CGL_vec2 bearing;
    CGL_vec2 advance;
    unsigned char* bitmap;
    char ch;
};
typedef struct CGL_font_character CGL_font_character;

struct CGL_font;
typedef struct CGL_font CGL_font;

bool CGL_text_init();
void CGL_text_shutdown();
CGL_font* CGL_font_load(const char* path);
void CGL_font_destory(CGL_font* font);
CGL_texture* CGL_font_get_atlas(CGL_font* font);
bool CGL_font_build_atlas(CGL_font* font, size_t width, size_t height, size_t font_size);
CGL_font_character* CGL_font_get_characters(CGL_font* font);

CGL_texture* CGL_text_bake_to_texture(const char* string, size_t string_length, CGL_font* font, size_t* width, size_t* height);

#endif

#endif

// widgets
#ifndef CGL_EXCLUDE_WIDGETS
#ifndef CGL_GRAPHICS_API

#ifndef CGL_WIDGETS_MAX_VERTICES
#define CGL_WIDGETS_MAX_VERTICES 1024 * 128
#endif

#ifndef CGL_WIDGETS_MAX_INDICES
#define CGL_WIDGETS_MAX_INDICES 1024 * 128
#endif

struct CGL_widgets_context;
typedef struct CGL_widgets_context CGL_widgets_context;

CGL_widgets_context* CGL_widgets_context_create(size_t max_vertices, size_t max_indices);
void CGL_widgets_context_destroy(CGL_widgets_context* context);
CGL_widgets_context* CGL_window_get_current_context();
bool CGL_widgets_init();
void CGL_widgets_shutdown();
void CGL_window_set_current_context(CGL_widgets_context* context);
bool CGL_widgets_begin();
bool CGL_widgets_begin_int(CGL_float scale_x, CGL_float scale_y, CGL_float offset_x, CGL_float offset_y);
bool CGL_widgets_end();
bool CGL_widgets_flush();
bool CGL_widgets_flush_if_required();
bool CGL_widgets_add_vertices(CGL_mesh_vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count);
void CGL_widgets_add_vertex(CGL_mesh_vertex* vertex);
void CGL_widgets_add_vertex_p(CGL_vec3 position);
void CGL_widgets_add_vertex_p3f(CGL_float pos_x, CGL_float pos_y, CGL_float pos_z);
void CGL_widgets_add_vertex_pt(CGL_vec3 position, CGL_vec2 tex_coord);
void CGL_widgets_add_vertex_p3ft(CGL_float pos_x, CGL_float pos_y, CGL_float pos_z, CGL_vec2 tex_coord);
void CGL_widgets_add_vertex_pt2f(CGL_vec3 position, CGL_float tex_x, CGL_float tex_y);
void CGL_widgets_add_vertex_p3ft2f(CGL_float pos_x, CGL_float pos_y, CGL_float pos_z, CGL_float tex_x, CGL_float tex_y);
void CGL_widgets_set_stroke_color(CGL_color color);
void CGL_widgets_set_stroke_colorf(CGL_float r, CGL_float g, CGL_float b, CGL_float a);
void CGL_widgets_set_stroke_thicnkess(CGL_float thickness);
void CGL_widgets_set_fill_color(CGL_color color);
void CGL_widgets_set_fill_colorf(CGL_float r, CGL_float g, CGL_float b, CGL_float a);
void CGL_widgets_set_fill_mode(bool is_enabled);
void CGL_widgets_add_triangle(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c);
void CGL_widgets_add_quad(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d);
void CGL_widgets_add_quad_8f(CGL_float ax, CGL_float ay, CGL_float bx, CGL_float by, CGL_float cx, CGL_float cy, CGL_float dx, CGL_float dy);
void CGL_widgets_add_line(CGL_vec3 start, CGL_vec3 end);
void CGL_widgets_add_line2f(CGL_float start_x, CGL_float start_y, CGL_float end_x, CGL_float end_y);
void CGL_widgets_add_rect(CGL_vec3 start, CGL_vec2 size);
void CGL_widgets_add_rect2f(CGL_float start_x, CGL_float start_y, CGL_float size_x, CGL_float size_y);
void CGL_widgets_add_circle(CGL_vec3 position, CGL_float radius);
void CGL_widgets_add_circle2f(CGL_float pos_x, CGL_float pos_y, CGL_float radius);
void CGL_widgets_add_oval(CGL_vec3 position, CGL_vec2 radius);
void CGL_widgets_add_oval2f(CGL_float pos_x, CGL_float pos_y, CGL_float radius_x, CGL_float radius_y);
void CGL_widgets_add_arc2f(CGL_float pos_x, CGL_float pos_y, CGL_float radius, CGL_float start_angle, CGL_float end_angle, CGL_int resolution);
CGL_bool CGL_widgets_add_character(char c, CGL_float x, CGL_float y, CGL_float sx, CGL_float sy);
CGL_bool CGL_widgets_add_string(const char* str, CGL_float x, CGL_float y, CGL_float sx, CGL_float sy);
void CGL_widgets_add_shape_out_line(CGL_shape* shape);
void CGL_widgets_add_cubic_bazier(CGL_vec3 start, CGL_vec3 end, CGL_vec3 control_1, CGL_vec3 control_2, CGL_int resolution);
void CGL_widgets_add_cubic_bazier2v(CGL_vec2 start, CGL_vec2 end, CGL_vec2 control_1, CGL_vec2 control_2, CGL_int resolution);
void CGL_widgets_add_cubic_bazier2f(CGL_float start_x, CGL_float start_y, CGL_float end_x, CGL_float end_y, CGL_float control_1_x, CGL_float control_1_y, CGL_float control_2_x, CGL_float control_2_y, CGL_int resolution);
void CGL_widgets_add_cubic_bazier_points(CGL_vec3 start, CGL_vec3 end, CGL_vec3 control_1, CGL_vec3 control_2, CGL_int resolution);
void CGL_widgets_add_cubic_bazier_points2v(CGL_vec2 start, CGL_vec2 end, CGL_vec2 control_1, CGL_vec2 control_2, CGL_int resolution);
void CGL_widgets_add_cubic_bazier_points2f(CGL_float start_x, CGL_float start_y, CGL_float end_x, CGL_float end_y, CGL_float control_1_x, CGL_float control_1_y, CGL_float control_2_x, CGL_float control_2_y, CGL_int resolution);

void CGL_widgets_add_plot_function(CGL_float start_x, CGL_float start_y, CGL_float size_x, CGL_float size_y, CGL_float(*func_to_plot)(CGL_float), CGL_int num_samples, CGL_float x_min, CGL_float x_max, CGL_float y_min, CGL_float y_max, CGL_float plot_thickness, CGL_vec3 plot_color, CGL_bool draw_axes, CGL_float axes_thickness, CGL_vec3 axes_color);
void CGL_widgets_add_plot_array(CGL_float start_x, CGL_float start_y, CGL_float size_x, CGL_float size_y, CGL_vec2* values, CGL_sizei count, CGL_float marker_size, CGL_vec3 marker_color, CGL_bool draw_axes, CGL_float axes_thickness, CGL_vec3 axes_color);
void CGL_widgets_add_plot_pie_chart(CGL_float start_x, CGL_float start_y, CGL_float radius, CGL_float* values, CGL_vec3* colors, CGL_sizei count, CGL_int resolution);
void CGL_widgets_add_bar_graph(CGL_float start_x, CGL_float start_y, CGL_float size_x, CGL_float size_y, CGL_float* values, CGL_vec3* colors, CGL_sizei count, CGL_bool draw_axes, CGL_float axes_thickness, CGL_vec3 axes_color, CGL_bool vertical);

#endif
#endif

#ifndef CGL_EXCLUDE_NODE_EDITOR

#ifdef CGL_EXCLUDE_WIDGETS
#error "CGL Widgets are required for CGL Node Editor"
#endif

#define CGL_NODE_EDITOR_NODE_MAX_PINS 8

struct CGL_node_editor;
typedef struct CGL_node_editor CGL_node_editor;

struct CGL_node_editor_node;
typedef struct CGL_node_editor_node CGL_node_editor_node;

struct CGL_node_editor_pin
{
    CGL_color color;
    CGL_node_editor_node* parent;
    void* user_data;
    CGL_float pos_x;
    CGL_float pos_y;
    CGL_int index;
    bool left;
    bool is_set;
};
typedef struct CGL_node_editor_pin CGL_node_editor_pin;

struct CGL_node_editor_node
{
    CGL_node_editor_pin left_pins[CGL_NODE_EDITOR_NODE_MAX_PINS];
    CGL_node_editor_pin right_pins[CGL_NODE_EDITOR_NODE_MAX_PINS];
    char title[256];
    CGL_color color;
    CGL_int pins_count[2];
    CGL_node_editor* editor;
    void* user_data;
    CGL_float pos_x;
    CGL_float pos_y;
    CGL_float size_x;
    CGL_float size_y;
    bool selected;
    bool render_title;
};

struct CGL_node_editor_input
{
    CGL_float mouse_pos_x;
    CGL_float mouse_pos_y;
    CGL_float aspect_ratio;
    CGL_float scale;
    bool mouse_button_left;    
    bool mouse_button_right;    
    bool mouse_button_middle;   
    bool shift; 
    bool ctrl; 
    bool escape;
};
typedef struct CGL_node_editor_input CGL_node_editor_input;

CGL_node_editor* CGL_node_editor_create();
void CGL_node_editor_destroy(CGL_node_editor* editor);
void CGL_node_editor_update(CGL_node_editor* editor, CGL_node_editor_input* input);
void CGL_node_editor_set_on_connect(CGL_node_editor* editor, void(*onconnect)(CGL_node_editor_pin*, CGL_node_editor_pin*));
void CGL_node_editor_set_on_drop(CGL_node_editor* editor, void(*ondrop)(CGL_float, CGL_float, CGL_node_editor_pin*));
void CGL_node_editor_render_begin(CGL_node_editor* editor);
void CGL_node_editor_clear_focused_pins(CGL_node_editor* editor);
void CGL_node_editor_render_end(CGL_node_editor* editor);
void CGL_node_editor_set_offset(CGL_node_editor* editor, CGL_float x, CGL_float y);
void CGL_node_editor_get_offset(CGL_node_editor* editor, CGL_float* x, CGL_float* y);
void CGL_node_editor_get_linked_pins(CGL_node_editor* editor, CGL_node_editor_pin** x, CGL_node_editor_pin** y);
void CGL_node_editor_render_link(CGL_node_editor_pin* left, CGL_node_editor_pin* right, CGL_color color, CGL_float midper);
void CGL_node_editor_render_linkf(CGL_node_editor_pin* left, CGL_node_editor_pin* right, CGL_float cr, CGL_float cg, CGL_float cb, CGL_float ca, CGL_float midper);
void CGL_node_editor_render_link_curved(CGL_node_editor_pin* left, CGL_node_editor_pin* right, CGL_color color, CGL_float x_dist, CGL_float y_dist, CGL_int resolution);
void CGL_node_editor_render_link_curvedf(CGL_node_editor_pin* left, CGL_node_editor_pin* right, CGL_float cr, CGL_float cg, CGL_float cb, CGL_float ca, CGL_float x_dist, CGL_float y_dist, CGL_int resolution);

void CGL_node_editor_node_init(CGL_node_editor* editor, CGL_node_editor_node* node);
void CGL_node_editor_node_update(CGL_node_editor_node* node);
void CGL_node_editor_node_render(CGL_node_editor_node* node);
void CGL_node_editor_node_set_position(CGL_node_editor_node* node, CGL_float x, CGL_float y);
void CGL_node_editor_node_set_title(CGL_node_editor_node* node, const CGL_byte* title);
CGL_node_editor_pin* CGL_node_editor_node_get_pin(CGL_node_editor_node* node, bool left, CGL_int index);

#endif


#ifndef CGL_EXCLUDE_RAY_CASTER

#define CGL_RAY_CASTER_MAX_TRIANGLES 4096
#define CGL_RAY_CASTER_MAX_WALLS     4096

struct CGL_ray_caster;
typedef struct CGL_ray_caster CGL_ray_caster;

CGL_ray_caster* CGL_ray_caster_create();
void CGL_ray_caster_destroy(CGL_ray_caster* caster);
void CGL_ray_caster_add_walls(CGL_ray_caster* caster, CGL_vec4* walls, CGL_int walls_count);
void CGL_ray_caster_add_wall(CGL_ray_caster* caster, CGL_vec4 wall);
void CGL_ray_caster_clear_walls(CGL_ray_caster* caster);
float CGL_ray_caster_get_intersection_point_for_wall(CGL_vec2 pos, CGL_vec2 dir, CGL_vec4 wall, CGL_vec2* intersection_point, float* angle, CGL_float max_dist);
float CGL_ray_caster_get_intersection_point_for_walls(CGL_vec2 pos, CGL_vec2 dir, CGL_vec4* walls, CGL_int wall_count, CGL_vec2* intersection_point, float* angle, CGL_float max_dist);
void CGL_ray_caster_calculate(CGL_ray_caster* caster, CGL_vec2 pos, CGL_float rotation, bool visualise_rays);
bool CGL_ray_caster_is_in_view(CGL_ray_caster* caster, CGL_vec2 point);
void CGL_ray_caster_set_angle_limits(CGL_ray_caster* caster, CGL_float min_angle, CGL_float max_angle);
void CGL_ray_caster_set_max_distance(CGL_ray_caster* caster, CGL_float val);
CGL_shape_triangle* CGL_ray_caster_get_triangles(CGL_ray_caster* caster, CGL_int* count);

#endif


#ifndef CGL_EXCLUDE_SQUARE_MARCHER

struct CGL_square_marcher;
typedef struct CGL_square_marcher CGL_square_marcher;

typedef bool(*CGL_square_marcher_distance_function)(CGL_vec2, float*, void*);

CGL_square_marcher* CGL_square_marcher_create();
void CGL_square_marcher_destroy(CGL_square_marcher* marcher);
void CGL_square_marcher_set_user_data(CGL_square_marcher* marcher, void* user_data);
void* CGL_square_marcher_get_user_data(CGL_square_marcher* marcher);
void CGL_square_marcher_enable_interpolation(CGL_square_marcher* marcher, CGL_bool enable);
CGL_mesh_cpu* CGL_square_marcher_generate_mesh(CGL_square_marcher* marcher, CGL_square_marcher_distance_function sampler, CGL_vec2 start, CGL_vec2 end, CGL_int resolution_x, CGL_int resolution_y);

#endif


#ifndef CGL_EXCLUDE_POST_PROCESSOR

struct CGL_post_processor;
typedef struct CGL_post_processor CGL_post_processor;

void CGL_post_processor_init();
void CGL_post_processor_shutdown();
void CGL_post_processor_process_shades(CGL_texture* output, CGL_texture* scene, CGL_texture* albedo, CGL_int shades);
void CGL_post_processor_process_outline(CGL_texture* output, CGL_texture* scene, CGL_texture* normal, CGL_texture* depth, CGL_float outline_width);
void CGL_post_processor_process_hatching(CGL_texture* output, CGL_texture* scene, CGL_texture* uv, CGL_texture** hatch_tex, CGL_int hatch_tex_count);

// for legacy code
#define  CGL_toon_post_processor_init CGL_post_processor_init
#define  CGL_toon_post_processor_shutdown CGL_post_processor_shutdown
#define  CGL_toon_post_processor_process_shades CGL_post_processor_process_shades
#define  CGL_toon_post_processor_process_outline CGL_post_processor_process_outline

#endif


struct CGL_wav_file
{
    CGL_byte* data;
    CGL_int size;
    CGL_int channel_count;
    CGL_int sample_rate;
    CGL_int bits_per_sample;
    CGL_int data_size;
    CGL_float duration;
};
typedef struct CGL_wav_file CGL_wav_file;

CGL_bool CGL_wav_file_load(CGL_wav_file* file, const char* filename);
void CGL_wav_file_destroy(CGL_wav_file* file);
CGL_int CGL_wav_file_sample(CGL_wav_file* file, CGL_int channel, CGL_int sample_id);
CGL_int CGL_wav_file_sample_at_time(CGL_wav_file* file, CGL_int channel, CGL_float time);


#ifndef CGL_EXCLUDE_AUDIO

#include <AL/al.h>
#include <AL/alc.h>

struct CGL_audio_context;
typedef struct CGL_audio_context CGL_audio_context;

struct CGL_audio_source;
typedef struct CGL_audio_source CGL_audio_source;

struct CGL_audio_buffer;
typedef struct CGL_audio_buffer CGL_audio_buffer;

struct CGL_audio_listener;
typedef struct CGL_audio_listener CGL_audio_listener;

CGL_int CGL_audio_get_last_error();
const CGL_byte* CGL_audio_get_error_string(CGL_int error);
const CGL_byte* CGL_audio_get_device_list();
const CGL_byte* CGL_audio_get_default_device();
CGL_audio_context* CGL_audio_context_create(const CGL_byte* device_name);
void CGL_audio_context_destroy(CGL_audio_context* context);
void CGL_audio_make_context_current(CGL_audio_context* context);
CGL_audio_source* CGL_audio_source_create();
void CGL_audio_source_destroy(CGL_audio_source* source);
void CGL_audio_source_reset_to_defaults(CGL_audio_source* source);
void CGL_audio_source_set_buffer(CGL_audio_source* source, CGL_audio_buffer* buffer);
void CGL_audio_source_set_looping(CGL_audio_source* source, CGL_bool looping);
void CGL_audio_source_set_pitch(CGL_audio_source* source, CGL_float pitch);
void CGL_audio_source_set_gain(CGL_audio_source* source, CGL_float gain);
void CGL_audio_source_set_position(CGL_audio_source* source, CGL_vec3 position);
void CGL_audio_source_set_velocity(CGL_audio_source* source, CGL_vec3 velocity);
void CGL_audio_source_set_direction(CGL_audio_source* source, CGL_vec3 direction);
CGL_float CGL_audio_source_get_seconds_offset(CGL_audio_source* source);
void CGL_audio_source_set_seconds_offset(CGL_audio_source* source, CGL_sizei seconds);
CGL_sizei CGL_audio_source_get_samples_offset(CGL_audio_source* source);
void CGL_audio_source_set_samples_offset(CGL_audio_source* source, CGL_sizei samples);
CGL_sizei CGL_audio_source_get_bytes_offset(CGL_audio_source* source);
void CGL_audio_source_set_bytes_offset(CGL_audio_source* source, CGL_sizei bytes);
void CGL_audio_source_play(CGL_audio_source* source);
void CGL_audio_source_pause(CGL_audio_source* source);
void CGL_audio_source_stop(CGL_audio_source* source);
CGL_bool CGL_audio_source_is_playing(CGL_audio_source* source);
CGL_bool CGL_audio_source_is_paused(CGL_audio_source* source);
CGL_bool CGL_audio_source_is_stopped(CGL_audio_source* source);
CGL_audio_buffer* CGL_audio_buffer_create();
void CGL_audio_buffer_destroy(CGL_audio_buffer* buffer);
void CGL_audio_buffer_set_data_from_wav_file(CGL_audio_buffer* buffer, CGL_wav_file* file);
void CGL_audio_buffer_set_data(CGL_audio_buffer* buffer, void* data, ALenum format, ALsizei size, ALsizei freq);
void CGL_audio_listener_set_position(CGL_vec3 position);
void CGL_audio_listener_set_velocity(CGL_vec3 velocity);
void CGL_audio_listener_set_orientation(CGL_vec3 forward, CGL_vec3 up);


#endif


#ifndef CGL_EXCLUDE_TRAIL_RENDERER

struct CGL_trail_point
{
    CGL_vec3 position;
    struct CGL_trail_point* next;
    void* user_data;
    CGL_int index;
    CGL_float lifespan;
    CGL_float thickness;
    CGL_float distance;
};
typedef struct CGL_trail_point CGL_trail_point;

struct CGL_trail;
typedef struct CGL_trail CGL_trail;

typedef CGL_bool(*CGL_trail_point_update_function)(CGL_trail*, CGL_trail_point*);

CGL_trail* CGL_trail_create();
void CGL_trail_destroy(CGL_trail* trail);
void CGL_trail_render(CGL_trail* trail, CGL_mat4* view, CGL_mat4* projection, CGL_shader* shader);
void CGL_trail_add_point(CGL_trail* trail, CGL_vec3 point, CGL_float lifespan, CGL_float thickness);
void CGL_trail_set_resolution(CGL_trail* trail, CGL_int resolution);
void CGL_trail_update(CGL_trail* trail, CGL_float delta_time);
void CGL_trail_bake_mesh(CGL_trail* trail);
void CGL_trail_clear(CGL_trail* trail);
void CGL_trail_calculate_distances(CGL_trail* trail);
void CGL_trail_set_point_update_function(CGL_trail* trail, CGL_trail_point_update_function function);
void CGL_trail_set_user_data(CGL_trail* trail, void* user_data);
void* CGL_trail_get_user_data(CGL_trail* trail);
CGL_mesh_gpu* CGL_trail_get_mesh_gpu(CGL_trail* trail);
CGL_trail_point* CGL_trail_get_first_point(CGL_trail* trail);
CGL_float CGL_trail_get_length(CGL_trail* trail);
void CGL_trail_set_max_length(CGL_trail* trail, CGL_float length);
void CGL_trail_set_min_points_distance(CGL_trail* trail, CGL_float min_points_distance);

#endif

#ifdef CGL_INCLUDE_PASCAL_CASE_TYPES
typedef CGL_int Integer;
typedef CGL_float Real;
typedef CGL_byte Byte;
typedef CGL_bool Boolean;
typedef CGL_vec2 Vector2;
typedef CGL_vec3 Vector3;
typedef CGL_vec4 Vector4;
typedef CGL_mat2 Matrix2x2;
typedef CGL_mat3 Matrix3x3;
typedef CGL_mat4 Matrix4x4;
typedef CGL_quat Quaternion;
typedef CGL_color Color;
#endif


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ Implementation of CGL -------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef CGL_IMPLEMENTATION

// include windows headers for windows builds
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push, 0)
#define WIN32_LEAN_AND_MEAN
#ifndef  CGL_WASM
#include <Windows.h>
#endif
#pragma warning(pop)
#endif

#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
#endif

// list
#if 1

// data structures
struct CGL_list
{
    size_t size;
    size_t capacity;
    size_t item_size;
    CGL_float increase_factor;
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
    CGL_free(list->data);
    CGL_free(list);
}

void CGL_list_set_increase_factor(CGL_list* list, CGL_float increase_factor)
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

void* CGL_list_get_random(CGL_list* list, void* data)
{
    CGL_int index = CGL_utils_random_int(0, list->size-1);
    return CGL_list_get(list, index, data);
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
    return (size_t)UINT64_MAX;
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
    list->size = CGL_utils_max(size, list->size);
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


// The function CGL_thread_start() starts a new thread. The function
// argument is the function that the thread will execute. The argument
// argument is passed to the thread function. The return value is true
// if the thread was successfully started, and false otherwise.
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


// This function is called when the thread is destroyed
// It frees the thread structure from memory
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

// Description: This function checks to see if the thread is running.
// Parameters:
//  thread: The thread to check
// Return value: true if the thread is running, false otherwise
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

/*
 * This function joins the thread, which means that the calling thread
 * will wait until the thread represented by the given thread pointer
 * has finished executing. This function returns true if the thread
 * was joined successfully, or false if an error occurred.
 */
bool CGL_thread_join(CGL_thread* thread)
{
    if(!thread->handle) return true;
    bool result = (WaitForSingleObject(thread->handle, INFINITE) != WAIT_FAILED);
    thread->running = false;
    return result;
}

/*
 * Returns true if the thread is joinable, false otherwise.
 *
 * A thread is joinable if it can be waited upon. A thread is not joinable
 * if it has already been joined or detached.
 *
 * When a thread is created, it is joinable by default. It can be made
 * unjoinable by calling pthread_detach. It can be made joinable again by
 * calling pthread_join.
 */
bool CGL_thread_joinable(CGL_thread* thread)
{
    return true; // Temporary
}


/*
 * Create a new mutex. If set is true, the mutex is created set. Otherwise, it
 * is created unset.
 */
CGL_mutex* CGL_mutex_create(bool set)
{
    CGL_mutex* mutex = (CGL_mutex*)malloc(sizeof(CGL_mutex));
    mutex->handle = CreateMutex(NULL, (BOOL)set, NULL);
    return mutex;
}

/* Destroy a mutex. */
void CGL_mutex_destroy(CGL_mutex* mutex)
{
    if(mutex->handle) CloseHandle(mutex->handle);
    CGL_free(mutex);
}

// This function locks a mutex, and returns 1 if it was successful, and 0 otherwise.
// If timeout is 0, it will wait forever for the lock to be acquired.
// If timeout is non-zero, it will wait for at most timeout microseconds for the lock to be acquired.
// If the lock is acquired within the timeout period, this function returns 1. Otherwise, it returns 0.
int CGL_mutex_lock(CGL_mutex* mutex, uint64_t timeout)
{
    return (int)WaitForSingleObject(mutex->handle, (DWORD)timeout);
}

// CGL_mutex_release: Releases a mutex.
//    mutex: The mutex to release.
void CGL_mutex_release(CGL_mutex* mutex)
{
    ReleaseMutex(mutex->handle);
}

#else // for posix (using pthread)

#include <pthread.h>
#include <signal.h> 

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
    // thread->handle = NULL;
    thread->id = 0;
    thread->running = false;
    return thread;
}

bool CGL_thread_start(CGL_thread* thread, CGL_thread_function function, void* argument)
{
    if(thread->running) CGL_thread_join(thread);
    thread->function = function;
    bool success = pthread_create(&thread->handle, 0, (void*)function, argument);
    thread->id = (uintptr_t)thread->handle; // Temporary
    thread->running = true;
    return success;
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
    pthread_mutex_destroy(&mutex->handle);
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

// hashtable
#if 1


struct CGL_hashtable_entry
{
    uint8_t key[CGL_HASHTABLE_MAX_KEY_SIZE];
    uint8_t value_static[CGL_HASHTABLE_ENTRY_STATIC_VALUE_SIZE]; // if value size is less than CGL_HASHTABLE_ENTRY_VALUE_SIZE then place here in static memory rather than allocating
    size_t value_size;
    size_t next_entry; // next entry in the linked list with same hash
    size_t index;
    void* value;
    bool set;
};

struct CGL_hashtable
{
    CGL_hashtable_entry* storage;
    size_t* table;
    CGL_hash_function hash_function;
    size_t capacity;
    size_t table_size;
    size_t key_size;
    size_t count;
    CGL_float growth_rate;
};

struct CGL_hashtable_iterator
{
    CGL_hashtable* hashtable;
    CGL_hashtable_entry* current_entry;
    size_t index;
};

static void __CGL_hashtable_calculate_hashtable_index_key_size(CGL_hashtable* table, size_t* key_size, size_t* hashtable_index, const void* key)
{
    size_t key_size_l = CGL_utils_clamp(((table->key_size == 0) ? strlen((const char*)key) + 1 : table->key_size), 0, CGL_HASHTABLE_MAX_KEY_SIZE);
    *key_size = key_size_l;
    size_t hashtable_index_l = table->hash_function(key, key_size_l) % table->table_size;
    *hashtable_index = hashtable_index_l;
}

static CGL_hashtable_entry* __CGL_hashtable_get_entry_ptr(CGL_hashtable* table, const void* key, size_t* table_index)
{
    size_t key_size, hashtable_index;
    __CGL_hashtable_calculate_hashtable_index_key_size(table, &key_size, &hashtable_index, key);
    size_t entry_id = table->table[hashtable_index];
    if(entry_id == 0) return NULL;
    CGL_hashtable_entry* entry = &table->storage[entry_id];
    if(table_index) 
    {
        if(memcmp(entry->key, key, key_size) == 0) *table_index = hashtable_index;
        else *table_index = (size_t)UINT64_MAX;
    }

    while(entry->index != 0)
    {
        if (memcmp(entry->key, key, key_size) == 0)
            return entry;
        entry = &table->storage[entry->next_entry];
    }

    return NULL;
}

static bool __CGL_hashtable_expand_storage(CGL_hashtable* table)
{
    size_t initial_capacity = table->capacity;
    void* initial_storage = table->storage;
    table->capacity = (size_t)(table->capacity * table->growth_rate);
    table->storage = (CGL_hashtable_entry*)CGL_malloc(table->capacity * sizeof(CGL_hashtable_entry));
    memset(table->storage, 0, table->capacity * sizeof(CGL_hashtable_entry));
    CGL_free(table->storage);
    return table->storage != NULL;
}

static void __CGL_hashtable_reset_hashtable_entry(CGL_hashtable_entry* entry)
{
    memset(entry->key, 0, CGL_HASHTABLE_MAX_KEY_SIZE);
    memset(entry->value_static, 0, CGL_HASHTABLE_ENTRY_STATIC_VALUE_SIZE);
    entry->value_size = 0;
    entry->next_entry = 0;
    if(entry->value) CGL_free(entry->value);
    entry->value = NULL;
    entry->set = false;
    entry->index = 0;
}

static size_t __CGL_hashtable_get_new_entry(CGL_hashtable* table, bool* expanded)
{
    size_t entry = 1;
    for(; entry < table->capacity ; entry ++)
        if(!table->storage[entry].set)
            break;
    if(entry >= table->capacity)
    {
        entry = table->capacity;
        if(!__CGL_hashtable_expand_storage(table)) return 0;
        if(expanded) *expanded = true;
    }
    table->storage[entry].index = entry;
    return entry;
}

CGL_hashtable* CGL_hashtable_create(size_t table_size, size_t key_size, size_t initial_capacity)
{
    CGL_hashtable* table = (CGL_hashtable*)CGL_malloc(sizeof(CGL_hashtable));
    if(!table) return NULL;
    table->table_size = table_size;
    table->key_size = key_size;
    table->capacity = initial_capacity;
    table->count = 0;
    table->growth_rate = 1.5f;
    table->hash_function = CGL_utils_super_fast_hash;
    table->storage = (CGL_hashtable_entry*)CGL_malloc(sizeof(CGL_hashtable_entry) * initial_capacity);
    if(!table->storage) {CGL_free(table); return NULL;}
    memset(table->storage, 0, (sizeof(CGL_hashtable_entry) * initial_capacity));
    table->table = (size_t*)CGL_malloc(sizeof(size_t) * table_size);
    if(!table->table) {CGL_free(table->storage); CGL_free(table); return NULL;}
    memset(table->table, 0, (sizeof(size_t) * table_size));
    return table;
}

void CGL_hashtable_destroy(CGL_hashtable* table)
{
    for(size_t i = 0 ; i < table->capacity ; i ++)
        if(table->storage[i].set && table->storage[i].value_size > CGL_HASHTABLE_ENTRY_STATIC_VALUE_SIZE)
            CGL_free(table->storage[i].value);
    CGL_free(table->storage);
    CGL_free(table->table);
    CGL_free(table);
}

void CGL_hashtable_set(CGL_hashtable* table, const void* key, const void* value, size_t value_size)
{
    size_t key_size, hashtable_index;
    __CGL_hashtable_calculate_hashtable_index_key_size(table, &key_size, &hashtable_index, key);
    

    size_t new_entry_id = 0;
    CGL_hashtable_entry* existing_entry = __CGL_hashtable_get_entry_ptr(table, key, NULL);
    if(existing_entry)
    {
        if(existing_entry->value_size > CGL_HASHTABLE_ENTRY_STATIC_VALUE_SIZE) CGL_free(existing_entry->value);
        new_entry_id = existing_entry->index;
    }
    else
    {
        new_entry_id = __CGL_hashtable_get_new_entry(table, NULL);    
        if (table->table[hashtable_index] == 0)
            table->table[hashtable_index] = new_entry_id;
        else
        {
            CGL_hashtable_entry* target_entry_ptr = &table->storage[table->table[hashtable_index]];
            while(target_entry_ptr->next_entry != 0) target_entry_ptr = &table->storage[target_entry_ptr->next_entry];
            target_entry_ptr->next_entry = new_entry_id;
        }
    }

    
    table->storage[new_entry_id].set = true;
    memcpy(table->storage[new_entry_id].key, key, key_size);
    table->storage[new_entry_id].value_size = value_size;
    table->storage[new_entry_id].next_entry = 0;
    table->storage[new_entry_id].value = NULL;
    void* target_value_ptr = NULL;
    if(value_size <= CGL_HASHTABLE_ENTRY_STATIC_VALUE_SIZE) target_value_ptr = table->storage[new_entry_id].value_static;
    else target_value_ptr = table->storage[new_entry_id].value = CGL_malloc(value_size);
    memcpy(target_value_ptr, value, value_size);
        
}

size_t CGL_hashtable_get(CGL_hashtable* table, const void* key, void* value)
{
    CGL_hashtable_entry* entry = __CGL_hashtable_get_entry_ptr(table, key, NULL);
    if(!entry) return 0;
    if(value && entry->value_size > 0) memcpy(value, ( (entry->value_size > CGL_HASHTABLE_ENTRY_STATIC_VALUE_SIZE) ? entry->value : entry->value_static ), entry->value_size);
    return entry->value_size;
}

void* CGL_hashtable_get_ptr(CGL_hashtable* table, const void* key, size_t* value)
{
    CGL_hashtable_entry* entry = __CGL_hashtable_get_entry_ptr(table, key, NULL);
    if(!entry) return NULL;
    if(value) *value = entry->value_size;   
    return (entry->value_size > CGL_HASHTABLE_ENTRY_STATIC_VALUE_SIZE) ? entry->value : entry->value_static;
}

bool CGL_hashtable_exists(CGL_hashtable* table, const void* key)
{
    return __CGL_hashtable_get_entry_ptr(table, key, NULL) != NULL;
}

bool CGL_hashtable_remove(CGL_hashtable* table, const void* key)
{
    if(!CGL_hashtable_exists(table, key)) return false;

    size_t key_size, hashtable_index;
    __CGL_hashtable_calculate_hashtable_index_key_size(table, &key_size, &hashtable_index, key);
    
    CGL_hashtable_entry* entry_to_clear = &table->storage[table->table[hashtable_index]];
    if(memcmp(entry_to_clear->key, key, key_size) == 0)
        table->table[hashtable_index] = entry_to_clear->next_entry;
    else
    {
        while(table->storage[table->table[hashtable_index]].next_entry != 0)
        {
            if(memcmp(table->storage[table->storage[table->table[hashtable_index]].next_entry].key, key, key_size) == 0)
            {
                entry_to_clear = &table->storage[table->storage[table->table[hashtable_index]].next_entry];
                table->storage[table->table[hashtable_index]].next_entry = table->storage[table->storage[table->table[hashtable_index]].next_entry].next_entry;
            }
        }
    }
    __CGL_hashtable_reset_hashtable_entry(entry_to_clear);
    return true;
}

void CGL_hashtable_set_growth_rate(CGL_hashtable* table, CGL_float rate)
{
    table->growth_rate = rate;
}

size_t CGL_hashtable_get_size(CGL_hashtable* table)
{
    return table->count;
}

void CGL_hashtable_set_hash_function(CGL_hashtable* table, CGL_hash_function hash_function)
{
    table->hash_function = hash_function;
}

CGL_hashtable_iterator* CGL_hashtable_iterator_create(CGL_hashtable* table)
{
    CGL_hashtable_iterator* iterator = (CGL_hashtable_iterator*)CGL_malloc(sizeof(CGL_hashtable_iterator));
    if(!iterator) return NULL;
    iterator->hashtable = table;
    iterator->index = 1;
    return iterator;
}

void CGL_hashtable_iterator_destroy(CGL_hashtable_iterator* iterator)
{
    CGL_free(iterator);
}

void CGL_hashtable_iterator_reset(CGL_hashtable_iterator* iterator)
{
    iterator->index = 1;
}

bool CGL_hashtable_iterator_next(CGL_hashtable_iterator* iterator, void* key, void* data, size_t* size)
{
    while(iterator->index < iterator->hashtable->capacity)
    {
        if(iterator->hashtable->storage[iterator->index].set) break;
        else iterator->index++;
    }
    if(iterator->index >= iterator->hashtable->capacity) iterator->current_entry = NULL;
    else iterator->current_entry = &iterator->hashtable->storage[iterator->index];
    iterator->index++;
    return CGL_hashtable_iterator_curr(iterator, key, data, size);
}

bool CGL_hashtable_iterator_curr(CGL_hashtable_iterator* iterator, void* key, void* data, size_t* size)
{
    if(iterator->current_entry)
    {
        size_t key_size, hashtable_index;
        __CGL_hashtable_calculate_hashtable_index_key_size(iterator->hashtable, &key_size, &hashtable_index, iterator->current_entry->key);
        if(key) memcpy(key, iterator->current_entry->key, key_size);
        if(data && iterator->current_entry->value_size <= CGL_HASHTABLE_ENTRY_STATIC_VALUE_SIZE) memcpy(data, iterator->current_entry->value_static, iterator->current_entry->value_size);
        else if(data && iterator->current_entry->value_size > CGL_HASHTABLE_ENTRY_STATIC_VALUE_SIZE) memcpy(data, iterator->current_entry->value, iterator->current_entry->value_size);
        if(size) *size = iterator->current_entry->value_size;
    }
    return iterator->current_entry != NULL;;
}

void* CGL_hashtable_iterator_curr_key(CGL_hashtable_iterator* iterator)
{
    if(!iterator->current_entry) return NULL;
    return iterator->current_entry->key;
}

#endif

// networking
#if 1

#ifndef CGL_EXCLUDE_NETWORKING

#pragma warning(push, 0)

#ifndef CGL_EXCLUDE_SSL_SOCKET

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>

#endif

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")



struct CGL_net_addrinfo
{
    CGL_int ai_flags;
    CGL_int ai_family;
    CGL_int ai_socktype;
    CGL_int ai_protocol;
    size_t ai_addrlen;
    struct sockaddr ai_addr;
};

struct CGL_net_socket
{
    SOCKET socket;
};

bool CGL_net_init()
{
    WSADATA wsa_data;
    bool result = WSAStartup(MAKEWORD(2, 2), &wsa_data) == 0;
#ifndef CGL_EXCLUDE_SSL_SOCKET
    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();
#endif
    return result;
}

void CGL_net_shutdown()
{
#ifndef CGL_EXCLUDE_SSL_SOCKET
    ERR_free_strings();
    EVP_cleanup();
#endif
    WSACleanup();
}

CGL_net_addrinfo* CGL_net_addrinfo_query(const char* name, const char* port, size_t* count)
{
    struct addrinfo *result = NULL; 
    struct addrinfo *ptr = NULL; 
    struct addrinfo hints; 
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // supports both ipv4
    hints.ai_socktype = SOCK_STREAM; // supports only streaming socketss
    hints.ai_protocol = IPPROTO_TCP; // supports only TCP
    CGL_int iresult = getaddrinfo(name, port, &hints, &result);
    if(iresult != 0) return NULL;
    size_t addr_count = 0;
    for(ptr = result; ptr != NULL; ptr = ptr->ai_next) addr_count++;
    if(count) *count = addr_count;
    CGL_net_addrinfo* infos = (CGL_net_addrinfo*)CGL_malloc(sizeof(CGL_net_addrinfo) * addr_count);
    if(!infos) {freeaddrinfo(result); return NULL;}
    size_t index = 0;
    for(ptr = result; ptr != NULL; ptr = ptr->ai_next, index++)
    {
        infos[index].ai_flags = ptr->ai_flags;
        infos[index].ai_family = ptr->ai_family;
        infos[index].ai_socktype = ptr->ai_socktype;
        infos[index].ai_protocol = ptr->ai_protocol;
        infos[index].ai_addrlen = ptr->ai_addrlen;
        infos[index].ai_addr = *ptr->ai_addr;
    }
    freeaddrinfo(result);
    return infos;
}

void CGL_net_addrinfo_destroy(CGL_net_addrinfo* infos)
{
    CGL_free(infos);
}

CGL_net_socket* CGL_net_socket_create()
{
    CGL_net_socket* soc = (CGL_net_socket*)CGL_malloc(sizeof(CGL_net_socket));
    if(!soc) return NULL;
    soc->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(soc->socket == INVALID_SOCKET)
    {
        CGL_free(soc);
        return NULL;
    }
    return soc;
}

bool CGL_net_socket_connect(CGL_net_socket* soc, CGL_net_addrinfo* target)
{
    CGL_int result = connect(soc->socket, &target->ai_addr, (int)target->ai_addrlen);
    return result != SOCKET_ERROR;
}

bool CGL_net_socket_bind(CGL_net_socket* soc, CGL_net_addrinfo* target)
{
    CGL_int result = bind(soc->socket, &target->ai_addr, (int)target->ai_addrlen);
    return result != SOCKET_ERROR;
}

bool CGL_net_socket_listen(CGL_net_socket* soc, size_t max_connections)
{
    CGL_int result = listen(soc->socket, (int_fast16_t)max_connections);
    return result != SOCKET_ERROR;
}

CGL_net_socket* CGL_net_socket_accept(CGL_net_socket* soc, CGL_net_addrinfo* addrinfo)
{
    CGL_net_socket* cli_soc = (CGL_net_socket*)CGL_malloc(sizeof(CGL_net_socket));
    if(!cli_soc) return NULL;
    CGL_int addr_len = 0;
    if(addrinfo)
    {
        cli_soc->socket = accept(soc->socket, &addrinfo->ai_addr, &addr_len);
        addrinfo->ai_addrlen = addr_len;
    }
    else cli_soc->socket = accept(soc->socket, NULL, NULL);
    if(cli_soc->socket == INVALID_SOCKET)
    {
        CGL_free(cli_soc);
        cli_soc = NULL;
    }
    return cli_soc;
}

void CGL_net_socket_close(CGL_net_socket* soc)
{
    closesocket(soc->socket);
    CGL_free(soc);
}

bool CGL_net_socket_send(CGL_net_socket* soc, void* buffer, size_t size, size_t* size_sent)
{
    CGL_int result = send(soc->socket, (const CGL_byte*)buffer, (int)size, 0);
    if(size_sent) *size_sent = result;
    return result != SOCKET_ERROR;
}

bool CGL_net_socket_recv(CGL_net_socket* soc, void* buffer, size_t size, size_t* size_recieved)
{
    CGL_int result = recv(soc->socket, (CGL_byte*)buffer, (int)size, 0);
    if(result > 0 && size_recieved) *size_recieved = (size_t)result;
    return result > 0;
}

bool CGL_net_socket_shutdown_send(CGL_net_socket* soc)
{
    return shutdown(soc->socket, SD_SEND) == SOCKET_ERROR;
}

bool CGL_net_socket_shutdown_recv(CGL_net_socket* soc)
{
    return shutdown(soc->socket, SD_RECEIVE) == SOCKET_ERROR;
}

#else // for unix based operating systems


#include <netdb.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct CGL_net_addrinfo
{
    struct sockaddr_in ai_addr;
};

struct CGL_net_socket
{
    CGL_int socket;
};

bool CGL_net_init()
{
#ifndef CGL_EXCLUDE_SSL_SOCKET
    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();
#endif
    return true;
}

void CGL_net_shutdown()
{
#ifndef CGL_EXCLUDE_SSL_SOCKET
    ERR_free_strings();
    EVP_cleanup();
#endif
}

CGL_net_addrinfo* CGL_net_addrinfo_query(const char* name, const char* port, size_t* count)
{
    struct hostent* server = NULL;
    server = gethostbyname(name);
    if(server == NULL) return NULL;
    CGL_net_addrinfo* addr_info = (CGL_net_addrinfo*)CGL_malloc(sizeof(CGL_net_addrinfo));
    if(!addr_info) return NULL;
    memset(&addr_info->ai_addr, 0, sizeof(addr_info->ai_addr));
    addr_info->ai_addr.sin_family = AF_UNSPEC;
    CGL_int iport = atoi(port);
    addr_info->ai_addr.sin_port = htons(iport);
    addr_info->ai_addr.sin_addr = *((struct in_addr *)server->h_addr_list[0]);
    if(count) *count = 1;
    return addr_info;
}

void CGL_net_addrinfo_destroy(CGL_net_addrinfo* infos)
{
    CGL_free(infos);
}

CGL_net_socket* CGL_net_socket_create()
{
    CGL_net_socket* soc = (CGL_net_socket*)CGL_malloc(sizeof(CGL_net_socket));
    if(!soc) return NULL;
    soc->socket = socket(AF_INET, SOCK_STREAM, 0);
    if(soc->socket < 0)
    {
        CGL_free(soc);
        return NULL;
    }
    return soc;
}

bool CGL_net_socket_connect(CGL_net_socket* soc, CGL_net_addrinfo* target)
{
    CGL_int result = connect(soc->socket, (struct sockaddr*)&target->ai_addr, sizeof(target->ai_addr));
    return result >= 0;
}

bool CGL_net_socket_bind(CGL_net_socket* soc, CGL_net_addrinfo* target)
{
    CGL_int result = bind(soc->socket, (struct sockaddr*)&target->ai_addr, sizeof(target->ai_addr));
    return result > 0;
}

bool CGL_net_socket_listen(CGL_net_socket* soc, size_t max_connections)
{
    listen(soc->socket, (int)max_connections);
    return true;
}

CGL_net_socket* CGL_net_socket_accept(CGL_net_socket* soc, CGL_net_addrinfo* addrinfo)
{
    CGL_net_socket* cli_soc = (CGL_net_socket*)CGL_malloc(sizeof(CGL_net_socket));
    if(!cli_soc) return NULL;
    CGL_int length = 0;
    if(addrinfo) cli_soc->socket = accept(soc->socket, (struct sockaddr*)&addrinfo->ai_addr, &length);
    else cli_soc->socket = accept(soc->socket, NULL, NULL);
    if(cli_soc->socket < 0)
    {
        CGL_free(cli_soc);
        return NULL;
    }
    return cli_soc;
}

void CGL_net_socket_close(CGL_net_socket* soc)
{
    close(soc->socket);
}

bool CGL_net_socket_send(CGL_net_socket* soc, void* buffer, size_t size, size_t* size_sent)
{
    CGL_int result = send(soc->socket, buffer, size, 0);
    if(result < 0) return false;
    if(size_sent) *size_sent = result;
    return result;
}

bool CGL_net_socket_recv(CGL_net_socket* soc, void* buffer, size_t size, size_t* size_recieved)
{
    CGL_int result = recv(soc->socket, buffer, size, 0);
    if(result < 0) return false;
    if(size_recieved) *size_recieved = result;
    return result;
}

bool CGL_net_socket_shutdown_send(CGL_net_socket* soc)
{
    shutdown(soc->socket, SHUT_WR);
}

bool CGL_net_socket_shutdown_recv(CGL_net_socket* soc)
{
    shutdown(soc->socket, SHUT_RD);
}



#endif // _WIN32

void __CGL_net_http_prepare_request(char* buffer, const char* method, const char* host, const char* path, const char* accept, const char* user_agent, const char* body)
{
    const char* path_ = path == NULL ? "/" : path;
    const char* accept_ = accept == NULL ? "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9" : accept;
    const char* user_agent_ = user_agent == NULL ? "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/105.0.0.0 Safari/537.36" : user_agent;
    const char* body_ = body == NULL ? "" : body;
    const char* method_ = method == NULL ? "GET" : method;
    sprintf(buffer,
    "%s %s HTTP/1.1\r\n"
    "Accept: %s\r\n"
    "Accept-Language: en-US,en;q=0.9,hi;q=0.8,zh-CN;q=0.7,zh;q=0.6\r\n"
    "Host: %s\r\n"
    "User-Agent: %s\r\n"
    "\r\n"
    "%s",
    method_, path_, accept_, host, user_agent_, body_);
}

int __CGL_net_http_parse_response(const char* response, const size_t recieved_length, size_t* body_size, char* body)
{
    CGL_int response_code = atoi(response + 9);
    const char* start_pt = response + 2;
    while(*start_pt != '\0')
    {
        start_pt ++;
        if(*start_pt == '\n' && *(start_pt - 1) == '\r' && *(start_pt - 2) == '\n' && *(start_pt - 3) == '\r') break;        
    }
    start_pt ++;
    size_t body_size_l = (recieved_length - (size_t)(start_pt - response));
    if(body_size) *body_size = body_size_l;;
    if(body)  memcpy(body, start_pt, body_size_l);
    return response_code;
}

int CGL_net_http_request(const char* method, const char* host, const char* path, void* buffer, size_t* size, const char* accept, const char* user_agent, const char* body)
{
    CGL_net_addrinfo* infos = CGL_net_addrinfo_query(host, "80", NULL);
    CGL_net_socket* sock = CGL_net_socket_create();
    if(!CGL_net_socket_connect(sock, infos))
    {
        CGL_net_socket_close(sock);
        CGL_net_addrinfo_destroy(infos);
        return 0;
    }
    static char temp_buffer[1024 * 1024 * 4]; // 4 MB static buffer
    __CGL_net_http_prepare_request(temp_buffer, method, host, path, accept, user_agent, body);
    if(!CGL_net_socket_send(sock, temp_buffer, strlen(temp_buffer), NULL))
    {
        CGL_net_socket_close(sock);
        CGL_net_addrinfo_destroy(infos);
        return 0;
    }
    memset(temp_buffer, 0, sizeof(temp_buffer));
    size_t recieved_length = 0;
    if(!CGL_net_socket_recv(sock, temp_buffer, sizeof(temp_buffer), &recieved_length))
    {
        CGL_net_socket_close(sock);
        CGL_net_addrinfo_destroy(infos);
        return 0;
    }
    CGL_net_socket_close(sock);
    CGL_net_addrinfo_destroy(infos);
    return __CGL_net_http_parse_response(temp_buffer, recieved_length, size, (CGL_byte*)buffer);
}

int CGL_net_http_get(const char* host, const char* path, void* buffer, size_t* size, const char* accept, const char* user_agent)
{
    return CGL_net_http_request("GET", host, path, buffer, size, accept, user_agent, NULL);
}

int CGL_net_http_post(const char* host, const char* path, void* buffer, size_t* size, const char* accept, const char* user_agent, const char* body)
{
    return CGL_net_http_request("POST", host, path, buffer, size, accept, user_agent, body);
}

#ifndef CGL_EXCLUDE_SSL_SOCKET

int CGL_net_https_request(const char* method, const char* host, const char* path, void* response_buffer, size_t* size, const char* accept, const char* user_agent, const char* body)
{
    CGL_log_internal("TO BE IMPLEMENTED!\n");
    return 0;
}

int CGL_net_https_get(const char* host, const char* path, void* buffer, size_t* size, const char* accept, const char* user_agent)
{
    return CGL_net_https_request("GET", host, path, buffer, size, accept, user_agent, NULL);
}

int CGL_net_https_post(const char* host, const char* path, void* buffer, size_t* size, const char* accept, const char* user_agent, const char* body)
{
    return CGL_net_https_request("POST", host, path, buffer, size, accept, user_agent, body);
}

struct CGL_net_ssl_socket
{
    CGL_net_socket* raw_socket;
    SSL* ssl;
    SSL_CTX* ctx;
    const SSL_METHOD* meth;
    CGL_int sock;
};

CGL_net_ssl_socket* CGL_net_ssl_socket_create(CGL_net_socket* soc)
{
    if(!soc) return NULL;
    CGL_net_ssl_socket* sock = (CGL_net_ssl_socket*)CGL_malloc(sizeof(CGL_net_ssl_socket));
    if(!sock) return NULL;
    sock->raw_socket = soc;
    sock->meth = TLSv1_2_client_method();
    sock->ctx = SSL_CTX_new(sock->meth);
    sock->ssl = SSL_new(sock->ctx);
    if(!sock->ssl)
    {
        CGL_log_internal("Error in creating SSL\n");
        CGL_free(sock);
        return NULL;
    }
    sock->sock = SSL_get_fd(sock->ssl);
    SSL_set_fd(sock->ssl, soc->socket);
    CGL_int err = SSL_connect(sock->ssl);
    if(err <= 0)
    {
        CGL_log_internal("Error in creating SSL connection (%d)\n", err);
        CGL_free(sock);
        return NULL;
    }
    CGL_log_internal("SSL connection established using %s\n", SSL_get_cipher(sock->ssl));    
    return sock;
}

bool CGL_net_ssl_socket_send(CGL_net_ssl_socket* soc, void* buffer, size_t size, size_t* size_sent)
{
    CGL_int result = SSL_write(soc->ssl, buffer, (int)size);
    if(result < 0) return false;
    if(size_sent) *size_sent = result;
    return true;
}

bool CGL_net_ssl_socket_recv(CGL_net_ssl_socket* soc, void* buffer, size_t size, size_t* size_recieved)
{
    CGL_int result = SSL_read(soc->ssl, buffer, (int)size);
    if(result < 0) return false;
    if(size_recieved) *size_recieved = result;
    return true;
}

void CGL_net_ssl_socket_destroy(CGL_net_ssl_socket* soc)
{
    SSL_free(soc->ssl);
    SSL_CTX_free(soc->ctx);
    CGL_net_socket_close(soc->raw_socket);
    CGL_free(soc);
}

void CGL_net_ssl_log_errors()
{
    CGL_int err = 0;
    while(err = ERR_get_error())
    {
        char* str = ERR_error_string(err, 0);
        if(!str) return;
        printf("%s\n", str);
        fflush(stdout);
    }
}

#endif // CGL_EXCLUDE_SSL_SOCKET

#pragma warning(pop)


#endif //  CGL_EXCLUDE_NETWORKING

#endif

#ifndef CGL_DISABLE_LOGGER


struct CGL_logger_context
{
    char log_file_paths[CGL_LOGGER_MAX_LOG_FILES][4096];
    char log_buffer[CGL_LOGGER_LOG_BUFFER_SIZE];
    CGL_int log_buffer_length;
    bool console_logging_enabled;
    bool flush_on_log;
};

static CGL_logger_context* __CGL_CURRENT_LOGGER_CONTEXT = NULL;

void CGL_logger_init(bool enable_console_logging)
{
    __CGL_CURRENT_LOGGER_CONTEXT = (CGL_logger_context*)CGL_malloc(sizeof(CGL_logger_context));
    __CGL_CURRENT_LOGGER_CONTEXT->log_buffer_length = 0;
    __CGL_CURRENT_LOGGER_CONTEXT->flush_on_log = false;
    __CGL_CURRENT_LOGGER_CONTEXT->console_logging_enabled = enable_console_logging;
    memset(__CGL_CURRENT_LOGGER_CONTEXT->log_buffer, 0, sizeof(char) * CGL_LOGGER_LOG_BUFFER_SIZE);
    memset(__CGL_CURRENT_LOGGER_CONTEXT->log_file_paths, 0, sizeof(char) * CGL_LOGGER_MAX_LOG_FILES * 4096);
    CGL_log_internal("Started Logger Session");
}

void CGL_logger_shutdown()
{
    CGL_log_internal("Ending Logger Session");
    CGL_logger_flush();
    CGL_free(__CGL_CURRENT_LOGGER_CONTEXT);
}

CGL_logger_context* CGL_logger_get_context()
{
    return __CGL_CURRENT_LOGGER_CONTEXT;
}

void CGL_logger_set_context(CGL_logger_context* context)
{
    __CGL_CURRENT_LOGGER_CONTEXT = context;
}

bool CGL_logger_attach_log_file(const char* path)
{
    for(CGL_int i = 0 ; i < CGL_LOGGER_MAX_LOG_FILES ; i++) if(__CGL_CURRENT_LOGGER_CONTEXT->log_file_paths[i][0] == '\0')
    {
        strcpy(__CGL_CURRENT_LOGGER_CONTEXT->log_file_paths[i], path);
        CGL_log_internal("Attached Log File : %s", path);
        return true;
    }
    return false;
}

bool CGL_logger_detach_log_file(const char* path)
{
    for(CGL_int i = 0 ; i < CGL_LOGGER_MAX_LOG_FILES ; i++) if(strcmp(__CGL_CURRENT_LOGGER_CONTEXT->log_file_paths[i], path) == 0)
    {
        __CGL_CURRENT_LOGGER_CONTEXT->log_file_paths[i][0] = '\0';
        CGL_log_internal("Detached Log File : %s", path);
        return true;
    }
    return false;
}

void CGL_logger_flush()
{
    for(CGL_int i = 0 ; i < CGL_LOGGER_MAX_LOG_FILES ; i++)
        if(__CGL_CURRENT_LOGGER_CONTEXT->log_file_paths[i][0] != '\0')
            CGL_utils_append_file(__CGL_CURRENT_LOGGER_CONTEXT->log_file_paths[i], __CGL_CURRENT_LOGGER_CONTEXT->log_buffer, __CGL_CURRENT_LOGGER_CONTEXT->log_buffer_length);
    __CGL_CURRENT_LOGGER_CONTEXT->log_buffer_length = 0;
    __CGL_CURRENT_LOGGER_CONTEXT->log_buffer[0] = '\0';
    if(!__CGL_CURRENT_LOGGER_CONTEXT->flush_on_log) { CGL_log_internal("Flushed Log Buffer"); }
}

void CGL_logger_disable_console_logging()
{
    __CGL_CURRENT_LOGGER_CONTEXT->console_logging_enabled = false;
}

void CGL_logger_enable_console_logging()
{
    __CGL_CURRENT_LOGGER_CONTEXT->console_logging_enabled = true;
}

void CGL_logger_log(CGL_int level, const char* log_format, ...)
{
    static char buffer1[1024 * 4];
    static char buffer2[256];
    static char buffer3[1024 * 4];
    static const char* LOG_LEVEL_STR[] = {
        "TRACE",   
        "INFO",    
        "WARN",    
        "ERROR",   
        "INTERNAL"
    };
    va_list args;
    va_start (args, log_format);
    vsprintf (buffer1, log_format, args);
    va_end (args);
    CGL_utils_get_timestamp(buffer2);
    sprintf(buffer3, "[%s] [%s] : %s\n", LOG_LEVEL_STR[level], buffer2, buffer1);
    CGL_int log_length = (int)strlen(buffer3);
    if(log_length + __CGL_CURRENT_LOGGER_CONTEXT->log_buffer_length >= CGL_LOGGER_LOG_BUFFER_SIZE) CGL_logger_flush();
    strcat(__CGL_CURRENT_LOGGER_CONTEXT->log_buffer, buffer3);
    __CGL_CURRENT_LOGGER_CONTEXT->log_buffer_length += log_length;
    switch(level)
    {
        case CGL_LOG_LEVEL_TRACE   : printf(buffer3); break;
        case CGL_LOG_LEVEL_INFO    : CGL_printf_green(buffer3); break;
        case CGL_LOG_LEVEL_WARN    : CGL_printf_gray(buffer3); break;
        case CGL_LOG_LEVEL_ERROR   : CGL_printf_red(buffer3); break;
        case CGL_LOG_LEVEL_INTERNAL: CGL_printf_blue(buffer3); break;
    }
    if(__CGL_CURRENT_LOGGER_CONTEXT->flush_on_log) CGL_logger_flush();
}

#endif

// utils
#if 1


void CGL_console_set_color(uint8_t color)
{
#ifdef CGL_WINDOWS
    static WORD saved_attributes;
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if(color == CGL_CONSOLE_COLOR_RESET) SetConsoleTextAttribute(console, saved_attributes);
    CONSOLE_SCREEN_BUFFER_INFO console_info;
    GetConsoleScreenBufferInfo(console, &console_info);    
    saved_attributes = console_info.wAttributes;
    switch(color)
    {
        case 1:  SetConsoleTextAttribute(console, FOREGROUND_RED); break;
        case 2:  SetConsoleTextAttribute(console, FOREGROUND_GREEN); break;
        case 3:  SetConsoleTextAttribute(console, FOREGROUND_INTENSITY); break;
        case 4:  SetConsoleTextAttribute(console, FOREGROUND_BLUE); break;
        default: break;
    }
#else
    if(color == CGL_CONSOLE_COLOR_RESET) printf("\x1B[0m");
    else
    {
        switch(color)
        {
            case 1:  printf("\x1B[31m"); break;
            case 2:  printf("\x1B[32m"); break;
            case 3:  printf("\x1B[33m"); break;
            case 3:  printf("\x1B[34m"); break;
            default: break;
        }   
    }
#endif   
}

void CGL_printf_red(const char* format, ...)
{
    CGL_console_set_color(CGL_CONSOLE_COLOR_RED);
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    CGL_console_set_color(CGL_CONSOLE_COLOR_RESET);
}

void CGL_printf_green(const char* format, ...)
{
    CGL_console_set_color(CGL_CONSOLE_COLOR_GREEN);
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    CGL_console_set_color(CGL_CONSOLE_COLOR_RESET);
}

void CGL_printf_gray(const char* format, ...)
{
    CGL_console_set_color(CGL_CONSOLE_COLOR_GRAY);
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    CGL_console_set_color(CGL_CONSOLE_COLOR_RESET);
}

void CGL_printf_blue(const char* format, ...)
{
    CGL_console_set_color(CGL_CONSOLE_COLOR_BLUE);
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    CGL_console_set_color(CGL_CONSOLE_COLOR_RESET);
}

float CGL_utils_get_time()
{
#ifndef CGL_WASM
#if defined(_WIN32) || defined(_WIN64)
    static LARGE_INTEGER starting_time;
    LARGE_INTEGER frequency, time;
    if(starting_time.QuadPart == 0) QueryPerformanceCounter(&starting_time);
    QueryPerformanceFrequency(&frequency); 
    QueryPerformanceCounter(&time);
    CGL_float ftime = (CGL_float)(time.QuadPart - starting_time.QuadPart) / (CGL_float)frequency.QuadPart;
    return ftime;
#else // for POSIX
    struct timespec spec;
    if (clock_gettime(1, &spec) == -1)
    {
        /* 1 is CLOCK_MONOTONIC */
        return 0.0f;
    } 
    return (float)(spec.tv_sec * 1000 + spec.tv_nsec / 1e6) / 1000.0f;
#endif
#else 
    return 0.0f;
#endif
}

CGL_sizei CGL_utils_get_random_with_probability(CGL_float* probabilities, CGL_sizei count)
{
    static CGL_float prefix[CGL_RAND_GEN_WITH_PROBABILITY_MAX_COUNT];
    prefix[0] = probabilities[0];
    for(CGL_sizei i = 1; i < count; i++) prefix[i] = prefix[i - 1] + probabilities[i];
    CGL_float r = CGL_utils_random_float() * prefix[count - 1];
    for(CGL_sizei i = 0; i < count; i++) if(r < prefix[i]) return i;
    return count - 1;
}

void CGL_utils_get_timestamp(char* buffer)
{
    time_t ltime = time(NULL);
    sprintf(buffer, "%s", asctime( localtime(&ltime)));
    buffer[strlen(buffer) - 2] = '\0';
}

// From : https://stackoverflow.com/a/12792056/14911094
CGL_bool CGL_utils_is_little_endian()
{
    CGL_int i = 0x01234567;
    return (*((CGL_ubyte*)(&i))) == 0x67;
}

void CGL_utils_reverse_bytes(void* vdata, size_t size)
{
    CGL_byte* data = (CGL_byte*)vdata;
    for(size_t i = 0; i < size / 2; i++)
    {
        CGL_byte temp = data[i];
        data[i] = data[size - i - 1];
        data[size - i - 1] = temp;
    }
}

void CGL_utils_little_endian_to_current(void* data, size_t size)
{
    if(CGL_utils_is_little_endian()) return;
    CGL_utils_reverse_bytes(data, size);
}

void CGL_utils_big_endian_to_current(void* data, size_t size)
{
    if(!CGL_utils_is_little_endian()) return;
    CGL_utils_reverse_bytes(data, size);
}

void CGL_shape_init(CGL_shape* shape, size_t vertices_count)
{
    shape->vertices_count = vertices_count;
    shape->vertices = (CGL_vec3*)CGL_malloc(sizeof(CGL_vec3) * vertices_count);
    shape->position = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    shape->rotation = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    shape->scale = CGL_vec3_init(1.0f, 1.0f, 1.0f);
    if(shape->vertices == NULL) return;
}

void CGL_shape_destroy(CGL_shape* shape)
{
    CGL_free(shape->vertices);
}

bool CGL_sat_collision_overlap_on_axis(CGL_shape* a, CGL_shape* b, CGL_vec2 axis, float* overlap_amount)
{
    CGL_float a_max = -FLT_MAX;
    CGL_float a_min = FLT_MAX;

    CGL_float b_max = -FLT_MAX;
    CGL_float b_min = FLT_MAX;

    CGL_vec2_normalize(axis);

    const CGL_vec2 a_translation = CGL_vec2_init(a->position.x, a->position.y);
    const CGL_vec2 a_scale = CGL_vec2_init(a->scale.x, a->scale.y);
    const CGL_float a_rotation = a->rotation.x;
    
    const CGL_vec2 b_translation = CGL_vec2_init(b->position.x, b->position.y);
    const CGL_vec2 b_scale = CGL_vec2_init(b->scale.x, b->scale.y);
    const CGL_float b_rotation = b->rotation.x;

    
    for(size_t i = 0; i < a->vertices_count; i++)
    {
        CGL_vec2 vertex = CGL_vec2_init(a->vertices[i].x, a->vertices[i].y);
        vertex = CGL_vec2_apply_transformations(vertex, &a_translation, &a_rotation, &a_scale);
        CGL_float projection = CGL_vec2_dot(vertex, axis);
        a_max = CGL_utils_max(a_max, projection);
        a_min = CGL_utils_min(a_min, projection);        
    }

    for(size_t i = 0; i < b->vertices_count; i++)
    {
        CGL_vec2 vertex = CGL_vec2_init(b->vertices[i].x, b->vertices[i].y);
        vertex = CGL_vec2_apply_transformations(vertex, &b_translation, &b_rotation, &b_scale);
        CGL_float projection = CGL_vec2_dot(vertex, axis);
        b_max = CGL_utils_max(b_max, projection);
        b_min = CGL_utils_min(b_min, projection);        
    }

    if
    (
        (a_max >= b_min && a_max <= b_max) ||
        (a_min >= b_min && a_min <= b_max) ||
        (b_max >= a_min && b_max <= a_max) ||
        (b_min >= a_min && b_min <= a_max)

    )
    {
        CGL_float overlap = 0.0f;
        if(a_max > b_max && a_min < b_min) overlap = CGL_utils_min(a_max - b_min, b_max - a_min);
        else if(a_max > b_max) overlap = b_max - a_min;
        else if(a_min < b_min) overlap = a_max - b_min;
        else overlap = CGL_utils_min(a_max - b_min, b_max - a_min);
        if(overlap_amount) *overlap_amount = overlap;
        return true;
    }

    return false;    
}

bool CGL_sat_collision_detect(CGL_shape* a, CGL_shape* b, CGL_vec2* n_vector)
{
    static CGL_vec2 sat_axes_a[CGL_SAT_COLLISION_MAX_COLLISIONS];
    static CGL_vec2 sat_axes_b[CGL_SAT_COLLISION_MAX_COLLISIONS];
    CGL_sat_collision_calculate_axes(a, sat_axes_a, NULL);    
    CGL_sat_collision_calculate_axes(b, sat_axes_b, NULL);
    CGL_float overlap = FLT_MAX, ov = FLT_MAX;
    CGL_vec2 n = CGL_vec2_init(0.0f, 0.0f);
    for(size_t i = 0; i < a->vertices_count; i++) { if(!CGL_sat_collision_overlap_on_axis(a, b, sat_axes_a[i], &ov)) return false; if(ov < overlap) { overlap = ov; n = sat_axes_a[i]; } }
    for(size_t i = 0; i < b->vertices_count; i++) { if(!CGL_sat_collision_overlap_on_axis(a, b, sat_axes_b[i], &ov)) return false; if(ov < overlap) { overlap = ov; n = sat_axes_b[i]; } }
    //if(overlap_amount) *overlap_amount = overlap;
    n = CGL_vec2_scale(n, overlap);
    if(n_vector) *n_vector = n;
    return true;
}

void CGL_sat_collision_calculate_axes(CGL_shape* shape, CGL_vec2* axes, CGL_int* axes_count)
{
    for(size_t i = 0; i < shape->vertices_count; i++)
    {
        CGL_vec2 a = CGL_vec2_init(shape->vertices[i].x, shape->vertices[i].y);
        CGL_vec2 b = CGL_vec2_init(shape->vertices[(i + 1) % shape->vertices_count].x, shape->vertices[(i + 1) % shape->vertices_count].y);
        CGL_vec2 edge = CGL_vec2_sub(b, a);
        CGL_vec2 normal = CGL_vec2_init(edge.y, -edge.x);
        axes[i] = normal;
    }
    if(axes_count) *axes_count = (int)shape->vertices_count;
}

// algorithm from https://stackoverflow.com/a/23186198/14911094
CGL_bool CGL_utils_is_point_in_triangle(CGL_vec2 p, CGL_vec2 p0, CGL_vec2 p1, CGL_vec2 p2)
{
    CGL_float s = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y);
    CGL_float t = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y);
    if (s <= 0 || t <= 0) return false;
    CGL_float A = (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
    return (s + t) < A;
}

// GJK & EPA 

CGL_vec3 CGL_gjk_shape_default_support(CGL_shape* a, CGL_vec3 d)
{
    CGL_float max_dp = -FLT_MAX, dp = 0.0f;
    CGL_vec3 support = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    CGL_vec3 vertex_pos = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    for(CGL_int i = 0 ; i < a->vertices_count ; i++)
    {
        vertex_pos = CGL_vec3_apply_transformations(a->vertices[i], &a->position, &a->rotation, &a->scale);
        dp = CGL_vec3_dot(vertex_pos, d);
        if(dp > max_dp) { max_dp = dp; support = vertex_pos;}
    }
    return support;
}

// the default support function 
CGL_vec3 CGL_gjk_default_support(CGL_shape* a, CGL_shape* b, CGL_vec3 d)
{
    CGL_vec3 a_support = CGL_gjk_shape_default_support(a, d);
    CGL_vec3 b_support = CGL_gjk_shape_default_support(b, CGL_vec3_scale_(d, -1.0f));
    return CGL_vec3_sub(a_support, b_support);
}

// Implementation inspired from
//
// https://github.com/kroitor/gjk.c/blob/master/gjk.c
// https://observablehq.com/@esperanc/2d-gjk-and-epa-algorithms
CGL_bool CGL_gjk_check_collision_2d(CGL_shape* sa, CGL_shape* sb, CGL_vec3* simplex_out)
{
    CGL_int index = 0;
    CGL_vec3 simplex[3];
    CGL_vec3  a, b, c;
    CGL_vec3 dir, ao, ab, ac, ab_perp, ac_perp;

    // initial direction is from a to b
    dir = CGL_vec3_sub(sa->position, sb->position);

    // if the direction is 0 (which means a and b are at same position), set it to (1, 0, 0) (x-axis)
    if(dir.x == 0 && dir.y == 0) dir = CGL_vec3_init(1.0f, 0.0f, 0.0f);

    // get the first point of the simplex
    a = simplex[0] = CGL_gjk_default_support(sa, sb, dir);

    if(CGL_vec3_dot(a, dir) < 0) return false; // no collision

    // set the direction to the opposite of the first point (towards origin)
    dir = CGL_vec3_scale(dir, -1.0f);

    while(true)
    {
        a = simplex[++index] = CGL_gjk_default_support(sa, sb, dir);

        // origin lies beyond the support point in the direction of the origin
        // thus the minkowski difference does not contain the origin
        if(CGL_vec3_dot(a, dir) < 0) return false; // no collision

        // vector from A to origin
        ao = CGL_vec3_scale(a, -1.0f);

        if(index < 2)
        {
            b = simplex[0];
            ab = CGL_vec3_sub(b, a); // vector from A to B
            dir = CGL_vec3_triple_product(ab, ao, ab); // perpendicular to AB towards origin
            if(CGL_vec3_dot(dir, dir) == 0) dir = CGL_vec3_init(dir.y, -dir.x, 0.0f); // perpendicular to AB
            continue;
        }

        // we reach here when index == 2 (3 points in the simplex)
        b = simplex[1];
        c = simplex[0];
        // a is already the last point added to the simplex
        ab = CGL_vec3_sub(b, a);
        ac = CGL_vec3_sub(c, a);
        ac_perp = CGL_vec3_triple_product(ab, ac, ac);
        if(CGL_vec3_dot(ac_perp, ao) >= 0) dir = ac_perp;
        else
        {
            ab_perp = CGL_vec3_triple_product(ac, ab, ab);
            if(CGL_vec3_dot(ab_perp, ao) < 0)
            {
                if(simplex_out != NULL)
                {
                    simplex_out[0] = simplex[0];
                    simplex_out[1] = simplex[1];
                    simplex_out[2] = simplex[2];
                }
                return true; // collision
            }
            simplex[0] = simplex[1]; // swap
            dir = ab_perp;
        }
        simplex[1] = simplex[2]; // swap
        index -= 1;
    }
    return false; // no collision
}

// From https://blog.winter.dev/2020/epa-algorithm/
CGL_vec3 CGL_gjk_epa_2d(CGL_shape* a, CGL_shape* b, CGL_vec3* simplex)
{
    CGL_int min_index = -1;
    CGL_float min_dist = FLT_MAX;
    CGL_vec3 min_normal = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    static CGL_vec3 polytope[CGL_GJK_EPA_MAX_POLYTOPE_VERTICES];
    static CGL_vec3 polytope_copy[CGL_GJK_EPA_MAX_POLYTOPE_VERTICES];
    CGL_int polytope_size = 3;
    CGL_vec3 vti, vtj, vtedge, normal, support;
    CGL_float dist = 0.0f;
    for(CGL_int i = 0 ; i < 3 ; i++) polytope[i] = simplex[i];
    
    while(min_dist >= FLT_MAX)
    {
        for(CGL_int i = 0 ; i < polytope_size ; i++)
        {
            CGL_int j = (i + 1) % polytope_size;
            vti = polytope[i];
            vtj = polytope[j];
            vtedge = CGL_vec3_sub(vtj, vti);
            normal = CGL_vec3_init(vtedge.y, -vtedge.x, 0.0f);
            CGL_vec3_normalize(normal);
            dist = CGL_vec3_dot(normal, vti);
            if(dist < 0)
            {
                dist *= -1;
                normal = CGL_vec3_scale(normal, -1.0f);
            }
            if(dist < min_dist)
            {
                min_dist = dist;
                min_normal = normal;
                min_index = j;
            }
        }
        support = CGL_gjk_default_support(a, b, min_normal);
        dist = CGL_vec3_dot(min_normal, support);
        if(polytope_size == CGL_GJK_EPA_MAX_POLYTOPE_VERTICES - 2) break; // max reached
        if(fabsf(dist - min_dist) > CGL_GJK_EPA_TOLERANCE)
        {
            min_dist = FLT_MAX;
            CGL_int amt_len = polytope_size - min_index;
            for(CGL_int i = polytope_size - 1 ; i >= min_index ; i--) polytope_copy[i + 1] = polytope[i];
            polytope[min_index] = support;
            polytope_size += 1;
        }
    }
    min_normal = CGL_vec3_scale(min_normal, min_dist);
    return min_normal;
}

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
   CGL_int case_type, idx, len;
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


// From : http://www.azillionmonkeys.com/qed/hash.html

#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define CGL_get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (CGL_get16bits)
#define CGL_get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

uint32_t CGL_utils_super_fast_hash(const void* dat, size_t len)
{
    const uint8_t* data = (const uint8_t*)dat;
    uint32_t hash = (uint32_t)len;
    uint32_t tmp;
    CGL_int rem;
    
    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += CGL_get16bits (data);
        tmp    = (CGL_get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += CGL_get16bits (data);
                hash ^= hash << 16;
                hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
                hash += hash >> 11;
                break;
        case 2: hash += CGL_get16bits (data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
        case 1: hash += (signed char)*data;
                hash ^= hash << 10;
                hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

#endif

// common lib and mat
#if 1 // Just to use code folding

CGL_float CGL_float_quadratic_lerp(CGL_float a, CGL_float b, CGL_float c, CGL_float t)
{
    CGL_float t2 = t * t;
    CGL_float a0 = c - a;
    CGL_float a1 = a - b * 2.0f + c;
    CGL_float a2 = b - a;
    return (a0 + a1 * t + a2 * t2);
}

CGL_float CGL_float_cubic_lerp(CGL_float a, CGL_float b, CGL_float c, CGL_float d, CGL_float t)
{
    CGL_float p0 = CGL_float_lerp(a, c, t);
    CGL_float p1 = CGL_float_lerp(c, d, t);
    CGL_float p2 = CGL_float_lerp(d, b, t);
    CGL_float q0 = CGL_float_lerp(p0, p1, t);
    CGL_float q1 = CGL_float_lerp(p1, p2, t);
    return CGL_float_lerp(q0, q1, t);
}

// Uses Rodrigues' rotation formula to rotate a vector about an axis
CGL_vec3 CGL_vec3_rotate_about_axis(CGL_vec3 v, CGL_vec3 axis, CGL_float theta)
{
    CGL_float cos_theta = cosf(theta);
    CGL_float sin_theta = sinf(theta);
    CGL_float dot = CGL_vec3_dot(axis, v);
    CGL_vec3 cross = CGL_vec3_cross(axis, v);
    CGL_vec3 result = CGL_vec3_scale(v, cos_theta);
    CGL_vec3 temp = CGL_vec3_scale(axis, dot * (1 - cos_theta));
    result = CGL_vec3_add(result, temp);
    temp = CGL_vec3_scale(cross, sin_theta);
    result = CGL_vec3_add(result, temp);
    return result;
}

// vector triple product
CGL_vec2 CGL_vec2_triple_product(CGL_vec2 a, CGL_vec2 b, CGL_vec2 c)
{
    CGL_vec2 tmp0 = CGL_vec2_scale_(b, CGL_vec2_dot(a, c));
    CGL_vec2 tmp1 = CGL_vec2_scale_(a, CGL_vec2_dot(b, c));
    return CGL_vec2_sub(tmp0, tmp1);
}

CGL_vec3 CGL_vec3_triple_product(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
{
    CGL_vec3 tmp0 = CGL_vec3_scale_(b, CGL_vec3_dot(a, c));
    CGL_vec3 tmp1 = CGL_vec3_scale_(a, CGL_vec3_dot(b, c));
    return CGL_vec3_sub(tmp0, tmp1);
}

CGL_vec4 CGL_vec4_triple_product(CGL_vec4 a, CGL_vec4 b, CGL_vec4 c)
{
    CGL_vec4 tmp0 = CGL_vec4_scale_(b, CGL_vec4_dot(a, c));
    CGL_vec4 tmp1 = CGL_vec4_scale_(a, CGL_vec4_dot(b, c));
    return CGL_vec4_sub(tmp0, tmp1);        
}


void CGL_vec3_calculate_orthonormal_basis_from_one_vector(CGL_vec3 a, CGL_vec3* pb, CGL_vec3* pc)
{
    CGL_vec3_normalize(a);
    CGL_vec3 b = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    CGL_vec3 c = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    if(a.x != 0.0f) b = CGL_vec3_init(-a.y/a.x, 1.0f, 0.0f);
    else if(a.y != 0.0f) b = CGL_vec3_init(0.0f, -a.z/a.y, 1.0f);
    else b = CGL_vec3_init(1.0f, 0.0f, -a.x/a.z);
    CGL_vec3_normalize(b);
    c = CGL_vec3_cross(a, b);
    if(pb) *pb = b;
    if(pc) *pc = c;
}

CGL_vec3 CGL_vec3_reflect(CGL_vec3 a, CGL_vec3 n)
{
    CGL_float dot = CGL_vec3_dot(a, n);
    CGL_vec3 result = CGL_vec3_scale(n, 2.0f * dot);
    result = CGL_vec3_sub(a, result);
    return result;
}


CGL_mat4 CGL_mat4_mul(CGL_mat4 b, CGL_mat4 a)
{
    CGL_mat4 result = CGL_mat4_zero();
    for (CGL_int i = 0; i < 4; i++) for (CGL_int j = 0; j < 4; j++)
    {
        for (CGL_int k = 0; k < 4; k++)
            CGL_mat4_elem_set(result, i, j, CGL_mat4_elem_get(result, i, j) + CGL_mat4_elem_get(a, i, k) * CGL_mat4_elem_get(b, k, j));
    }
    return result;
}

// LU decomposition using Kraut's algorithm
void CGL_mat4_decompose_lu(CGL_mat4 m, CGL_mat4* l, CGL_mat4* u)
{
    CGL_mat4 L = CGL_mat4_identity();
    CGL_mat4 U = CGL_mat4_identity();    
    for(CGL_int j = 0 ; j < 4 ; j++)
    {
        for(CGL_int i = 0 ; i <= j ; i++)
        {
            CGL_float sum = CGL_mat4_elem_get(m, i, j);
            for(CGL_int k = 0 ; k < i ; k++)
                sum -= CGL_mat4_elem_get(L, i, k) * CGL_mat4_elem_get(U, k, j);
            CGL_mat4_elem_set(U, i, j, sum);
        }
        for(CGL_int i = j + 1 ; i < 4 ; i++)
        {
            CGL_float sum = CGL_mat4_elem_get(m, i, j);
            for(CGL_int k = 0 ; k < j ; k++)
                sum -= CGL_mat4_elem_get(L, i, k) * CGL_mat4_elem_get(U, k, j);
            CGL_mat4_elem_set(L, i, j, sum / CGL_mat4_elem_get(U, j, j));
        }
    }
    if (l) *l = L;
    if (u) *u = U;
}

CGL_float CGL_mat4_det(CGL_mat4 m)
{
    return CGL_mat4_det_by_gauss(m);
}

CGL_float CGL_mat4_det_by_lu(CGL_mat4 m)
{
    CGL_mat4 U = CGL_mat4_identity();
    CGL_mat4_decompose_lu(m, NULL, &U);
    CGL_float det = 1.0f;
    for (CGL_int i = 0; i < 4; i++) det *= CGL_mat4_elem_get(U, i, i);
    return det;
}

CGL_float CGL_mat4_det_by_gauss(CGL_mat4 m)
{
    m = CGL_mat4_gauss_elim(m);
    CGL_float det = 1.0f;
    for (CGL_int i = 0; i < 4; i++) det *= CGL_mat4_elem_get(m, i, i);
    return det;
}

CGL_vec4 CGL_mat4_mul_vec4(CGL_mat4 m, CGL_vec4 v)
{
    CGL_vec4 result = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    for (CGL_int i = 0; i < 4; i++)
    {
        CGL_float temp = 0.0f;
        for (CGL_int j = 0; j < 4; j++)
            temp += CGL_mat4_elem_get(m, i, j) * CGL_vec4_elem_get(v, j);
        CGL_vec4_elem_set(result, i, temp);
    }
    return result;
}

CGL_mat4 CGL_mat4_adjoint(CGL_mat4 m)
{
    CGL_mat4 result = CGL_mat4_zero();
    for (CGL_int i = 0; i < 4; i++) for (CGL_int j = 0; j < 4; j++)
    {
        CGL_mat4 temp = CGL_mat4_zero();
        for (CGL_int k = 0; k < 4; k++) for (CGL_int l = 0; l < 4; l++)
        {
            if (k != i && l != j)
                CGL_mat4_elem_set(temp, k < i ? k : k - 1, l < j ? l : l - 1, CGL_mat4_elem_get(m, k, l));
        }
        CGL_mat4_elem_set(result, i, j, CGL_mat4_det(temp) * ((i + j) % 2 == 0 ? 1.0f : -1.0f));
    }
    return result;
}

CGL_mat4 CGL_mat4_inverse(CGL_mat4 m)
{
    CGL_mat4 result = CGL_mat4_adjoint(m);
    CGL_float det = CGL_mat4_det(m);
    det = 1.0f / det;
    result = CGL_mat4_mul_scalar(result, det);
    return result;
}

CGL_mat4 CGL_mat4_transpose(CGL_mat4 m)
{
    return CGL_mat4_init(
        CGL_mat4_elem_get(m, 0, 0), CGL_mat4_elem_get(m, 1, 0), CGL_mat4_elem_get(m, 2, 0), CGL_mat4_elem_get(m, 3, 0),
        CGL_mat4_elem_get(m, 0, 1), CGL_mat4_elem_get(m, 1, 1), CGL_mat4_elem_get(m, 2, 1), CGL_mat4_elem_get(m, 3, 1),
        CGL_mat4_elem_get(m, 0, 2), CGL_mat4_elem_get(m, 1, 2), CGL_mat4_elem_get(m, 2, 2), CGL_mat4_elem_get(m, 3, 2),
        CGL_mat4_elem_get(m, 0, 3), CGL_mat4_elem_get(m, 1, 3), CGL_mat4_elem_get(m, 2, 3), CGL_mat4_elem_get(m, 3, 3)
    );
}

CGL_mat4 CGL_mat4_gauss_elim(CGL_mat4 m)
{
    CGL_float ratio = 0.0f;
    for(CGL_int i = 0 ; i < 4 ; i++) for(CGL_int j = 0 ; j < 4 ; j++)
    {
        if(j > i)
        {
            // cannot divide by zero
            if(CGL_mat4_elem_get(m, i, i) == 0.0f) CGL_mat4_elem_set(m, i, i, powf(2.2204f, -16));
            ratio = CGL_mat4_elem_get(m, j, i) / CGL_mat4_elem_get(m, i, i);
            for(CGL_int k = 0 ; k < 4 ; k++) CGL_mat4_elem_set(m, j, k, CGL_mat4_elem_get(m, j, k) - ratio * CGL_mat4_elem_get(m, i, k));
        }
    }
    return m;
}

CGL_int CGL_mat4_rank(CGL_mat4 m)
{
    m = CGL_mat4_gauss_elim(m);
    CGL_int rank = 0;
    CGL_float eps = powf(2.2204f, -16);
    CGL_float sum = 0.0f;
    for (CGL_int i = 0; i < 4; i++)
    {
        sum = 0.0f;
        for (CGL_int j = 0; j < 4; j++) sum += CGL_mat4_elem_get(m, i, j);
        if(fabsf(sum) > eps) rank++;
        else break;
    }
    return rank;
}

CGL_float CGL_mat4_trace(CGL_mat4 m)
{
    CGL_float trace = 0.0f;
    for (CGL_int i = 0; i < 4; i++) trace += CGL_mat4_elem_get(m, i, i);
    return trace;
}

CGL_mat3 CGL_mat4_to_mat3(CGL_mat4 m)
{
    return CGL_mat3_init(
        CGL_mat4_elem_get(m, 0, 0), CGL_mat4_elem_get(m, 0, 1), CGL_mat4_elem_get(m, 0, 2),
        CGL_mat4_elem_get(m, 1, 0), CGL_mat4_elem_get(m, 1, 1), CGL_mat4_elem_get(m, 1, 2),
        CGL_mat4_elem_get(m, 2, 0), CGL_mat4_elem_get(m, 2, 1), CGL_mat4_elem_get(m, 2, 2)
    );
}

CGL_mat4 CGL_mat4_from_mat3(CGL_mat3 m)
{
    return CGL_mat4_init(
        CGL_mat3_elem_get(m, 0, 0), CGL_mat3_elem_get(m, 0, 1), CGL_mat3_elem_get(m, 0, 2), 0.0f,
        CGL_mat3_elem_get(m, 1, 0), CGL_mat3_elem_get(m, 1, 1), CGL_mat3_elem_get(m, 1, 2), 0.0f,
        CGL_mat3_elem_get(m, 2, 0), CGL_mat3_elem_get(m, 2, 1), CGL_mat3_elem_get(m, 2, 2), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

// Golfman's method for matrix rotation
CGL_mat4 CGL_mat4_rotate_about_axis(CGL_vec3 axis, CGL_float angle)
{
    CGL_float cos_theta = cosf(angle);
    CGL_float sin_theta = sinf(angle);
    CGL_float one_minus_cos_theta = 1.0f - cos_theta;
    CGL_float x = axis.x;
    CGL_float y = axis.y;
    CGL_float z = axis.z;
    return CGL_mat4_init(
        cos_theta + x * x * one_minus_cos_theta, x * y * one_minus_cos_theta - z * sin_theta, x * z * one_minus_cos_theta + y * sin_theta, 0.0f,
        y * x * one_minus_cos_theta + z * sin_theta, cos_theta + y * y * one_minus_cos_theta, y * z * one_minus_cos_theta - x * sin_theta, 0.0f,
        z * x * one_minus_cos_theta - y * sin_theta, z * y * one_minus_cos_theta + x * sin_theta, cos_theta + z * z * one_minus_cos_theta, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

CGL_float CGL_mat3_det(CGL_mat3 a)
{
    CGL_float result = 0.0f;
    result += CGL_mat3_elem_get(a, 0, 0) * (CGL_mat3_elem_get(a, 1, 1) * CGL_mat3_elem_get(a, 2, 2) - CGL_mat3_elem_get(a, 1, 2) * CGL_mat3_elem_get(a, 2, 1));
    result -= CGL_mat3_elem_get(a, 0, 1) * (CGL_mat3_elem_get(a, 1, 0) * CGL_mat3_elem_get(a, 2, 2) - CGL_mat3_elem_get(a, 1, 2) * CGL_mat3_elem_get(a, 2, 0));
    result += CGL_mat3_elem_get(a, 0, 2) * (CGL_mat3_elem_get(a, 1, 0) * CGL_mat3_elem_get(a, 2, 1) - CGL_mat3_elem_get(a, 1, 1) * CGL_mat3_elem_get(a, 2, 0));
    return result;
}

CGL_float CGL_mat3_trace(CGL_mat3 a)
{
    CGL_float result = 0.0f;
    for (CGL_int i = 0; i < 3; i++) result += CGL_mat3_elem_get(a, i, i);
    return result;
}

CGL_mat3 CGL_mat3_transpose(CGL_mat3 a)
{
    return CGL_mat3_init(
        CGL_mat3_elem_get(a, 0, 0), CGL_mat3_elem_get(a, 1, 0), CGL_mat3_elem_get(a, 2, 0),
        CGL_mat3_elem_get(a, 0, 1), CGL_mat3_elem_get(a, 1, 1), CGL_mat3_elem_get(a, 2, 1),
        CGL_mat3_elem_get(a, 0, 2), CGL_mat3_elem_get(a, 1, 2), CGL_mat3_elem_get(a, 2, 2)
    );
}


float CGL_quat_to_axis_angle(CGL_quat quat, float* x, float* y, float* z)
{
    CGL_float angle = 2.0f * acosf(quat.w);
    // CGL_float divider = sqrtf(1.0f - quat.w * quat.w);
    CGL_float divider = sinf(angle / 2.0f);
    if(divider == 0.0f) return angle;
    if(x) *x = quat.vec.x / divider;
    if(y) *y = quat.vec.y / divider;
    if(z) *z = quat.vec.z / divider;
    return angle;
}

void CGL_quat_to_euler_zyx(CGL_quat q, float* z, float* y, float* x)
{
    // roll (x-axis rotation)
    if(z)
    {
        CGL_float sinr_cosp = 2.0f * (q.w * q.vec.x + q.vec.y * q.vec.z);
        CGL_float cosr_cosp = 1.0f - 2.0f * (q.vec.x * q.vec.x + q.vec.y * q.vec.y);
        *z = atan2f(sinr_cosp, cosr_cosp);
    }
    // pitch (y-axis rotation)
    if(y)
    {
        CGL_float sinp = 2.0f * (q.w * q.vec.y - q.vec.z * q.vec.x);
        if(fabs(sinp) >= 1) *y = copysignf(3.141f / 2.0f, sinp); // use 90 deg if out of range
        else *y = asinf(sinp);
    }
    // yaw (z-axis rotation)
    if(z)
    {
        CGL_float siny_cosp = 2.0f * (q.w * q.vec.z + q.vec.x * q.vec.y);
        CGL_float cosy_cosp = 1.0f - 2.0f * (q.vec.y * q.vec.y - q.vec.z * q.vec.z);
        *z = atan2f(siny_cosp, cosy_cosp);
    }
}

CGL_quat CGL_quat_mul(CGL_quat a, CGL_quat b)
{
    CGL_quat result;
    CGL_vec3 temp1, temp2, temp3;
    temp1 = CGL_vec3_scale(a.vec, b.w);
    temp2 = CGL_vec3_scale(b.vec, a.w);
    temp3 = CGL_vec3_cross(a.vec, b.vec);
    result.w = a.w * b.w - CGL_vec3_dot(a.vec, b.vec);
    result.vec = CGL_vec3_add(temp1, temp2);
    result.vec = CGL_vec3_add(result.vec, temp3);
    return result;
}

void CGL_quat_rotate(CGL_quat q, CGL_float x, CGL_float y, CGL_float z, float* ox, float* oy, float* oz)
{
    CGL_float ww = q.w * q.w;
    CGL_float xx = q.vec.x * q.vec.x;
    CGL_float yy = q.vec.y * q.vec.y;
    CGL_float zz = q.vec.z * q.vec.z;
    CGL_float wx = q.w * q.vec.x;
    CGL_float wy = q.w * q.vec.y;
    CGL_float wz = q.w * q.vec.z;
    CGL_float xy = q.vec.x * q.vec.y;
    CGL_float xz = q.vec.x * q.vec.z;
    CGL_float yz = q.vec.y * q.vec.z;

    if(ox) *ox = ww * x + 2.0f * wy * z - 2.0f * wz * y + xx * x + 2.0f * xy * y + 2.0f * xz * z - zz * x - yy * x;
    if(oy) *oy = 2.0f * xy * x + yy * y + 2.0f * yz * z + 2.0f * wz * x - zz * y + ww * y - 2.0f * wx * z - xx * y;
    if(oz) *oz = 2.0f * xz * x + 2.0f * yz * y + zz * z - 2.0f * wy * x - yy * z + 2.0f * wx * y - xx * z + ww * z;
}

CGL_quat CGL_quat_inverse(CGL_quat quat)
{
    CGL_float normal = CGL_quat_norm(quat);
    normal = 1.0f / normal * normal;
    quat = CGL_quat_conjugate(quat);
    quat = CGL_quat_mul_scalar(quat, normal);
    return quat;
}

CGL_mat4 CGL_quat_to_mat4(CGL_quat quat)
{
    CGL_float s = CGL_quat_norm(quat);
    s = 2.0f / s * s;
    return CGL_mat4_init(
        1.0f - s * (quat.vec.y * quat.vec.y + quat.vec.z * quat.vec.z), s * (quat.vec.x * quat.vec.y - quat.vec.z * quat.w), s * (quat.vec.x * quat.vec.z + quat.vec.y * quat.w), 0.0f,
        s * (quat.vec.x * quat.vec.y + quat.vec.z * quat.w), 1.0f - s * (quat.vec.x * quat.vec.x + quat.vec.z * quat.vec.z), s * (quat.vec.y * quat.vec.z - quat.vec.x * quat.w), 0.0f,
        s * (quat.vec.x * quat.vec.z - quat.vec.y * quat.w), s * (quat.vec.y * quat.vec.z + quat.vec.x * quat.w), 1.0f - s * (quat.vec.x * quat.vec.x + quat.vec.y * quat.vec.y), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

CGL_quat CGL_quat_slerp(CGL_quat q, CGL_quat r, CGL_float t)
{
    CGL_float cos_theta = q.w * r.w + q.vec.x * r.vec.x + q.vec.y * r.vec.y + q.vec.z * r.vec.z;
    CGL_float theta = acosf(cos_theta);
    CGL_float sin_theta = sinf(theta);
    CGL_float s0 = sinf((1.0f - t) * theta) / sin_theta;
    CGL_float s1 = sinf(t * theta) / sin_theta;
    CGL_quat result = CGL_quat_mul_scalar(q, s0);
    CGL_quat temp = CGL_quat_mul_scalar(r, s1);
    result = CGL_quat_add(result, temp);
    return result;
}

CGL_quat CGL_quat_squad(CGL_quat q0, CGL_quat q1, CGL_quat a0, CGL_quat a1, CGL_float t)
{
    CGL_quat qr0 = CGL_quat_slerp(q0, q1, t);
    CGL_quat qr1 = CGL_quat_slerp(a0, a1, t);
    return CGL_quat_slerp(qr0, qr1, 2.0f * t * (1.0f - t));
}

CGL_mat4 CGL_mat4_look_at(CGL_vec3 eye, CGL_vec3 target, CGL_vec3 up)
{
    CGL_vec3 z = CGL_vec3_sub(eye, target);
    CGL_vec3_normalize(z);
    CGL_vec3 x = CGL_vec3_cross(up, z);
    CGL_vec3_normalize(x);
    CGL_vec3 y = CGL_vec3_cross(z, x);
    CGL_vec3_normalize(y);
    CGL_mat4 mt = CGL_mat4_init(
        x.x, x.y, x.z, -CGL_vec3_dot(x, eye),
        y.x, y.y, y.z, -CGL_vec3_dot(y, eye),
        z.x, z.y, z.z, -CGL_vec3_dot(z, eye),
        0.0f, 0.0f, 0.0f, 1.0f
    );
    return (mt);
}

CGL_vec3 CGL_vec3_apply_transformations(CGL_vec3 original, const CGL_vec3* translation, const CGL_vec3* rotation, const CGL_vec3* scale)
{
    if(rotation)
    {
        original = CGL_vec3_rotate_x(original, rotation->x);
        original = CGL_vec3_rotate_y(original, rotation->y);
        original = CGL_vec3_rotate_z(original, rotation->z);                
    }
    if(scale) original = CGL_vec3_mul(original, (*scale));
    if(translation) original = CGL_vec3_add(original, (*translation));
    return original;
}

CGL_vec2 CGL_vec2_apply_transformations(CGL_vec2 original, const CGL_vec2* translation, const CGL_float* rotation, const CGL_vec2* scale)
{
    CGL_vec2 zero = CGL_vec2_init(0.0f, 0.0f);
    if(rotation) CGL_vec2_rotate_about_point(original, zero, (*rotation));
    if(scale) original = CGL_vec2_mul(original, (*scale));
    if(translation) original = CGL_vec2_add(original, (*translation));
    return original;
}


CGL_vec4 CGL_quat_mul_vec4(CGL_quat q, CGL_vec4 v)
{
    CGL_warn("CGL_quat_mul_vec4 is not implemented yet");
    return v;
}

#ifndef CGL_EXCLUDE_MATH_FUNCTIONS
// vec2 
CGL_vec2 CGL_vec2_add_(CGL_vec2 a, CGL_vec2 b)
{
    return CGL_vec2_init(a.x + b.x, a.y + b.y);
}

CGL_vec2 CGL_vec2_sub_(CGL_vec2 a, CGL_vec2 b)
{
    return CGL_vec2_init(a.x - b.x, a.y - b.y);
}

CGL_vec2 CGL_vec2_mul_(CGL_vec2 a, CGL_vec2 b)
{
    return CGL_vec2_init(a.x * b.x, a.y * b.y);
}

CGL_vec2 CGL_vec2_div_(CGL_vec2 a, CGL_vec2 b)
{
    return CGL_vec2_init(a.x / b.x, a.y / b.y);
}

CGL_vec2 CGL_vec2_add_scalar_(CGL_vec2 a, CGL_float b)
{
    return CGL_vec2_init(a.x + b, a.y + b);
}

CGL_vec2 CGL_vec2_sub_scalar_(CGL_vec2 a, CGL_float b)
{
    return CGL_vec2_init(a.x - b, a.y - b);
}

CGL_vec2 CGL_vec2_scale_(CGL_vec2 a, CGL_float b)
{
    return CGL_vec2_init(a.x * b, a.y * b);
}

CGL_vec2 CGL_vec2_normalize_(CGL_vec2 a)
{
    CGL_float length = CGL_vec2_length_(a);
    return CGL_vec2_init(a.x / length, a.y / length);
}

CGL_float CGL_vec2_dot_(CGL_vec2 a, CGL_vec2 b)
{
    return a.x * b.x + a.y * b.y;
}

CGL_float CGL_vec2_length_(CGL_vec2 a)
{
    return sqrtf(CGL_vec2_dot_(a, a));
}

CGL_float CGL_vec2_cross_(CGL_vec2 a, CGL_vec2 b)
{
    return a.x * b.y - a.y * b.x;
}

CGL_vec2 CGL_vec2_reflect_(CGL_vec2 a, CGL_vec2 b)
{
    CGL_float dot = CGL_vec2_dot_(a, b);
    return CGL_vec2_init(a.x - 2.0f * dot * b.x, a.y - 2.0f * dot * b.y);
}

CGL_vec2 CGL_vec2_refract_(CGL_vec2 a, CGL_vec2 b, CGL_float eta)
{
    CGL_float dot = CGL_vec2_dot_(a, b);
    CGL_float k = 1.0f - eta * eta * (1.0f - dot * dot);
    if(k < 0.0f) return CGL_vec2_init(0.0f, 0.0f);
    return CGL_vec2_init(eta * a.x - (eta * dot + sqrtf(k)) * b.x, eta * a.y - (eta * dot + sqrtf(k)) * b.y);
}

CGL_vec2 CGL_vec2_rotate_(CGL_vec2 a, CGL_float angle)
{
    CGL_float s = sinf(angle);
    CGL_float c = cosf(angle);
    return CGL_vec2_init(a.x * c - a.y * s, a.x * s + a.y * c);
}

CGL_vec2 CGL_vec2_lerp_(CGL_vec2 a, CGL_vec2 b, CGL_float t)
{
    return CGL_vec2_init(a.x + t * (b.x - a.x), a.y + t * (b.y - a.y));
}

// vec3
CGL_vec3 CGL_vec3_add_(CGL_vec3 a, CGL_vec3 b)
{
    return CGL_vec3_init(a.x + b.x, a.y + b.y, a.z + b.z);
}

CGL_vec3 CGL_vec3_add3_(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
{
    return CGL_vec3_init(a.x + b.x + c.x, a.y + b.y + c.y, a.z + b.z + c.z);
}

CGL_vec3 CGL_vec3_sub_(CGL_vec3 a, CGL_vec3 b)
{
    return CGL_vec3_init(a.x - b.x, a.y - b.y, a.z - b.z);
}

CGL_vec3 CGL_vec3_mul_(CGL_vec3 a, CGL_vec3 b)
{
    return CGL_vec3_init(a.x * b.x, a.y * b.y, a.z * b.z);
}

CGL_vec3 CGL_vec3_div_(CGL_vec3 a, CGL_vec3 b)
{
    return CGL_vec3_init(a.x / b.x, a.y / b.y, a.z / b.z);
}

CGL_vec3 CGL_vec3_add_scalar_(CGL_vec3 a, CGL_float b)
{
    return CGL_vec3_init(a.x + b, a.y + b, a.z + b);
}

CGL_vec3 CGL_vec3_sub_scalar_(CGL_vec3 a, CGL_float b)
{
    return CGL_vec3_init(a.x - b, a.y - b, a.z - b);
}

CGL_vec3 CGL_vec3_scale_(CGL_vec3 a, CGL_float b)
{
    return CGL_vec3_init(a.x * b, a.y * b, a.z * b);
}

CGL_vec3 CGL_vec3_normalize_(CGL_vec3 a)
{
    CGL_float length = CGL_vec3_length_(a);
    return CGL_vec3_init(a.x / length, a.y / length, a.z / length);
}

CGL_float CGL_vec3_dot_(CGL_vec3 a, CGL_vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

CGL_float CGL_vec3_length_(CGL_vec3 a)
{
    return sqrtf(CGL_vec3_dot_(a, a));
}

CGL_vec3 CGL_vec3_cross_(CGL_vec3 a, CGL_vec3 b)
{
    return CGL_vec3_init(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

CGL_vec3 CGL_vec3_reflect_(CGL_vec3 a, CGL_vec3 b)
{
    CGL_float dot = CGL_vec3_dot_(a, b);
    return CGL_vec3_init(a.x - 2.0f * dot * b.x, a.y - 2.0f * dot * b.y, a.z - 2.0f * dot * b.z);
}

CGL_vec3 CGL_vec3_refract_(CGL_vec3 a, CGL_vec3 b, CGL_float eta)
{
    CGL_float dot = CGL_vec3_dot_(a, b);
    CGL_float k = 1.0f - eta * eta * (1.0f - dot * dot);
    if(k < 0.0f) return CGL_vec3_init(0.0f, 0.0f, 0.0f);
    return CGL_vec3_init(eta * a.x - (eta * dot + sqrtf(k)) * b.x, eta * a.y - (eta * dot + sqrtf(k)) * b.y, eta * a.z - (eta * dot + sqrtf(k)) * b.z);
}

CGL_vec3 CGL_vec3_lerp_(CGL_vec3 a, CGL_vec3 b, CGL_float t)
{
    return CGL_vec3_init(a.x + t * (b.x - a.x), a.y + t * (b.y - a.y), a.z + t * (b.z - a.z));
}

// vec4
CGL_vec4 CGL_vec4_add_(CGL_vec4 a, CGL_vec4 b)
{
    return CGL_vec4_init(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

CGL_vec4 CGL_vec4_sub_(CGL_vec4 a, CGL_vec4 b)
{
    return CGL_vec4_init(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

CGL_vec4 CGL_vec4_mul_(CGL_vec4 a, CGL_vec4 b)
{
    return CGL_vec4_init(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

CGL_vec4 CGL_vec4_div_(CGL_vec4 a, CGL_vec4 b)
{
    return CGL_vec4_init(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w);
}

CGL_vec4 CGL_vec4_add_scalar_(CGL_vec4 a, CGL_float b)
{
    return CGL_vec4_init(a.x + b, a.y + b, a.z + b, a.w + b);
}

CGL_vec4 CGL_vec4_sub_scalar_(CGL_vec4 a, CGL_float b)
{
    return CGL_vec4_init(a.x - b, a.y - b, a.z - b, a.w - b);
}

CGL_vec4 CGL_vec4_scale_(CGL_vec4 a, CGL_float b)
{
    return CGL_vec4_init(a.x * b, a.y * b, a.z * b, a.w * b);
}

CGL_vec4 CGL_vec4_normalize_(CGL_vec4 a)
{
    CGL_float length = CGL_vec4_length_(a);
    return CGL_vec4_init(a.x / length, a.y / length, a.z / length, a.w / length);
}

CGL_float CGL_vec4_dot_(CGL_vec4 a, CGL_vec4 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

CGL_float CGL_vec4_length_(CGL_vec4 a)
{
    return sqrtf(CGL_vec4_dot_(a, a));
}

CGL_vec4 CGL_vec4_lerp_(CGL_vec4 a, CGL_vec4 b, CGL_float t)
{
    return CGL_vec4_init(a.x + t * (b.x - a.x), a.y + t * (b.y - a.y), a.z + t * (b.z - a.z), a.w + t * (b.w - a.w));
}

#endif


struct CGL_context
{
    bool is_initialized;
    CGL_int window_count;
    #ifndef CGL_EXCLUDE_WINDOW_API 
    CGL_window* window_table[CGL_WINDOW_TABLE_SIZE];
    #endif
};

CGL_context* __CGL_context = NULL;

bool CGL_init()
{
    if(__CGL_context != NULL) return true;
    __CGL_context = (CGL_context*)malloc(sizeof(CGL_context));
    if(__CGL_context == NULL) return false;
    __CGL_context->is_initialized = true;
    __CGL_context->window_count = 0;
    CGL_logger_init(CGL_ENABLE_CONSOLE_LOGGING);
    return true;
}

void CGL_shutdown()
{
    if(__CGL_context == NULL) return;
    free(__CGL_context);
    __CGL_context = NULL;
    CGL_logger_shutdown();
}

#endif


#if 1 // just to use code folding in ide

#if defined(_WIN32) || defined(_WIN64)
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif


CGL_sizei CGL_utils_get_file_size(const CGL_byte* path)
{
#if defined(_WIN32) || defined(_WIN64)
    HANDLE file = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(file == INVALID_HANDLE_VALUE) return -1;
    LARGE_INTEGER size;
    if(GetFileSizeEx(file, &size) == 0) return -1;
    CloseHandle(file);
    return size.QuadPart;
#else
    struct stat st;
    if(stat(path, &st) == 0) return st.st_size;
    return -1;
#endif
}

void CGL_utils_sleep(const CGL_sizei milis)
{
#if defined(_WIN32) || defined(_WIN64)
    Sleep((DWORD)milis);
#else
    usleep(milis * 1000);
#endif
}

// read file into memory
CGL_byte* CGL_utils_read_file(const CGL_byte* path, size_t* size_ptr)
{
#if defined(_WIN32) || defined(_WIN64)
    HANDLE file = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(file == INVALID_HANDLE_VALUE) return NULL;
    LARGE_INTEGER size;
    if(GetFileSizeEx(file, &size) == 0) return NULL;
    CGL_byte* data = (CGL_byte*)malloc(size.QuadPart + 1);
    if(data == NULL) return NULL;
    DWORD read = 0;
    if(ReadFile(file, data, (DWORD)size.QuadPart, &read, NULL) == 0) return NULL;
    CloseHandle(file);
    data[size.QuadPart] = 0;
    if(size_ptr != NULL) *size_ptr = size.QuadPart;
    return data;
#else
    struct stat st;
    if(stat(path, &st) != 0) return NULL;
    CGL_byte* data = (CGL_byte*)malloc(st.st_size + 1);
    if(data == NULL) return NULL;
    FILE* file = fopen(path, "r");
    if(file == NULL) return NULL;
    fread(data, 1, st.st_size, file);
    fclose(file);
    data[st.st_size] = 0;
    if(size_ptr != NULL) *size_ptr = st.st_size;
    return data;
#endif
}

// write data to file
bool CGL_utils_write_file(const char* path, const char* data, size_t size)
{
    FILE* file = fopen(path, "w");
    if(file == NULL)
        return false;
    fwrite(data, 1, size, file);
    fclose(file);
    return true;
}

bool CGL_utils_append_file(const char* path, const char* data, size_t size)
{
    FILE* file = fopen(path, "a");
    if(file == NULL)
        return false;
    fwrite(data, 1, size, file);
    fclose(file);
    return true;
}


#ifndef CGL_EXCLUDE_WINDOW_API

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
    CGL_window_drag_n_drop_callback drag_n_drop_callback;

    GLFWkeyfun previous_key_callback;
    GLFWmousebuttonfun previous_mouse_button_callback;
    GLFWcursorposfun previous_mouse_position_callback;
    GLFWscrollfun previous_mouse_scroll_callback;
    GLFWframebuffersizefun previous_framebuffer_size_callback;
    GLFWwindowclosefun previous_close_callback;
    GLFWdropfun previous_drag_n_drop_callback;
};

// callbacks
void __CGL_window_key_callback(GLFWwindow* window, CGL_int key, CGL_int scancode, CGL_int action, CGL_int mods)
{
    // CGL_window* cgl_window = (CGL_window*)glfwGetWindowUserPointer(window);
    CGL_window* cgl_window = __CGL_context->window_table[(uintptr_t)window % CGL_WINDOW_TABLE_SIZE];
    if(cgl_window->key_callback != NULL)
        cgl_window->key_callback(cgl_window, key, scancode, action, mods);
    if(cgl_window->previous_key_callback)
        cgl_window->previous_key_callback(window, key, scancode, action, mods);
}

void __CGL_window_mouse_button_callback(GLFWwindow* window, CGL_int button, CGL_int action, CGL_int mods)
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

void __CGL_window_framebuffer_size_callback(GLFWwindow* window, CGL_int width, CGL_int height)
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
    if(cgl_window->close_callback)
        cgl_window->close_callback(cgl_window);
    if(cgl_window->previous_close_callback)
        cgl_window->previous_close_callback(window);
}

void  __CGL_window_drag_n_drop_callback(GLFWwindow* window, CGL_int count, const char** paths)
{
    CGL_window* cgl_window = __CGL_context->window_table[(uintptr_t)window % CGL_WINDOW_TABLE_SIZE];
    if(cgl_window->drag_n_drop_callback)
        cgl_window->drag_n_drop_callback(cgl_window, paths, count);
    if(cgl_window->previous_drag_n_drop_callback)
        cgl_window->previous_drag_n_drop_callback(window, count, paths);
}

// create window
CGL_window* CGL_window_create(CGL_int width, CGL_int height, const char* title)
{
    if (__CGL_context->window_count == 0)
    {
        if (!glfwInit())
        {
            CGL_log_internal("Failed to initialize GLFW\n");
            return false;
        }
    }
#ifdef CGL_WASM
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_CONTEXT_API);
#else
    // tell glfw we are going to use opengl api
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	// tell glfw to initialize opengl context for opengl version 4.6 (latest)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif
	// tell glfw to use the opengl core profile and not the compatibility profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	// disabling resinfg gets rid of managing things like aspect ration and stuff
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    CGL_window* window = (CGL_window*)malloc(sizeof(CGL_window));
    if(window == NULL)
    {
        CGL_log_internal("Failed to allocate memory for window\n");
        return NULL;
    }
    window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window->handle == NULL)
    {
        CGL_log_internal("Failed to create GLFW window\n");
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
    window->drag_n_drop_callback = NULL;
    window->user_data = NULL;
    window->previous_close_callback = NULL;
    window->previous_framebuffer_size_callback = NULL;
    window->previous_key_callback = NULL;
    window->previous_mouse_button_callback = NULL;
    window->previous_mouse_position_callback = NULL;
    window->previous_mouse_scroll_callback = NULL;
    window->previous_drag_n_drop_callback = NULL;
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
void CGL_window_set_size(CGL_window* window, CGL_int width, CGL_int height)
{
    glfwSetWindowSize(window->handle, width, height);
}

// set window position
void CGL_window_set_position(CGL_window* window, CGL_int x, CGL_int y)
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
void CGL_window_get_size(CGL_window* window, CGL_int* width, CGL_int* height)
{
    glfwGetWindowSize(window->handle, width, height);
}

// get window position
void CGL_window_get_position(CGL_window* window, CGL_int* x, CGL_int* y)
{
    glfwGetWindowPos(window->handle, x, y);
}

// get window framebuffer size
void CGL_window_get_framebuffer_size(CGL_window* window, CGL_int* width, CGL_int* height)
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

void CGL_window_set_drag_n_drop_callback(CGL_window* window, CGL_window_drag_n_drop_callback callback) // set drag and drop callback
{
    if(window->drag_n_drop_callback == NULL)
        window->previous_drag_n_drop_callback = glfwSetDropCallback(window->handle, __CGL_window_drag_n_drop_callback);
    window->drag_n_drop_callback = callback;
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

    if(window->drag_n_drop_callback != NULL)
    {
        window->previous_drag_n_drop_callback = glfwSetDropCallback(window->handle, __CGL_window_drag_n_drop_callback);
        if(window->previous_drag_n_drop_callback == __CGL_window_drag_n_drop_callback) window->previous_drag_n_drop_callback = NULL;
    }
}

// get key state
CGL_int CGL_window_get_key(CGL_window* window, CGL_int key)
{
    return glfwGetKey(window->handle, key);
}

// check if key is pressed
CGL_bool CGL_window_is_key_pressed(CGL_window* window, CGL_int key)
{
    return glfwGetKey(window->handle, key) == GLFW_PRESS;
}

// get mouse button state
CGL_int CGL_window_get_mouse_button(CGL_window* window, CGL_int button)
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

#endif

// opengl 
#if 1 // Just to use code folding

#ifndef CGL_EXCLUDE_GRAPHICS_API

// texture

struct CGL_texture
{
    GLuint handle;
    CGL_int width;
    CGL_int height;
    CGL_int depth;
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
    {CGL_error("Invalid channel count for image\n");return NULL;}
    if(image->bytes_per_channel == 8)
        type = GL_UNSIGNED_BYTE;
    else if(image->bytes_per_channel == 16)
        type = GL_UNSIGNED_SHORT;
    else if(image->bytes_per_channel == 32)
        type = GL_FLOAT;
    else
    {CGL_error("Invalid bit depth for image\n");return NULL;}        
    CGL_texture* texture = (CGL_texture*)CGL_malloc(sizeof(CGL_texture));
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
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(texture->target, 0, internal_format, image->width, image->height, 0, format, type, image->data);
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(texture->target, 0);
    return texture;
}

// create texture
CGL_texture* CGL_texture_create_blank(CGL_int width, CGL_int height, GLenum format, GLenum internal_format, GLenum type)
{
    CGL_texture* texture = (CGL_texture*)CGL_malloc(sizeof(CGL_texture));
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
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_REPEAT);
#ifndef CGL_WASM
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_R, GL_REPEAT);
#endif
    glBindTexture(texture->target, 0);
    return texture;
}

CGL_texture* CGL_texture_create_cubemap()
{
    CGL_texture* texture = (CGL_texture*)CGL_malloc(sizeof(CGL_texture));
    texture->width = 0;
    texture->height = 0;
    texture->depth = 0;
    texture->target = GL_TEXTURE_CUBE_MAP;
    texture->user_data = NULL;
	glGenTextures(1, &texture->handle);
	glBindTexture(texture->target, texture->handle);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifndef CGL_WASM
	glTexParameteri(texture->target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
#endif
	glBindTexture(texture->target, 0);
    return texture;
}

CGL_texture* CGL_texture_create_3d(CGL_int width, CGL_int height, CGL_int depth, GLenum format, GLenum internal_format, GLenum type)
{
    CGL_texture* texture = (CGL_texture*)CGL_malloc(sizeof(CGL_texture));
    texture->width = width;
    texture->height = height;
    texture->depth = depth;
    texture->format = format;
    texture->internal_format = internal_format;
    texture->type = type;
    texture->target = GL_TEXTURE_3D;
    texture->user_data = NULL;
    glGenTextures(1, &texture->handle);
    glBindTexture(texture->target, texture->handle);
    glTexImage3D(texture->target, 0, texture->internal_format, width, height, depth, 0, texture->format, texture->type, NULL);
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_REPEAT);
#ifndef CGL_WASM
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_R, GL_REPEAT);
#endif
    glBindTexture(texture->target, 0);
    return texture;
}

CGL_texture* CGL_texture_create_array(CGL_int width, CGL_int height, CGL_int layers, GLenum format, GLenum internal_format, GLenum type)
{
    CGL_texture* texture = (CGL_texture*)CGL_malloc(sizeof(CGL_texture));
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
    glTexImage3D(texture->target, 0, texture->internal_format, width, height, layers, 0, texture->format, texture->type, NULL);
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glBindTexture(texture->target, 0);
    return texture;
}

void CGL_texture_cubemap_set_face(CGL_texture* texture, CGL_int face, CGL_image* image)
{
    GLenum format, internal_format, type;
    if(image->channels == 3)
        format = internal_format = GL_RGB;
    else if(image->channels == 4)
        format = internal_format = GL_RGBA;
    else
    {printf("Invalid channel count for image\n");return;}
    if(image->bytes_per_channel == 8)
        type = GL_UNSIGNED_BYTE;
    else if(image->bytes_per_channel == 16)
        type = GL_UNSIGNED_SHORT;
    else if(image->bytes_per_channel == 32)
        type = GL_FLOAT;
    else
    {printf("Invalid bit depth for image\n");return;}
	glBindTexture(texture->target, texture->handle);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(face, 0, internal_format, image->width, image->height, 0, format, type, image->data);
	glBindTexture(texture->target, 0);
}

void CGL_texture_array_set_layer_data(CGL_texture* texture, CGL_int layer, void* data)
{
    glBindTexture(texture->target, texture->handle);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage3D(texture->target, 0, 0, 0, layer, texture->width, texture->height, 1, texture->format, texture->type, data);
	glBindTexture(texture->target, 0);
}

void CGL_texture_set_scaling_method(CGL_texture* texture, GLint method)
{
	glBindTexture(texture->target, texture->handle);
    glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, method);
    glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, method);
}

void CGL_texture_set_wrapping_method(CGL_texture* texture, GLint method)
{
	glBindTexture(texture->target, texture->handle);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_S, method);
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_T, method);
#ifndef CGL_WASM
    glTexParameteri(texture->target, GL_TEXTURE_WRAP_R, method);
#endif
}

// destroy texture
void CGL_texture_destroy(CGL_texture* texture)
{
    glDeleteTextures(1, &texture->handle);
    CGL_free(texture);
}

// bind texture to unit
void CGL_texture_bind(CGL_texture* texture, CGL_int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(texture->target, texture->handle);
}

// set texture data
void CGL_texture_set_data(CGL_texture* texture, void* data)
{
    glBindTexture(texture->target, texture->handle);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(texture->target, 0, texture->internal_format, texture->width, texture->height, 0, texture->format, texture->type, data);
    glBindTexture(texture->target, 0);
}

void CGL_texture_set_sub_data(CGL_texture* texture, size_t offset_x, size_t offset_y, size_t size_x, size_t size_y,  void* data) // set texture sub data
{
    glBindTexture(texture->target, texture->handle);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(texture->target, 0, (GLint)offset_x, (GLint)offset_y, (GLsizei)size_x, (GLsizei)size_y, texture->format, texture->type, data);
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

CGL_uint CGL_texture_get_internal_handle(CGL_texture* texture)
{
    return (CGL_uint)texture->handle;
}

// get texture size
void CGL_texture_get_size(CGL_texture* texture, CGL_int* width, CGL_int* height)
{
    if(width)   *width = texture->width;
    if(height)  *height = texture->height;
}

// framebuffer
struct CGL_framebuffer
{
    CGL_texture* mousepick_texture[3];
    CGL_texture* color_attachments[4];
    CGL_texture* color_texture;
    CGL_texture* depth_texture;
    void* user_data;
    GLuint handle;
    CGL_int width;
    CGL_int height;
    CGL_int color_attachment_count;
    bool is_default;
};

// create framebuffer (32 bit)
CGL_framebuffer* CGL_framebuffer_create(CGL_int width, CGL_int height)
{
    CGL_framebuffer* framebuffer = (CGL_framebuffer*)CGL_malloc(sizeof(CGL_framebuffer));
    if(framebuffer == NULL) return NULL;

    framebuffer->color_attachment_count = 0;
    
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
        
    for(CGL_int i = 0; i < 3; i++)
    {
        framebuffer->mousepick_texture[i] = CGL_texture_create_blank(width, height, GL_RED_INTEGER, GL_R32I, GL_INT);
        if(!framebuffer->mousepick_texture[i])
        {
            CGL_texture_destroy(framebuffer->color_texture);
            CGL_texture_destroy(framebuffer->depth_texture);
            for(CGL_int j = 0; j < i; j++)
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
        for(CGL_int i = 0; i < 3; i++)
            CGL_texture_destroy(framebuffer->mousepick_texture[i]);
        free(framebuffer);
        CGL_log_internal("Framebuffer is not complete\n");
        // get and print opengl error
        GLenum error = glGetError();
        if(error != GL_NO_ERROR)
            CGL_log_internal("OpenGL error: %d\n", (error));
        return NULL;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->is_default = false;
    framebuffer->user_data = NULL;

    return framebuffer;
}

CGL_framebuffer* CGL_framebuffer_create_basic(CGL_int width, CGL_int height)
{
    CGL_framebuffer* framebuffer = (CGL_framebuffer*)CGL_malloc(sizeof(CGL_framebuffer));
    if(framebuffer == NULL) return NULL;
    framebuffer->color_attachment_count = 1;    
    glGenFramebuffers(1, &framebuffer->handle);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle);    
    // create the textures
    framebuffer->color_texture = CGL_texture_create_blank(width, height, GL_RGBA, GL_RGBA32F, GL_FLOAT);    
    if(!framebuffer->color_texture)
    {
        CGL_free(framebuffer);
        return NULL;
    }
    framebuffer->color_attachments[0] = framebuffer->color_texture;
    
    framebuffer->depth_texture = CGL_texture_create_blank(width, height, GL_DEPTH_COMPONENT, GL_DEPTH24_STENCIL8, GL_FLOAT);
    if(!framebuffer->depth_texture)
    {
        CGL_texture_destroy(framebuffer->color_texture);
        CGL_free(framebuffer);
        return NULL;
    }
        
    // attach the textures
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer->color_texture->handle, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, framebuffer->depth_texture->handle, 0);

    GLenum buffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, buffers);

    
    // check if framebuffer is complete
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        CGL_texture_destroy(framebuffer->color_texture);
        CGL_texture_destroy(framebuffer->depth_texture);
        free(framebuffer);
        CGL_log_internal("Framebuffer is not complete\n");
        // get and print opengl error
        GLenum error = glGetError();
        if(error != GL_NO_ERROR) CGL_log_internal("OpenGL error: %d\n", (error));
        return NULL;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    framebuffer->width = width;
    framebuffer->height = height;
    framebuffer->is_default = false;
    framebuffer->user_data = NULL;

    return framebuffer;

}

void CGL_framebuffer_add_color_attachment(CGL_framebuffer* framebuffer, CGL_texture* texture)
{
    if(framebuffer->is_default)
    {
        CGL_log_internal("Cannot add color attachment to default framebuffer\n");
        return;
    }

    if(framebuffer->color_attachment_count >= 8)
    {
        CGL_log_internal("Framebuffer already has 4 color attachments\n");
        return;
    }

    if(framebuffer->color_attachment_count == 0)
    {
        CGL_log_internal("This type of framebuffer cannot have a custom color attachment");
        return;
    }

    framebuffer->color_attachments[framebuffer->color_attachment_count++] = texture;

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + framebuffer->color_attachment_count - 1, GL_TEXTURE_2D, texture->handle, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLenum buffers[8] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
	glDrawBuffers(framebuffer->color_attachment_count, buffers);

    // check if framebuffer is complete
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        CGL_texture_destroy(framebuffer->color_texture);
        CGL_texture_destroy(framebuffer->depth_texture);
        for(CGL_int i = 0; i < 3; i++)
            CGL_texture_destroy(framebuffer->mousepick_texture[i]);
        free(framebuffer);
        CGL_log_internal("Framebuffer is not complete\n");
        // get and print opengl error
        GLenum error = glGetError();
        if(error != GL_NO_ERROR)
            CGL_log_internal("OpenGL error: %d\n", (error));
        return;
    }
}

CGL_texture* CGL_framebuffer_get_color_attachment(CGL_framebuffer* framebuffer, CGL_int index)
{
    if(index >= framebuffer->color_attachment_count)
    {
        CGL_log_internal("Framebuffer does not have a color attachment at index %d\n", index);
        return NULL;
    }
    return framebuffer->color_attachments[index];
}

#ifndef CGL_EXCLUDE_WINDOW_API
// create framebuffer from default framebuffer
CGL_framebuffer* CGL_framebuffer_create_from_default(CGL_window* window)
{
    CGL_framebuffer* framebuffer = (CGL_framebuffer*)malloc(sizeof(CGL_framebuffer));
    if(framebuffer == NULL) return NULL;
    framebuffer->handle = 0;
    framebuffer->color_attachment_count = 0;
    framebuffer->is_default = true;
    framebuffer->user_data = window;
    return framebuffer;
}
#endif

// destroy framebuffer
void CGL_framebuffer_destroy(CGL_framebuffer* framebuffer)
{
    if(!framebuffer->is_default)
    {
        if(framebuffer->color_attachment_count == 0)
        {
            CGL_texture_destroy(framebuffer->color_texture);
            for(CGL_int i = 0; i < 3; i++) CGL_texture_destroy(framebuffer->mousepick_texture[i]);
        }
        else for(CGL_int i = 0; i < framebuffer->color_attachment_count; i++) CGL_texture_destroy(framebuffer->color_attachments[i]);
        CGL_texture_destroy(framebuffer->depth_texture);
        glDeleteFramebuffers(1, &framebuffer->handle);
    }
    free(framebuffer);
}

// bind framebuffer
void CGL_framebuffer_bind(CGL_framebuffer* framebuffer)
{
    if(framebuffer->is_default)
    {
        CGL_int width = 0, height = 0;
#ifdef CGL_WASM
        // TODO
#else
        CGL_window_get_size((CGL_window*)framebuffer->user_data, &width, &height);
#endif
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle); 
        glViewport(0, 0, framebuffer->width, framebuffer->height);
        if(framebuffer->color_attachment_count > 0)
        {
            GLenum buffers[8] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
        	glDrawBuffers(framebuffer->color_attachment_count, buffers);
        }            
    }
}

// get framebuffer size
void CGL_framebuffer_get_size(CGL_framebuffer* framebuffer, CGL_int* width, CGL_int* height)
{
    if(framebuffer->is_default)
    {
        CGL_int w = 0, h = 0;
#ifdef CGL_WASM
        // TODO
#else
        CGL_window_get_framebuffer_size((CGL_window*)framebuffer->user_data, &w, &h);
#endif
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
    if(!framebuffer->is_default) framebuffer->user_data = user_data;
}

// get framebuffer user data
void* CGL_framebuffer_get_user_data(CGL_framebuffer* framebuffer)
{
    if(!framebuffer->is_default) return framebuffer->user_data;
    return NULL;
}

// read pixels from framebuffer
void CGL_framebuffer_read_pixels(CGL_framebuffer* framebuffer, CGL_int x, CGL_int y, CGL_int width, CGL_int height, void* pixels)
{    
    if(framebuffer->color_attachment_count != 0) return;
    if(framebuffer->is_default) return;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle);
    glReadPixels(x, y, width, height, framebuffer->color_texture->format, framebuffer->color_texture->type, pixels);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);    
}
 
// get mouse pick id
CGL_int CGL_framebuffer_get_mouse_pick_id(CGL_framebuffer* framebuffer, CGL_int x, CGL_int y, CGL_int index)
{
    if(framebuffer->color_attachment_count != 0) return 0;
    if(index >= 3) return -1;
    if(framebuffer->is_default) return 0;
    CGL_int id = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->handle);
    glReadBuffer(GL_COLOR_ATTACHMENT0 + 1 + index);
    glReadPixels(x, framebuffer->height - y, 1, 1, GL_RED_INTEGER, GL_INT, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return id;
}

// get color texture
CGL_texture* CGL_framebuffer_get_color_texture(CGL_framebuffer* framebuffer)
{
    if(framebuffer->is_default) return NULL;
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
    CGL_int binding;
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
        CGL_log_internal("CGL_ssbo_set_sub_data: offset + size > ssbo->size");
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
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, ssbo->size, data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// get ssbo sub data
void CGL_ssbo_get_sub_data(CGL_ssbo* ssbo, size_t offset, size_t size, void* data)
{
    if(offset + size > ssbo->size)
    {
        CGL_log_internal("CGL_ssbo_get_sub_data: offset + size > ssbo->size");
        return;
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->handle);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
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
        CGL_log_internal("CGL_ssbo_copy: dst_offset + size > dst->size");
        return;
    }
    if(src_offset + size > src->size)
    {
        CGL_log_internal("CGL_ssbo_copy: src_offset + size > src->size");
        return;
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, dst->handle);
    glCopyNamedBufferSubData(src->handle, dst->handle, src_offset, dst_offset, size);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}



struct CGL_shader
{
    GLuint handle;
    void* user_data;
};

struct CGL_ubo
{
    CGL_sizei size;
    CGL_uint handle;
    CGL_uint binding;
    void* user_data;
};

CGL_ubo* CGL_ubo_create()
{
    CGL_ubo* ubo = (CGL_ubo*)CGL_malloc(sizeof(CGL_ubo));
    if(ubo == NULL) return NULL;
    glGenBuffers(1, &ubo->handle);
    ubo->size = 0;
    return ubo;
}

void CGL_ubo_destroy(CGL_ubo* ubo)
{
    glDeleteBuffers(1, &ubo->handle);
    CGL_free(ubo);
}

void CGL_ubo_bind(CGL_ubo* ubo, CGL_shader* shader, const CGL_byte* name, uint32_t binding)
{
    CGL_uint index = glGetUniformBlockIndex(shader->handle, name);
    if(index == GL_INVALID_INDEX) {CGL_log_internal("CGL_ubo_bind: glGetUniformBlockIndex failed");return;}
    glUniformBlockBinding(shader->handle, index, binding);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, ubo->handle);
    ubo->binding = binding;
}

void CGL_ubo_set_data(CGL_ubo* ubo, size_t size, void* data, bool static_draw)
{
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->handle);
    glBufferData(GL_UNIFORM_BUFFER, size, data, static_draw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    ubo->size = size;
}

void CGL_ubo_set_sub_data(CGL_ubo* ubo, size_t offset, size_t size, void* data, bool static_draw)
{
    if(offset + size > ubo->size) {CGL_log_internal("CGL_ubo_set_sub_data: offset + size > ubo->size");return;}
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->handle);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    ubo->size = CGL_utils_max(ubo->size, offset + size);
}

void CGL_ubo_get_data(CGL_ubo* ubo, size_t* size, void* data)
{
    if(size) *size = ubo->size;
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->handle);
    glGetBufferSubData(GL_UNIFORM_BUFFER, 0, ubo->size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void CGL_ubo_get_sub_data(CGL_ubo* ubo, size_t offset, size_t size, void* data)
{
    if(offset + size > ubo->size) {CGL_log_internal("CGL_ubo_get_sub_data: offset + size > ubo->size");return;}
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->handle);
    glGetBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void CGL_ubo_set_user_data(CGL_ubo* ubo, void* user_data)
{
    ubo->user_data = user_data;
}

void* CGL_ubo_get_user_data(CGL_ubo* ubo)
{
    return ubo->user_data;
}

size_t CGL_ubo_get_size(CGL_ubo* ubo)
{
    return ubo->size;
}


// gl
// clear 
void CGL_gl_clear(CGL_float r, CGL_float g, CGL_float b, CGL_float a)
{    
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static CGL_mesh_gpu* __CGL_GL_SCREEN_QUAD_MESH = NULL;

// load opengl functions
bool CGL_gl_init()
{
#ifdef CGL_WASM
    CGL_int gles_version = gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress);
    return true;
#else
    bool result = gladLoadGL();
    if(!result) CGL_log_internal("Failed to load OpenGL functions");
#endif
    CGL_mesh_cpu* mesh_cpu = CGL_mesh_cpu_quad(
        CGL_vec3_init(-1.0f, -1.0f, 0.0f),
        CGL_vec3_init(1.0f, -1.0f, 0.0f),
        CGL_vec3_init(1.0f, 1.0f, 0.0f),
        CGL_vec3_init(-1.0f, 1.0f, 0.0f)
    );
    __CGL_GL_SCREEN_QUAD_MESH = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(__CGL_GL_SCREEN_QUAD_MESH, mesh_cpu, true);
    CGL_mesh_cpu_destroy(mesh_cpu);    
    return result;
}

// clean up
void CGL_gl_shutdown()
{
    CGL_mesh_gpu_destroy(__CGL_GL_SCREEN_QUAD_MESH);
}

void CGL_gl_render_screen_quad()
{
    CGL_mesh_gpu_render(__CGL_GL_SCREEN_QUAD_MESH);
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
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(CGL_mesh_vertex), (void*)offsetof(CGL_mesh_vertex, bone_wieghts));
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(4, 4, GL_INT, sizeof(CGL_mesh_vertex), (void*)offsetof(CGL_mesh_vertex, bone_ids));
    glEnableVertexAttribArray(4);
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
    if(mesh->index_count <= 0) return;
    glBindVertexArray(mesh->vertex_array);
    glDrawElements(GL_TRIANGLES, (GLsizei)mesh->index_count, GL_UNSIGNED_INT, 0);
}

// render mesh instanfced (gpu)
void CGL_mesh_gpu_render_instanced(CGL_mesh_gpu* mesh, uint32_t count)
{
    if(mesh->index_count <= 0) return;
    glBindVertexArray(mesh->vertex_array);
    glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)mesh->index_count, GL_UNSIGNED_INT, 0, count);
}

// upload mesh from (cpu) to (gpu)
void CGL_mesh_gpu_upload(CGL_mesh_gpu* mesh, CGL_mesh_cpu* mesh_cpu, bool static_draw)
{
    if(mesh_cpu->index_count <= 0) return;
    mesh->index_count = mesh_cpu->index_count_used;
    mesh->vertex_count = mesh_cpu->vertex_count_used;
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh_cpu->vertex_count_used * sizeof(CGL_mesh_vertex), mesh_cpu->vertices, static_draw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_cpu->index_count_used * sizeof(unsigned int), mesh_cpu->indices, static_draw ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
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
    if(mesh->vertices) CGL_free(mesh->vertices);
    if(mesh->indices) CGL_free(mesh->indices);
    CGL_free(mesh);
}

// create mesh (cpu)
CGL_mesh_cpu* CGL_mesh_cpu_create(size_t vertex_count, size_t index_count)
{
    CGL_mesh_cpu* mesh = (CGL_mesh_cpu*)malloc(sizeof(CGL_mesh_cpu));
    if(mesh == NULL) return NULL;
    mesh->vertex_count = vertex_count;
    mesh->index_count = index_count;
    mesh->vertex_count_used = 0;
    mesh->index_count_used = 0;
    mesh->vertices = (CGL_mesh_vertex*)malloc(mesh->vertex_count * sizeof(CGL_mesh_vertex));
    if(mesh->vertices == NULL)
    {
        CGL_free(mesh);
        return NULL;
    }
    mesh->indices = (uint32_t*)malloc(mesh->index_count * sizeof(uint32_t));
    if(mesh->indices == NULL)
    {
        CGL_free(mesh->vertices);
        CGL_free(mesh);
        return NULL;
    }
    for(CGL_sizei i = 0 ; i < index_count ; i++ ) mesh->indices[i] = (CGL_uint)i;
    return mesh;
}

CGL_mesh_cpu* CGL_mesh_cpu_recalculate_normals(CGL_mesh_cpu* mesh)
{
    // clear normals
    for(size_t i = 0; i < mesh->vertex_count; i++) mesh->vertices[i].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    // calculate un normalized normals
    for(size_t i = 0; i < mesh->index_count; i += 3)
    {
        CGL_vec4 a = mesh->vertices[mesh->indices[i + 0]].position;
        CGL_vec4 b = mesh->vertices[mesh->indices[i + 1]].position;
        CGL_vec4 c = mesh->vertices[mesh->indices[i + 2]].position;
        CGL_vec3 ab = CGL_vec3_sub(b, a);
        CGL_vec3 ac = CGL_vec3_sub(c, a);
        CGL_vec3 normal_3 = CGL_vec3_cross(ac, ab);
        CGL_vec4 normal = CGL_vec4_init(normal_3.x, normal_3.y, normal_3.z, 0.0f);
        mesh->vertices[mesh->indices[i + 0]].normal = CGL_vec4_add(mesh->vertices[mesh->indices[i + 0]].normal, normal);
        mesh->vertices[mesh->indices[i + 1]].normal = CGL_vec4_add(mesh->vertices[mesh->indices[i + 1]].normal, normal);
        mesh->vertices[mesh->indices[i + 2]].normal = CGL_vec4_add(mesh->vertices[mesh->indices[i + 2]].normal, normal);
    }
    // normalize normals
    for(size_t i = 0; i < mesh->vertex_count; i++)
    {
        CGL_vec4_normalize_vec3(mesh->vertices[i].normal);
    }  

    return mesh;
}

void __CGL_mesh_cpu_load_obj_helper_parse_obj_line(char* line, float* items, CGL_int count)
{
    char* begin = line;
    char* end = line + strlen(line);
    CGL_int i = 0, j = 0;
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
    CGL_int object_count = 0;
    CGL_float item_data[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
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
            CGL_int i = 0, j = 0, k = 0;
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
            for (CGL_int i = 0; i < 3; i++)
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
    mesh->vertex_count_used = mesh->vertex_count;
    mesh->index_count_used = mesh->index_count;
    return mesh;
}

// generate plane mesh
CGL_mesh_cpu* CGL_mesh_cpu_plane(CGL_vec3 front, CGL_vec3 right, CGL_int resolution, CGL_float scale)
{
    CGL_vec3_normalize(front);
    CGL_vec3_normalize(right);
    CGL_vec3 up = CGL_vec3_cross(front, right); // up is perpendicular to front and right
    CGL_mesh_cpu* mesh = CGL_mesh_cpu_create(resolution * resolution, (resolution - 1) * (resolution - 1) * 6);
    CGL_int index = 0, tri_index = 0;
    CGL_float temp0 = 0.0f, temp1 = 0.0f;
    CGL_vec3 tempv0, tempv1, point_on_plane;
    for(CGL_int y = 0; y < resolution; y++)
    {
        for (CGL_int x = 0; x < resolution; x++)
        {
            index = x + y * resolution;
            CGL_vec2 percent = CGL_vec2_init((float)x / ((float)resolution - 1), (float)y / ((float)resolution - 1));
            temp0 = (percent.x - 0.5f) * 2.0f;
            temp1 = (percent.y - 0.5f) * 2.0f;
            tempv0 = CGL_vec3_scale(right, temp0);
            tempv1 = CGL_vec3_scale(front, temp1);
            point_on_plane = CGL_vec3_add(tempv0, tempv1);
            point_on_plane = CGL_vec3_scale(point_on_plane, scale);
            mesh->vertices[index].position = CGL_vec4_init(point_on_plane.x, point_on_plane.y, point_on_plane.z, 1.0f);
            mesh->vertices[index].texture_coordinates = CGL_vec4_init(percent.x, percent.y, 0.0f, 0.0f);
            mesh->vertices[index].normal = CGL_vec4_init(up.x, up.y, up.z, 0.0f);
            if(x != resolution - 1 && y != resolution - 1)
            {
                mesh->indices[tri_index + 0] = index;
                mesh->indices[tri_index + 1] = index + resolution + 1;
                mesh->indices[tri_index + 2] = index + resolution;
                mesh->indices[tri_index + 3] = index;
                mesh->indices[tri_index + 4] = index + 1;
                mesh->indices[tri_index + 5] = index + resolution + 1;
                tri_index += 6;
            }
        }
    }
    mesh->vertex_count_used = mesh->vertex_count;
    mesh->index_count_used = mesh->index_count;
    return mesh;
}

// generate triangle mesh
CGL_mesh_cpu* CGL_mesh_cpu_triangle(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
{
    CGL_mesh_cpu* mesh = CGL_mesh_cpu_create(3, 3);
    return CGL_mesh_cpu_add_triangle(mesh, a, b, c);
}

// generate quad mesh
CGL_mesh_cpu* CGL_mesh_cpu_quad(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d)
{
    CGL_mesh_cpu* mesh = CGL_mesh_cpu_create(6, 6);
    return CGL_mesh_cpu_add_quad(mesh, a, b, c, d);
}

// Algorithm from https://stackoverflow.com/a/31326534/14911094
CGL_mesh_cpu* CGL_mesh_cpu_create_from_parametric_function(CGL_int res_u, CGL_int res_v, CGL_float start_u, CGL_float start_v, CGL_float end_u, CGL_float end_v, CGL_parametric_function function)
{
    CGL_mesh_cpu* mesh = CGL_mesh_cpu_create(res_u * res_v * 4, res_u * res_v * 6);
    return CGL_mesh_cpu_add_from_parametric_function(mesh, res_u, res_v, start_u, start_v, end_u, end_v, function);
}

CGL_mesh_cpu* CGL_mesh_cpu_add_triangle(CGL_mesh_cpu* mesh, CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
{
    if(mesh == NULL) return NULL;
    CGL_vec3 ab = CGL_vec3_sub(b, a);
    CGL_vec3 ac = CGL_vec3_sub(c, a);
    CGL_vec3 normal = CGL_vec3_cross(ab, ac);
    CGL_vec4 normal_4 = CGL_vec4_init(normal.x, normal.y, normal.z, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 0].position = CGL_vec4_init(a.x, a.y, a.z, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 0].normal = normal_4;
    mesh->vertices[mesh->vertex_count_used + 0].texture_coordinates = CGL_vec4_init(0.0f, 0.0f, 0.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 1].position = CGL_vec4_init(b.x, b.y, b.z, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 1].normal = normal_4;
    mesh->vertices[mesh->vertex_count_used + 1].texture_coordinates = CGL_vec4_init(0.0f, 0.0f, 0.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 2].position = CGL_vec4_init(c.x, c.y, c.z, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 2].normal = normal_4;
    mesh->vertices[mesh->vertex_count_used + 2].texture_coordinates = CGL_vec4_init(0.0f, 0.0f, 0.0f, 1.0f);
    mesh->vertex_count_used += 3;
    mesh->indices[mesh->index_count_used + 0] = 0;
    mesh->indices[mesh->index_count_used + 1] = 1;
    mesh->indices[mesh->index_count_used + 2] = 2;
    mesh->index_count_used += 3;
    return mesh;
}

CGL_mesh_cpu* CGL_mesh_cpu_add_quad(CGL_mesh_cpu* mesh, CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d)
{
    if(mesh == NULL) return NULL;
    CGL_vec3 ab = CGL_vec3_sub(b, a);
    CGL_vec3 ac = CGL_vec3_sub(c, a);
    // CGL_vec3 ad = CGL_vec3_sub(d, a);
    CGL_vec3 normal_abc = CGL_vec3_cross(ab, ac);
    CGL_vec4 normal_abc_4 = CGL_vec4_init(normal_abc.x, normal_abc.y, normal_abc.z, 0.0f);

    mesh->vertices[mesh->vertex_count_used + 0].position = CGL_vec4_init(a.x, a.y, a.z, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 0].normal = normal_abc_4;
    mesh->vertices[mesh->vertex_count_used + 0].texture_coordinates = CGL_vec4_init(1.0f, 1.0f, 0.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 1].position = CGL_vec4_init(b.x, b.y, b.z, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 1].normal = normal_abc_4;
    mesh->vertices[mesh->vertex_count_used + 1].texture_coordinates = CGL_vec4_init(1.0f, 0.0f, 0.0f, 1.0f);    
    mesh->vertices[mesh->vertex_count_used + 2].position = CGL_vec4_init(c.x, c.y, c.z, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 2].normal = normal_abc_4;
    mesh->vertices[mesh->vertex_count_used + 2].texture_coordinates = CGL_vec4_init(0.0f, 0.0f, 0.0f, 1.0f);    
    mesh->vertices[mesh->vertex_count_used + 3].position = CGL_vec4_init(a.x, a.y, a.z, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 3].normal = normal_abc_4;
    mesh->vertices[mesh->vertex_count_used + 3].texture_coordinates = CGL_vec4_init(1.0f, 1.0f, 0.0f, 1.0f);    
    mesh->vertices[mesh->vertex_count_used + 4].position = CGL_vec4_init(c.x, c.y, c.z, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 4].normal = normal_abc_4;
    mesh->vertices[mesh->vertex_count_used + 4].texture_coordinates = CGL_vec4_init(0.0f, 0.0f, 0.0f, 1.0f);    
    mesh->vertices[mesh->vertex_count_used + 5].position = CGL_vec4_init(d.x, d.y, d.z, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 5].normal = normal_abc_4;
    mesh->vertices[mesh->vertex_count_used + 5].texture_coordinates = CGL_vec4_init(0.0f, 1.0f, 0.0f, 1.0f);
    mesh->vertex_count_used += 6;
    for(CGL_int i = 0; i < 6; i++) mesh->indices[mesh->index_count_used + i] = (CGL_uint)mesh->index_count_used + (CGL_uint)i;
    mesh->index_count_used += 6;
    return mesh;
}

CGL_mesh_cpu* CGL_mesh_cpu_add_from_parametric_function(CGL_mesh_cpu* mesh, CGL_int res_u, CGL_int res_v, CGL_float start_u, CGL_float start_v, CGL_float end_u, CGL_float end_v, CGL_parametric_function function)
{
    if(mesh == NULL) return NULL;
    CGL_float step_u = (end_u - start_u) / res_u;
    CGL_float step_v = (end_v - start_v) / res_v;
    size_t vertex_index = 0;
    size_t index_index = 0;
    for(CGL_int i = 0 ; i < res_u ; i++)
    {
        for(CGL_int j = 0 ; j < res_v ; j++)
        {
            CGL_float u = i * step_u + start_u;
            CGL_float v = j * step_v + start_v;
            CGL_float un = ((i + 1) == res_u) ? end_u : ((i + 1)  * step_u + start_u);
            CGL_float vn = ((j + 1) == res_v) ? end_v : ((j + 1)  * step_v + start_v);
            // find the four points of the grid square
            // by evaluating the paramatric surface function
            CGL_vec3 p0 = function(u, v);
            CGL_vec3 p1 = function(u, vn);
            CGL_vec3 p2 = function(un, v);
            CGL_vec3 p3 = function(un, vn);
            mesh->vertices[mesh->vertex_count_used + vertex_index].position = CGL_vec4_init(p0.x, p0.y, p0.z, 1.0f);
            mesh->vertices[mesh->vertex_count_used + vertex_index].texture_coordinates = CGL_vec4_init(u, v, 0.0f, 0.0f);
            vertex_index += 1;
            mesh->vertices[mesh->vertex_count_used + vertex_index].position = CGL_vec4_init(p1.x, p1.y, p1.z, 1.0f);
            mesh->vertices[mesh->vertex_count_used + vertex_index].texture_coordinates = CGL_vec4_init(u, vn, 0.0f, 0.0f);
            vertex_index += 1;
            mesh->vertices[mesh->vertex_count_used + vertex_index].position = CGL_vec4_init(p2.x, p2.y, p2.z, 1.0f);
            mesh->vertices[mesh->vertex_count_used + vertex_index].texture_coordinates = CGL_vec4_init(un, v, 0.0f, 0.0f);
            vertex_index += 1;
            mesh->vertices[mesh->vertex_count_used + vertex_index].position = CGL_vec4_init(p3.x, p3.y, p3.z, 1.0f);
            mesh->vertices[mesh->vertex_count_used + vertex_index].texture_coordinates = CGL_vec4_init(un, vn, 0.0f, 0.0f);
            vertex_index += 1;
            // Output the first triangle of this grid square
            // triangle(p0, p2, p1)
            mesh->indices[mesh->index_count_used + index_index++] = (uint32_t)(mesh->vertex_count_used + vertex_index) - 4; // p0
            mesh->indices[mesh->index_count_used + index_index++] = (uint32_t)(mesh->vertex_count_used + vertex_index) - 2; // p2
            mesh->indices[mesh->index_count_used + index_index++] = (uint32_t)(mesh->vertex_count_used + vertex_index) - 3; // p1
            // Output the other triangle of this grid square
            // triangle(p3, p1, p2) 
            mesh->indices[mesh->index_count_used + index_index++] = (uint32_t)(mesh->vertex_count_used + vertex_index) - 1; // p3
            mesh->indices[mesh->index_count_used + index_index++] = (uint32_t)(mesh->vertex_count_used + vertex_index) - 3; // p1
            mesh->indices[mesh->index_count_used + index_index++] = (uint32_t)(mesh->vertex_count_used + vertex_index) - 2; // p2
        }
    }
    mesh->vertex_count_used += res_u * res_v * 4;
    mesh->index_count_used += res_u * res_v * 6;
    return mesh;
}


static CGL_vec3 __CGL_mesh_cpu_sphere_parametric_function(CGL_float u, CGL_float v)
{
    return CGL_vec3_init(cosf(u)*sinf(v), cosf(v), sinf(u)*sinf(v));
}

CGL_mesh_cpu* CGL_mesh_cpu_add_sphere(CGL_mesh_cpu* mesh, CGL_int res_u, CGL_int res_v)
{
    return CGL_mesh_cpu_add_from_parametric_function(mesh, res_u, res_v, 0.0f, 0.0f, 3.14f * 2.0f, 3.14f, __CGL_mesh_cpu_sphere_parametric_function);
}

CGL_mesh_cpu* CGL_mesh_cpu_create_cylinder(CGL_vec3 start, CGL_vec3 end, CGL_float radius0, CGL_float radius1, CGL_int resolution)
{
    CGL_mesh_cpu* mesh = CGL_mesh_cpu_create(resolution * 2 * 3, resolution * 2 * 3);
    CGL_mesh_cpu_add_cylinder(mesh, start, end, radius0, radius1, resolution);
    CGL_mesh_cpu_recalculate_normals(mesh);
    return mesh;
}

CGL_mesh_cpu* CGL_mesh_cpu_add_cylinder(CGL_mesh_cpu* mesh, CGL_vec3 start, CGL_vec3 end, CGL_float radius0, CGL_float radius1, CGL_int resolution)
{
    if(mesh == NULL) return NULL;
    if(resolution < 3) return NULL;
    
    CGL_vec3 direction = CGL_vec3_sub(end, start);
    CGL_float length = CGL_vec3_length(direction);
    CGL_vec3_normalize(direction);
    CGL_vec3 right = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    CGL_vec3 top = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    CGL_vec3_calculate_orthonormal_basis_from_one_vector(direction, &right, &top);
    CGL_float angle = 0.0f;
    CGL_float angle_step = 3.14f * 2.0f / (CGL_float)resolution;
    CGL_int vertex_index = 0;
    CGL_int index_index = 0;
    for(CGL_int i = 0; i < resolution ; i++)
    {
        CGL_vec3 p0 = CGL_vec3_add3_(start, CGL_vec3_scale_(right, cosf(angle) * radius0), CGL_vec3_scale_(top, sinf(angle) * radius0));
        CGL_vec3 p1 = CGL_vec3_add3_(start, CGL_vec3_scale_(right, cosf(angle + angle_step) * radius0), CGL_vec3_scale_(top, sinf(angle + angle_step) * radius0));
        CGL_vec3 p2 = CGL_vec3_add3_(end, CGL_vec3_scale_(right, cosf(angle) * radius1), CGL_vec3_scale_(top, sinf(angle) * radius1));
        CGL_vec3 p3 = CGL_vec3_add3_(end, CGL_vec3_scale_(right, cosf(angle + angle_step) * radius1), CGL_vec3_scale_(top, sinf(angle + angle_step) * radius1));
        angle += angle_step;
        // Output the first triangle of this grid square
        // triangle(p0, p2, p1)
        mesh->vertices[mesh->vertex_count_used + vertex_index].position = CGL_vec4_init(p0.x, p0.y, p0.z, 1.0f);
        mesh->vertices[mesh->vertex_count_used + vertex_index].texture_coordinates = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
        vertex_index += 1;
        mesh->vertices[mesh->vertex_count_used + vertex_index].position = CGL_vec4_init(p2.x, p2.y, p2.z, 1.0f);
        mesh->vertices[mesh->vertex_count_used + vertex_index].texture_coordinates = CGL_vec4_init(0.0f, 1.0f, 0.0f, 0.0f);
        vertex_index += 1;
        mesh->vertices[mesh->vertex_count_used + vertex_index].position = CGL_vec4_init(p1.x, p1.y, p1.z, 1.0f);
        mesh->vertices[mesh->vertex_count_used + vertex_index].texture_coordinates = CGL_vec4_init(1.0f, 0.0f, 0.0f, 0.0f);
        vertex_index += 1;
        // Output the second triangle of this grid square
        // triangle(p1, p2, p3)
        mesh->vertices[mesh->vertex_count_used + vertex_index].position = CGL_vec4_init(p1.x, p1.y, p1.z, 1.0f);
        mesh->vertices[mesh->vertex_count_used + vertex_index].texture_coordinates = CGL_vec4_init(1.0f, 0.0f, 0.0f, 0.0f);
        vertex_index += 1;
        mesh->vertices[mesh->vertex_count_used + vertex_index].position = CGL_vec4_init(p2.x, p2.y, p2.z, 1.0f);
        mesh->vertices[mesh->vertex_count_used + vertex_index].texture_coordinates = CGL_vec4_init(0.0f, 1.0f, 0.0f, 0.0f);
        vertex_index += 1;
        mesh->vertices[mesh->vertex_count_used + vertex_index].position = CGL_vec4_init(p3.x, p3.y, p3.z, 1.0f);
        mesh->vertices[mesh->vertex_count_used + vertex_index].texture_coordinates = CGL_vec4_init(1.0f, 1.0f, 0.0f, 0.0f);
        vertex_index += 1;
        // Output the indices for this grid square
        mesh->indices[mesh->index_count_used + index_index] = (CGL_uint)mesh->vertex_count_used + vertex_index - 6; index_index += 1;
        mesh->indices[mesh->index_count_used + index_index] = (CGL_uint)mesh->vertex_count_used + vertex_index - 5; index_index += 1;
        mesh->indices[mesh->index_count_used + index_index] = (CGL_uint)mesh->vertex_count_used + vertex_index - 4; index_index += 1;
        mesh->indices[mesh->index_count_used + index_index] = (CGL_uint)mesh->vertex_count_used + vertex_index - 3; index_index += 1;
        mesh->indices[mesh->index_count_used + index_index] = (CGL_uint)mesh->vertex_count_used + vertex_index - 2; index_index += 1;
        mesh->indices[mesh->index_count_used + index_index] = (CGL_uint)mesh->vertex_count_used + vertex_index - 1; index_index += 1;
    }
    mesh->vertex_count_used += vertex_index;
    mesh->index_count_used += index_index;
    return mesh;
}

CGL_mesh_cpu* CGL_mesh_cpu_offset_vertices(CGL_mesh_cpu* mesh, CGL_vec3 offset)
{
    for(CGL_int i = 0; i < mesh->vertex_count_used; i++)
    {
        mesh->vertices[i].position.x += offset.x;
        mesh->vertices[i].position.y += offset.y;
        mesh->vertices[i].position.z += offset.z;
        mesh->vertices[i].position.w = 1.0f;
    }
    return mesh;
}

CGL_mesh_cpu* CGL_mesh_cpu_scale_vertices(CGL_mesh_cpu* mesh, CGL_float scale)
{
    for(CGL_int i = 0; i < mesh->vertex_count_used; i++)
    {
        mesh->vertices[i].position.x *= scale;
        mesh->vertices[i].position.y *= scale;
        mesh->vertices[i].position.z *= scale;
        mesh->vertices[i].position.w = 1.0f;
    }
    return mesh;
}

CGL_mesh_cpu* CGL_mesh_cpu_rotate_vertices(CGL_mesh_cpu* mesh, CGL_quat rotation)
{
    for(CGL_int i = 0; i < mesh->vertex_count_used; i++)
    {
        mesh->vertices[i].position.w = 1.0f;
        mesh->vertices[i].position = CGL_quat_mul_vec4(rotation, mesh->vertices[i].position);
    }
    return mesh;
}

CGL_mesh_cpu* CGL_mesh_cpu_transform_vertices(CGL_mesh_cpu* mesh, CGL_mat4 transform)
{
    for(CGL_int i = 0; i < mesh->vertex_count_used; i++)
    {
        mesh->vertices[i].position.w = 1.0f;
        mesh->vertices[i].position = CGL_mat4_mul_vec4(transform, mesh->vertices[i].position);
    }
    return mesh;
}

CGL_mesh_cpu* CGL_mesh_cpu_sphere(CGL_int res_u, CGL_int res_v)
{
    return CGL_mesh_cpu_create_from_parametric_function(res_u, res_v, 0.0f, 0.0f, 3.14f * 2.0f, 3.14f, __CGL_mesh_cpu_sphere_parametric_function);
}

CGL_mesh_cpu* CGL_mesh_cpu_add_mesh(CGL_mesh_cpu* mesh, CGL_mesh_cpu* mesh_other)
{
    for(CGL_int i = 0; i < mesh_other->vertex_count_used; i++) mesh->vertices[mesh->vertex_count_used + i] = mesh_other->vertices[i];
    for(CGL_int i = 0; i < mesh_other->index_count_used; i++) mesh->indices[mesh->index_count_used + i] = mesh_other->indices[i] + (CGL_uint)mesh->vertex_count_used;
    mesh->vertex_count_used += mesh_other->vertex_count_used; mesh->index_count_used += mesh_other->index_count_used;
    return mesh;
}

CGL_mesh_cpu* CGL_mesh_cpu_add_cube(CGL_mesh_cpu* mesh, CGL_bool use_3d_tex_coords)
{
    mesh->vertices[mesh->vertex_count_used + 0].position = CGL_vec4_init(-1.0f,  1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 0].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f,  1.0f, -1.0f, 0.0f) : CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 0].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 1].position = CGL_vec4_init(-1.0f, -1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 1].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f, -1.0f, -1.0f, 0.0f) : CGL_vec4_init(1.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 1].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 2].position = CGL_vec4_init( 1.0f, -1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 2].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f, -1.0f, -1.0f, 0.0f) : CGL_vec4_init(1.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 2].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 3].position = CGL_vec4_init( 1.0f, -1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 3].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f, -1.0f, -1.0f, 0.0f) : CGL_vec4_init(1.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 3].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 4].position = CGL_vec4_init( 1.0f,  1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 4].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f,  1.0f, -1.0f, 0.0f) : CGL_vec4_init(0.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 4].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 5].position = CGL_vec4_init(-1.0f,  1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 5].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f,  1.0f, -1.0f, 0.0f) : CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 5].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 6].position = CGL_vec4_init(-1.0f, -1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 6].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f, -1.0f,  1.0f, 0.0f) : CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 6].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 7].position = CGL_vec4_init(-1.0f, -1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 7].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f, -1.0f, -1.0f, 0.0f) : CGL_vec4_init(1.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 7].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 8].position = CGL_vec4_init(-1.0f,  1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 8].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f,  1.0f, -1.0f, 0.0f) : CGL_vec4_init(1.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 8].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 9].position = CGL_vec4_init(-1.0f,  1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 9].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f,  1.0f, -1.0f, 0.0f) : CGL_vec4_init(1.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 9].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 10].position = CGL_vec4_init(-1.0f,  1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 10].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f,  1.0f,  1.0f, 0.0f) : CGL_vec4_init(0.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 10].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 11].position = CGL_vec4_init(-1.0f, -1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 11].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f, -1.0f,  1.0f, 0.0f) : CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 11].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 12].position = CGL_vec4_init( 1.0f, -1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 12].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f, -1.0f, -1.0f, 0.0f) : CGL_vec4_init(1.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 12].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 13].position = CGL_vec4_init( 1.0f, -1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 13].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f, -1.0f,  1.0f, 0.0f) : CGL_vec4_init(1.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 13].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 14].position = CGL_vec4_init( 1.0f,  1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 14].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f,  1.0f,  1.0f, 0.0f) : CGL_vec4_init(0.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 14].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 15].position = CGL_vec4_init( 1.0f,  1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 15].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f,  1.0f,  1.0f, 0.0f) : CGL_vec4_init(0.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 15].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 16].position = CGL_vec4_init( 1.0f,  1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 16].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f,  1.0f, -1.0f, 0.0f) : CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 16].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 17].position = CGL_vec4_init( 1.0f, -1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 17].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f, -1.0f, -1.0f, 0.0f) : CGL_vec4_init(1.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 17].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 18].position = CGL_vec4_init(-1.0f, -1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 18].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f, -1.0f,  1.0f, 0.0f) : CGL_vec4_init(1.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 18].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 19].position = CGL_vec4_init(-1.0f,  1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 19].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f,  1.0f,  1.0f, 0.0f) : CGL_vec4_init(1.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 19].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 20].position = CGL_vec4_init( 1.0f,  1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 20].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f,  1.0f,  1.0f, 0.0f) : CGL_vec4_init(0.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 20].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 21].position = CGL_vec4_init( 1.0f,  1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 21].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f,  1.0f,  1.0f, 0.0f) : CGL_vec4_init(0.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 21].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 22].position = CGL_vec4_init( 1.0f, -1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 22].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f, -1.0f,  1.0f, 0.0f) : CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 22].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 23].position = CGL_vec4_init(-1.0f, -1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 23].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f, -1.0f,  1.0f, 0.0f) : CGL_vec4_init(1.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 23].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 24].position = CGL_vec4_init(-1.0f,  1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 24].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f,  1.0f, -1.0f, 0.0f) : CGL_vec4_init(0.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 24].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 25].position = CGL_vec4_init( 1.0f,  1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 25].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f,  1.0f, -1.0f, 0.0f) : CGL_vec4_init(1.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 25].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 26].position = CGL_vec4_init( 1.0f,  1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 26].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f,  1.0f,  1.0f, 0.0f) : CGL_vec4_init(1.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 26].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 27].position = CGL_vec4_init( 1.0f,  1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 27].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f,  1.0f,  1.0f, 0.0f) : CGL_vec4_init(1.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 27].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 28].position = CGL_vec4_init(-1.0f,  1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 28].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f,  1.0f,  1.0f, 0.0f) : CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 28].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 29].position = CGL_vec4_init(-1.0f,  1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 29].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f,  1.0f, -1.0f, 0.0f) : CGL_vec4_init(0.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 29].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 30].position = CGL_vec4_init(-1.0f, -1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 30].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f, -1.0f, -1.0f, 0.0f) : CGL_vec4_init(0.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 30].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 31].position = CGL_vec4_init(-1.0f, -1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 31].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f, -1.0f,  1.0f, 0.0f) : CGL_vec4_init(1.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 31].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 32].position = CGL_vec4_init( 1.0f, -1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 32].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f, -1.0f, -1.0f, 0.0f) : CGL_vec4_init(1.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 32].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 33].position = CGL_vec4_init( 1.0f, -1.0f, -1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 33].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f, -1.0f, -1.0f, 0.0f) : CGL_vec4_init(1.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 33].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 34].position = CGL_vec4_init(-1.0f, -1.0f,  1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 34].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init(-1.0f, -1.0f,  1.0f, 0.0f) : CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 34].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    mesh->vertices[mesh->vertex_count_used + 35].position = CGL_vec4_init( 1.0f, -1.0f, 1.0f, 1.0f);
    mesh->vertices[mesh->vertex_count_used + 35].texture_coordinates = (use_3d_tex_coords ? CGL_vec4_init( 1.0f, -1.0f,  1.0f, 0.0f) : CGL_vec4_init(0.0f, 1.0f, 0.0f, 0.0f));
    mesh->vertices[mesh->vertex_count_used + 35].normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    for(CGL_int i = (CGL_int)mesh->index_count_used ; i < mesh->index_count_used + 36 ; i++) mesh->indices[i] = i;
    mesh->vertex_count_used += 36;
    mesh->index_count_used += 36;
    return mesh;
}

CGL_mesh_cpu* CGL_mesh_cpu_cube(bool use_3d_tex_coords)
{
    CGL_mesh_cpu* mesh = CGL_mesh_cpu_create(36, 36);
    if(mesh == NULL) return NULL;
    return CGL_mesh_cpu_add_cube(mesh, use_3d_tex_coords);
}

void CGL_mesh_cpu_generate_c_initialization_code(CGL_mesh_cpu* mesh, char* buffer, const char* function_name)
{
    static char temp_buffer[1024];
    buffer[0] = '\0';
    if(!function_name) function_name = "generate_mesh";
    sprintf(temp_buffer,
    "CGL_mesh_cpu* %s()\n{\n"
    "\tCGL_mesh_cpu* mesh = CGL_mesh_cpu_create(%zu, %zu);\n"
    "\tif(mesh == NULL)\n"
    "\t\treturn NULL;\n\n",
    function_name,
    mesh->vertex_count, mesh->index_count);
    strcat(buffer, temp_buffer);
    for(CGL_int i = 0 ; i < mesh->vertex_count ; i++)
    {
        sprintf(temp_buffer,
                "\tmesh->vertices[%d].position = CGL_vec4_init(%f, %f, %f, %f);\n"
                "\tmesh->vertices[%d].normal =  CGL_vec4_init(%f, %f, %f, %f);\n"
                "\tmesh->vertices[%d].texture_coordinates = CGL_vec4_init(%f, %f, %f, %f);\n"
                "\tmesh->vertices[%d].bone_wieghts = CGL_vec4_init(%f, %f, %f, %f);\n"
                "\tmesh->vertices[%d].bone_ids = CGL_ivec4_init(%d, %d, %d, %d);\n",
                i, mesh->vertices[i].position.x, mesh->vertices[i].position.y, mesh->vertices[i].position.z, mesh->vertices[i].position.w,
                i, mesh->vertices[i].normal.x, mesh->vertices[i].normal.y, mesh->vertices[i].normal.z, mesh->vertices[i].normal.w,
                i, mesh->vertices[i].texture_coordinates.x, mesh->vertices[i].texture_coordinates.y, mesh->vertices[i].texture_coordinates.z, mesh->vertices[i].texture_coordinates.w,
                i, mesh->vertices[i].bone_wieghts.x, mesh->vertices[i].bone_wieghts.y, mesh->vertices[i].bone_wieghts.z, mesh->vertices[i].bone_wieghts.w,
                i, mesh->vertices[i].bone_ids.x, mesh->vertices[i].bone_ids.y, mesh->vertices[i].bone_ids.z, mesh->vertices[i].bone_ids.w
            );
        strcat(buffer, temp_buffer);
    }
    strcat(buffer, "\n\tmesh->indices = (CGL_int []){ ");
    for(CGL_int i = 0 ; i < mesh->index_count ; i++)
    {
        sprintf(temp_buffer,  ((i == mesh->index_count - 1) ? "%d " : "%d, "), mesh->indices[i]);
        strcat(buffer, temp_buffer);
    }
    strcat(buffer, "};\n\n\treturn mesh;");    
}

// shader

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
        CGL_log_internal("%s\n", log);
        if(error) *error = log;
        else CGL_free(log);
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
        CGL_free(shader);
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
        CGL_log_internal("%s\n", log);
        if(error) *error = log;
        else CGL_free(log);
        glDeleteProgram(program);
        glDeleteShader(compute_shader);
        CGL_free(shader);
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
        CGL_log_internal("Failed to read compute shader file %s\n", compute_shader_file);
        return NULL;
    }
    CGL_shader* shader = CGL_shader_compute_create(compute_shader_source, error);
    CGL_free(compute_shader_source);
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
        CGL_free(shader);
        return NULL;
    }

    if(!__CGL_shader_compile(fragment_shader_source, GL_FRAGMENT_SHADER, &fragment_shader, error))
    {
        glDeleteShader(vertex_shader);
        CGL_free(shader);
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
        CGL_log_internal("%s\n", log);
        if(error)  *error = log;
        else CGL_free(log);
        glDeleteProgram(program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        CGL_free(shader);
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
        CGL_log_internal("Failed to read vertex shader file %s\n", vertex_shader_file);
        return NULL;
    }
    char* fragment_shader_source = CGL_utils_read_file(fragment_shader_file, NULL);
    if(fragment_shader_source == NULL)
    {
        CGL_log_internal("Failed to read fragment shader file %s\n", fragment_shader_file);
        CGL_free(vertex_shader_source);
        return NULL;
    }
    CGL_shader* shader = CGL_shader_create(vertex_shader_source, fragment_shader_source, error);
    CGL_free(vertex_shader_source);
    CGL_free(fragment_shader_source);
    return shader;
}

// destroy shader
void CGL_shader_destroy(CGL_shader* shader)
{
    glDeleteProgram(shader->handle);
    CGL_free(shader);
}

// dispatch compute shader
void CGL_shader_compute_dispatch(CGL_shader* shader, CGL_int x, CGL_int y, CGL_int z)
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
void CGL_shader_set_uniform_bool(CGL_shader* shader, CGL_int location, bool value)
{
    glUniform1i(location, value);
}

// set uniform matrix
void CGL_shader_set_uniform_mat4(CGL_shader* shader, CGL_int location, CGL_mat4* matrix)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat*)matrix->m);
    //glUniformMatrix4fv(location, 1, GL_TRUE, (GLfloat*)matrix);
}

// set uniform vector
void CGL_shader_set_uniform_vec4(CGL_shader* shader, CGL_int location, CGL_vec4* vector)
{
    glUniform4fv(location, 1, (GLfloat*)vector);
}

// set uniform vector
void CGL_shader_set_uniform_vec3(CGL_shader* shader, CGL_int location, CGL_vec3* vector)
{
    glUniform3fv(location, 1, (GLfloat*)vector);
}

// set uniform vector
void CGL_shader_set_uniform_vec2(CGL_shader* shader, CGL_int location, CGL_vec2* vector)
{
    glUniform2fv(location, 1, (GLfloat*)vector);
}

// set uniform int
void CGL_shader_set_uniform_int(CGL_shader* shader, CGL_int location, CGL_int value)
{
    glUniform1i(location, value);
}

// set uniform float
void CGL_shader_set_uniform_float(CGL_shader* shader, CGL_int location, CGL_float value)
{
    glUniform1f(location, value);
}

// set uniform vector
void CGL_shader_set_uniform_vec2v(CGL_shader* shader, CGL_int location, CGL_float x, CGL_float y)
{
    glUniform2f(location, x, y);
}

// set uniform vector
void CGL_shader_set_uniform_vec3v(CGL_shader* shader, CGL_int location, CGL_float x, CGL_float y, CGL_float z)
{
    glUniform3f(location, x, y, z);
}

// set uniform vector
void CGL_shader_set_uniform_vec4v(CGL_shader* shader, CGL_int location, CGL_float x, CGL_float y, CGL_float z, CGL_float w)
{
    glUniform4f(location, x, y, z, w);
}

void CGL_shader_set_uniform_ivec2v(CGL_shader* shader, CGL_int location, CGL_int x, CGL_int y)
{
    glUniform2i(location, x, y);
}

// set uniform vector
void CGL_shader_set_uniform_ivec3v(CGL_shader* shader, CGL_int location, CGL_int x, CGL_int y, CGL_int z)
{
    glUniform3i(location, x, y, z);
}

// set uniform vector
void CGL_shader_set_uniform_ivec4v(CGL_shader* shader, CGL_int location, CGL_int x, CGL_int y, CGL_int z, CGL_int w)
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


// BLOOM
#ifndef CGL_EXCLUDE_BLOOM

struct CGL_bloom
{
    CGL_texture* tex_lod[CGL_BLOOM_MAX_ITERATIONS * 2];    
    CGL_texture* prefiltered;
    CGL_shader* compute;
    CGL_float offset_x;
    CGL_float offset_y;
    CGL_int width;
    CGL_int height;
    CGL_int iterations;
    CGL_float threshold;
    CGL_float knee;
    CGL_int cs_u_mode;
    CGL_int cs_u_src_size;
    CGL_int cs_u_dst_size;
    CGL_int cs_u_prefilter_threshold;
    CGL_int cs_u_offset;
};

const char* __CGL_BLOOM_SHADER_SOURCE = "#version 430 core\n"
"layout(local_size_x = 16, local_size_y = 16) in;\n"
"layout(rgba32f, binding = 0) uniform image2D tex_src;\n"
"layout(rgba32f, binding = 1) uniform image2D tex_dst;\n"
"\n"
"#define MODE_PREFILTER      0\n"
"#define MODE_DOWNSAMPLE     1\n"
"#define MODE_UPSAMPLE       2\n"
"#define MODE_COMPOSITE      3\n"
"\n"
"uniform int u_mode;\n"
"uniform vec2 u_src_size;\n"
"uniform ivec2 u_offset;\n"
"uniform vec2 u_dst_size;\n"
"uniform vec4 u_prefilter_threshold;\n"
"\n"
"vec4 prefilter(vec4 color)\n"
"{\n"
"    // pixel brightness\n"
"    float br = max(color.x, max(color.y, color.z));\n"
"\n"
"    // under-threshold part : quadratic curve\n"
"    vec3 curve = u_prefilter_threshold.yzw; // curve = (threshold - knee, knee * 2, 0.25 / knee)\n"
"    float rq = clamp(br - curve.x, 0.0f, curve.y);\n"
"    rq = curve.z * rq * rq;\n"
"\n"
"    //  combine and apply the brightness response curve\n"
"    float threshold = u_prefilter_threshold.x;\n"
"    color *= max(rq, br - threshold) / max(br, 0.00001f);\n"
"    //color *= (br - threshold) / max(br, 0.00001f);\n"
"\n"
"    return color;\n"
"}\n"
"\n"
"vec4 downsample_box_13_tap(ivec2 src_coord)\n"
"{\n"
"    vec4 A = imageLoad(tex_src, src_coord + ivec2(-2, -2));\n"
"    vec4 B = imageLoad(tex_src, src_coord + ivec2( 0, -2));\n"
"    vec4 C = imageLoad(tex_src, src_coord + ivec2( 2, -2));\n"
"    vec4 D = imageLoad(tex_src, src_coord + ivec2(-1, -1));\n"
"    vec4 E = imageLoad(tex_src, src_coord + ivec2( 1, -1));\n"
"    vec4 F = imageLoad(tex_src, src_coord + ivec2(-2,  0));\n"
"    vec4 G = imageLoad(tex_src, src_coord                );\n"
"    vec4 H = imageLoad(tex_src, src_coord + ivec2( 2,  0));\n"
"    vec4 I = imageLoad(tex_src, src_coord + ivec2(-1,  1));\n"
"    vec4 J = imageLoad(tex_src, src_coord + ivec2( 1,  1));\n"
"    vec4 K = imageLoad(tex_src, src_coord + ivec2(-2,  2));\n"
"    vec4 L = imageLoad(tex_src, src_coord + ivec2( 0,  2));\n"
"    vec4 M = imageLoad(tex_src, src_coord + ivec2( 2,  2));\n"
"\n"
"    vec2 div = (1.0f / 4.0f) * vec2(0.5f, 0.125f);\n"
"\n"
"    vec4 o = (D + E + I + J) * div.x;\n"
"    o += (A + B + G + F) * div.y;\n"
"    o += (B + C + H + G) * div.y;\n"
"    o += (F + G + L + K) * div.y;\n"
"    o += (G + H + M + L) * div.y;\n"
"\n"
"        //pixel = imageLoad(tex_src, pixel_coords);\n"
"    return o;\n"
"}\n"
"\n"
"vec4 upsample_tent(ivec2 src_coord)\n"
"{\n"
"    ivec4 d = ivec4(1, 1, -1, 0);\n"
"\n"
"    vec4 s;\n"
"    s =  imageLoad(tex_src, src_coord - d.xy);\n"
"    s += imageLoad(tex_src, src_coord - d.wy) * 2.0f;\n"
"    s += imageLoad(tex_src, src_coord - d.zy);\n"
"    s += imageLoad(tex_src, src_coord + d.zw) * 2.0f;\n"
"    s += imageLoad(tex_src, src_coord       ) * 4.0f;\n"
"    s += imageLoad(tex_src, src_coord + d.xw) * 2.0f;\n"
"    s += imageLoad(tex_src, src_coord + d.zy);\n"
"    s += imageLoad(tex_src, src_coord + d.wy) * 2.0f;\n"
"    s += imageLoad(tex_src, src_coord + d.xy);\n"
"\n"
"    return s * (1.0f / 16.0f);\n"
"}\n"
"\n"
"vec3 aces_tonemap(vec3 x){	\n"
"	const float a = 2.51f;\n"
"    const float b = 0.03f;\n"
"    const float c = 2.43f;\n"
"    const float d = 0.59f;\n"
"    const float e = 0.14f;\n"
"    return clamp((x * (a * x + b)) / (x * (c * x + d ) + e), 0.0f, 1.0f);\n"
"}\n"
"\n"
"void main() {\n"
"    if(u_mode == MODE_PREFILTER)\n"
"    {\n"
"        vec4 pixel = vec4(1.0f, 1.0f, 0.0f, 1.0f);    \n"
"        ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);    \n"
"        pixel = imageLoad(tex_src, pixel_coords);\n"
"        imageStore(tex_dst, pixel_coords, prefilter(pixel));\n"
"    }\n"
"    else if(u_mode == MODE_DOWNSAMPLE)\n"
"    {\n"
"        vec4 pixel = vec4(1.0f, 0.0f, 1.0f, 1.0f);    \n"
"        ivec2 dst_pixel_coords = ivec2(gl_GlobalInvocationID.xy);    \n"
"        vec2 uv = vec2(dst_pixel_coords) / u_dst_size;\n"
"        ivec2 src_pixel_coords = ivec2(uv * u_src_size);\n"
"        imageStore(tex_dst, dst_pixel_coords, downsample_box_13_tap(src_pixel_coords));\n"
"    }\n"
"    else if(u_mode == MODE_UPSAMPLE)\n"
"    {\n"
"        vec4 pixel = vec4(1.0f, 0.0f, 1.0f, 1.0f);    \n"
"        ivec2 dst_pixel_coords = ivec2(gl_GlobalInvocationID.xy);    \n"
"        vec2 uv = vec2(dst_pixel_coords) / u_dst_size;\n"
"        ivec2 src_pixel_coords = ivec2(uv * u_src_size);\n"
"        imageStore(tex_dst, dst_pixel_coords, upsample_tent(src_pixel_coords));\n"
"    }\n"
"    else if(u_mode == MODE_COMPOSITE)\n"
"    {\n"
"        vec4 pixel = vec4(0.0f, 0.0f, 0.0f, 0.0f);    \n"
"        ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);\n"
"        ivec2 pixel_coords2 = ivec2(gl_GlobalInvocationID.xy) + u_offset;\n"
"        pixel += imageLoad(tex_dst, pixel_coords); \n"
"        pixel += imageLoad(tex_src, pixel_coords2); \n"
"        pixel = vec4(pixel.xyz, clamp(pixel.w, 0.0f, 1.0f));\n"
"        imageStore(tex_dst, pixel_coords, pixel); \n"
"    }\n"
"    else\n"
"    {\n"
"        vec4 pixel = vec4(1.0f, 0.0f, 0.0f, 1.0f);    \n"
"        ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);\n"
"        imageStore(tex_dst, pixel_coords, pixel);\n"
"    }\n"
"}";

CGL_bloom* CGL_bloom_create(CGL_int width, CGL_int height, CGL_int iterations)
{
    CGL_bloom* bloom = (CGL_bloom*)CGL_malloc(sizeof(CGL_bloom));
    if(bloom == NULL) return NULL;
    bloom->threshold = 0.5f;
    bloom->knee = 0.5f;
    bloom->iterations = 0;
    bloom->width = width;
    bloom->height = height;
    bloom->offset_x = 0.0f;
    bloom->offset_y = 0.0f;
    iterations = CGL_utils_min(CGL_utils_max(1, iterations), CGL_BLOOM_MAX_ITERATIONS);
    bloom->prefiltered = CGL_texture_create_blank(width, height, GL_RGBA, GL_RGBA32F, GL_FLOAT);
    CGL_int i = 0;
    for(i = 0 ; i < iterations ; i++)
    {
        width /= 2; height /= 2;
        if(width < 10 || height < 10)
        {
            width *= 2; height *= 2;
            iterations = i;
            break;
        }
        bloom->tex_lod[i] = CGL_texture_create_blank(width, height, GL_RGBA, GL_RGBA32F, GL_FLOAT);
    }
    for(; i < 2 * iterations ; i++)
    {
        width *= 2; height *= 2;
        if(i == 2 * iterations - 1) {width = bloom->width; height = bloom->height;}
        bloom->tex_lod[i] = CGL_texture_create_blank(width, height, GL_RGBA, GL_RGBA32F, GL_FLOAT);
    }
    bloom->iterations = iterations;
    bloom->compute = CGL_shader_compute_create(__CGL_BLOOM_SHADER_SOURCE, NULL);
    bloom->cs_u_mode = CGL_shader_get_uniform_location(bloom->compute, "u_mode");
    bloom->cs_u_src_size = CGL_shader_get_uniform_location(bloom->compute, "u_src_size");
    bloom->cs_u_dst_size = CGL_shader_get_uniform_location(bloom->compute, "u_dst_size");
    bloom->cs_u_prefilter_threshold = CGL_shader_get_uniform_location(bloom->compute, "u_prefilter_threshold");
    bloom->cs_u_offset = CGL_shader_get_uniform_location(bloom->compute, "u_offset");
    return bloom;
}

void CGL_bloom_destroy(CGL_bloom* bloom)
{
    CGL_texture_destroy(bloom->prefiltered);
    for(CGL_int i = 0 ; i < bloom->iterations * 2 ; i++)
        CGL_texture_destroy(bloom->tex_lod[i]);
    CGL_shader_destroy(bloom->compute);
    CGL_free(bloom);
}

void CGL_bloom_apply2(CGL_bloom* bloom, CGL_texture* tex_src, CGL_texture* tex_dst)
{
    CGL_shader_bind(bloom->compute);

    // prefilter
    glBindImageTexture(0, tex_src->handle, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, bloom->prefiltered->handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    CGL_shader_set_uniform_int(bloom->compute, bloom->cs_u_mode, CGL_BLOOM_SHADER_MODE_PREFILTER);
    CGL_shader_set_uniform_vec2v(bloom->compute, bloom->cs_u_src_size, (float)bloom->width, (float)bloom->height);
    CGL_shader_set_uniform_vec2v(bloom->compute, bloom->cs_u_dst_size, (float)bloom->width, (float)bloom->height);
    CGL_shader_set_uniform_vec4v(bloom->compute, bloom->cs_u_prefilter_threshold, bloom->threshold, bloom->threshold - bloom->knee, bloom->knee * 2.0f, 0.25f / bloom->knee);
    CGL_shader_compute_dispatch(bloom->compute, bloom->width/16 + 1, bloom->height / 16 + 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    // downsample
    glBindImageTexture(0, bloom->prefiltered->handle, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, bloom->tex_lod[0]->handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    CGL_shader_set_uniform_int(bloom->compute, bloom->cs_u_mode, CGL_BLOOM_SHADER_MODE_DOWNSAMPLE);
    CGL_shader_set_uniform_vec2v(bloom->compute, bloom->cs_u_src_size, (float)bloom->width, (float)bloom->height);
    CGL_shader_set_uniform_vec2v(bloom->compute, bloom->cs_u_dst_size, (float)bloom->tex_lod[0]->width, (float)bloom->tex_lod[0]->height);
    CGL_shader_compute_dispatch(bloom->compute, bloom->tex_lod[0]->width / 16 + 1, bloom->tex_lod[0]->height / 16 + 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    CGL_int i = 0;
    for(i = 0 ; i < bloom->iterations - 1 ; i++)
    {
        glBindImageTexture(0, bloom->tex_lod[i]->handle, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, bloom->tex_lod[i + 1]->handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        CGL_shader_set_uniform_int(bloom->compute, bloom->cs_u_mode, CGL_BLOOM_SHADER_MODE_DOWNSAMPLE);
        CGL_shader_set_uniform_vec2v(bloom->compute, bloom->cs_u_src_size, (float)bloom->tex_lod[i]->width, (float)bloom->tex_lod[i]->height);
        CGL_shader_set_uniform_vec2v(bloom->compute, bloom->cs_u_dst_size, (float)bloom->tex_lod[i + 1]->width, (float)bloom->tex_lod[i + 1]->height);
        CGL_shader_compute_dispatch(bloom->compute, bloom->tex_lod[i + 1]->width / 16 + 1, bloom->tex_lod[i + 1]->height / 16 + 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    // upsample
    for(; i < 2 * bloom->iterations - 1 ; i++)
    {
        glBindImageTexture(0, bloom->tex_lod[i]->handle, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
        glBindImageTexture(1, bloom->tex_lod[i + 1]->handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        CGL_shader_set_uniform_int(bloom->compute, bloom->cs_u_mode, CGL_BLOOM_SHADER_MODE_UPSAMPLE);
        CGL_shader_set_uniform_vec2v(bloom->compute, bloom->cs_u_src_size, (float)bloom->tex_lod[i]->width, (float)bloom->tex_lod[i]->height);
        CGL_shader_set_uniform_vec2v(bloom->compute, bloom->cs_u_dst_size, (float)bloom->tex_lod[i + 1]->width, (float)bloom->tex_lod[i + 1]->height);
        CGL_shader_compute_dispatch(bloom->compute, bloom->tex_lod[i + 1]->width / 16 + 1, bloom->tex_lod[i + 1]->height / 16 + 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    // composite
    glBindImageTexture(0, bloom->tex_lod[bloom->iterations * 2 - 1]->handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindImageTexture(1, tex_dst->handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    CGL_shader_set_uniform_int(bloom->compute, bloom->cs_u_mode, CGL_BLOOM_SHADER_MODE_COMPOSITE);
    CGL_shader_set_uniform_vec2v(bloom->compute, bloom->cs_u_src_size, (float)bloom->width, (float)bloom->height);
    CGL_shader_set_uniform_vec2v(bloom->compute, bloom->cs_u_dst_size, (float)bloom->width, (float)bloom->height);
    CGL_shader_set_uniform_ivec2v(bloom->compute, bloom->cs_u_offset, (int)bloom->offset_x, (int)bloom->offset_y);
    CGL_shader_compute_dispatch(bloom->compute, bloom->width / 16 + 1, bloom->height / 16 + 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void CGL_bloom_apply(CGL_bloom* bloom, CGL_texture* tex)
{
    CGL_bloom_apply2(bloom, tex, tex);
}

CGL_texture* CGL_bloom_get_lod_texture(CGL_bloom* bloom, CGL_int index)
{
    return bloom->tex_lod[index];
}

CGL_texture* CGL_bloom_get_prefiltered_texture(CGL_bloom* bloom)
{
    return bloom->prefiltered;
}

void CGL_bloom_set_knee(CGL_bloom* bloom, CGL_float val)
{
    bloom->knee = val;
}

float CGL_bloom_get_knee(CGL_bloom* bloom)
{
    return bloom->knee;
}

void CGL_bloom_set_threshold(CGL_bloom* bloom, CGL_float val)
{
    bloom->threshold = val;
}

float CGL_bloom_get_threshold(CGL_bloom* bloom)
{
    return bloom->threshold;
}

int CGL_bloom_get_iterations(CGL_bloom* bloom)
{
    return bloom->iterations;
}

void CGL_bloom_set_offset(CGL_bloom* bloom, CGL_float x, CGL_float y)
{
    bloom->offset_x = x;
    bloom->offset_y = y;
}

#endif

#endif

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
    CGL_float fov;
    CGL_float aspect;
    CGL_float z_near;
    CGL_float z_far;
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

void CGL_camera_set_fov(CGL_camera* camera, CGL_float fov)
{
    camera->fov = fov;
}

void CGL_camera_set_aspect_ratio(CGL_camera* camera, CGL_float aspect_ratio)
{
    if(fabs(aspect_ratio) < 0.001f || fabs(aspect_ratio) > 100.0f )aspect_ratio = 1.0f;
    camera->aspect = aspect_ratio;
}

void CGL_camera_set_z_near(CGL_camera* camera, CGL_float z_near)
{
    camera->z_near = z_near;
}

void CGL_camera_set_z_far(CGL_camera* camera, CGL_float z_far)
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

// phong renderer
#ifndef CGL_EXCLUDE_GRAPHICS_API // cannot have PHONG RENDERER without Graphics API

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
    CGL_float shininess;    
    bool wireframe;
    void* user_data;
};

struct CGL_phong_pipeline
{
    bool use_blinn;
    CGL_shader* shader;
    CGL_phong_light* lights[CGL_PHONG_MAX_LIGHTS];
    CGL_vec3 ambient_light_color;
    CGL_float ambient_light_strength;
    uint32_t light_count;
    bool use_gamma_correction;
    bool depth_testing;
    void* user_data;
    // uniform locations
    CGL_int u_lights_v4[4][CGL_PHONG_MAX_LIGHTS];
    CGL_int u_projection;
    CGL_int u_view;
    CGL_int u_pv;
    CGL_int u_model_matrix;
    CGL_int u_light_count;
    CGL_int u_use_diffuse_texture;
    CGL_int u_diffuse_color;
    CGL_int u_diffuse_texture;
    CGL_int u_use_specular_texture;
    CGL_int u_specular_color;
    CGL_int u_specular_texture;
    CGL_int u_use_normal_map;
    CGL_int u_normal_map_texture;
    CGL_int u_shininess;
    CGL_int u_camera_position;
    CGL_int u_use_blinn;
    CGL_int u_ambient_light_color;
    CGL_int u_ambient_light_strength;
    CGL_int u_use_gamma_correction;
};

struct CGL_phong_light
{
    uint32_t light_type;
    CGL_vec3 vector;
    CGL_vec3 color;
    CGL_float intensity;
    CGL_float constant;
    CGL_float linear;
    CGL_float quadratic;
    void* user_data;
};

static const char* __CGL_PHONG_VERTEX_SHADER =
#ifdef CGL_WASM
"#version 300 es\n"
#else
"#version 430 core\n"
#endif
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
"	gl_Position = u_projection * u_view * transpose(u_model_matrix) * vec4(position.xyz, 1.0f);\n"
"	Position = (transpose(u_model_matrix) * position).xyz;\n"
"	Normal = normal.xyz;\n"
"	TexCoord = texcoord.xy;		\n"
"}";


static const char* __CGL_PHONG_FRAGMENT_SHADER = 
#ifdef CGL_WASM
"#version 300 es\n"
#else
"#version 430 core\n"
#endif
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
"//out CGL_int MousePick0;\n"
"//out CGL_int MousePick1;\n"
"//out CGL_int MousePick2;\n"
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
"        return vec4(pow(texture(u_diffuse_texture, TexCoord).xyz, vec3(2.2f)), 1.0f);\n"
"    return vec4(u_diffuse_color, 1.0f);\n"
"}\n"
"\n"
"vec4 get_material_specular_color()\n"
"{\n"
"    if(u_use_specular_texture)\n"
"        return vec4(pow(texture(u_specular_texture, TexCoord).xyz, vec3(2.2f)), 1.0f);\n"
"    return vec4(u_specular_color, 1.0f);\n"
"}\n"
"\n"
"\n"
"vec3 aces_tonemap(vec3 x){	\n"
"	const float a = 2.51f;\n"
"    const float b = 0.03f;\n"
"    const float c = 2.43f;\n"
"    const float d = 0.59f;\n"
"    const float e = 0.14f;\n"
"    return clamp((x * (a * x + b)) / (x * (c * x + d ) + e), 0.0f, 1.0f);\n"
"}\n"
"\n"
"vec4 calculate_directional_light(CGL_int index)\n"
"{\n"
"    vec3 light_direcion = normalize(-LIGHT_VECTOR(index));\n"
"    // diffuse shading\n"
"    float diff = max(dot(Normal, light_direcion), 0.0f);\n"
"    // specular shading\n"
"    vec3 view_dir = normalize(u_camera_position - Position);\n"
"    float spec = 0.0f;\n"
"    if(u_use_blinn)\n"
"    {\n"
"        vec3 halfway_direction = normalize(light_direcion + view_dir);\n"
"        spec = pow(max(dot(view_dir, halfway_direction), 0.0f), u_shininess);\n"
"    }\n"
"    else\n"
"    {\n"
"        vec3 reflect_direction = reflect(-light_direcion, Normal);\n"
"        spec = pow(max(dot(view_dir, reflect_direction), 0.0f), u_shininess);\n"
"    }\n"
"    vec4 material_diffuse = get_material_diffuse_color();\n"
"    vec4 material_specular = get_material_specular_color();\n"
"    vec4 ambient_lighting = vec4(u_ambient_light_color * u_ambient_light_strength, 1.0f);\n"
"    vec4 diffuse_lighting = LIGHT_COLOR(index) * diff * material_diffuse;\n"
"    vec4 specular_lighting = LIGHT_COLOR(index) * spec * material_specular;\n"
"    return (ambient_lighting + diffuse_lighting + specular_lighting) * LIGHT_INTENSITY(index);\n"
"}\n"
"\n"
"vec4 calculate_point_light(CGL_int index)\n"
"{\n"
"    vec3 light_direcion = normalize(LIGHT_VECTOR(index) - Position);\n"
"    // diffuse shading\n"
"    float diff = max(dot(Normal, light_direcion), 0.0f);\n"
"    // specular shading\n"
"    vec3 view_dir = normalize(u_camera_position - Position);\n"
"    float spec = 0.0f;\n"
"    if(u_use_blinn)\n"
"    {\n"
"        vec3 halfway_direction = normalize(light_direcion + view_dir);\n"
"        spec = pow(max(dot(view_dir, halfway_direction), 0.0f), u_shininess);\n"
"    }\n"
"    else\n"
"    {\n"
"        vec3 reflect_direction = reflect(-light_direcion, Normal);\n"
"        spec = pow(max(dot(view_dir, reflect_direction), 0.0f), u_shininess);\n"
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
"vec4 calculate_spot_light(CGL_int index)\n"
"{\n"
"    return vec4(0.0f);\n"
"}\n"
"\n"
"\n"
"void main()\n"
"{\n"
"    vec4 color = vec4(0.0f, 1.0f, 0.0f, 1.0f);\n"
"    vec4 light_output = vec4(0.0f);\n"
"    for ( CGL_int i = 0 ; i < u_light_count ; i++)\n"
"    {\n"
"        if(LIGHT_TYPE(i) < LIGHT_TYPE_DIRECTIONAL)\n"
"            light_output += calculate_directional_light(i);\n"
"        else if(LIGHT_TYPE(i) < LIGHT_TYPE_POINT)\n"
"            light_output += calculate_point_light(i);\n"
"        else if(LIGHT_TYPE(i) < LIGHT_TYPE_SPOT)        \n"
"            light_output += calculate_spot_light(i);\n"
"    }\n"
"\n"
"    light_output.xyz = aces_tonemap(light_output.xyz);\n"
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
    for(CGL_int i = 0 ; i < 4 ; i ++)
    {
        for(CGL_int j = 0 ; j < CGL_PHONG_MAX_LIGHTS ; j ++)
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
    for(CGL_int i = 0 ; i < CGL_utils_array_size(pipeline->lights); i++)
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

void CGL_phong_pipeline_set_ambient_light_strength(CGL_phong_pipeline* pipeline, CGL_float strength)
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
    for(CGL_int i = 0 ; i < CGL_utils_array_size(pipeline->lights);i++)
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
void CGL_phong_mat_set_shininess(CGL_phong_mat* mat, CGL_float shininess)
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
CGL_phong_light* CGL_phong_light_directional(CGL_vec3 direction, CGL_vec3 color, CGL_float intensity)
{
    CGL_phong_light* light = (CGL_phong_light*)malloc(sizeof(CGL_phong_light));
    light->light_type = CGL_PHONG_LIGHT_DIRECTIONAL;
    light->color = color;
    light->intensity = intensity;
    light->vector = direction;
    return light;
}

// light_point
CGL_phong_light* CGL_phong_light_point(CGL_vec3 position, CGL_vec3 color, CGL_float intensity, CGL_float constant, CGL_float linear, CGL_float quadratic)
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
void CGL_phong_light_set_intensity(CGL_phong_light* light, CGL_float intensity)
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
        CGL_texture_bind(material->diffuse_texture, 4);
        CGL_shader_set_uniform_int(pipeline->shader, pipeline->u_diffuse_texture, 4);
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
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    CGL_mesh_gpu_render(mesh);
    if(material->wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CGL_phong_render_end(CGL_phong_pipeline* pipeline, CGL_camera* camera)
{
    
}


#endif

#endif

#endif

// tilemap renderer
#ifndef CGL_EXCLUDE_GRAPHICS_API // cannot have TILE RENDERER without Graphics API

// tilemap renderer
#if 1

#ifndef CGL_EXCLUDE_TILEMAP_RENDERER

static const char* __CGL_TILEMAP_VERTEX_SHADER = 
#ifdef CGL_WASM
"#version 300 es\n"
#else
"#version 430 core\n"
#endif
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(position.xyz, 1.0f);\n"
"}";

static const char* __CGL_TILEMAP_FRAGENT_SHADER = 
#ifdef CGL_WASM
"#version 300 es\n"
#else
"#version 430 core\n"
#endif
"\n"
"out vec4 FragColor;\n"
"//out CGL_int MousePick0;\n"
"//out CGL_int MousePick1;\n"
"//out CGL_int MousePick2;\n"
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
    bool auto_update;
    // renderer data
    CGL_mesh_gpu* mesh;
    CGL_shader* shader;
    CGL_ssbo* ssbo;
    // unifrom locations
    CGL_int u_tile_count;
    CGL_int u_tile_size;
    CGL_int u_offset;
    CGL_int u_scale;
    CGL_int u_texture_tileset;
    CGL_int u_texture_array;
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
    CGL_mesh_cpu* screen_quad_mesh_cpu = CGL_mesh_cpu_quad(CGL_vec3_init( 1.0,  1.0, 0.0),
                                                           CGL_vec3_init( 1.0, -1.0, 0.0),
                                                           CGL_vec3_init(-1.0, -1.0, 0.0),
                                                           CGL_vec3_init(-1.0,  1.0, 0.0));
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
    tilemap->auto_update = true;
    CGL_tilemap_reset(tilemap);
    return tilemap;
}

void CGL_tilemap_destroy(CGL_tilemap* tilemap)
{
    CGL_ssbo_destroy(tilemap->ssbo);
    CGL_shader_destroy(tilemap->shader);
    CGL_mesh_gpu_destroy(tilemap->mesh);
    CGL_free(tilemap->tile_data);
    CGL_free(tilemap);
}

void CGL_tilemap_set_auto_upload(CGL_tilemap* tilemap, bool value)
{
    tilemap->auto_update = value;
}

bool CGL_tilemap_get_auto_upload(CGL_tilemap* tilemap)
{
    return tilemap->auto_update;
}

bool CGL_tilemap_upload(CGL_tilemap* tilemap)
{
    CGL_ssbo_set_data(tilemap->ssbo, (sizeof(CGL_tile) * tilemap->tile_count_x * tilemap->tile_count_y), tilemap->tile_data, false);
    return true;
}

void CGL_tilemap_set_tile_color(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, CGL_float r, CGL_float g, CGL_float b)
{
    CGL_tile* tile = &tilemap->tile_data[tile_y * tilemap->tile_count_x + tile_x];
    tile->color = CGL_vec4_init(r, g, b, 2.5f);
    if(tilemap->auto_update) CGL_ssbo_set_sub_data(tilemap->ssbo, (tile_y * tilemap->tile_count_x + tile_x) * sizeof(CGL_tile), sizeof(CGL_tile), tile, false);
}

void CGL_tilemap_set_tile_texture_from_array(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, uint32_t texture_index)
{
    CGL_tile* tile = &tilemap->tile_data[tile_y * tilemap->tile_count_x + tile_x];
    tile->color = CGL_vec4_init((float)texture_index, 0.0f, 0.0f, 1.5f);
    if(tilemap->auto_update) CGL_ssbo_set_sub_data(tilemap->ssbo, (tile_y * tilemap->tile_count_x + tile_x) * sizeof(CGL_tile), sizeof(CGL_tile), tile, false);
}

void CGL_tilemap_set_tile_texture_from_tileset(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y, CGL_float texture_x_min, CGL_float texture_y_min, CGL_float texture_x_max, CGL_float texture_y_max)
{
    CGL_tile* tile = &tilemap->tile_data[tile_y * tilemap->tile_count_x + tile_x];
    tile->color = CGL_vec4_init(texture_x_min, texture_y_min, texture_x_max, texture_y_max);
    if(tilemap->auto_update) CGL_ssbo_set_sub_data(tilemap->ssbo, (tile_y * tilemap->tile_count_x + tile_x) * sizeof(CGL_tile), sizeof(CGL_tile), tile, false);
}

void CGL_tilemap_clear_tile(CGL_tilemap* tilemap, uint32_t tile_x, uint32_t tile_y)
{
    CGL_tile* tile = &tilemap->tile_data[tile_y * tilemap->tile_count_x + tile_x];
    tile->color = CGL_vec4_init(0.0f, 0.0f, 0.0f, 3.5f);
    if(tilemap->auto_update) CGL_ssbo_set_sub_data(tilemap->ssbo, (tile_y * tilemap->tile_count_x + tile_x) * sizeof(CGL_tile), sizeof(CGL_tile), tile, false);
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

void CGL_tilemap_render(CGL_tilemap* tilemap, CGL_float scale_x, CGL_float scale_y, CGL_float offset_x, CGL_float offset_y, CGL_texture* texture)
{
    CGL_shader_bind(tilemap->shader);
    if(texture) CGL_texture_bind(texture, 0);
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


#endif

// sky renderer
#ifndef CGL_EXCLUDE_GRAPHICS_API
#ifndef CGL_EXCLUDE_SKY_RENDERER

struct CGL_sky
{
    CGL_vec3 sun_pos;
    CGL_texture* cubemap;
    CGL_shader* cubemap_shader;
    CGL_shader* procedural_shader;
    CGL_mesh_gpu* skybox_mesh;
    CGL_mesh_gpu* skysphere_mesh;
    CGL_mesh_gpu* active_mesh;
    CGL_float cirrus;
    CGL_float cumulus;
    CGL_float upf;
    CGL_float time;
    bool is_procedural;
    // unifroms
    CGL_int cs_u_projection;
    CGL_int cs_u_view;
    CGL_int cs_u_texture;
    CGL_int ps_u_projection;
    CGL_int ps_u_view;
    CGL_int ps_u_cirrus;
    CGL_int ps_u_cumulus;
    CGL_int ps_u_upf;
    CGL_int ps_u_time;
    CGL_int ps_u_fsun;

};

static const char* __CGL_SKY_VERTEX_SHADER = 
#ifdef CGL_WASM
"#version 300 es\n"
#else
"#version 430 core\n"
#endif
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"out vec3 Position;\n"
"\n"
"\n"
"uniform mat4 u_projection;\n"
"uniform mat4 u_view;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = u_projection * transpose(u_view) * vec4(position.xyz, 1.0f);\n"
"	Position = position.xyz;\n"
"}";

static const char* __CGL_SKY_CUBEMAP_FRAGMENT_SHADER =
#ifdef CGL_WASM
"#version 300 es\n"
#else
"#version 430 core\n"
#endif
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Position;\n"
"\n"
"uniform samplerCube u_texture;\n"
"\n"
"void main()\n"
"{\n"
"	vec4 color = texture(u_texture, Position);\n"
"	FragColor = color;\n"
"}";

static const char* __CGL_SKY_PROCEDURAL_FRAGMENT_SHADER =
#ifdef CGL_WASM
"#version 300 es\n"
#else
"#version 430 core\n"
#endif
"uniform vec3 fsun = vec3(0, 0.2, 0.1);\n"
"uniform CGL_float time = 0.0;\n"
"uniform CGL_float cirrus = 0.4;\n"
"uniform CGL_float cumulus = 0.8;\n"
"uniform CGL_float upf = 0.35;\n"
"\n"
"in vec3 Position;"
"\n"
"out vec4 FragColor;\n"
"\n"
"const float Br = 0.0025;\n"
"const float Bm = 0.0003;\n"
"const float g = 0.9800;\n"
"const vec3 nitrogen = vec3(0.650, 0.570, 0.475);\n"
"const vec3 Kr = Br / pow(nitrogen, vec3(4.0));\n"
"const vec3 Km = Bm / pow(nitrogen, vec3(0.84));\n"
"\n"
"float hash(float n) {\n"
"  return fract(sin(n) * 43758.5453123);\n"
"}\n"
"\n"
"float noise(vec3 x) {\n"
"  vec3 f = fract(x);\n"
"  float n = dot(floor(x), vec3(1.0, 157.0, 113.0));\n"
"  return mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),\n"
"      mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),\n"
"    mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),\n"
"      mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);\n"
"}\n"
"\n"
"const mat3 m = mat3(0.0, 1.60, 1.20, -1.6, 0.72, -0.96, -1.2, -0.96, 1.28);\n"
"float fbm(vec3 p) {\n"
"  float f = 0.0;\n"
"  f += noise(p) / 2;\n"
"  p = m * p * 1.1;\n"
"  f += noise(p) / 4;\n"
"  p = m * p * 1.2;\n"
"  f += noise(p) / 6;\n"
"  p = m * p * 1.3;\n"
"  f += noise(p) / 12;\n"
"  p = m * p * 1.4;\n"
"  f += noise(p) / 24;\n"
"  return f;\n"
"}\n"
"\n"
"void main() {\n"
"  vec3 pos = vec3(Position.x, Position.y+upf, Position.z);\n"
"\n"
"  // Atmosphere Scattering\n"
"  float mu = dot(normalize(pos), normalize(fsun));\n"
"  float rayleigh = 3.0 / (8.0 * 3.14) * (1.0 + mu * mu);\n"
"  vec3 mie = (Kr + Km * (1.0 - g * g) / (2.0 + g * g) / pow(1.0 + g * g - 2.0 * g * mu, 1.5)) / (Br + Bm);\n"
"\n"
"  vec3 day_extinction = exp(-exp(-((pos.y + fsun.y * 4.0) * (exp(-pos.y * 16.0) + 0.1) / 80.0) / Br) * (exp(-pos.y * 16.0) + 0.1) * Kr / Br) * exp(-pos.y * exp(-pos.y * 8.0) * 4.0) * exp(-pos.y * 2.0) * 4.0;\n"
"  vec3 night_extinction = vec3(1.0 - exp(fsun.y)) * 0.2;\n"
"  vec3 extinction = mix(day_extinction, night_extinction, -fsun.y * 0.2 + 0.5);\n"
"  FragColor.rgb = rayleigh * mie * extinction;\n"
"\n"
"  // Cirrus Clouds\n"
"  float density = smoothstep(1.0 - cirrus, 1.0, fbm(pos.xyz / pos.y * 2.0 + time * 0.05)) * 0.3;\n"
"  FragColor.rgb = mix(FragColor.rgb, extinction * 4.0, density * max(pos.y, 0.0));\n"
"\n"
"  // Cumulus Clouds\n"
"  for (int i = 0; i < 3; i++) {\n"
"    float density = smoothstep(1.0 - cumulus, 1.0, fbm((0.7 + float(i) * 0.01) * pos.xyz / pos.y + time * 0.3));\n"
"    FragColor.rgb = mix(FragColor.rgb, extinction * density * 5.0, min(density, 1.0) * max(pos.y, 0.0));\n"
"  }\n"
"\n"
"  // Dithering Noise\n"
"  FragColor.rgb += noise(pos * 1000) * 0.01;\n"
"}";

CGL_sky* CGL_sky_create()
{
    CGL_sky* sky = (CGL_sky*)CGL_malloc(sizeof(CGL_sky));
    if(!sky) return NULL;
    sky->sun_pos = CGL_vec3_init(0.0f, 0.2f, 0.1f);
    sky->time = 0.0f;
    sky->cirrus = 0.4f;
    sky->cumulus = 0.8f;
    sky->upf = 0.35f;
    sky->is_procedural = true;
    sky->cubemap = CGL_texture_create_cubemap();

    CGL_mesh_cpu* skybox_cpu = CGL_mesh_cpu_cube(true);
    sky->skybox_mesh = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(sky->skybox_mesh, skybox_cpu, true);
    CGL_free(skybox_cpu);

    CGL_mesh_cpu* skysphere_cpu = CGL_mesh_cpu_sphere(16, 16);
    sky->skysphere_mesh = CGL_mesh_gpu_create();
    CGL_mesh_gpu_upload(sky->skysphere_mesh, skysphere_cpu, true);
    CGL_free(skysphere_cpu);

    sky->active_mesh = sky->skybox_mesh;

    sky->cubemap_shader = CGL_shader_create(__CGL_SKY_VERTEX_SHADER, __CGL_SKY_CUBEMAP_FRAGMENT_SHADER, NULL);
    sky->cs_u_projection = CGL_shader_get_uniform_location(sky->cubemap_shader, "u_projection");
    sky->cs_u_view = CGL_shader_get_uniform_location(sky->cubemap_shader, "u_view");
    sky->cs_u_texture = CGL_shader_get_uniform_location(sky->cubemap_shader, "u_texture");

    sky->procedural_shader = CGL_shader_create(__CGL_SKY_VERTEX_SHADER, __CGL_SKY_PROCEDURAL_FRAGMENT_SHADER, NULL);
    sky->ps_u_projection = CGL_shader_get_uniform_location(sky->procedural_shader, "u_projection");
    sky->ps_u_view = CGL_shader_get_uniform_location(sky->procedural_shader, "u_view");
    sky->ps_u_cirrus = CGL_shader_get_uniform_location(sky->procedural_shader, "cirrus");
    sky->ps_u_cumulus = CGL_shader_get_uniform_location(sky->procedural_shader, "cumulus");
    sky->ps_u_upf = CGL_shader_get_uniform_location(sky->procedural_shader, "upf");
    sky->ps_u_time = CGL_shader_get_uniform_location(sky->procedural_shader, "time");
    sky->ps_u_fsun = CGL_shader_get_uniform_location(sky->procedural_shader, "fsun");

    return sky;
}

void CGL_sky_destroy(CGL_sky* sky)
{
    CGL_texture_destroy(sky->cubemap);
    CGL_mesh_gpu_destroy(sky->skybox_mesh);
    CGL_mesh_gpu_destroy(sky->skysphere_mesh);
    CGL_shader_destroy(sky->cubemap_shader);
    CGL_shader_destroy(sky->procedural_shader);
    CGL_free(sky);
}

void CGL_sky_use_skybox(CGL_sky* sky)
{
    sky->active_mesh = sky->skybox_mesh;
}

void CGL_sky_use_skysphere(CGL_sky* sky)
{
    sky->active_mesh = sky->skysphere_mesh;
}

void CGL_sky_use_cubemap(CGL_sky* sky)
{
    sky->is_procedural = false;
}

void CGL_sky_use_procedural(CGL_sky* sky)
{
    sky->is_procedural = true;
}

void CGL_sky_cubemap_set_face(CGL_sky* sky, CGL_int face, CGL_image* image)
{
    CGL_texture_cubemap_set_face(sky->cubemap, face, image);
}

void CGL_sky_procedural_set_options(CGL_sky* sky, CGL_float cirrus, CGL_float cumulus, CGL_float upf)
{
    sky->cirrus = cirrus;
    sky->cumulus = cumulus;
    sky->upf = upf;
}

void CGL_sky_procedural_set_time(CGL_sky* sky, CGL_float time)
{
    sky->time = time;
}

void CGL_sky_procedural_set_sun_position(CGL_sky* sky, CGL_vec3 position)
{
    sky->sun_pos = position;
}

void CGL_sky_render(CGL_sky* sky, CGL_camera* camera)
{
    glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
    if(sky->is_procedural)
    {
        CGL_shader_bind(sky->procedural_shader);
        CGL_shader_set_uniform_mat4(sky->procedural_shader, sky->ps_u_projection, &camera->projection);
        CGL_mat4 view_matrix = camera->view;
        view_matrix.m[3] = view_matrix.m[7] = view_matrix.m[11] = 0.0f; // removing the translation
        CGL_shader_set_uniform_mat4(sky->procedural_shader, sky->ps_u_view, &view_matrix);
        CGL_shader_set_uniform_float(sky->procedural_shader, sky->ps_u_cirrus, sky->cirrus);
        CGL_shader_set_uniform_float(sky->procedural_shader, sky->ps_u_cumulus, sky->cumulus);
        CGL_shader_set_uniform_float(sky->procedural_shader, sky->ps_u_upf, sky->upf);
        CGL_shader_set_uniform_float(sky->procedural_shader, sky->ps_u_time, sky->time);
        CGL_shader_set_uniform_vec3v(sky->procedural_shader, sky->ps_u_fsun, sky->sun_pos.x, sky->sun_pos.y, sky->sun_pos.z);
    }
    else
    {
        CGL_shader_bind(sky->cubemap_shader);
        CGL_shader_set_uniform_mat4(sky->cubemap_shader, sky->cs_u_projection, &camera->projection);
        CGL_mat4 view_matrix = camera->view;
        view_matrix.m[3] = view_matrix.m[7] = view_matrix.m[11] = 0.0f; // removing the translation
        CGL_shader_set_uniform_mat4(sky->cubemap_shader, sky->cs_u_view, &view_matrix);
        CGL_texture_bind(sky->cubemap, 0);
        CGL_shader_set_uniform_int(sky->cubemap_shader, sky->cs_u_texture, 0);
    }
    CGL_mesh_gpu_render(sky->active_mesh);
    glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

#endif // CGL_EXCLUDE_SKY_RENDERER
#endif // CGL_EXCLUDE_GRAPHICS_API

// markov chains
#ifndef CGL_EXCLUDE_MARKOV_API

struct CGL_markov
{
    CGL_hashtable* hashtable;
    size_t key_size;
    size_t value_size;
    void* temp_key;
    void* temp_value;
};

struct CGL_markov_token_function_ngram_text_context
{
    size_t index;
    size_t n;
};

CGL_markov_token_function_ngram_text_context* CGL_markov_token_function_ngram_text_context_create(size_t n)
{
    CGL_markov_token_function_ngram_text_context* ctx = (CGL_markov_token_function_ngram_text_context*)CGL_malloc(sizeof(CGL_markov_token_function_ngram_text_context));
    if(!ctx) return NULL;
    ctx->index = 0;
    ctx->n = n;
    return ctx;
}

void CGL_markov_token_function_ngram_text_context_destroy(CGL_markov_token_function_ngram_text_context* context)
{
    CGL_free(context);
}


bool CGL_markov_token_function_ngram_text(void* context, const void* dat, const size_t data_size, void* key, void* value)
{
    CGL_markov_token_function_ngram_text_context* ctx = (CGL_markov_token_function_ngram_text_context*)context;
    const char* data = (const char*)dat;
    if(ctx->index >= data_size - ctx->n - 1) return false;
    if(key) memcpy(key, data + ctx->index, ctx->n);
    if(value) *((char*)value) = *(data + ctx->index + ctx->n);
    ctx->index++;
    return true;
}

CGL_markov* CGL_markov_create(const size_t key_size, const size_t value_size)
{
    CGL_markov* mk = (CGL_markov*)CGL_malloc(sizeof(CGL_markov));
    if(!mk) return NULL;
    mk->key_size = key_size;
    mk->value_size = value_size;
    mk->hashtable = CGL_hashtable_create(10000, key_size, 100000);
    mk->temp_key = CGL_malloc(key_size);
    mk->temp_value = CGL_malloc(value_size);
    return mk;
}

bool CGL_markov_train(CGL_markov* markov, void* context, const void* data, const size_t data_size, const CGL_markov_token_function function)
{
    memset(markov->temp_key, 0, markov->key_size);
    memset(markov->temp_value, 0, markov->value_size);
    while(function(context, data, data_size, markov->temp_key, markov->temp_value))
    {   
        if(!CGL_hashtable_exists(markov->hashtable, markov->temp_key))
        {
            CGL_list* list = CGL_list_create(markov->value_size, 10);

            CGL_hashtable_set(markov->hashtable, markov->temp_key, &list, sizeof(CGL_list*));
        }
        CGL_list* list = NULL;
        CGL_hashtable_get(markov->hashtable, markov->temp_key, &list);
        CGL_list_push(list, markov->temp_value);    
        memset(markov->temp_key, 0, markov->key_size);
        memset(markov->temp_value, 0, markov->value_size);
    }
    return true;
}


bool CGL_markov_generate(const CGL_markov* markov, const void* key, void* value)
{
    if(!CGL_hashtable_exists(markov->hashtable, key)) return false;
    CGL_list* list = NULL;
    CGL_hashtable_get(markov->hashtable, key, &list);
    CGL_list_get_random(list, value);
    return true;
}

void CGL_markov_destroy(CGL_markov* markov)
{
    CGL_hashtable_iterator* iterator = CGL_hashtable_iterator_create(markov->hashtable);
    CGL_list* lst = NULL;
    while(CGL_hashtable_iterator_next(iterator, NULL, &lst, NULL)) CGL_list_destroy(lst);
    CGL_hashtable_iterator_destroy(iterator);
    CGL_hashtable_destroy(markov->hashtable);
    CGL_free(markov->temp_key);
    CGL_free(markov->temp_value);
    CGL_free(markov);
}

#endif

// text rendering
#ifndef CGL_EXCLUDE_TEXT_RENDER

#ifndef CGL_EXCLUDE_GRAPHICS_API

#include <ft2build.h>
#include FT_FREETYPE_H  

struct CGL_font
{
    CGL_font_character characters[128];
    CGL_texture* atlas;
    size_t atlas_width;
    size_t atlas_height;
    FT_Face face;
};

static FT_Library __CGL_free_type_library;

bool CGL_text_init()
{
    bool result = FT_Init_FreeType(&__CGL_free_type_library);
    if(result) { CGL_log_internal("Could not Initialize FreeType\n"); }
    return !result;
}

void CGL_text_shutdown()
{
    FT_Done_FreeType(__CGL_free_type_library);
}

CGL_font* CGL_font_load(const char* path)
{
    CGL_font* font = (CGL_font*)CGL_malloc(sizeof(CGL_font));
    if(!font) return NULL;
    if(FT_New_Face(__CGL_free_type_library, path, 0, &font->face)) {CGL_log_internal("Could not Load Font\n");}
    memset(font->characters, 0, sizeof(CGL_font_character) * 128);
    font->atlas_width = 0;
    font->atlas_height = 0;
    font->atlas = NULL;
    return font;
}

void CGL_font_destory(CGL_font* font)
{
    FT_Done_Face(font->face);
    if(font->atlas) CGL_texture_destroy(font->atlas);
    for(size_t i = 0 ; i < 127 ; i++)
        if(font->characters[i].bitmap)
            CGL_free(font->characters[i].bitmap);
    CGL_free(font);
}

CGL_texture* CGL_font_get_atlas(CGL_font* font)
{
    return font->atlas;
}

bool CGL_font_build_atlas(CGL_font* font, size_t width, size_t height, size_t font_size)
{
    if(font->atlas) CGL_texture_destroy(font->atlas);
    FT_Set_Pixel_Sizes(font->face, 0, (FT_UInt)font_size);  
    int64_t offset_x = 0;
    int64_t offset_y = 0;
    int64_t max_size_y = 0;
    font->atlas_width = width;
    font->atlas_height = height;
    font->atlas = CGL_texture_create_blank((int)width, (int)height, GL_RED, GL_RED, GL_UNSIGNED_BYTE);
    for(char ch = 0 ; ch < 127 ; ch++)
    {
        if(FT_Load_Char(font->face, (FT_ULong)ch, FT_LOAD_RENDER))
        {
            CGL_log_internal("Could not Font Load Character %c\n", ch);
            return false;
        }
        int64_t size_x = font->face->glyph->bitmap.width;
        int64_t size_y = font->face->glyph->bitmap.rows;

        if(font->characters[ch].bitmap) CGL_free(font->characters[ch].bitmap);
        font->characters[ch].size = CGL_vec2_init((float)size_x, (float)size_y);
        font->characters[ch].normalized_size = CGL_vec2_init((float)size_x / font->atlas_width, (float)size_y / font->atlas_height);
        font->characters[ch].offset = CGL_vec2_init((float)offset_x, (float)offset_y);
        font->characters[ch].normalized_offset = CGL_vec2_init((float)offset_x / font->atlas_width, (float)offset_y / font->atlas_height);
        font->characters[ch].bearing = CGL_vec2_init((float)font->face->glyph->bitmap_left, (float)font->face->glyph->bitmap_top);
        font->characters[ch].advance = CGL_vec2_init((float)font->face->glyph->advance.x, (float)0.0f);
        font->characters[ch].ch = ch;
        font->characters[ch].bitmap = (unsigned char*)CGL_malloc(sizeof(unsigned char) * size_x * size_y);
        if(!font->characters[ch].bitmap) return false;
        memcpy(font->characters[ch].bitmap, font->face->glyph->bitmap.buffer, (sizeof(unsigned char) * size_x * size_y));
        
        CGL_texture_set_sub_data(font->atlas, offset_x, offset_y, size_x, size_y, font->characters[ch].bitmap);
       
        offset_x += size_x + 5;
        max_size_y = max(max_size_y, size_y);
        if(((int64_t)width - offset_x) <= 10 )
        {
            offset_x = 0;
            offset_y += max_size_y + 10;
            max_size_y = 0;
        }
    }
    return true;
}

CGL_font_character* CGL_font_get_characters(CGL_font* font)
{
    return font->characters;
}

CGL_texture* CGL_text_bake_to_texture(const char* string, size_t string_length, CGL_font* font, size_t* width, size_t* height)
{
    size_t tex_width = 0;
    size_t tex_height = 0;
    size_t max_bottom_pad = 0;
    for(CGL_int i = 0 ; i < string_length ; i++ )
    {
        char ch = string[i];
        tex_width += (((int)font->characters[ch].advance.x) >> 6);
        max_bottom_pad = max(max_bottom_pad, (size_t)(font->characters[ch].size.y - font->characters[ch].bearing.y));
        tex_height = max(tex_height, (size_t)font->characters[ch].size.y + max_bottom_pad);
    }
    if(width) *width = tex_width;
    if(height) *height = tex_height;
    CGL_texture* tex = CGL_texture_create_blank((int)tex_width, (int)tex_height, GL_RED, GL_RED, GL_UNSIGNED_BYTE);
    if(!tex) return NULL;
    size_t offset_x = 0;
    size_t offset_y = 0;
    for(CGL_int i = 0 ; i < string_length ; i++ )
    {
        char ch = string[i];
        size_t offset_x_local = offset_x + (size_t)font->characters[ch].bearing.x;
        size_t offset_y_local = offset_y + (size_t)(font->characters[ch].size.y - font->characters[ch].bearing.y) - max_bottom_pad;
        offset_y_local += ((tex_height) - (size_t)font->characters[ch].size.y);
        CGL_texture_set_sub_data(tex, offset_x_local, offset_y_local, (int)font->characters[ch].size.x, (int)font->characters[ch].size.y, font->characters[ch].bitmap);
        offset_x += (((int)font->characters[ch].advance.x) >> 6);        
    }
    return tex;
}

#endif

#endif


// widgets
#ifndef CGL_EXCLUDE_WIDGETS
#ifndef CGL_GRAPHICS_API

struct CGL_widgets_context
{
    CGL_vec4 stroke_color;
    CGL_vec4 fill_color;
    CGL_vec4 scale;
    CGL_vec4 offset;
    CGL_mesh_vertex* vertices;
    CGL_shader* shader;    
    uint32_t* indices;
    size_t max_vertices;
    size_t vertices_count;
    size_t max_indices;
    size_t indices_count;
    CGL_float stroke_thickness;
    CGL_float aspect_ratio;
    GLuint vertex_array;
    GLuint vertex_buffer;
    GLuint index_buffer;
    bool flushed;
    bool is_fill;
    bool adjust_for_aspect_ratio;
};

static const char* __CGL_WIDGETS_VERTEX_SHADER_SOURCE = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n"
"layout (location = 1) in vec4 normal;\n"
"layout (location = 2) in vec4 texcoord;\n"
"\n"
"out vec3 Position;\n"
"out vec3 Normal;\n"
"out vec2 TexCoord; \n"
"out vec4 Color;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(position.xyz, 1.0f);\n"
"	Position = position.xyz;\n"
"	Normal = normal.xyz;\n"
"	TexCoord = texcoord.xy;		\n"
"	Color = vec4(position.w, texcoord.zw, normal.w);\n"
"}";

static const char* __CGL_WIDGETS_FRAGMENT_SHADER_SOURCE = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"//out CGL_int MousePick0;\n"
"//out CGL_int MousePick1;\n"
"//out CGL_int MousePick2;\n"
"\n"
"in vec3 Position;\n"
"in vec3 Normal;\n"
"in vec2 TexCoord;\n"
"in vec4 Color;\n"
"\n"
"\n"
"void main()\n"
"{\n"
"	vec4 color = Color;\n"
"	FragColor = color;\n"
"//	MousePick0 = 0;\n"
"//	MousePick1 = 0;\n"
"//	MousePick2 = 0;\n"
"}";

static CGL_widgets_context* __CGL_WIDGETS_CURRENT_CONTEXT = NULL;

CGL_widgets_context* CGL_widgets_context_create(size_t max_vertices, size_t max_indices)
{
    CGL_widgets_context* context = (CGL_widgets_context*)CGL_malloc(sizeof(CGL_widgets_context));
    if(!context) return NULL;
    context->max_indices = max_indices;
    context->max_vertices = max_vertices;
    context->vertices = (CGL_mesh_vertex*)CGL_malloc(sizeof(CGL_mesh_vertex) * max_vertices);
    context->indices = (uint32_t*)CGL_malloc(sizeof(uint32_t) * max_indices);
    context->vertices_count = 0;
    context->indices_count = 0;
    context->flushed = true;
    context->adjust_for_aspect_ratio = false;
    context->aspect_ratio = 1.0f;
    context->offset = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    context->scale = CGL_vec4_init(1.0f, 1.0f, 1.0f, 1.0f);
    glGenVertexArrays(1, &context->vertex_array);
    glBindVertexArray(context->vertex_array);
    glGenBuffers(1, &context->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, context->vertex_buffer);
    glGenBuffers(1, &context->index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->index_buffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(CGL_mesh_vertex), (void*)offsetof(CGL_mesh_vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(CGL_mesh_vertex), (void*)offsetof(CGL_mesh_vertex, normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(CGL_mesh_vertex), (void*)offsetof(CGL_mesh_vertex, texture_coordinates));
    glEnableVertexAttribArray(2);
    context->shader = CGL_shader_create(__CGL_WIDGETS_VERTEX_SHADER_SOURCE, __CGL_WIDGETS_FRAGMENT_SHADER_SOURCE, NULL);
    return context;
}

void CGL_widgets_context_destory(CGL_widgets_context* context)
{
    glDeleteBuffers(1, &context->index_buffer);
    glDeleteBuffers(1, &context->vertex_buffer);
    glDeleteVertexArrays(1, &context->vertex_array);
    CGL_shader_destroy(context->shader);
    if(context->indices) CGL_free(context->indices);
    if(context->vertices) CGL_free(context->vertices);
    CGL_free(context);
}

void CGL_widgets_adjust_for_aspect_ratio(CGL_float aspect_ratio)
{
    __CGL_WIDGETS_CURRENT_CONTEXT->aspect_ratio = aspect_ratio;
    __CGL_WIDGETS_CURRENT_CONTEXT->adjust_for_aspect_ratio = true;
}

void CGL_widgets_do_not_adjust_for_aspect_ratio()
{
    __CGL_WIDGETS_CURRENT_CONTEXT->adjust_for_aspect_ratio = false;
}

CGL_widgets_context* CGL_window_get_current_context()
{
    return __CGL_WIDGETS_CURRENT_CONTEXT;   
}

bool CGL_widgets_init()
{
    __CGL_WIDGETS_CURRENT_CONTEXT = CGL_widgets_context_create(CGL_WIDGETS_MAX_VERTICES, CGL_WIDGETS_MAX_INDICES);
    return __CGL_WIDGETS_CURRENT_CONTEXT != NULL;
}

void CGL_widgets_shutdown()
{
    if(__CGL_WIDGETS_CURRENT_CONTEXT) CGL_widgets_context_destory(__CGL_WIDGETS_CURRENT_CONTEXT);
}

void CGL_window_set_current_context(CGL_widgets_context* context)
{
    if(context) __CGL_WIDGETS_CURRENT_CONTEXT = context;
}

bool CGL_widgets_begin_int(CGL_float scale_x, CGL_float scale_y, CGL_float offset_x, CGL_float offset_y)
{
    __CGL_WIDGETS_CURRENT_CONTEXT->flushed = false;
    __CGL_WIDGETS_CURRENT_CONTEXT->is_fill = true;
    __CGL_WIDGETS_CURRENT_CONTEXT->stroke_color = CGL_vec4_init(0.0f, 0.0f, 0.0f, 1.0f);
    __CGL_WIDGETS_CURRENT_CONTEXT->fill_color = CGL_vec4_init(1.0f, 1.0f, 1.0f, 1.0f);
    __CGL_WIDGETS_CURRENT_CONTEXT->stroke_thickness = 0.05f;
    __CGL_WIDGETS_CURRENT_CONTEXT->scale = CGL_vec4_init(scale_x, scale_y, 0.0f, 0.0f);
    __CGL_WIDGETS_CURRENT_CONTEXT->offset = CGL_vec4_init(offset_x, offset_y, 0.0f, 0.0f);
    return true;
}

bool CGL_widgets_begin()
{
    return CGL_widgets_begin_int(1.0f, 1.0f, 0.0f, 0.0f);
}

bool CGL_widgets_end()
{
    return CGL_widgets_flush();
}

bool CGL_widgets_flush()
{
    //if(__CGL_WIDGETS_CURRENT_CONTEXT->flushed) return false;
    if(__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count == 0 || __CGL_WIDGETS_CURRENT_CONTEXT->indices_count == 0) return false;
    // first upload the data
    glBindBuffer(GL_ARRAY_BUFFER, __CGL_WIDGETS_CURRENT_CONTEXT->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, __CGL_WIDGETS_CURRENT_CONTEXT->vertices_count * sizeof(CGL_mesh_vertex), __CGL_WIDGETS_CURRENT_CONTEXT->vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, __CGL_WIDGETS_CURRENT_CONTEXT->index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, __CGL_WIDGETS_CURRENT_CONTEXT->indices_count * sizeof(uint32_t), __CGL_WIDGETS_CURRENT_CONTEXT->indices, GL_DYNAMIC_DRAW);
    // render
    CGL_shader_bind(__CGL_WIDGETS_CURRENT_CONTEXT->shader);
    glBindVertexArray(__CGL_WIDGETS_CURRENT_CONTEXT->vertex_array);
    glDrawElements(GL_TRIANGLES, (GLsizei)__CGL_WIDGETS_CURRENT_CONTEXT->indices_count, GL_UNSIGNED_INT, NULL);
    __CGL_WIDGETS_CURRENT_CONTEXT->vertices_count = 0;
    __CGL_WIDGETS_CURRENT_CONTEXT->indices_count = 0;
    __CGL_WIDGETS_CURRENT_CONTEXT->flushed = true;
    return true;
}

bool CGL_widgets_flush_if_required()
{
    //if(__CGL_WIDGETS_CURRENT_CONTEXT->flushed) return false;
    if(__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count >= (__CGL_WIDGETS_CURRENT_CONTEXT->max_vertices - 10) && __CGL_WIDGETS_CURRENT_CONTEXT->indices_count % 3 == 0) return CGL_widgets_flush();
    return false;
}


bool CGL_widgets_add_vertices(CGL_mesh_vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count)
{
    if(__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count + vertex_count >= (__CGL_WIDGETS_CURRENT_CONTEXT->max_vertices - 10)) return false;
    if(__CGL_WIDGETS_CURRENT_CONTEXT->indices_count + index_count >= (__CGL_WIDGETS_CURRENT_CONTEXT->max_indices - 10)) return false;
    memcpy(&__CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count], vertices, sizeof(CGL_mesh_vertex) * vertex_count);
    memcpy(&__CGL_WIDGETS_CURRENT_CONTEXT->indices[__CGL_WIDGETS_CURRENT_CONTEXT->indices_count], indices, sizeof(uint32_t) * index_count);
    for(size_t i = __CGL_WIDGETS_CURRENT_CONTEXT->indices_count ; i < __CGL_WIDGETS_CURRENT_CONTEXT->indices_count + index_count ; i++) __CGL_WIDGETS_CURRENT_CONTEXT->indices[i] += (uint32_t)__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count;
    __CGL_WIDGETS_CURRENT_CONTEXT->vertices_count += vertex_count;
    __CGL_WIDGETS_CURRENT_CONTEXT->indices_count += index_count;
    CGL_widgets_flush_if_required();
    return true;
}

void CGL_widgets_add_vertex(CGL_mesh_vertex* vertex)
{
    CGL_widgets_flush_if_required();
    CGL_vec4 scale = __CGL_WIDGETS_CURRENT_CONTEXT->scale;
    if(__CGL_WIDGETS_CURRENT_CONTEXT->adjust_for_aspect_ratio)
    {
        // for now only works for 2D
        if(scale.x > scale.y) scale.x = scale.y / __CGL_WIDGETS_CURRENT_CONTEXT->aspect_ratio;
        else scale.y = scale.x * __CGL_WIDGETS_CURRENT_CONTEXT->aspect_ratio;
    }
    vertex->position.x = vertex->position.x * scale.x + __CGL_WIDGETS_CURRENT_CONTEXT->offset.x;
    vertex->position.y = vertex->position.y * scale.y + __CGL_WIDGETS_CURRENT_CONTEXT->offset.y;
    vertex->position.z = vertex->position.z * scale.z + __CGL_WIDGETS_CURRENT_CONTEXT->offset.z; // for future
    

    __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count++] = *vertex;
    __CGL_WIDGETS_CURRENT_CONTEXT->indices[__CGL_WIDGETS_CURRENT_CONTEXT->indices_count++] = (uint32_t)(__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 1);
    if(__CGL_WIDGETS_CURRENT_CONTEXT->is_fill)
    {
        __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 1].position.w = __CGL_WIDGETS_CURRENT_CONTEXT->fill_color.x;
        __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 1].texture_coordinates.z = __CGL_WIDGETS_CURRENT_CONTEXT->fill_color.y;
        __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 1].texture_coordinates.w = __CGL_WIDGETS_CURRENT_CONTEXT->fill_color.z;
        __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 1].normal.w = __CGL_WIDGETS_CURRENT_CONTEXT->fill_color.w;
    }
    else
    {
        __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 1].position.w = __CGL_WIDGETS_CURRENT_CONTEXT->stroke_color.x;
        __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 1].texture_coordinates.z = __CGL_WIDGETS_CURRENT_CONTEXT->stroke_color.y;
        __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 1].texture_coordinates.w = __CGL_WIDGETS_CURRENT_CONTEXT->stroke_color.z;
        __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 1].normal.w = __CGL_WIDGETS_CURRENT_CONTEXT->stroke_color.w;
    }
}

void CGL_widgets_add_vertex_pt(CGL_vec3 position, CGL_vec2 tex_coord)
{
    CGL_mesh_vertex vertex;
    vertex.position = CGL_vec4_init(position.x, position.y, position.z, 1.0f);
    vertex.texture_coordinates = CGL_vec4_init(tex_coord.x, tex_coord.y, 0.0f, 0.0f);
    vertex.normal = CGL_vec4_init(0.0f, 0.0f, 0.0f, 0.0f);
    if((__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count + 1) % 3 == 0)
    {
        CGL_vec4 a = __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 2].position;
        CGL_vec4 b = __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 1].position;
        CGL_vec4 c = vertex.position;
        CGL_vec3 ab = CGL_vec3_sub(b, a);
        CGL_vec3 bc = CGL_vec3_sub(c, b);
        CGL_vec3 normal = CGL_vec3_cross(ab, bc);
        __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 2].normal = CGL_vec4_init(normal.x, normal.y, normal.z, 0.0f);
        __CGL_WIDGETS_CURRENT_CONTEXT->vertices[__CGL_WIDGETS_CURRENT_CONTEXT->vertices_count - 1].normal = CGL_vec4_init(normal.x, normal.y, normal.z, 0.0f);
        vertex.normal = CGL_vec4_init(normal.x, normal.y, normal.z, 0.0f);
    }
    CGL_widgets_add_vertex(&vertex);
}

void CGL_widgets_add_vertex_p3ft2f(CGL_float pos_x, CGL_float pos_y, CGL_float pos_z, CGL_float tex_x, CGL_float tex_y)
{
    CGL_widgets_add_vertex_pt(CGL_vec3_init(pos_x, pos_y, pos_z), CGL_vec2_init(tex_x, tex_y));
}

void CGL_widgets_add_vertex_p(CGL_vec3 position)
{
    CGL_widgets_add_vertex_pt(position, CGL_vec2_init(0.0f, 0.0f));
}

void CGL_widgets_add_vertex_p3f(CGL_float pos_x, CGL_float pos_y, CGL_float pos_z)
{
    CGL_widgets_add_vertex_pt(CGL_vec3_init(pos_x, pos_y, pos_z), CGL_vec2_init(0.0f, 0.0f));
}

void CGL_widgets_add_vertex_p3ft(CGL_float pos_x, CGL_float pos_y, CGL_float pos_z, CGL_vec2 tex_coord)
{
    CGL_widgets_add_vertex_pt(CGL_vec3_init(pos_x, pos_y, pos_z), tex_coord);
}

void CGL_widgets_add_vertex_pt2f(CGL_vec3 position, CGL_float tex_x, CGL_float tex_y)
{
    CGL_widgets_add_vertex_pt(position, CGL_vec2_init(tex_x, tex_y));
}

void CGL_widgets_set_stroke_color(CGL_color color)
{
    __CGL_WIDGETS_CURRENT_CONTEXT->stroke_color = CGL_vec4_init(color.x, color.y, color.z, color.w);
}

void CGL_widgets_set_stroke_colorf(CGL_float r, CGL_float g, CGL_float b, CGL_float a)
{
    __CGL_WIDGETS_CURRENT_CONTEXT->stroke_color = CGL_vec4_init(r, g, b, a);
}

void CGL_widgets_set_stroke_thicnkess(CGL_float thickness)
{
    __CGL_WIDGETS_CURRENT_CONTEXT->stroke_thickness = thickness;
}

void CGL_widgets_set_fill_color(CGL_color color)
{
    __CGL_WIDGETS_CURRENT_CONTEXT->fill_color = CGL_vec4_init(color.x, color.y, color.z, color.w);
}

void CGL_widgets_set_fill_colorf(CGL_float r, CGL_float g, CGL_float b, CGL_float a)
{
    __CGL_WIDGETS_CURRENT_CONTEXT->fill_color = CGL_vec4_init(r, g, b, a);
}

void CGL_widgets_set_fill_mode(bool is_enabled)
{
    __CGL_WIDGETS_CURRENT_CONTEXT->is_fill = is_enabled;
}

void __CGL_widgets_add_triangle_filled(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
{
    CGL_widgets_add_vertex_p(a);
    CGL_widgets_add_vertex_p(b);
    CGL_widgets_add_vertex_p(c);
}

static void __CGL_widgets_add_triangle_stroked(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
{
    CGL_widgets_add_line(a, b);
    CGL_widgets_add_line(b, c);
    CGL_widgets_add_line(a, c);
}

void CGL_widgets_add_triangle(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
{
    if(__CGL_WIDGETS_CURRENT_CONTEXT->is_fill) __CGL_widgets_add_triangle_filled(a, b, c);
    else __CGL_widgets_add_triangle_stroked(a, b, c);
}

static void __CGL_widgets_add_quad_filled(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d)
{
    CGL_widgets_add_vertex_pt2f(a, 0.0f, 0.0f);
    CGL_widgets_add_vertex_pt2f(c, 1.0f, 1.0f);
    CGL_widgets_add_vertex_pt2f(d, 0.0f, 1.0f);
    CGL_widgets_add_vertex_pt2f(a, 0.0f, 0.0f);
    CGL_widgets_add_vertex_pt2f(b, 1.0f, 0.0f);
    CGL_widgets_add_vertex_pt2f(c, 1.0f, 1.0f);
}

static void __CGL_widgets_add_quad_stroked(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d)
{
    CGL_widgets_add_line(a, b);
    CGL_widgets_add_line(b, c);
    CGL_widgets_add_line(c, d);
    CGL_widgets_add_line(a, d);
}

void CGL_widgets_add_quad(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c, CGL_vec3 d)
{
    if(__CGL_WIDGETS_CURRENT_CONTEXT->is_fill) __CGL_widgets_add_quad_filled(a, b, c, d);
    else __CGL_widgets_add_quad_stroked(a, b, c, d);
}

void CGL_widgets_add_quad_8f(CGL_float ax, CGL_float ay, CGL_float bx, CGL_float by, CGL_float cx, CGL_float cy, CGL_float dx, CGL_float dy)
{
    CGL_widgets_add_quad(
        CGL_vec3_init(ax, ay, 0.0f),
        CGL_vec3_init(bx, by, 0.0f),
        CGL_vec3_init(cx, cy, 0.0f),
        CGL_vec3_init(dx, dy, 0.0f)
        );
}

void CGL_widgets_add_line(CGL_vec3 start, CGL_vec3 end)
{
    CGL_vec3 a, b, c, d;

    CGL_float original_slope = (end.y - start.y) / (end.x - start.x);
    CGL_float perp_slope = -1.0f / original_slope;
    CGL_float angle = atanf(perp_slope);
    CGL_float thickness = __CGL_WIDGETS_CURRENT_CONTEXT->stroke_thickness;
    CGL_float r = thickness / 2.0f;

    a = CGL_vec3_init(start.x + r * cosf(angle), start.y + r * sinf(angle), start.z);
    b = CGL_vec3_init(start.x - r * cosf(angle), start.y - r * sinf(angle), start.z);

    c = CGL_vec3_init(end.x - r * cosf(angle), end.y - r * sinf(angle), end.z);
    d = CGL_vec3_init(end.x + r * cosf(angle), end.y + r * sinf(angle), end.z);

    bool was_fill = __CGL_WIDGETS_CURRENT_CONTEXT->is_fill;
    __CGL_WIDGETS_CURRENT_CONTEXT->is_fill = false;
    __CGL_widgets_add_quad_filled(a, b, c, d);
    __CGL_WIDGETS_CURRENT_CONTEXT->is_fill = was_fill;
}

void CGL_widgets_add_line2f(CGL_float start_x, CGL_float start_y, CGL_float end_x, CGL_float end_y)
{
    CGL_widgets_add_line(
        CGL_vec3_init(start_x, start_y, 0.0f),
        CGL_vec3_init(end_x, end_y, 0.0f)
        );
}

void CGL_widgets_add_rect(CGL_vec3 start, CGL_vec2 size)
{
    CGL_widgets_add_quad(
        CGL_vec3_init(start.x, start.y, start.z),
        CGL_vec3_init(start.x + size.x, start.y, start.z),
        CGL_vec3_init(start.x + size.x, start.y + size.y, start.z),
        CGL_vec3_init(start.x, start.y + size.y, start.z)
    );
}

void CGL_widgets_add_rect2f(CGL_float start_x, CGL_float start_y, CGL_float size_x, CGL_float size_y)
{
    CGL_widgets_add_quad(
        CGL_vec3_init(start_x, start_y, 0.0f),
        CGL_vec3_init(start_x + size_x, start_y, 0.0f),
        CGL_vec3_init(start_x + size_x, start_y + size_y, 0.0f),
        CGL_vec3_init(start_x, start_y + size_y, 0.0f)
    );
}

void __CGL_widgets_add_oval_filled(CGL_vec3 position, CGL_vec2 radius)
{
    CGL_float x = 0.0f, y = 0.0f;
    for(CGL_float i =0; i <= 360;)
    {
        x = radius.x * cosf(i);
        y = radius.y * sinf(i);
        CGL_widgets_add_vertex_p3f(x + position.x, y + position.y, position.z);
        i = i + 0.5f;
        x = radius.x * cosf(i);
        y = radius.y * sinf(i);
        CGL_widgets_add_vertex_p3f(x + position.x, y + position.y, position.z);
        CGL_widgets_add_vertex_p3f(position.x, position.y, position.z);
        i = i + 0.5f;
    }
}

void __CGL_widgets_add_oval_stroked(CGL_vec3 position, CGL_vec2 radius)
{
    CGL_LOG("void __CGL_widgets_add_oval_stroked(CGL_vec3 position, CGL_vec2 radius) not implemented\n");
}

void CGL_widgets_add_circle2f(CGL_float pos_x, CGL_float pos_y, CGL_float radius)
{
    if(__CGL_WIDGETS_CURRENT_CONTEXT->is_fill) __CGL_widgets_add_oval_filled(CGL_vec3_init(pos_x, pos_y, 0.0f), CGL_vec2_init(radius, radius));
    else __CGL_widgets_add_oval_stroked(CGL_vec3_init(pos_x, pos_y, 0.0f), CGL_vec2_init(radius, radius));
}

void CGL_widgets_add_oval(CGL_vec3 position, CGL_vec2 radius)
{
    if(__CGL_WIDGETS_CURRENT_CONTEXT->is_fill) __CGL_widgets_add_oval_filled(position, radius);
    else __CGL_widgets_add_oval_stroked(position, radius);
}

void CGL_widgets_add_oval2f(CGL_float pos_x, CGL_float pos_y, CGL_float radius_x, CGL_float radius_y)
{
    if(__CGL_WIDGETS_CURRENT_CONTEXT->is_fill) __CGL_widgets_add_oval_filled(CGL_vec3_init(pos_x, pos_y, 0.0f), CGL_vec2_init(radius_x, radius_y));
    else __CGL_widgets_add_oval_stroked(CGL_vec3_init(pos_x, pos_y, 0.0f), CGL_vec2_init(radius_x, radius_y));
}

static void __CGL_widgets_add_arc_filled2f(CGL_float pos_x, CGL_float pos_y, CGL_float radius, CGL_float start_angle, CGL_float end_angle, CGL_int resolution)
{
    CGL_float x = 0.0f, y = 0.0f;
    CGL_float angle = start_angle;
    CGL_float angle_step = (end_angle - start_angle) / resolution;
    for(CGL_int i = 0; i < resolution; i++)
    {
        x = radius * cosf(angle); y = radius * sinf(angle);
        CGL_widgets_add_vertex_p3f(x + pos_x, y + pos_y, 0.0f);
        angle += angle_step;
        x = radius * cosf(angle); y = radius * sinf(angle);
        CGL_widgets_add_vertex_p3f(x + pos_x, y + pos_y, 0.0f);
        CGL_widgets_add_vertex_p3f(pos_x, pos_y, 0.0f);
    }
}

static void __CGL_widgets_add_arc_stroked2f(CGL_float pos_x, CGL_float pos_y, CGL_float radius, CGL_float start_angle, CGL_float end_angle, CGL_int resolution)
{
    CGL_float x0 = 0.0f, y0 = 0.0f, x1 = 0.0f, y1 = 0.0f;
    CGL_float angle = start_angle;
    CGL_float angle_step = (end_angle - start_angle) / resolution;
    CGL_widgets_add_line2f(pos_x, pos_y, pos_x + radius * cosf(start_angle), pos_y + radius * sinf(start_angle));
    CGL_widgets_add_line2f(pos_x, pos_y, pos_x + radius * cosf(end_angle), pos_y + radius * sinf(end_angle));
    for(CGL_int i = 0; i < resolution; i++)
    {
        x0 = radius * cosf(angle); y0 = radius * sinf(angle);
        angle += angle_step;
        x1 = radius * cosf(angle); y1 = radius * sinf(angle);
        CGL_widgets_add_line2f(pos_x + x0, pos_y + y0, pos_x + x1, pos_y + y1);
    }
}

void CGL_widgets_add_arc2f(CGL_float pos_x, CGL_float pos_y, CGL_float radius, CGL_float start_angle, CGL_float end_angle, CGL_int resolution)
{
    if(__CGL_WIDGETS_CURRENT_CONTEXT->is_fill) __CGL_widgets_add_arc_filled2f(pos_x, pos_y, radius, start_angle, end_angle, resolution);
    else __CGL_widgets_add_arc_stroked2f(pos_x, pos_y, radius, start_angle, end_angle, resolution);
}


void CGL_widgets_add_circle(CGL_vec3 position, CGL_float radius)
{
    if(__CGL_WIDGETS_CURRENT_CONTEXT->is_fill) __CGL_widgets_add_oval_filled(position, CGL_vec2_init(radius, radius));
    else __CGL_widgets_add_oval_stroked(position, CGL_vec2_init(radius, radius));    
}


bool CGL_widgets_add_character(char c, CGL_float x, CGL_float y, CGL_float sx, CGL_float sy)
{
    bool was_fill = __CGL_WIDGETS_CURRENT_CONTEXT->is_fill;
    __CGL_WIDGETS_CURRENT_CONTEXT->is_fill = true;
    switch(c)
    {
        case '0':
        {            
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            return true;
        }
        case '1':
        {
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            return true;
        }
        case '2':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y + 0.5f * sy, sx * 0.25f, sy * 0.5f);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy * 0.5f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            return true;
        }
        case '3':
        {
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            return true;
        }
        case '4':
        {
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y + 0.5f * sy, sx * 0.25f, sy * 0.5f);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            return true;
        }
        case '5':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y + 0.5f * sy, sx * 0.25f, sy * 0.5f);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy * 0.5f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            return true;
        }
        case '6':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy * 0.5f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            return true;
        }
        case '7':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            return true;
        }
        case '8':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            return true;
        }
        case '9':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y + 0.5f * sy, sx * 0.25f, sy * 0.5f);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            return true;
        }
        case 'a':
        case 'A':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            return true;
        }
        case 'b':
        case 'B':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            return true;
        }
        case 'c':
        case 'C':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            return true;
        }
        case 'd':
        case 'D':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            return true;
        }
        case 'e':
        case 'E':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            return true;
        }
        case 'f':
        case 'F':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            return true;
        }
        case 'g':
        case 'G':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy * 0.5f);
            CGL_widgets_add_rect2f(x + 0.5f * sx, y + (0.5f - 0.125f) * sy, sx * 0.5f, sy * 0.25f);
            return true;
        }
        case 'h':
        case 'H':
        {
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            return true;
        }
        case 'i':
        case 'I':
        {
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            return true;
        }
        case 'j':
        case 'J':
        {
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy * 0.5f);
            return true;
        }
        case 'k':
        case 'K':
        {
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_quad_8f(
                x, y + (0.5f + 0.15f) * sy,
                x, y + (0.5f - 0.15f) * sy,
                x + sx, y + 0.85f * sy,
                x + 0.85f * sx, y + sy
            );
            CGL_widgets_add_quad_8f(
                x, y + (0.5f + 0.15f) * sy,
                x, y + (0.5f - 0.15f) * sy,
                x + 0.85f * sx, y,
                x + sx, y + 0.15f * sy
            );
            return true;
        }
        case 'l':
        case 'L':
        {
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            return true;
        }
        case 'm':
        case 'M':
        {
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x + (0.5f - 0.125f) * sx, y + 0.5f * sy, sx * 0.25f, sy * 0.5f);
            return true;
        }
        case 'n':
        case 'N':
        {
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_quad_8f(
                x + 0.15f * sx, y + sy,
                x, y + 0.85f * sy,
                x + 0.85f * sx, y,
                x + sx, y + 0.15f * sy
            );
            return true;
        }
        case 'o':
        case 'O':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            return true;
        }
        case 'p':
        case 'P':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y + 0.5f * sy, sx * 0.25f, sy * 0.5f);
            return true;
        }
        case 'q':
        case 'Q':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x + (0.5f - 0.125f) * sx, y, sx * 0.25f, sy * 0.5f);
            return true;
        }
        case 'r':
        case 'R':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y + 0.5f * sy, sx * 0.25f, sy * 0.5f);
            CGL_widgets_add_quad_8f(
                x, y + (0.5f + 0.15f) * sy,
                x, y + (0.5f - 0.15f) * sy,
                x + 0.85f * sx, y,
                x + sx, y + 0.15f * sy
            );
            return true;
        }
        case 's':
        case 'S':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy * 0.5f);
            CGL_widgets_add_rect2f(x, y + 0.5f * sy, sx * 0.25f, sy * 0.5f);
            return true;
        }
        case 't':
        case 'T':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x + (0.5f - 0.125f) * sx, y, sx * 0.25f, sy);
            return true;
        }
        case 'u':
        case 'U':
        {
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            return true;
        }
        case 'v':
        case 'V':
        {
            CGL_widgets_add_quad_8f(
                x + sx, y + 0.85f * sy,
                x + 0.85f * sx, y + sy,
                x + (0.5f - 0.125f) * sx, y,
                x + (0.5f + 0.125f) * sx, y
            );
            CGL_widgets_add_quad_8f(
                x + 0.15f * sx, y + sy,
                x, y + 0.85f * sy,
                x + (0.5f - 0.125f) * sx, y,
                x + (0.5f + 0.125f) * sx, y
            );
            return true;
        }
        case 'w':
        case 'W':
        {
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x + (0.5f - 0.125f) * sx, y, sx * 0.25f, sy * 0.5f);
            return true;
        }
        case 'x':
        case 'X':
        {
            CGL_widgets_add_quad_8f(
                x + 0.15f * sx, y + sy,
                x, y + 0.85f * sy,
                x + 0.85f * sx, y,
                x + sx, y + 0.15f * sy
            );
            CGL_widgets_add_quad_8f(
                x + 0.85f * sx, y + sy,
                x + sx, y + 0.85f * sy,
                x + 0.15f * sx, y,
                x, y + 0.15f * sy
            );
            return true;
        }
        case 'y':
        case 'Y':
        {
            CGL_widgets_add_rect2f(x, y + 0.5f * sy, sx * 0.25f, sy * 0.5f);
            CGL_widgets_add_rect2f(x + sx * 0.75f, y, sx * 0.25f, sy);
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            return true;
        }
        case 'z':
        case 'Z':
        {
            CGL_widgets_add_rect2f(x, y + sy * 0.75f, sx, sy * 0.25f);
            CGL_widgets_add_rect2f(x, y, sx, sy * 0.25f);
            CGL_widgets_add_quad_8f(
                x + 0.85f * sx, y + sy,
                x + sx, y + 0.85f * sy,
                x + 0.15f * sx, y,
                x, y + 0.15f * sy
            );
            return true;
        }
        case '-':
        {
            CGL_widgets_add_rect2f(x, y + (0.5f - 0.125f) * sy, sx, sy * 0.25f);
            return true;
        }
        case '.':
        {
            CGL_widgets_add_rect2f(x + (0.5f - 0.125f) * sx, y, sx * 0.25f, sy * 0.25f);
            return true;
        }
        case ':':
        {
            CGL_widgets_add_rect2f(x + (0.5f - 0.125f) * sx, y + 0.1f * sy , sx * 0.25f, sy * 0.25f);
            CGL_widgets_add_rect2f(x + (0.5f - 0.125f) * sx, y + (0.1f + 0.25f + 0.1f) * sy , sx * 0.25f, sy * 0.25f);
            return true;
        }
        case ' ':
        {
            return true;
        }
    }
    __CGL_WIDGETS_CURRENT_CONTEXT->is_fill = was_fill;
    // return false;
    return true;
}

bool CGL_widgets_add_string(const char* str, CGL_float x, CGL_float y, CGL_float sx, CGL_float sy)
{
    const CGL_float line_gap = 0.01f;
    const CGL_float char_gap = 0.01f;
    CGL_int length = (int)strlen(str);
    CGL_int line_length = 0;
    CGL_int line_length_max = 0;
    CGL_int num_lines = 1;
    CGL_float ix = x, iy = y + sy;
    for(CGL_int i = 0; i < length ; i++)
    {
        line_length++;
        if(str[i] == '\n')
        {
            line_length_max = CGL_utils_max(line_length, line_length_max);
            line_length = 0;
            num_lines ++;
        }        
    }
    line_length_max = CGL_utils_max(line_length, line_length_max);
    CGL_float character_sx =  sx / line_length - char_gap;
    CGL_float character_sy = (sy - line_gap * num_lines) / (num_lines);
    y += sy - character_sy;
    for(CGL_int i = 0 ; i < length ; i ++)
    {
        if(str[i] == '\n')
        {
            y = y - line_gap - character_sy;
            x = ix;
            continue;
        }
        if(!CGL_widgets_add_character(str[i], x, y, character_sx, character_sy)) return false;
        x = x + character_sx + char_gap;
    }
    return true;
}


void CGL_widgets_add_shape_out_line(CGL_shape* shape)
{
    for(CGL_int i = 0 ; i < shape->vertices_count ; i++)
    {
        CGL_vec3 p0 = CGL_vec3_apply_transformations(shape->vertices[i], &shape->position, &shape->rotation, &shape->scale);
        CGL_vec3 p1 = CGL_vec3_apply_transformations(shape->vertices[(i + 1) % shape->vertices_count], &shape->position, &shape->rotation, &shape->scale);
        CGL_widgets_add_line(p0, p1);
    }
}

void CGL_widgets_add_cubic_bazier(CGL_vec3 start, CGL_vec3 end, CGL_vec3 control_1, CGL_vec3 control_2, CGL_int resolution)
{
    CGL_float step_size = 1.0f / (CGL_float)resolution;
    CGL_float t = 0.0f;
    CGL_vec2 p = CGL_vec2_init(0.0f, 0.0f);
    CGL_vec2 p_old = CGL_vec2_init(0.0f, 0.0f);
    for(CGL_int i = 0 ; i <= resolution; i++)
    {
        p.x = CGL_float_cubic_lerp(start.x, end.x, control_1.x, control_2.x, t);
        p.y = CGL_float_cubic_lerp(start.y, end.y, control_1.y, control_2.y, t);
        if(i > 0) CGL_widgets_add_line2f(p_old.x, p_old.y, p.x, p.y);
        p_old = p;
        t += step_size;
    }
}

void CGL_widgets_add_cubic_bazier2v(CGL_vec2 start, CGL_vec2 end, CGL_vec2 control_1, CGL_vec2 control_2, CGL_int resolution)
{
    CGL_widgets_add_cubic_bazier(
        CGL_vec3_init(start.x, start.y, 0.0f),
        CGL_vec3_init(end.x, end.y, 0.0f),
        CGL_vec3_init(control_1.x, control_1.y, 0.0f),
        CGL_vec3_init(control_2.x, control_2.y, 0.0f),
        resolution
    );
}

void CGL_widgets_add_cubic_bazier2f(CGL_float start_x, CGL_float start_y, CGL_float end_x, CGL_float end_y, CGL_float control_1_x, CGL_float control_1_y, CGL_float control_2_x, CGL_float control_2_y, CGL_int resolution)
{
    CGL_widgets_add_cubic_bazier(
        CGL_vec3_init(start_x, start_y, 0.0f),
        CGL_vec3_init(end_x, end_y, 0.0f),
        CGL_vec3_init(control_1_x, control_1_y, 0.0f),
        CGL_vec3_init(control_2_x, control_2_y, 0.0f),
        resolution
    );
}

void CGL_widgets_add_cubic_bazier_points(CGL_vec3 start, CGL_vec3 end, CGL_vec3 control_1, CGL_vec3 control_2, CGL_int resolution)
{
    CGL_float step_size = 1.0f / (CGL_float)resolution;
    CGL_float t = 0.0f;
    CGL_vec2 p = CGL_vec2_init(0.0f, 0.0f);
    for(CGL_int i = 0 ; i <= resolution; i++)
    {
        p.x = CGL_float_cubic_lerp(start.x, end.x, control_1.x, control_2.x, t);
        p.y = CGL_float_cubic_lerp(start.y, end.y, control_1.y, control_2.y, t);
        CGL_widgets_add_circle2f(p.x, p.y, 0.02f);
        t += step_size;
    }
}

void CGL_widgets_add_cubic_bazier_points2v(CGL_vec2 start, CGL_vec2 end, CGL_vec2 control_1, CGL_vec2 control_2, CGL_int resolution)
{
    CGL_widgets_add_cubic_bazier_points(
        CGL_vec3_init(start.x, start.y, 0.0f),
        CGL_vec3_init(end.x, end.y, 0.0f),
        CGL_vec3_init(control_1.x, control_1.y, 0.0f),
        CGL_vec3_init(control_2.x, control_2.y, 0.0f),
        resolution
    );
}

void CGL_widgets_add_cubic_bazier_points2f(CGL_float start_x, CGL_float start_y, CGL_float end_x, CGL_float end_y, CGL_float control_1_x, CGL_float control_1_y, CGL_float control_2_x, CGL_float control_2_y, CGL_int resolution)
{
    CGL_widgets_add_cubic_bazier_points(
        CGL_vec3_init(start_x, start_y, 0.0f),
        CGL_vec3_init(end_x, end_y, 0.0f),
        CGL_vec3_init(control_1_x, control_1_y, 0.0f),
        CGL_vec3_init(control_2_x, control_2_y, 0.0f),
        resolution
    );
}

void CGL_widgets_add_plot_function(CGL_float start_x, CGL_float start_y, CGL_float size_x, CGL_float size_y, CGL_float(*func_to_plot)(CGL_float), CGL_int num_samples, CGL_float x_min, CGL_float x_max, CGL_float y_min, CGL_float y_max, CGL_float plot_thickness, CGL_vec3 plot_color, CGL_bool draw_axes, CGL_float axes_thickness, CGL_vec3 axes_color)
{
    CGL_float step_size = (x_max - x_min) / (CGL_float)num_samples;
    CGL_float step_size_plot = size_x / (CGL_float)num_samples;
    
    CGL_vec2 prev_point = CGL_vec2_init(0.0f, 0.0f);
    CGL_vec2 curr_point = CGL_vec2_init(0.0f, 0.0f);

    prev_point.x = x_min;
    prev_point.y = func_to_plot(x_min);

    CGL_widgets_set_stroke_colorf(plot_color.x, plot_color.y, plot_color.z, 1.0f);
    CGL_widgets_set_stroke_thicnkess(plot_thickness);
    for(CGL_int i = 1 ; i < num_samples; i++)
    {
        curr_point.x = x_min + (step_size * (CGL_float)i);
        curr_point.y = CGL_utils_clamp(func_to_plot(curr_point.x), y_min, y_max);

        CGL_widgets_add_line2f(
            start_x + (step_size_plot * (CGL_float)i),
            start_y + (size_y * (prev_point.y - y_min) / (y_max - y_min)),
            start_x + (step_size_plot * (CGL_float)(i + 1)),
            start_y + (size_y * (curr_point.y - y_min) / (y_max - y_min))
        );

        prev_point = curr_point;
    }


    if(draw_axes)
    {
        CGL_widgets_set_stroke_colorf(axes_color.x, axes_color.y, axes_color.z, 1.0f);
        CGL_widgets_set_stroke_thicnkess(plot_thickness);
        if(y_min < 0.0f && y_max > 0.0f)
        {
            CGL_widgets_add_line2f(
                start_x,
                start_y + (size_y * fabsf(y_min) / (y_max - y_min)),
                start_x + size_x,
                start_y + (size_y * fabsf(y_min) / (y_max - y_min))
            );
        }
        if(x_min < 0.0f && x_max > 0.0f)
        {
            CGL_widgets_add_line2f(
                start_x + (size_x * fabsf(x_min) / (x_max - x_min)),
                start_y,
                start_x + (size_x * fabsf(x_min) / (x_max - x_min)),
                start_y + size_y
            );
        }
    }
}

void CGL_widgets_add_plot_array(CGL_float start_x, CGL_float start_y, CGL_float size_x, CGL_float size_y, CGL_vec2* values, CGL_sizei count, CGL_float marker_size, CGL_vec3 marker_color, CGL_bool draw_axes, CGL_float axes_thickness, CGL_vec3 axes_color)
{
    CGL_vec2 x_min_max = CGL_vec2_init(values[0].x, values[0].x);
    CGL_vec2 y_min_max = CGL_vec2_init(values[0].y, values[0].y);
    for(CGL_sizei i = 1; i < count; i++)
    {
        x_min_max.x = CGL_utils_min(x_min_max.x, values[i].x);
        x_min_max.y = CGL_utils_max(x_min_max.y, values[i].x);
        y_min_max.x = CGL_utils_min(y_min_max.x, values[i].y);
        y_min_max.y = CGL_utils_max(y_min_max.y, values[i].y);
    }

    CGL_widgets_set_fill_colorf(marker_color.x, marker_color.y, marker_color.z, 1.0f);
    for(CGL_sizei i = 0; i < count; i++)
    {
        CGL_widgets_add_rect2f(
            start_x + (size_x * (values[i].x - x_min_max.x) / (x_min_max.y - x_min_max.x)),
            start_y + (size_y * (values[i].y - y_min_max.x) / (y_min_max.y - y_min_max.x)),
            marker_size, marker_size
        );
    }

    if(draw_axes)
    {
        CGL_widgets_set_stroke_colorf(axes_color.x, axes_color.y, axes_color.z, 1.0f);
        CGL_widgets_set_stroke_thicnkess(axes_thickness);
        if(y_min_max.x < 0.0f && y_min_max.y > 0.0f)
        {
            CGL_widgets_add_line2f(
                start_x,
                start_y + (size_y * fabsf(y_min_max.x) / (y_min_max.y - y_min_max.x)),
                start_x + size_x,
                start_y + (size_y * fabsf(y_min_max.x) / (y_min_max.y - y_min_max.x))
            );
        }
        if(x_min_max.x < 0.0f && x_min_max.y > 0.0f)
        {
            CGL_widgets_add_line2f(
                start_x + (size_x * fabsf(x_min_max.x) / (x_min_max.y - x_min_max.x)),
                start_y,
                start_x + (size_x * fabsf(x_min_max.x) / (x_min_max.y - x_min_max.x)),
                start_y + size_y
            );
        }
    }
    
}

void CGL_widgets_add_plot_pie_chart(CGL_float start_x, CGL_float start_y, CGL_float radius, CGL_float* values, CGL_vec3* colors, CGL_sizei count, CGL_int resolution)
{
    CGL_float total = 0.0f;
    for(CGL_sizei i = 0; i < count; i++)
    {
        total += values[i];
    }

    CGL_float curr_angle = 0.0f;
    for(CGL_sizei i = 0; i < count; i++)
    {
        CGL_widgets_set_fill_colorf(colors[i].x, colors[i].y, colors[i].z, 1.0f);
        CGL_widgets_add_arc2f(
            start_x, start_y, radius, curr_angle, curr_angle + (values[i] / total) * CGL_2PI, (CGL_int)(resolution * (values[i] / total) + 1.0f)
        );
        curr_angle += (values[i] / total) * CGL_2PI;
    }
}

void CGL_widgets_add_bar_graph(CGL_float start_x, CGL_float start_y, CGL_float size_x, CGL_float size_y, CGL_float* values, CGL_vec3* colors, CGL_sizei count, CGL_bool draw_axes, CGL_float axes_thickness, CGL_vec3 axes_color, CGL_bool vertical)
{
    CGL_float max_value = values[0];
    for(CGL_sizei i = 1; i < count; i++) max_value = CGL_utils_max(max_value, values[i]);

    CGL_float bar_width = ((vertical ? size_x : size_y) / (CGL_float)(count + 2)) * 0.8f;
    CGL_float bar_spacing = ((vertical ? size_x : size_y) / (CGL_float)(count + 2)) * 0.2f;

    CGL_float curr_x = start_x + bar_spacing;
    CGL_float curr_y = start_y + bar_spacing;
    CGL_float total_spacing = bar_spacing;

    for(CGL_sizei i = 0; i < count; i++)
    {
        CGL_widgets_set_fill_colorf(colors[i].x, colors[i].y, colors[i].z, 1.0f);
        if(vertical)
        {
            CGL_widgets_add_rect2f(curr_x, start_y + bar_spacing, bar_width, (size_y - bar_spacing * 2.0f) * (values[i] / max_value));
            curr_x += bar_width + bar_spacing;
        }
        else
        {
            CGL_widgets_add_rect2f(start_x + bar_spacing, curr_y, (size_x - bar_spacing * 2.0f) * (values[i] / max_value), bar_width);
            curr_y += bar_width + bar_spacing;
        }
    }

    if(draw_axes)
    {
        CGL_widgets_set_stroke_colorf(axes_color.x, axes_color.y, axes_color.z, 1.0f);
        CGL_widgets_set_stroke_thicnkess(axes_thickness);
        CGL_widgets_add_line2f(start_x + bar_spacing, start_y, start_x + bar_spacing, start_y + size_y);
        CGL_widgets_add_line2f(start_x, start_y + bar_spacing, start_x + size_x, start_y + bar_spacing);
    }
}

#endif
#endif

#ifndef CGL_EXCLUDE_NODE_EDITOR

struct CGL_node_editor
{
    CGL_node_editor_input* input;
    CGL_node_editor_pin* start_pin;
    CGL_node_editor_pin* end_pin;
    void(*ondrop)(float, float, CGL_node_editor_pin*);
    void(*onconnect)(CGL_node_editor_pin*, CGL_node_editor_pin*);
    CGL_float mouse_pos_x;
    CGL_float mouse_pos_y;
    CGL_float mouse_delta_x;
    CGL_float mouse_delta_y;
    CGL_float offset_x;
    CGL_float offset_y;
    CGL_float aspect_ratio;
    CGL_float scale;
    CGL_int num_selected;
    bool has_lifted;
};

CGL_node_editor* CGL_node_editor_create()
{
    CGL_node_editor* editor = (CGL_node_editor*)CGL_malloc(sizeof(CGL_node_editor));
    if(!editor) return NULL;
    editor->mouse_pos_x = 0.0f;
    editor->mouse_pos_y = 0.0f;
    editor->mouse_delta_x = 0.0f;
    editor->mouse_delta_y = 0.0f;
    editor->offset_x = 0.0f;
    editor->offset_y = 0.0f;
    editor->input = NULL;
    editor->aspect_ratio = 1.0f;
    editor->scale = 1.0f;
    editor->start_pin = editor->end_pin = NULL;
    editor->ondrop = NULL;
    editor->onconnect = NULL;
    editor->has_lifted = true;
    return editor;
}

void CGL_node_editor_destroy(CGL_node_editor* editor)
{
    CGL_free(editor);
}

void CGL_node_editor_set_on_connect(CGL_node_editor* editor, void(*onconnect)(CGL_node_editor_pin*, CGL_node_editor_pin*))
{
    editor->onconnect = onconnect;
}

void CGL_node_editor_set_on_drop(CGL_node_editor* editor, void(*ondrop)(float, float, CGL_node_editor_pin*))
{
    editor->ondrop = ondrop;
}

void CGL_node_editor_update(CGL_node_editor* editor, CGL_node_editor_input* input)
{
    editor->input = input;
    editor->mouse_delta_x = CGL_utils_clamp(input->mouse_pos_x - editor->mouse_pos_x, -0.2f, 0.2f);
    editor->mouse_delta_y = CGL_utils_clamp(input->mouse_pos_y - editor->mouse_pos_y, -0.2f, 0.2f);
    editor->mouse_pos_x = input->mouse_pos_x;
    editor->mouse_pos_y = input->mouse_pos_y;
    editor->aspect_ratio = input->aspect_ratio;
    editor->scale = CGL_utils_max(input->scale, 0.01f);
    if (editor->start_pin && editor->end_pin) { if (editor->onconnect) editor->onconnect(editor->start_pin, editor->end_pin); editor->start_pin = editor->end_pin = NULL; editor->has_lifted = false; }
    if (editor->start_pin && !input->mouse_button_left) { if (editor->ondrop) editor->ondrop(editor->mouse_pos_x / editor->scale, editor->mouse_pos_y / editor->scale, editor->start_pin); editor->start_pin = NULL;  }
    if (!input->mouse_button_left) editor->has_lifted = true;
    if(input->mouse_button_middle)
    {
        editor->offset_x += editor->mouse_delta_x * editor->scale;
        editor->offset_y += editor->mouse_delta_y * editor->scale;
    }
    editor->num_selected = 0;
}

void CGL_node_editor_render_begin(CGL_node_editor* editor)
{
    CGL_widgets_begin_int(editor->scale, editor->scale, 0.0f, 0.0f);
    CGL_widgets_set_stroke_thicnkess(0.03f);
}

void CGL_node_editor_clear_focused_pins(CGL_node_editor* editor)
{
    editor->start_pin = editor->end_pin = NULL;
}

void CGL_node_editor_render_end(CGL_node_editor* editor)
{
    if(editor->start_pin && !editor->end_pin)
    {
        CGL_widgets_set_stroke_colorf(1.0f, 1.0f, 0.0f, 1.0f);
        CGL_widgets_add_line(
            CGL_vec3_init(editor->offset_x + editor->start_pin->pos_x, editor->offset_y + editor->start_pin->pos_y, 0.0f),
            CGL_vec3_init(editor->mouse_pos_x / editor->scale, editor->mouse_pos_y / editor->scale, 0.0f)
        );
    }
    CGL_widgets_end();
}

void CGL_node_editor_set_offset(CGL_node_editor* editor, CGL_float x, CGL_float y)
{
    editor->offset_x = x;
    editor->offset_y = y;
}

void CGL_node_editor_get_offset(CGL_node_editor* editor, CGL_float* x, CGL_float* y)
{
    if(x) *x = editor->offset_x;
    if(y) *y = editor->offset_y;
}

void CGL_node_editor_get_linked_pins(CGL_node_editor* editor, CGL_node_editor_pin** x, CGL_node_editor_pin** y)
{
    if(x) *x = editor->start_pin;
    if(y) *y = editor->end_pin;
}

void CGL_node_editor_render_link(CGL_node_editor_pin* left, CGL_node_editor_pin* right, CGL_color color, CGL_float midper)
{
    if(!left || !right) return;
    CGL_widgets_set_stroke_color(color);
    
    CGL_float mx = CGL_utils_mix(left->pos_x, right->pos_x, midper);

    CGL_float ofx = right->parent->editor->offset_x;
    CGL_float ofy = right->parent->editor->offset_y;

    CGL_widgets_add_line(
        CGL_vec3_init(ofx + left->pos_x, ofy + left->pos_y, 0.0f),
        CGL_vec3_init(ofx + mx, ofy + left->pos_y, 0.0f)
    );

    CGL_widgets_add_line(
        CGL_vec3_init(ofx + mx, ofy + left->pos_y, 0.0f),
        CGL_vec3_init(ofx + mx, ofy + right->pos_y, 0.0f)
    );

    CGL_widgets_add_line(
        CGL_vec3_init(ofx + mx, ofy + right->pos_y, 0.0f),
        CGL_vec3_init(ofx + right->pos_x, ofy + right->pos_y, 0.0f)
    );
}

void CGL_node_editor_render_linkf(CGL_node_editor_pin* left, CGL_node_editor_pin* right, CGL_float cr, CGL_float cg, CGL_float cb, CGL_float ca, CGL_float midper)
{
    CGL_node_editor_render_link(left, right, CGL_vec4_init(cr, cg, cb, ca), midper);
}

void CGL_node_editor_render_link_curved(CGL_node_editor_pin* left, CGL_node_editor_pin* right, CGL_color color, CGL_float x_dist, CGL_float y_dist, CGL_int resolution)
{
    if(!left || !right) return;
    
    
    CGL_float y_length = fabsf(right->pos_y - left->pos_y);
    
    CGL_float ofx = right->parent->editor->offset_x;
    CGL_float ofy = right->parent->editor->offset_y;

    CGL_float mx0 = CGL_utils_mix(left->pos_x, right->pos_x, x_dist);
    CGL_float my0 = CGL_utils_clamp(y_length * y_dist, 0.0f, 0.1f);

    CGL_float mx1 = CGL_utils_mix(left->pos_x, right->pos_x, (1.0f - x_dist));
    CGL_float my1 = CGL_utils_clamp(y_length * (1.0f - y_dist), 0.0f, 0.1f);



    CGL_vec2 end = CGL_vec2_init(ofx + right->pos_x, ofy + right->pos_y);
    CGL_vec2 start = CGL_vec2_init(ofx + left->pos_x, ofy + left->pos_y);
    CGL_vec2 control0 = CGL_vec2_init(ofx + mx0, ofy + left->pos_y + my0);
    CGL_vec2 control1 = CGL_vec2_init(ofx + mx1, ofy + right->pos_y - my1);

    CGL_widgets_set_stroke_color(color);
    CGL_widgets_add_cubic_bazier2v(start, end, control0, control1, resolution);
}

void CGL_node_editor_render_link_curvedf(CGL_node_editor_pin* left, CGL_node_editor_pin* right, CGL_float cr, CGL_float cg, CGL_float cb, CGL_float ca, CGL_float x_dist, CGL_float y_dist, CGL_int resolution)
{
    CGL_node_editor_render_link_curved(left, right, CGL_vec4_init(cr, cg, cb, ca), x_dist, y_dist, resolution);
}

void CGL_node_editor_node_init(CGL_node_editor* editor, CGL_node_editor_node* node)
{
    node->editor = editor;
    node->color = CGL_utils_random_color();
    strcpy(node->title, "Node");
    node->render_title = true;
    node->pins_count[0] = node->pins_count[1] = 0;
    node->pos_y = node->pos_x = 0.0f;
    node->size_y = node->size_x = 0.3f;
    node->selected = false;
    memset(node->left_pins, 0, sizeof(CGL_node_editor_pin) * CGL_NODE_EDITOR_NODE_MAX_PINS);
    memset(node->right_pins, 0, sizeof(CGL_node_editor_pin) * CGL_NODE_EDITOR_NODE_MAX_PINS);
}

static bool __CGL_node_editor_pin_update(CGL_node_editor_pin* pin)
{
    CGL_node_editor_node* node = pin->parent;
    CGL_node_editor* editor = node->editor;
    CGL_float sx = 0.05f / pin->parent->editor->input->aspect_ratio;
    CGL_float sy = 0.05f;

    bool focused = false;
    CGL_float ofx = node->editor->offset_x;
    CGL_float ofy = node->editor->offset_y;

    if(editor->input->mouse_button_left
    && CGL_utils_is_point_in_rect(
        editor->input->mouse_pos_x, editor->input->mouse_pos_y,
        ofx + pin->pos_x, ofy + pin->pos_y,
        sx, sy,
        editor->scale, editor->scale
    )) focused = true;

    if(focused && editor->has_lifted)
    {
        if(editor->start_pin && pin != editor->start_pin) editor->end_pin = pin;
        else if(editor->start_pin == NULL) { editor->start_pin = pin; editor->end_pin = NULL;}
    }

    return focused;
}

void CGL_node_editor_node_update(CGL_node_editor_node* node)
{
    const CGL_node_editor* editor = node->editor;
    node->pins_count[0] = node->pins_count[1] = 0;
    for(CGL_int i = 0 ; i < CGL_NODE_EDITOR_NODE_MAX_PINS ; i++) 
    {
        if(node->left_pins[i].is_set) node->pins_count[0]++;
        if(node->right_pins[i].is_set) node->pins_count[1]++;
    }    
    CGL_float ofx = node->editor->offset_x;
    CGL_float ofy = node->editor->offset_y;
    CGL_float sx = node->size_x;
    CGL_float sy = node->size_y = (0.1f + (float)CGL_utils_max(node->pins_count[0], node->pins_count[1]) * (0.05f + 0.1f));
    if(node->render_title) sy += 0.1f;
    bool require_movement = false;
    if(editor->input->mouse_button_left && !editor->input->shift) node->selected = false;
    require_movement = node->selected && editor->input->ctrl; 
    if(editor->input->mouse_button_left && !editor->start_pin
    && CGL_utils_is_point_in_rect(
        editor->input->mouse_pos_x, editor->input->mouse_pos_y,
        ofx + node->pos_x, ofy + node->pos_y,
        sx, sy,
        editor->scale, editor->scale
    ))  require_movement = node->selected = true;
    if(node->selected) node->editor->num_selected++;

    // TODO: try to do it in the loop above
    for(CGL_int i = 0 ; i < CGL_NODE_EDITOR_NODE_MAX_PINS ; i++) 
    {
        if(node->left_pins[i].is_set) 
            require_movement &= !__CGL_node_editor_pin_update(&node->left_pins[i]);
        if(node->right_pins[i].is_set)
            require_movement &= !__CGL_node_editor_pin_update(&node->right_pins[i]);
    }    

    if(require_movement)
    {
        node->pos_x += editor->mouse_delta_x;
        node->pos_y += editor->mouse_delta_y;
    }
}

void CGL_node_editor_node_render(CGL_node_editor_node* node)
{
    const CGL_node_editor* editor = node->editor;
    CGL_float sx = node->size_x;
    CGL_float sy = node->size_y;
    if(node->render_title) sy += 0.1f;
    //if(sx > sy) sy = sx * node->editor->input->aspect_ratio;
    //else sx = sy / node->editor->input->aspect_ratio;    
    CGL_float ofx = node->editor->offset_x;
    CGL_float ofy = node->editor->offset_y;
    // render selection
    if(node->selected)
    {
        CGL_widgets_set_fill_colorf(0.8f, 0.8f, 0.1f, 1.0f);
        CGL_widgets_add_rect2f(ofx + node->pos_x - 0.01f, ofy + node->pos_y - 0.01f, sx +  0.02f, sy + 0.02f);
    }
    // render body
    CGL_widgets_set_fill_color(node->color);
    CGL_widgets_add_rect2f(ofx + node->pos_x, ofy + node->pos_y, sx, sy);
    //render title
    if(node->render_title)
    {
        CGL_widgets_set_fill_colorf(node->color.x * 0.7f, node->color.y * 0.7f, node->color.z * 0.7f, 1.0f);
        CGL_widgets_add_rect2f(ofx + node->pos_x, ofy + node->pos_y + sy - 0.1f - 0.01f, sx, 0.1f);
        CGL_widgets_set_fill_colorf(0.0f, 0.0f, 0.0f, 1.0f);
        CGL_widgets_add_string(node->title, ofx + node->pos_x + 0.01f, ofy + node->pos_y + sy - 0.1f - 0.01f, sx - 0.02f, (0.1f - 0.02f));
    }
    // render pins

    // left pins
    CGL_float ty1 = ofy + node->pos_y + 0.1f;
    CGL_float ty2 = ofy + node->pos_y + 0.1f;
    for(CGL_int i = CGL_NODE_EDITOR_NODE_MAX_PINS - 1 ; i >= 0 ; i--)
    {
        if(node->left_pins[i].is_set)
        {
            CGL_widgets_set_fill_color(node->left_pins[i].color);
            CGL_widgets_add_rect2f(ofx + node->pos_x - 0.02f, ty1, 0.05f / node->editor->input->aspect_ratio, 0.05f);
            node->left_pins[i].pos_x = node->pos_x - 0.02f;
            node->left_pins[i].pos_y = ty1 - ofy;
            ty1 += 0.1f + 0.05f;
        }

        if(node->right_pins[i].is_set)
        {
            CGL_widgets_set_fill_color(node->right_pins[i].color);
            CGL_widgets_add_rect2f(ofx + node->pos_x + sx - 0.05f / node->editor->input->aspect_ratio + 0.02f, ty2, 0.05f / node->editor->input->aspect_ratio, 0.05f);
            node->right_pins[i].pos_x = node->pos_x + sx - 0.05f / node->editor->input->aspect_ratio + 0.02f;
            node->right_pins[i].pos_y = ty2 - ofy;
            ty2 += 0.1f + 0.05f;
        }
    }

}

void CGL_node_editor_node_set_position(CGL_node_editor_node* node, CGL_float x, CGL_float y)
{
    node->pos_x = x;
    node->pos_y = y;
}

void CGL_node_editor_node_set_title(CGL_node_editor_node* node, const char* title)
{
    if(title == NULL) {node->render_title = false; return;}
    node->render_title = true;
    strcpy(node->title, title);
}

CGL_node_editor_pin* CGL_node_editor_node_get_pin(CGL_node_editor_node* node, bool left, CGL_int index)
{
    CGL_node_editor_pin* pin = NULL;
    if(left) pin = &node->left_pins[index];
    else pin = &node->right_pins[index];
    pin->index = index;
    pin->left = left;
    pin->parent = node;
    pin->color = CGL_vec4_init(0.0f, 0.0f, 0.0f, 1.0f);
    return pin;
}

#endif


#ifndef CGL_EXCLUDE_RAY_CASTER

struct CGL_ray_caster
{
    CGL_shape_triangle triangles[CGL_RAY_CASTER_MAX_TRIANGLES];
    CGL_vec4 walls[CGL_RAY_CASTER_MAX_WALLS];
    CGL_int wall_count;
    CGL_int triangle_count;
    CGL_float theta_min;
    CGL_float theta_max;
    CGL_float max_dist;
#ifndef CGL_EXCLUDE_GRAPHICS_API
    CGL_mesh_gpu* mesh;
#endif
};

CGL_ray_caster* CGL_ray_caster_create()
{
    CGL_ray_caster* caster = (CGL_ray_caster*)CGL_malloc(sizeof(CGL_ray_caster));
    if(!caster) return NULL;
    caster->theta_max = 3.141f;
    caster->theta_min = 0.0f;
    caster->max_dist = 1.0f;
    caster->triangle_count = 0;
    caster->wall_count = 0;
#ifndef CGL_EXCLUDE_GRAPHICS_API
    caster->mesh = CGL_mesh_gpu_create();
#endif
    return caster;
}

void CGL_ray_caster_destroy(CGL_ray_caster* caster)
{
#ifndef CGL_EXCLUDE_GRAPHICS_API
    if(caster->mesh) CGL_mesh_gpu_destroy(caster->mesh);
#endif
    CGL_free(caster);
}

void CGL_ray_caster_add_walls(CGL_ray_caster* caster, CGL_vec4* walls, CGL_int walls_count)
{
    if((caster->wall_count + walls_count) >= CGL_RAY_CASTER_MAX_WALLS) CGL_warn("Max Ray Caster walls exceeded");
    walls_count = CGL_utils_min(walls_count, CGL_RAY_CASTER_MAX_WALLS - caster->wall_count - 1);
    memcpy(caster->walls + caster->wall_count, walls, sizeof(CGL_vec4) * walls_count);
    caster->wall_count += walls_count;
}

void CGL_ray_caster_add_wall(CGL_ray_caster* caster, CGL_vec4 wall)
{
    CGL_ray_caster_add_walls(caster, &wall, 1);
}

void CGL_ray_caster_clear_walls(CGL_ray_caster* caster)
{
    caster->wall_count = 0;
}

// algorithm from http://www.gamers.org/dEngine/rsc/usenet/comp.graphics.algorithms.faq
float CGL_ray_caster_get_intersection_point_for_wall(CGL_vec2 pos, CGL_vec2 dir, CGL_vec4 wall, CGL_vec2* intersection_point, float* angle, CGL_float max_dist)
{
    const CGL_float infinity = max_dist;
    CGL_vec2 A = pos;
    CGL_vec2 B = CGL_vec2_init(pos.x + infinity * dir.x, pos.y + infinity * dir.y);
    CGL_vec2 C = CGL_vec2_init(wall.x, wall.y);
    CGL_vec2 D = CGL_vec2_init(wall.z, wall.w);
    CGL_float n = 0.0f, d = 0.0f;
    // calculate r
    n = (A.y - C.y) * (D.x - C.x) - (A.x - C.x) * (D.y - C.y);
    d = (B.x - A.x) * (D.y - C.y) - (B.y - A.y) * (D.x - C.x);
    if(d == 0.0f) return -1.0f; // they are parallel
    CGL_float r = n / d;
    // calculate s
    n = (A.y - C.y) * (B.x - A.x) - (A.x - C.x) * (B.y - A.y);
    d = (B.x - A.x) * (D.y - C.y) - (B.y - A.y) * (D.x - C.x);
    if(d == 0.0f) return -1.0f; // they are parallel
    CGL_float s = n / d;
    // check if they intersect
    if(r < 0.0f || r > 1.0f || s < 0.0f || s > 1.0f) return -1.0f;
    // calculate intersection point
    if(intersection_point)
    {
        intersection_point->x = A.x + r * (B.x - A.x);
        intersection_point->y = A.y + r * (B.y - A.y);
    }
    if(angle) *angle = atan2f(dir.y, dir.x);
    return r * infinity;
}

float CGL_ray_caster_get_intersection_point_for_walls(CGL_vec2 pos, CGL_vec2 dir, CGL_vec4* walls, CGL_int wall_count, CGL_vec2* intersection_point, float* angle, CGL_float max_dist)
{
    CGL_float t = max_dist;
    CGL_float tmp = 0.0f;
    for(CGL_int i = 0 ; i < wall_count ; i++)
    {
        tmp = CGL_ray_caster_get_intersection_point_for_wall(pos, dir, walls[i], NULL, NULL, max_dist);
        if(tmp > 0.0001f) t = CGL_utils_min(t, tmp);
    }
    CGL_float a = pos.x;
    CGL_float b = pos.y;
    CGL_float d = dir.x;
    CGL_float e = dir.y;
    if(intersection_point) *intersection_point = CGL_vec2_init(a + t * d, b + t * e);
    if(angle) *angle = atan2f(dir.y, dir.x);
    return t;
}

struct __CGL_ray_caster_ray
{
    CGL_vec2 target;
    CGL_vec2 dir;
    CGL_float angle;
    CGL_float dist;
};
typedef struct __CGL_ray_caster_ray __CGL_ray_caster_ray;

static void __CGL_ray_caster_calc_ray(CGL_ray_caster* caster, CGL_vec2* pos, CGL_float theta, __CGL_ray_caster_ray* ray)
{
    ray->dir = CGL_vec2_init(cosf(theta), sinf(theta));
    CGL_float t = CGL_ray_caster_get_intersection_point_for_walls(*pos, ray->dir, caster->walls, caster->wall_count, &ray->target, &ray->angle, caster->max_dist);
    ray->dist = t;
}

static CGL_int __CGL_ray_caster_ray_compare(const void *a, const void *b) {
    CGL_float c = ((__CGL_ray_caster_ray *) a)->angle;
    CGL_float d = ((__CGL_ray_caster_ray *) b)->angle;
    if(c > d) return -1;
    if(c < d) return 1;
    return 0;
}

float __CGL_ray_caster_calculate_angle_in_range(CGL_float ang)
{
    CGL_int n = (int)(fabsf(ang) / 3.141f);
    CGL_float th = fmodf(fabsf(ang), 3.141f);
    if(ang > 3.141f) ang =  th -((n % 2 == 0) ? 0.0f : 3.141f);
    else if(ang < -3.141f) ang =  ((n % 2 == 0) ? 0.0f : 3.141f) - th;
    return ang;
}

void CGL_ray_caster_calculate(CGL_ray_caster* caster, CGL_vec2 pos, CGL_float rotation, bool visualise_rays)
{
    static __CGL_ray_caster_ray rays[CGL_RAY_CASTER_MAX_WALLS * 2];
    CGL_int ray_count = 0;
    
    CGL_float angmn = __CGL_ray_caster_calculate_angle_in_range(caster->theta_min + rotation);        
    CGL_float angmx = __CGL_ray_caster_calculate_angle_in_range(caster->theta_max + rotation);
    CGL_float cdelta_half = cosf(fabsf(angmx - angmn) * 0.5f);
    CGL_vec2 min_border = CGL_vec2_init(cosf(angmn), sinf(angmn));
    CGL_vec2 max_border = CGL_vec2_init(cosf(angmx), sinf(angmx));
    CGL_vec2 dir = CGL_vec2_add(min_border, max_border);
    CGL_vec2_normalize(dir);

    
    //__CGL_ray_caster_calc_ray(caster, &pos, caster->theta_min + rotation, &rays[ray_count++]);
    //__CGL_ray_caster_calc_ray(caster, &pos, caster->theta_max + rotation, &rays[ray_count++]);
    for(CGL_float st = -3.141f ; st < 3.141f ; st += 0.5f)
        __CGL_ray_caster_calc_ray(caster, &pos, st, &rays[ray_count++]); 
    const CGL_float epsilon = 0.0001f;
    for(CGL_int i = 0 ; i < caster->wall_count ; i++)
    {
        CGL_vec4 wall = caster->walls[i];
        CGL_float theta = 0.0f;

        // calculate theta for wall start
        theta = atan2f(wall.y - pos.y, wall.x - pos.x);
        __CGL_ray_caster_calc_ray(caster, &pos, theta, &rays[ray_count++]);
        // calculate theta for wall start +- 0.0001f theta
        __CGL_ray_caster_calc_ray(caster, &pos, theta + epsilon, &rays[ray_count++]);
        __CGL_ray_caster_calc_ray(caster, &pos, theta - epsilon, &rays[ray_count++]);

        // calculate theta for wall end
        theta = atan2f(wall.w - pos.y, wall.z - pos.x);
        __CGL_ray_caster_calc_ray(caster, &pos, theta, &rays[ray_count++]);
        // calculate theta for wall end +- 0.0001f theta
        __CGL_ray_caster_calc_ray(caster, &pos, theta + epsilon, &rays[ray_count++]);
        __CGL_ray_caster_calc_ray(caster, &pos, theta - epsilon, &rays[ray_count++]);
    }

    // sort rays by angle using quicksort
    qsort(rays, ray_count, sizeof(__CGL_ray_caster_ray), __CGL_ray_caster_ray_compare);

    // create tiangle fans
    caster->triangle_count = 0;
    for(CGL_int i = 0 ; i < ray_count ; i++)
    {
        //CGL_widgets_set_fill_color(CGL_utils_random_color());
        caster->triangles[caster->triangle_count].a = CGL_vec3_init(pos.x, pos.y, 0.0f);
        caster->triangles[caster->triangle_count].b = CGL_vec3_init(rays[(i + 1) % ray_count].target.x, rays[(i + 1) % ray_count].target.y, 0.0f);
        caster->triangles[caster->triangle_count].c = CGL_vec3_init(rays[i].target.x, rays[i].target.y, 0.0f);        
#ifndef CGL_EXCLUDE_WIDGETS
        if(visualise_rays) CGL_widgets_add_triangle(caster->triangles[caster->triangle_count].a, caster->triangles[caster->triangle_count].b, caster->triangles[caster->triangle_count].c);
#endif
        caster->triangle_count++;
    }
    /*
    if(ray_count > 1)
    {
        caster->triangles[caster->triangle_count].a = CGL_vec3_init(pos.x, pos.y, 0.0f);
        caster->triangles[caster->triangle_count].b = CGL_vec3_init(rays[0].target.x, rays[0].target.y, 0.0f);
        caster->triangles[caster->triangle_count].c = CGL_vec3_init(rays[ray_count - 1].target.x, rays[ray_count - 1].target.y, 0.0f);        
#ifndef CGL_EXCLUDE_WIDGETS
        if(visualise_rays) CGL_widgets_add_triangle(caster->triangles[caster->triangle_count].a, caster->triangles[caster->triangle_count].b, caster->triangles[caster->triangle_count].c);
#endif
        caster->triangle_count++;
    }
    */


#ifndef CGL_EXCLUDE_WIDGETS
    // visualize rays
    if(visualise_rays)
    {     
        CGL_widgets_set_stroke_colorf(0.2f, 0.3f, 0.7f, 1.0f);
        CGL_widgets_set_stroke_thicnkess(0.01f);
        for(CGL_int i = 0 ; i < ray_count ; i++)
        {
            //if( ((rays[i].angle) >= (angmn - 0.1f) && (rays[i].angle) <= (angmx + 0.1f)) || ( (angmn >= 3.141f / 2 && angmx <= -3.141f/2) && (rays[i].angle >= (angmx - 0.1f) || (rays[i].angle) <= (angmn + 0.1f)) ) )
            // if( CGL_vec2_dot(dir, rays[i].dir) >= cdelta_half - 0.01f) // for future
                CGL_widgets_add_line(
                    CGL_vec3_init(pos.x, pos.y, 0.0f),
                    CGL_vec3_init(rays[i].target.x, rays[i].target.y, 0.0f)
                );
        }
    }
#endif
}

bool CGL_ray_caster_is_in_view(CGL_ray_caster* caster, CGL_vec2 point)
{
    for(CGL_int i = 0 ; i < caster->triangle_count ; i++)
        if(CGL_utils_is_point_in_triangle(point, CGL_vec2_init(caster->triangles[i].a.x, caster->triangles[i].a.y), CGL_vec2_init(caster->triangles[i].b.x, caster->triangles[i].b.y), CGL_vec2_init(caster->triangles[i].c.x, caster->triangles[i].c.y))) return true;
    return false;
}

void CGL_ray_caster_set_angle_limits(CGL_ray_caster* caster, CGL_float min_angle, CGL_float max_angle)
{
    caster->theta_min = min_angle;
    caster->theta_max = max_angle;
}

void CGL_ray_caster_set_max_distance(CGL_ray_caster* caster, CGL_float val)
{
    caster->max_dist = val;
}

CGL_shape_triangle* CGL_ray_caster_get_triangles(CGL_ray_caster* caster, CGL_int* count)
{
    if(count) *count = caster->triangle_count;
    return caster->triangles;
}


#endif

#ifndef CGL_EXCLUDE_SQUARE_MARCHER

struct CGL_square_marcher
{
    void* user_data;
    bool interpolate;
};

CGL_square_marcher* CGL_square_marcher_create()
{
    CGL_square_marcher* marcher = (CGL_square_marcher*)CGL_malloc(sizeof(CGL_square_marcher));
    if(!marcher) return NULL;
    marcher->user_data = NULL;
    marcher->interpolate = false;
    return marcher;
}

void CGL_square_marcher_destroy(CGL_square_marcher* marcher)
{
    CGL_free(marcher);
}

void CGL_square_marcher_set_user_data(CGL_square_marcher* marcher, void* user_data)
{
    marcher->user_data = user_data;
}

void* CGL_square_marcher_get_user_data(CGL_square_marcher* marcher)
{
    return marcher->user_data;
}   

void CGL_square_marcher_enable_interpolation(CGL_square_marcher* marcher, bool enable)
{
    marcher->interpolate = enable;
}


void __CGL_square_marcher_generate_mesh_add_triangle(CGL_list* list, CGL_vec2 a, CGL_vec2 b, CGL_vec2 c)
{
    CGL_mesh_vertex v;
    v.normal = CGL_vec4_init(0.0f, 0.0f, 1.0f, 1.0f);
    v.position = CGL_vec4_init(a.x, a.y, 0.0f, 0.0f);
    CGL_list_push(list, &v);
    v.position = CGL_vec4_init(b.x, b.y, 0.0f, 0.0f);
    CGL_list_push(list, &v);
    v.position = CGL_vec4_init(c.x, c.y, 0.0f, 0.0f);
    CGL_list_push(list, &v);
}


CGL_mesh_cpu* CGL_square_marcher_generate_mesh(CGL_square_marcher* marcher, CGL_square_marcher_distance_function sampler, CGL_vec2 start, CGL_vec2 end, CGL_int resolution_x, CGL_int resolution_y)
{
    CGL_vec2 step_size = CGL_vec2_init((end.x - start.x) / (float)resolution_x, (end.y - start.y) / (float)resolution_y);
    CGL_vec2 pos[4], mpts[4];
    CGL_bool smpb[4];
    CGL_float smpv[4], intr[4];
    CGL_list* mesh_list = CGL_list_create(sizeof(CGL_mesh_vertex), 1000);

    for(CGL_int xi = -1 ; xi < resolution_x ; xi ++)
    {
        for(CGL_int yi = -1 ; yi < resolution_y ; yi ++)
        {
            pos[0] = CGL_vec2_init(start.x + xi * step_size.x, start.y + (yi + 1) * step_size.y);
            pos[1] = CGL_vec2_init(start.x + (xi + 1) * step_size.x, start.y + (yi + 1) * step_size.y);
            pos[2] = CGL_vec2_init(start.x + (xi + 1) * step_size.x, start.y + yi * step_size.y);
            pos[3] = CGL_vec2_init(start.x + xi * step_size.x, start.y + yi * step_size.y);

            for (CGL_int i = 0 ; i < 4 ; i++) intr[i] = 0.5f;

            /*
            *  A Possible Optimization
            *
            * We could have a cache of say CGL_float cache[10 * 10];
            * Now we loop from x = 0 to x = resolution_x / 10 and y = 0 to y = resolution_y / 10 in the outer loops
            * And have 2 inner loops from 1 - 10 each there e could cache the vlaue of the distance function
            * And then use the cached values in the inner loop saving a lot of calls to the distance function
            */

            for (CGL_int i = 0 ; i < 4 ; i++) smpb[i] = sampler(pos[i], &smpv[i], marcher->user_data);

            if(marcher->interpolate) for (CGL_int i = 0 ; i < 4 ; i++) intr[i] = smpv[i] / ( smpv[i] + smpv[(i + 1) % 4]);

            for (CGL_int i = 0 ; i < 4 ; i++) mpts[i] = CGL_vec2_init(CGL_utils_mix(pos[i].x, pos[(i + 1) % 4].x, intr[i]), CGL_utils_mix(pos[i].y, pos[(i + 1) % 4].y, intr[i]));

            if(!smpb[0] && !smpb[1] && !smpb[2] && !smpb[3]) // 0000
            {

            }
            else if(!smpb[0] && !smpb[1] && !smpb[2] && smpb[3]) // 0001
            {
                //CGL_widgets_add_line2f(mpts[2].x, mpts[2].y, mpts[3].x, mpts[3].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[3], mpts[2], mpts[3]);
            }
            else if(!smpb[0] && !smpb[1] && smpb[2] && !smpb[3]) // 0010
            {
                //CGL_widgets_add_line2f(mpts[1].x, mpts[1].y, mpts[2].x, mpts[2].y); 
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[2], mpts[1], mpts[2]);
            }
            else if(!smpb[0] && !smpb[1] && smpb[2] && smpb[3]) // 0011
            {
                //CGL_widgets_add_line2f(mpts[3].x, mpts[3].y, mpts[1].x, mpts[1].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[3], pos[2], mpts[1]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[3], mpts[1], mpts[3]);
            }
            else if(!smpb[0] && smpb[1] && smpb[2] && smpb[3]) // 0111
            {
                //CGL_widgets_add_line2f(mpts[0].x, mpts[0].y, mpts[3].x, mpts[3].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[3], pos[2], pos[1]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[3], pos[1], mpts[0]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[3], mpts[0], mpts[3]);
            }
            else if(!smpb[0] && smpb[1] && smpb[2] && !smpb[3]) // 0110
            {
                //CGL_widgets_add_line2f(mpts[0].x, mpts[0].y, mpts[2].x, mpts[2].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[2], pos[1], mpts[0]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[2], mpts[0], mpts[2]);
            }
            else if(!smpb[0] && smpb[1] && !smpb[2] && smpb[3]) // 0101
            {
                //CGL_widgets_add_line2f(mpts[0].x, mpts[0].y, mpts[1].x, mpts[1].y);
                //CGL_widgets_add_line2f(mpts[2].x, mpts[2].y, mpts[3].x, mpts[3].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[3], mpts[2], mpts[3]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[1], mpts[1], mpts[0]);
            }
            else if(!smpb[0] && smpb[1] && !smpb[2] && !smpb[3]) // 0100
            {
                //CGL_widgets_add_line2f(mpts[0].x, mpts[0].y, mpts[1].x, mpts[1].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[1], mpts[1], mpts[0]);
            }
            else if(smpb[0] && !smpb[1] && !smpb[2] && !smpb[3]) // 1000
            {
                //CGL_widgets_add_line2f(mpts[0].x, mpts[0].y, mpts[3].x, mpts[3].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], mpts[0], mpts[3]);
            }
            else if(smpb[0] && !smpb[1] && smpb[2] && !smpb[3]) // 1010
            {
                //CGL_widgets_add_line2f(mpts[0].x, mpts[0].y, mpts[3].x, mpts[3].y);
                //CGL_widgets_add_line2f(mpts[1].x, mpts[1].y, mpts[2].x, mpts[2].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], mpts[0], mpts[3]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[2], mpts[1], mpts[2]);
            }
            else if(smpb[0] && !smpb[1] && smpb[2] && smpb[3]) // 1011
            {
                //CGL_widgets_add_line2f(mpts[0].x, mpts[0].y, mpts[1].x, mpts[1].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], pos[3], pos[2]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], pos[2], mpts[1]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], mpts[1], mpts[0]);
            }
            else if(smpb[0] && smpb[1] && !smpb[2] && smpb[3]) // 1101
            {
                //CGL_widgets_add_line2f(mpts[1].x, mpts[1].y, mpts[2].x, mpts[2].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], pos[1], pos[3]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[1], mpts[2], pos[3]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[1], mpts[1], mpts[2]);
            }
            else if(smpb[0] && smpb[1] && smpb[2] && !smpb[3]) // 1110
            {
                //CGL_widgets_add_line2f(mpts[2].x, mpts[2].y, mpts[3].x, mpts[3].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], pos[1], pos[2]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], pos[2], mpts[2]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], mpts[2], mpts[3]);
            }
            else if(smpb[0] && smpb[1] && !smpb[2] && !smpb[3]) // 1100
            {
                //CGL_widgets_add_line2f(mpts[3].x, mpts[3].y, mpts[1].x, mpts[1].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], pos[1], mpts[1]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], mpts[1], mpts[3]);
            }
            else if(smpb[0] && !smpb[1] && !smpb[2] && smpb[3]) // 1001
            {
                //CGL_widgets_add_line2f(mpts[0].x, mpts[0].y, mpts[2].x, mpts[2].y);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[3], mpts[2], pos[0]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, mpts[2], mpts[0], pos[0]);
            }
            else if(smpb[0] && smpb[1] && smpb[2] && smpb[3]) // 1111
            {
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], pos[2], pos[1]);
                __CGL_square_marcher_generate_mesh_add_triangle(mesh_list, pos[0], pos[3], pos[2]);
            }

        }

    }
    size_t vt_ct = CGL_list_get_size(mesh_list);
    CGL_mesh_cpu* mesh = CGL_mesh_cpu_create(vt_ct, vt_ct);
    memcpy(mesh->vertices, mesh_list->data, sizeof(CGL_mesh_vertex) * vt_ct);
    for(size_t i = 0; i < vt_ct; i++) mesh->indices[i] = (CGL_uint)i;
    CGL_list_destroy(mesh_list);
    return mesh;
}



#endif



#ifndef CGL_EXCLUDE_POST_PROCESSOR



struct CGL_post_processor
{
    CGL_shader* shades_shader;
    CGL_shader* outlines_shader;
    CGL_shader* hatching_shader;
};



static CGL_post_processor* __CGL_POST_PROCESSOR_CONTEXT = NULL;



static const CGL_byte* __CGL_TOON_SHADES_SHADER_SOURCE =
"#version 430 core\n"
"\n"
"layout (local_size_x = 16, local_size_y = 16) in;\n"
"\n"
"layout (rgba32f, binding = 0) uniform image2D output_tex;\n"
"layout (rgba32f, binding = 1) uniform image2D input_tex;\n"
"\n"
"uniform int shades = 1;\n"
"uniform sampler2D albedo_tex;\n"
"uniform ivec2 resolution;\n"
"\n"
"void main()\n"
"{\n"
"    ivec2 pixel_coord = ivec2(gl_GlobalInvocationID.xy);\n"
"    vec2 uv = vec2(pixel_coord) / vec2(resolution);\n"
"    vec3 scene_pixel = imageLoad(input_tex, pixel_coord).rgb;\n"
"    scene_pixel = floor(scene_pixel * float(shades)) / float(shades);\n"
"    vec3 albedo_pixel = texture(albedo_tex, uv).rgb;\n"
"    vec3 output_pixel = scene_pixel + albedo_pixel;\n"
"    imageStore(output_tex, pixel_coord, vec4(output_pixel, 1.0));\n"
"}\n"
"\n";



static const CGL_byte* __CGL_TOON_OUTLINES_SHADER_SOURCE = 
"#version 430 core\n"

"layout (local_size_x = 16, local_size_y = 16) in;\n"

"layout (rgba32f, binding = 0) uniform image2D output_tex;\n"
"layout (rgba32f, binding = 1) uniform image2D scene;\n"

"uniform float outline_width = 1.0f;\n"
"uniform ivec2 resolution;\n"
"uniform sampler2D normal_tex;\n"
"uniform sampler2D depth_tex;\n"

"float max_vec3(vec3 v)\n"
"{\n"
"    return max(max(v.x, v.y), v.z);\n"
"}\n"

// TODO: Take zFar and zNear as uniforms 
"float linearize_depth(float depth)\n"
"{\n"
"    float zFar = 100.0f;\n"
"    float zNear = 0.01f;\n"
"    return (2.0f * zNear) / (zFar + zNear - depth * (zFar - zNear));\n"
"}\n"


"float get_n_factor(vec2 uv)\n"
"{\n"
"    float current_normal = max_vec3(texture(normal_tex, uv).rgb);\n"
"    float left_normal = max_vec3(texture(normal_tex, uv + vec2(-outline_width / float(resolution.x), 0.0)).rgb);\n"
"    float right_normal = max_vec3(texture(normal_tex, uv + vec2(outline_width / float(resolution.x), 0.0)).rgb);\n"
"    float top_normal = max_vec3(texture(normal_tex, uv + vec2(0.0, outline_width / float(resolution.y))).rgb);\n"
"    float bottom_normal = max_vec3(texture(normal_tex, uv + vec2(0.0, -outline_width / float(resolution.y))).rgb);\n"
"    float n_factor = 0.0;\n"
"    n_factor = abs(current_normal - left_normal) + abs(current_normal - right_normal) + abs(current_normal - top_normal) + abs(current_normal - bottom_normal);\n"
//"    return clamp(n_factor, 0.0f, 1.0f);\n" // another special effect
"    return 1.0f - clamp(smoothstep(n_factor, 0.2f, 0.85f), 0.0f, 1.0f);\n"
"}\n"

"float get_d_factor(vec2 uv)\n"
"{\n"
"    float current_depth = linearize_depth(texture(depth_tex, uv).r);\n"
"    float left_depth = linearize_depth(texture(depth_tex, uv + vec2(-outline_width / float(resolution.x), 0.0)).r);\n"
"    float right_depth = linearize_depth(texture(depth_tex, uv + vec2(outline_width / float(resolution.x), 0.0)).r);\n"
"    float top_depth = linearize_depth(texture(depth_tex, uv + vec2(0.0, outline_width / float(resolution.y))).r);\n"
"    float bottom_depth = linearize_depth(texture(depth_tex, uv + vec2(0.0, -outline_width / float(resolution.y))).r);\n"
"    float d_factor = 0.0;\n"
"    d_factor = abs(current_depth - left_depth) + abs(current_depth - right_depth) + abs(current_depth - top_depth) + abs(current_depth - bottom_depth);\n"
"    return clamp(d_factor, 0.0f, 1.0f);\n"
"}\n"

"void main()\n"
"{\n"
"    ivec2 pixel_coord = ivec2(gl_GlobalInvocationID.xy);\n"
"    vec2 uv = vec2(pixel_coord) / vec2(resolution);\n"
"    float n_factor = get_n_factor(uv);\n"
"    float d_factor = get_d_factor(uv);\n"
"    float factor = max(n_factor, d_factor);\n"
"    vec3 scene_pixel = imageLoad(scene, pixel_coord).rgb;\n"
"    vec3 output_pixel = scene_pixel * (1.0f - factor);\n"
"    imageStore(output_tex, pixel_coord, vec4(output_pixel, 1.0));\n"
"}\n";



static const CGL_byte* __CGL_TOON_HATCHING_SHADER_SOURCE =
"#version 430 core\n"
"\n"
"layout (local_size_x = 16, local_size_y = 16) in;\n"
"\n"
"layout (rgba32f, binding = 0) uniform image2D output_tex;\n"
"\n"
"uniform sampler2D input_tex;\n"
"uniform sampler2D uv_tex;\n"
"uniform sampler2D hatch_tex[8];\n"
"uniform int hatch_tex_count;\n"
"uniform ivec2 resolution;\n"
"\n"
"vec3 hatching(vec2 uv, float intensity)\n"
"{\n"
"   vec3 overbright = vec3(max(0.0f, intensity - 1.0f));\n"
"   vec3 opt = overbright;\n"
"   float tot_weight = 0.0f;\n"
"   for(int i = 0 ; i < hatch_tex_count; i++)\n"
"   {\n"
"       float a0 = clamp(intensity * float(hatch_tex_count) - float(i), 0.0f, 1.0f);\n"
"       float a1 = clamp(intensity * float(hatch_tex_count) - float(i + 1), 0.0f, 1.0f);\n"
"       float weight = a0 - a1;\n"
"       tot_weight += weight;"
"       opt += texture(hatch_tex[i], uv * 8.0f).rgb * weight;\n"
"   }\n"
"   return opt / tot_weight;\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"    ivec2 pixel_coord = ivec2(gl_GlobalInvocationID.xy);\n"
"    vec2 uv = vec2(pixel_coord) / vec2(resolution);\n"
"    vec2 sc_uv = texture(uv_tex, uv).rg;\n"
"    float filter_f = length(sc_uv);\n"
"    vec3 input_color = texture(input_tex, uv).rgb;\n"
"    vec3 output_pixel = vec3(0.0f);\n"
"    output_pixel = hatching(uv, dot(input_color, vec3(0.2326f, 0.7152f, 0.0722f)));\n"
"    if(filter_f > 1.0f) {output_pixel = vec3(1.0f); }\n"
"    imageStore(output_tex, pixel_coord, vec4(output_pixel, 1.0));\n"
"}\n"
"\n";


void CGL_post_processor_init()
{
    __CGL_POST_PROCESSOR_CONTEXT = (CGL_post_processor*)CGL_malloc(sizeof(CGL_post_processor));
    __CGL_POST_PROCESSOR_CONTEXT->shades_shader = CGL_shader_compute_create(__CGL_TOON_SHADES_SHADER_SOURCE, NULL);
    __CGL_POST_PROCESSOR_CONTEXT->outlines_shader = CGL_shader_compute_create(__CGL_TOON_OUTLINES_SHADER_SOURCE, NULL);
    __CGL_POST_PROCESSOR_CONTEXT->hatching_shader = CGL_shader_compute_create(__CGL_TOON_HATCHING_SHADER_SOURCE, NULL);
}


void CGL_post_processor_shutdown()
{
    CGL_shader_destroy(__CGL_POST_PROCESSOR_CONTEXT->shades_shader);
    CGL_shader_destroy(__CGL_POST_PROCESSOR_CONTEXT->outlines_shader);
    CGL_shader_destroy(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader);
    CGL_free(__CGL_POST_PROCESSOR_CONTEXT);
}


void CGL_post_processor_process_shades(CGL_texture* output, CGL_texture* scene, CGL_texture* albedo, CGL_int shades)
{
    CGL_shader_bind(__CGL_POST_PROCESSOR_CONTEXT->shades_shader);
    glBindImageTexture(0, output->handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, scene->handle, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    CGL_shader_set_uniform_int(__CGL_POST_PROCESSOR_CONTEXT->shades_shader, CGL_shader_get_uniform_location(__CGL_POST_PROCESSOR_CONTEXT->shades_shader, "shades"), shades);    
    CGL_texture_bind(albedo, 2);
    CGL_shader_set_uniform_int(__CGL_POST_PROCESSOR_CONTEXT->shades_shader, CGL_shader_get_uniform_location(__CGL_POST_PROCESSOR_CONTEXT->shades_shader, "albedo_tex"), 2);    
    CGL_shader_set_uniform_ivec2v(__CGL_POST_PROCESSOR_CONTEXT->shades_shader, CGL_shader_get_uniform_location(__CGL_POST_PROCESSOR_CONTEXT->shades_shader, "resolution"), output->width, output->height);
    CGL_shader_compute_dispatch(__CGL_POST_PROCESSOR_CONTEXT->shades_shader, output->width/16+1, output->height/16+1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}


void CGL_post_processor_process_outline(CGL_texture* output, CGL_texture* scene, CGL_texture* normal, CGL_texture* depth, CGL_float outline_width)
{
    CGL_shader_bind(__CGL_POST_PROCESSOR_CONTEXT->outlines_shader);
    glBindImageTexture(0, output->handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(1, scene->handle, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    CGL_texture_bind(normal, 2);
    CGL_shader_set_uniform_int(__CGL_POST_PROCESSOR_CONTEXT->outlines_shader, CGL_shader_get_uniform_location(__CGL_POST_PROCESSOR_CONTEXT->outlines_shader, "normal_tex"), 2);    
    CGL_texture_bind(depth, 3);
    CGL_shader_set_uniform_int(__CGL_POST_PROCESSOR_CONTEXT->outlines_shader, CGL_shader_get_uniform_location(__CGL_POST_PROCESSOR_CONTEXT->outlines_shader, "depth_tex"), 3);
    CGL_shader_set_uniform_ivec2v(__CGL_POST_PROCESSOR_CONTEXT->outlines_shader, CGL_shader_get_uniform_location(__CGL_POST_PROCESSOR_CONTEXT->outlines_shader, "resolution"), output->width, output->height);
    CGL_shader_set_uniform_float(__CGL_POST_PROCESSOR_CONTEXT->outlines_shader, CGL_shader_get_uniform_location(__CGL_POST_PROCESSOR_CONTEXT->outlines_shader, "outline_width"), outline_width);
    CGL_shader_compute_dispatch(__CGL_POST_PROCESSOR_CONTEXT->outlines_shader, output->width/16+1, output->height/16+1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}



void CGL_post_processor_process_hatching(CGL_texture* output, CGL_texture* scene, CGL_texture* uv_tex, CGL_texture** hatch_tex, CGL_int hatch_tex_count)
{
    static char buffer[256];
    CGL_shader_bind(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader);
    glBindImageTexture(0, output->handle, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    CGL_texture_bind(uv_tex, 2);
    CGL_shader_set_uniform_int(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader, CGL_shader_get_uniform_location(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader, "uv_tex"), 2);
    CGL_texture_bind(scene, 3);
    CGL_shader_set_uniform_int(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader, CGL_shader_get_uniform_location(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader, "input_tex"), 3);
    for(CGL_int i = 0 ; i < hatch_tex_count ; i++)
    {
        sprintf(buffer, "hatch_tex[%d]", i);
        CGL_texture_bind(hatch_tex[i], 4 + i);
        CGL_shader_set_uniform_int(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader, CGL_shader_get_uniform_location(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader, buffer), 4 + i);
    }
    CGL_shader_set_uniform_int(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader, CGL_shader_get_uniform_location(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader, "hatch_tex_count"), hatch_tex_count);
    CGL_shader_set_uniform_ivec2v(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader, CGL_shader_get_uniform_location(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader, "resolution"), output->width, output->height);
    CGL_shader_compute_dispatch(__CGL_POST_PROCESSOR_CONTEXT->hatching_shader, output->width/16+1, output->height/16+1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

#endif

    // Referred from https://www.mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
CGL_bool CGL_wav_file_load(CGL_wav_file* file, const char* filename)
{
    FILE* fd = fopen(filename, "rb");
    if(!fd) return false;
    CGL_int tmp = 0, ck_size = 0, fmt_ck_size = 0, audio_format = 0, channel_count = 0, sample_rate = 0, byte_rate = 0, block_align = 0, bits_per_sample = 0, data_ck_size = 0;

    // check RIFF signature
    fread(&tmp, 4, 1, fd);
    if(memcmp((CGL_byte*)&tmp, "RIFF", 4) != 0) { CGL_log_internal("WAV Loader (%s): Invalid RIFF signature", filename); fclose(fd); return false; }

    // read chunk size
    fread(&ck_size, 4, 1, fd);
    CGL_utils_little_endian_to_current(&ck_size, 4);

    // check WAVE signature
    fread(&tmp, 4, 1, fd);
    if(memcmp((CGL_byte*)&tmp, "WAVE", 4) != 0) { CGL_log_internal("WAV Loader (%s): Invalid WAVE signature", filename); fclose(fd); return false; }

    // check fmt signature
    fread(&tmp, 4, 1, fd);
    if(memcmp((CGL_byte*)&tmp, "fmt ", 4) != 0) { CGL_log_internal("WAV Loader (%s): Invalid fmt signature", filename); fclose(fd); return false; }

    // read fmt chunk size
    fread(&fmt_ck_size, 4, 1, fd);
    CGL_utils_little_endian_to_current(&fmt_ck_size, 4);

    // read audio format
    fread(&audio_format, 2, 1, fd);
    CGL_utils_little_endian_to_current(&audio_format, 2);
    audio_format = (CGL_int)(*((CGL_short*)&audio_format));
    if(audio_format != 1) { CGL_log_internal("WAV Loader (%s): Unsupported audio format", filename); fclose(fd); return false; }

    // read channel count
    fread(&channel_count, 2, 1, fd);
    CGL_utils_little_endian_to_current(&channel_count, 2);
    channel_count = (CGL_int)(*((CGL_short*)&channel_count));

    // read sample rate
    fread(&sample_rate, 4, 1, fd);
    CGL_utils_little_endian_to_current(&sample_rate, 4);

    // read byte rate = sample rate * block align(= channel count * bits per sample / 8)
    fread(&byte_rate, 4, 1, fd);
    CGL_utils_little_endian_to_current(&byte_rate, 4);

    // read block align = channel count * bits per sample / 8
    fread(&block_align, 2, 1, fd);
    CGL_utils_little_endian_to_current(&block_align, 2);
    block_align = (CGL_int)(*((CGL_short*)&block_align));

    // read bits per sample
    fread(&bits_per_sample, 2, 1, fd);
    CGL_utils_little_endian_to_current(&bits_per_sample, 2);
    bits_per_sample = (CGL_int)(*((CGL_short*)&bits_per_sample));

    // skip to data chunk
    fread(&tmp, 4, 1, fd);
    while(memcmp((CGL_byte*)&tmp, "data", 4) != 0)
    {
        fread(&tmp, 4, 1, fd);
        fseek(fd, tmp, SEEK_CUR);
        if(fread(&tmp, 4, 1, fd) < 1) { CGL_log_internal("WAV Loader (%s): Failed to read data chunk", filename); fclose(fd); return false; }
    }
    if(memcmp((CGL_byte*)&tmp, "data", 4) != 0) { CGL_log_internal("WAV Loader (%s): Invalid data signature", filename); fclose(fd); return false; }

    // read data chunk size
    fread(&data_ck_size, 4, 1, fd);
    CGL_utils_little_endian_to_current(&data_ck_size, 4);

    // read data
    file->data = (CGL_byte*)CGL_malloc(data_ck_size);
    if(!file->data) { CGL_log_internal("WAV Loader (%s): Failed to allocate memory", filename); fclose(fd); return false; }

    fread(file->data, data_ck_size, 1, fd);

    file->channel_count = channel_count;
    file->sample_rate = sample_rate;
    file->bits_per_sample = bits_per_sample;
    file->data_size = data_ck_size;
    file->duration = (CGL_float)data_ck_size / (CGL_float)(sample_rate * channel_count * bits_per_sample / 8);

    
    fclose(fd);
    return true;
}

void CGL_wav_file_destroy(CGL_wav_file* file)
{
    CGL_free(file->data);
}

CGL_int CGL_wav_file_sample(CGL_wav_file* file, CGL_int channel, CGL_int sample_offset)
{
    if(channel >= file->channel_count) return 0;
    if(sample_offset >= file->data_size) return 0;
    CGL_short sample = 0;
    sample_offset += channel * file->bits_per_sample / 8;
    memcpy(&sample, file->data + sample_offset, file->bits_per_sample / 8);
    //CGL_utils_little_endian_to_current(&sample, file->bits_per_sample / 8);
    return (CGL_int)sample;
}

CGL_int CGL_wav_file_sample_at_time(CGL_wav_file* file, CGL_int channel, CGL_float time)
{
    if(channel >= file->channel_count) return 0;
    if(time >= file->duration) return 0;
    CGL_int sample_offset = (CGL_int)((CGL_int)(time * file->sample_rate) * file->channel_count * file->bits_per_sample / 8);
    return CGL_wav_file_sample(file, channel, sample_offset);
}

#ifndef CGL_EXCLUDE_AUDIO

#include <AL/al.h>
#include <AL/alc.h>

struct CGL_audio_context
{
    ALCdevice* device;
    ALCcontext* context;
};

struct CGL_audio_source
{
    CGL_vec3 position;
    CGL_vec3 velocity;
    CGL_vec3 direction;
    ALuint source;
    float pitch;
    float gain;
    CGL_bool loop;
};

struct CGL_audio_buffer
{
    ALuint buffer;
    size_t size;
};



CGL_int CGL_audio_get_last_error()
{
    return alGetError();
}

const CGL_byte* CGL_audio_get_error_string(CGL_int error)
{
    return alGetString(error);
}

const CGL_byte* CGL_audio_get_device_list()
{
    return alcGetString(NULL, ALC_DEVICE_SPECIFIER);
}

const CGL_byte* CGL_audio_get_default_device()
{
    return alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
}

CGL_audio_context* CGL_audio_context_create(const CGL_byte* device_name)
{
    ALCdevice* device = alcOpenDevice(device_name);
    if(!device) { CGL_log_internal("Audio: Failed to open device %s", device_name); return NULL; }
    CGL_log_internal("Audio: Opened device %s", device_name);
    ALCcontext* context = alcCreateContext(device, NULL);
    if(!context) { CGL_log_internal("Audio: Failed to create context"); alcCloseDevice(device); return NULL; }
    CGL_log_internal("Audio: Created context");
    alcMakeContextCurrent(context);
    CGL_audio_context* cgl_context = (CGL_audio_context*)CGL_malloc(sizeof(CGL_audio_context));
    if(!cgl_context) { CGL_log_internal("Audio: Failed to allocate memory"); alcDestroyContext(context); alcCloseDevice(device); return NULL; }
    cgl_context->device = device;
    cgl_context->context = context;
    return cgl_context;
}

void CGL_audio_context_destroy(CGL_audio_context* context)
{
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context->context);
    alcCloseDevice(context->device);
    CGL_free(context);
}

void CGL_audio_make_context_current(CGL_audio_context* context)
{
    if(context) alcMakeContextCurrent(context->context);
    else alcMakeContextCurrent(NULL);
}



CGL_audio_source* CGL_audio_source_create()
{
    CGL_audio_source* source = (CGL_audio_source*)CGL_malloc(sizeof(CGL_audio_source));
    if(!source) { CGL_log_internal("Audio: Failed to allocate memory"); return NULL; }
    alGenSources(1, &source->source);
    source->position = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    source->velocity = CGL_vec3_init(0.0f, 0.0f, 0.0f);
    source->pitch = 1.0f;
    source->gain = 1.0f;
    source->loop = false;
    return source;
}

void CGL_audio_source_destroy(CGL_audio_source* source)
{
    alDeleteSources(1, &source->source);
    CGL_free(source);
}

void CGL_audio_source_reset_to_defaults(CGL_audio_source* source)
{
    CGL_audio_source_set_position(source, CGL_vec3_init(0.0f, 0.0f, 0.0f));
    CGL_audio_source_set_velocity(source, CGL_vec3_init(0.0f, 0.0f, 0.0f));
    CGL_audio_source_set_direction(source, CGL_vec3_init(0.0f, 0.0f, -1.0f));
    CGL_audio_source_set_pitch(source, 1.0f);
    CGL_audio_source_set_gain(source, 1.0f);
    CGL_audio_source_set_looping(source, false);
}

void CGL_audio_source_set_buffer(CGL_audio_source* source, CGL_audio_buffer* buffer)
{
    alSourcei(source->source, AL_BUFFER, buffer->buffer);
}

void CGL_audio_source_set_looping(CGL_audio_source* source, CGL_bool looping)
{
    source->loop = looping;
    alSourcei(source->source, AL_LOOPING, looping);
}

void CGL_audio_source_set_pitch(CGL_audio_source* source, CGL_float pitch)
{
    source->pitch = pitch;
    alSourcef(source->source, AL_PITCH, pitch);
}

void CGL_audio_source_set_gain(CGL_audio_source* source, CGL_float gain)
{
    source->gain = gain;
    alSourcef(source->source, AL_GAIN, gain);
}

void CGL_audio_source_set_position(CGL_audio_source* source, CGL_vec3 position)
{
    source->position = position;
    alSource3f(source->source, AL_POSITION, position.x, position.y, position.z);
}

void CGL_audio_source_set_velocity(CGL_audio_source* source, CGL_vec3 velocity)
{
    source->velocity = velocity;
    alSource3f(source->source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void CGL_audio_source_set_direction(CGL_audio_source* source, CGL_vec3 direction)
{
    source->direction = direction;
    alSource3f(source->source, AL_DIRECTION, direction.x, direction.y, direction.z);
}

CGL_float CGL_audio_source_get_seconds_offset(CGL_audio_source* source)
{
    ALfloat offset = 0;
    alGetSourcef(source->source, AL_SEC_OFFSET, &offset);
    return (CGL_float)offset;
}

void CGL_audio_source_set_seconds_offset(CGL_audio_source* source, CGL_sizei seconds)
{
    alSourcei(source->source, AL_SEC_OFFSET, (ALint)seconds);
}

CGL_sizei CGL_audio_source_get_samples_offset(CGL_audio_source* source)
{
    ALint offset = 0;
    alGetSourcei(source->source, AL_SAMPLE_OFFSET, &offset);
    return (CGL_sizei)offset;
}

void CGL_audio_source_set_samples_offset(CGL_audio_source* source, CGL_sizei samples)
{
    alSourcei(source->source, AL_SAMPLE_OFFSET, (ALint)samples);
}

CGL_sizei CGL_audio_source_get_bytes_offset(CGL_audio_source* source)
{
    ALint offset = 0;
    alGetSourcei(source->source, AL_BYTE_OFFSET, &offset);
    return (CGL_sizei)offset;
}

void CGL_audio_source_set_bytes_offset(CGL_audio_source* source, CGL_sizei bytes)
{
    alSourcei(source->source, AL_BYTE_OFFSET, (ALint)bytes);
}

void CGL_audio_source_play(CGL_audio_source* source)
{
    alSourcePlay(source->source);
}

void CGL_audio_source_pause(CGL_audio_source* source)
{
    alSourcePause(source->source);
}

void CGL_audio_source_stop(CGL_audio_source* source)
{
    alSourceStop(source->source);
}

CGL_bool CGL_audio_source_is_playing(CGL_audio_source* source)
{
    CGL_int state = 0;
    alGetSourcei(source->source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

CGL_bool CGL_audio_source_is_paused(CGL_audio_source* source)
{
    CGL_int state = 0;
    alGetSourcei(source->source, AL_SOURCE_STATE, &state);
    return state == AL_PAUSED;
}

CGL_bool CGL_audio_source_is_stopped(CGL_audio_source* source)
{
    CGL_int state = 0;
    alGetSourcei(source->source, AL_SOURCE_STATE, &state);
    return state == AL_STOPPED;
}

CGL_audio_buffer* CGL_audio_buffer_create()
{
    CGL_audio_buffer* buffer = (CGL_audio_buffer*)CGL_malloc(sizeof(CGL_audio_buffer));
    if(!buffer) { CGL_log_internal("Audio: Failed to allocate memory"); return NULL; }
    buffer->size = 0;
    alGenBuffers(1, &buffer->buffer);
    return buffer;
}

void CGL_audio_buffer_destroy(CGL_audio_buffer* buffer)
{
    alDeleteBuffers(1, &buffer->buffer);
    CGL_free(buffer);
}

void CGL_audio_buffer_set_data_from_wav_file(CGL_audio_buffer* buffer, CGL_wav_file* file)
{
    ALenum format = AL_FORMAT_MONO16;
    ALsizei size = 0;
    ALsizei freq = 0;
    if(file->channel_count == 1 && file->bits_per_sample == 8)
        format = AL_FORMAT_MONO8;
    else if(file->channel_count == 1 && file->bits_per_sample == 16)
        format = AL_FORMAT_MONO16;
    else if(file->channel_count == 2 && file->bits_per_sample == 8)
        format = AL_FORMAT_STEREO8;
    else if(file->channel_count == 2 && file->bits_per_sample == 16)
        format = AL_FORMAT_STEREO16;
    else
    {
        CGL_log_internal("Audio: Unsupported WAV file format");
        return;
    }
    size = file->data_size;
    freq = file->sample_rate;
    alBufferData(buffer->buffer, format, file->data, size, freq);
}

void CGL_audio_buffer_set_data(CGL_audio_buffer* buffer, void* data, ALenum format, ALsizei size, ALsizei freq)
{
    alBufferData(buffer->buffer, format, data, size, freq);
}

void CGL_audio_listener_set_position(CGL_vec3 position)
{
    alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void CGL_audio_listener_set_velocity(CGL_vec3 velocity)
{
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void CGL_audio_listener_set_orientation(CGL_vec3 forward, CGL_vec3 up)
{
    CGL_float orientation[6] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
    alListenerfv(AL_ORIENTATION, orientation);
}


#endif



#ifndef CGL_EXCLUDE_TRAIL_RENDERER

#ifndef CGL_TRAIL_MAX_POINTS
#define CGL_TRAIL_MAX_POINTS 1024
#endif

/*
struct CGL_trail_point
{
    CGL_vec3 position;
    CGL_trail_point* next;
    void* user_data;
    CGL_int index;
    CGL_float lifespan;
    CGL_float thickness;
    CGL_float distance;
};
*/

static const char* __CGL_TRAIL_DEFAULT_VERTEX_SHADER = "#version 430 core\n"
"\n"
"layout (location = 0) in vec4 position;\n" // w is lifespan
"layout (location = 1) in vec4 normal;\n" // w is distance
"layout (location = 2) in vec4 texcoord;\n" // zw is reserved for future use
"\n"
"out vec3 Position;\n"
"out vec2 TexCoord;\n"
"out float Distance;\n"
"\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * vec4(position.xyz, 1.0f);\n"
"	Position = position.xyz;\n"
"	TexCoord = texcoord.xy;\n"
"   Distance = normal.w;\n"
"}\n";

static const char* __CGL_TRAIL_DEFAULT_FRAGMENT_SHADER = "#version 430 core\n"
"\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Position;\n"
"in vec2 TexCoord;\n"
"in float Distance;\n"
"\n"
"void main()\n"
"{\n"
"	FragColor = vec4(vec3(1.0f - 0.0f), 1.0f);\n"
"}\n";


struct CGL_trail
{
    CGL_trail_point points[CGL_TRAIL_MAX_POINTS];
    CGL_trail_point* first;
    CGL_trail_point* last;
    CGL_shader* shader;
    CGL_mesh_gpu* mesh;
    CGL_mesh_cpu* mesh_cpu;
    void* user_data;
    CGL_trail_point_update_function trail_point_func;
    CGL_int point_count;
    CGL_int resolution;
    CGL_float min_points_distance;
    CGL_float max_length;
    CGL_float length;
};

CGL_int __CGL_trail_get_next_index(CGL_trail* trail)
{
    CGL_int index = -1;
    for(CGL_int i = 0; i < CGL_TRAIL_MAX_POINTS; i++)
    {
        if(trail->points[i].index == -1)
        {
            trail->points[i].distance = 0.0f;
            trail->points[i].next = NULL;
            trail->points[i].lifespan = 0.0f;
            index = i;
            break;
        }
    }
    return index;
}

CGL_trail* CGL_trail_create()
{
    CGL_trail* trail = (CGL_trail*)CGL_malloc(sizeof(CGL_trail));
    if(!trail) { CGL_log_internal("Trail: Failed to allocate memory"); return NULL; }
    for(CGL_int i = 0; i < CGL_TRAIL_MAX_POINTS; i++) trail->points[i].index = -1;
    trail->first = NULL;
    trail->last = NULL;
    trail->user_data = NULL;
    trail->trail_point_func = NULL;
    trail->point_count = 0;
    trail->min_points_distance = 0.0f;
    trail->max_length = 1000000000.0f;
    trail->length = 0.0f;
    trail->resolution = 3;
    trail->mesh = CGL_mesh_gpu_create();
    trail->mesh_cpu = CGL_mesh_cpu_create(1024, 1024);
    trail->shader = CGL_shader_create(__CGL_TRAIL_DEFAULT_VERTEX_SHADER, __CGL_TRAIL_DEFAULT_FRAGMENT_SHADER, NULL);
    return trail;
}

void CGL_trail_destroy(CGL_trail* trail)
{
    CGL_mesh_cpu_destroy(trail->mesh_cpu);
    CGL_mesh_gpu_destroy(trail->mesh);
    CGL_shader_destroy(trail->shader);
    CGL_free(trail);
}

void CGL_trail_render(CGL_trail* trail, CGL_mat4* view, CGL_mat4* projection, CGL_shader* shader)
{
    if(!trail->mesh) return;
    if(!shader) { shader = trail->shader; CGL_shader_bind(shader);}
    CGL_shader_set_uniform_mat4(shader, CGL_shader_get_uniform_location(shader, "projection"), projection);
    CGL_shader_set_uniform_mat4(shader, CGL_shader_get_uniform_location(shader, "view"), view);
    CGL_shader_set_uniform_float(shader, CGL_shader_get_uniform_location(shader, "total_length"), trail->length);
    CGL_mesh_gpu_render(trail->mesh);
}

void CGL_trail_add_point(CGL_trail* trail, CGL_vec3 point, CGL_float lifespan, CGL_float thickness)
{
    if(trail->last)
    {
        CGL_float dist = sqrtf(powf(point.x - trail->last->position.x, 2) + powf(point.y - trail->last->position.y, 2) + powf(point.z - trail->last->position.z, 2));
        if(dist < trail->min_points_distance) return;
    }
    CGL_int index = __CGL_trail_get_next_index(trail);
    if(index == -1) { CGL_log_internal("Trail: Failed to add point, trail is full"); return; }
    CGL_trail_point* new_point = &trail->points[index];
    new_point->position = point;
    new_point->lifespan = lifespan;
    new_point->thickness = thickness;
    new_point->distance = 0.0f;
    new_point->user_data = NULL;
    new_point->index = index;
    new_point->next = trail->first;
    trail->first = new_point;
    trail->last = new_point;
    trail->point_count++;    
    CGL_trail_calculate_distances(trail);
}

void CGL_trail_calculate_distances(CGL_trail* trail)
{
    CGL_trail_point* point = trail->first;
    CGL_trail_point* previous = NULL;
    CGL_float distance = 0.0f;
    while(point)
    {
        if(previous) { distance += sqrtf(powf(point->position.x - previous->position.x, 2) + powf(point->position.y - previous->position.y, 2) + powf(point->position.z - previous->position.z, 2)); }
        point->distance = distance;
        previous = point;
        point = point->next;
    }
    trail->length = distance;
}

void CGL_trail_set_resolution(CGL_trail* trail, CGL_int resolution)
{
    trail->resolution = resolution; // to be implemented in the future
}

void CGL_trail_update(CGL_trail* trail, CGL_float delta_time)
{
    CGL_trail_point* point = trail->first;
    CGL_trail_point* previous = NULL;
    CGL_bool remove = false;
    while(point)
    {
        remove = false;
        point->lifespan -= delta_time;
        if(trail->trail_point_func) remove = trail->trail_point_func(trail, point);
        if(point->lifespan <= 0.0f || remove || point->distance > trail->max_length)
        {
            if(previous) previous->next = point->next;
            else trail->first = point->next;
            point->index = -1;
            trail->point_count--;
            point = point->next;
        }
        if(!point) break;
        previous = point;
        point = point->next;
    }
}

void CGL_trail_bake_mesh(CGL_trail* trail)
{
    if(trail->point_count < 2) return;    
    CGL_mesh_cpu* mesh = trail->mesh_cpu;

    if (mesh->index_count < trail->point_count * trail->resolution * 6 || mesh->vertex_count < trail->point_count * trail->resolution * 6)
    {
        CGL_sizei vct = mesh->vertex_count;
        CGL_sizei ict = mesh->index_count;
        CGL_mesh_cpu_destroy(mesh);
        mesh = CGL_mesh_cpu_create(
            CGL_utils_max(trail->point_count * trail->resolution * 6, vct),
            CGL_utils_max(trail->point_count * trail->resolution * 6, ict));
        trail->mesh_cpu = mesh;
    }
    mesh->vertex_count_used = mesh->index_count_used = 0;
    
    CGL_vec3 rt, tp, front, a0, a1, b0, b1, tmp0, tmp1;
    CGL_float angle0, angle1, angle_step = 360.0f / trail->resolution;
    CGL_trail_point* point = trail->first;
    while(point->next)
    {
        front = CGL_vec3_sub(point->next->position, point->position);
        CGL_vec3_normalize(front);
        if(front.x != 0.0f) rt = CGL_vec3_init(-front.y / front.x, 1.0f, 0.0f);
        else if(front.y != 0.0f) rt = CGL_vec3_init(0.0f, -front.z / front.y, 1.0f);
        else rt = CGL_vec3_init(1.0f, 0.0f, -front.x / front.z);
        tp = CGL_vec3_cross(front, rt);
        CGL_vec3_normalize(rt);
        CGL_vec3_normalize(tp);
        for(CGL_int i = 0 ; i < trail->resolution; i ++)
        {
            angle0 = angle_step * i;
            CGL_float c0 = cosf(angle0) * point->thickness; CGL_float s0 = sinf(angle0) * point->thickness;
            a0 = CGL_vec3_scale(rt, s0);
            tmp0 = CGL_vec3_scale(tp, c0);
            a0 = CGL_vec3_add(a0, tmp0);
            a1 = CGL_vec3_add(a0, point->next->position);
            a0 = CGL_vec3_add(a0, point->position);


            angle1 = angle_step * ((i + 1) % trail->resolution);
            CGL_float c1 = cosf(angle1) * point->thickness; CGL_float s1 = sinf(angle1) * point->thickness;
            b0 = CGL_vec3_scale(rt, s1);
            tmp1 = CGL_vec3_scale(tp, c1);
            b0 = CGL_vec3_add(b0, tmp1);
            b1 = CGL_vec3_add(b0, point->next->position);
            b0 = CGL_vec3_add(b0, point->position);

            //CGL_mesh_cpu_add_quad(mesh, a0, b0, b1, a1);

            // triangle 1
            mesh->vertices[mesh->vertex_count_used + 0].position = CGL_vec4_init(a0.x, a0.y, a0.z, point->lifespan);
            mesh->vertices[mesh->vertex_count_used + 0].normal = CGL_vec4_init(a0.x, a0.y, a0.z, point->distance);

            mesh->vertices[mesh->vertex_count_used + 1].position = CGL_vec4_init(b0.x, b0.y, b0.z, point->lifespan);
            mesh->vertices[mesh->vertex_count_used + 1].normal = CGL_vec4_init(b0.x, b0.y, b0.z, point->distance);

            mesh->vertices[mesh->vertex_count_used + 2].position = CGL_vec4_init(b1.x, b1.y, b1.z, point->lifespan);
            mesh->vertices[mesh->vertex_count_used + 2].normal = CGL_vec4_init(b1.x, b1.y, b1.z, point->distance);

            // triangle 2
            mesh->vertices[mesh->vertex_count_used + 3].position = CGL_vec4_init(a0.x, a0.y, a0.z, point->lifespan);
            mesh->vertices[mesh->vertex_count_used + 3].normal = CGL_vec4_init(a0.x, a0.y, a0.z, point->distance);

            mesh->vertices[mesh->vertex_count_used + 4].position = CGL_vec4_init(b1.x, b1.y, b1.z, point->lifespan);
            mesh->vertices[mesh->vertex_count_used + 4].normal = CGL_vec4_init(b1.x, b1.y, b1.z, point->distance);

            mesh->vertices[mesh->vertex_count_used + 5].position = CGL_vec4_init(a1.x, a1.y, a1.z, point->lifespan);
            mesh->vertices[mesh->vertex_count_used + 5].normal = CGL_vec4_init(a1.x, a1.y, a1.z, point->distance);

            mesh->vertex_count_used += 6;
            mesh->index_count_used += 6;
        }   
        
        point = point->next;
    }


    CGL_mesh_gpu_upload(trail->mesh, mesh, false);
}

void CGL_trail_clear(CGL_trail* trail)
{
    for(CGL_int i = 0; i < CGL_TRAIL_MAX_POINTS; i++) trail->points[i].index = -1;
    trail->first = NULL;
    trail->last = NULL;
    trail->point_count = 0;
}

void CGL_trail_set_point_update_function(CGL_trail* trail, CGL_trail_point_update_function function)
{
    trail->trail_point_func = function;
}

CGL_mesh_gpu* CGL_trail_get_mesh_gpu(CGL_trail* trail)
{
    return trail->mesh;
}

CGL_trail_point* CGL_trail_get_first_point(CGL_trail* trail)
{
    return trail->first;
}

void CGL_trail_set_user_data(CGL_trail* trail, void* user_data)
{
    trail->user_data = user_data;
}

void* CGL_trail_get_user_data(CGL_trail* trail)
{
    return trail->user_data;
}

CGL_float CGL_trail_get_length(CGL_trail* trail)
{
    return trail->length;
}

void CGL_trail_set_max_length(CGL_trail* trail, CGL_float length)
{
    trail->max_length = length;
}

void CGL_trail_set_min_points_distance(CGL_trail* trail, CGL_float min_points_distance)
{
    trail->min_points_distance = min_points_distance;
}

#endif


#endif // CGL_IMPLEMENTATION

#endif // CGL_H