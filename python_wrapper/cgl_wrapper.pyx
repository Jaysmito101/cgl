from libcpp cimport bool
from libc.stdint cimport uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, uintptr_t, intptr_t
cimport libc.stdlib as stdlib
cimport libc.math as math
import copy
from typing import Callable

# CGL typedefs of standard types

cdef extern from "cgl.h":
    ctypedef unsigned char CGL_ubyte;
    ctypedef unsigned short CGL_ushort;
    ctypedef unsigned int CGL_uint;
    ctypedef unsigned long CGL_ulong;
    ctypedef unsigned long long CGL_ulonglong;
    ctypedef char CGL_byte;
    ctypedef short CGL_short;
    ctypedef int CGL_int;
    ctypedef long CGL_long;
    ctypedef long long CGL_longlong;
    ctypedef float CGL_float;
    ctypedef double CGL_double;
    ctypedef long double CGL_longdouble;
    ctypedef uint64_t CGL_sizei;
    ctypedef bool CGL_bool;
    ctypedef void CGL_void;

    CGL_bool CGL_init()
    void CGL_shutdown()

    void CGL_utils_sleep(const CGL_sizei milliseconds)
    CGL_byte* CGL_utils_read_file(const char* path, CGL_sizei* size)
    CGL_sizei CGL_utils_get_file_size(const CGL_byte* path)
    CGL_bool CGL_utils_append_file(const CGL_byte* path, const CGL_byte* data, size_t size)
    CGL_bool CGL_utils_write_file(const CGL_byte* path, const CGL_byte* data, size_t size)
    CGL_float CGL_utils_get_time()
    void CGL_utils_get_timestamp(char* buffer)
    CGL_bool CGL_utils_is_little_endian()
    CGL_sizei CGL_utils_get_random_with_probability(CGL_float* probabilities, CGL_sizei count)
    void CGL_utils_reverse_bytes(void* data, size_t size)
    void CGL_utils_little_endian_to_current(void* data, size_t size)
    void CGL_utils_big_endian_to_current(void* data, size_t size)
    void CGL_utils_fast_srand(CGL_int seed)
    CGL_int CGL_utils_fast_rand()
    CGL_ulong CGL_utils_xorshf96()
    void CGL_utils_srand31(CGL_uint seed)
    CGL_uint CGL_utils_rand31()

    CGL_float CGL_utils_sigmoid(CGL_float x)
    CGL_float CGL_utils_sigmoid_derivative(CGL_float x)
    CGL_float CGL_utils_relu(CGL_float x)
    CGL_float CGL_utils_relu_derivative(CGL_float x)
    CGL_float CGL_utils_tanh(CGL_float x)
    CGL_float CGL_utils_tanh_derivative(CGL_float x)
    CGL_float CGL_utils_step(CGL_float x)
    CGL_float CGL_utils_step_derivative(CGL_float x)
    CGL_float CGL_utils_relu_leaky(CGL_float x)
    CGL_float CGL_utils_relu_leaky_derivative(CGL_float x)
    CGL_float CGL_utils_relu_smooth(CGL_float x)
    CGL_float CGL_utils_relu_smooth_derivative(CGL_float x)

    void CGL_console_set_color(uint8_t color)
    void CGL_printf_red(const char* format, ...)
    void CGL_printf_green(const char* format, ...)
    void CGL_printf_gray(const char* format, ...)
    void CGL_printf_blue(const char* format, ...)
    void CGL_console_progress_bar(CGL_float progress, CGL_int width, CGL_byte* prefix, CGL_byte* suffix, CGL_byte complete_char, CGL_byte incomplete_char)

    cdef struct CGL_logger_context:
        pass

    void CGL_logger_init(bool enable_console_logging);
    void CGL_logger_shutdown();
    bool CGL_logger_attach_log_file(const char* path);
    bool CGL_logger_detach_log_file(const char* path);
    void CGL_logger_flush();
    void CGL_logger_disable_console_logging();
    void CGL_logger_enable_console_logging();
    void CGL_logger_log(CGL_int level, const char* log_format, ...);

    uint32_t CGL_utils_crc32(const void* data, size_t size)
    uint64_t CGL_utils_crc64(const void* data, size_t size)
    void CGL_utils_rot13(const char* data_in, char* data_out)
    uint32_t CGL_utils_super_fast_hash(const void* data, size_t size)

    cdef struct CGL_vec2:
        CGL_float x
        CGL_float y

    cdef struct CGL_vec3:
        CGL_float x
        CGL_float y
        CGL_float z

    cdef struct CGL_vec4:
        CGL_float x
        CGL_float y
        CGL_float z
        CGL_float w
    ctypedef CGL_vec4 CGL_color

    cdef struct CGL_ivec4:
        CGL_int x
        CGL_int y
        CGL_int z
        CGL_int w

    cdef struct CGL_mat3:
        CGL_float m[9]

    cdef struct CGL_mat4:
        CGL_float m[16]

    cdef struct CGL_quat:
        CGL_float x
        CGL_float y
        CGL_float z
        CGL_float w

    cdef CGL_float CGL_float_quadratic_lerp(CGL_float a, CGL_float b, CGL_float c, CGL_float t)
    cdef CGL_float CGL_float_cubic_lerp(CGL_float a, CGL_float b, CGL_float c, CGL_float d, CGL_float t)

    CGL_vec2 CGL_vec2_triple_product(CGL_vec2 a, CGL_vec2 b, CGL_vec2 c)
    CGL_vec3 CGL_vec3_reflect(CGL_vec3 a, CGL_vec3 n)
    CGL_vec3 CGL_vec3_rotate_about_axis(CGL_vec3 v, CGL_vec3 axis, CGL_float theta)
    CGL_vec3 CGL_vec3_triple_product(CGL_vec3 a, CGL_vec3 b, CGL_vec3 c)
    void CGL_vec3_calculate_orthonormal_basis_from_one_vector(CGL_vec3 a, CGL_vec3* pb, CGL_vec3* pc)
    CGL_vec4 CGL_vec4_triple_product(CGL_vec4 a, CGL_vec4 b, CGL_vec4 c)

    CGL_mat4 CGL_mat4_mul(CGL_mat4 a, CGL_mat4 b)
    CGL_float CGL_mat4_det(CGL_mat4 m)
    CGL_float CGL_mat4_det_by_lu(CGL_mat4 m)
    CGL_float CGL_mat4_det_by_gauss(CGL_mat4 m)
    CGL_vec4 CGL_mat4_mul_vec4(CGL_mat4 m, CGL_vec4 v)
    CGL_mat4 CGL_mat4_inverse(CGL_mat4 m)
    CGL_mat4 CGL_mat4_transpose(CGL_mat4 m)
    CGL_mat4 CGL_mat4_adjoint(CGL_mat4 m)
    CGL_mat4 CGL_mat4_gauss_elim(CGL_mat4 m)
    CGL_int CGL_mat4_rank(CGL_mat4 m)
    CGL_float CGL_mat4_trace(CGL_mat4 m)
    CGL_mat3 CGL_mat4_to_mat3(CGL_mat4 m)
    CGL_mat4 CGL_mat4_from_mat3(CGL_mat3 m)
    CGL_mat4 CGL_mat4_rotate_about_axis(CGL_vec3 axis, CGL_float angle)
    CGL_mat4 CGL_mat4_look_at(CGL_vec3 eye, CGL_vec3 target, CGL_vec3 up)
    CGL_mat4 CGL_mat4_lerp(CGL_mat4 a, CGL_mat4 b, CGL_float t)
    void CGL_mat4_decompose_lu(CGL_mat4 m, CGL_mat4* l, CGL_mat4* u)
    
    CGL_mat4 CGL_mat4_perspective(CGL_float fov, CGL_float aspect, CGL_float near, CGL_float far)
    CGL_mat4 CGL_mat4_orthographic(CGL_float left, CGL_float right, CGL_float bottom, CGL_float top, CGL_float near, CGL_float far)
    CGL_mat4 CGL_mat4_zero()
    CGL_mat4 CGL_mat4_identity()
    CGL_mat4 CGL_mat4_scale(CGL_float x, CGL_float y, CGL_float z)
    CGL_mat4 CGL_mat4_translate(CGL_float x, CGL_float y, CGL_float z)
    CGL_mat4 CGL_mat4_rotate_x(CGL_float angle)
    CGL_mat4 CGL_mat4_rotate_y(CGL_float angle)
    CGL_mat4 CGL_mat4_rotate_z(CGL_float angle)

    cdef struct CGL_window:
        pass
    
    cdef struct GLFWwindow:
        pass
    
    ctypedef void(*CGL_window_key_callback)(CGL_window* window, CGL_int key, CGL_int scancode, CGL_int action, CGL_int mods)
    ctypedef void(*CGL_window_mouse_button_callback)(CGL_window* window, CGL_int button, CGL_int action, CGL_int mods)
    ctypedef void(*CGL_window_mouse_position_callback)(CGL_window* window, double xpos, double ypos)
    ctypedef void(*CGL_window_mouse_scroll_callback)(CGL_window* window, double xoffset, double yoffset)
    ctypedef void(*CGL_window_framebuffer_size_callback)(CGL_window* window, CGL_int width, CGL_int height)
    ctypedef void(*CGL_window_close_callback)(CGL_window* window)
    ctypedef void(*CGL_window_drag_n_drop_callback)(CGL_window* window, const CGL_byte** paths, CGL_int count)

    CGL_window* CGL_window_create(CGL_int width, CGL_int height, const char* title)
    CGL_window* CGL_window_create_undecorated(CGL_int width, CGL_int height, const char* title)
    void CGL_window_destroy(CGL_window* window)
    void CGL_window_poll_events(CGL_window* window)
    void CGL_window_swap_buffers(CGL_window* window)
    bool CGL_window_should_close(CGL_window* window)
    void CGL_window_set_title(CGL_window* window, const char* title)
    void CGL_window_set_size(CGL_window* window, CGL_int width, CGL_int height)
    void CGL_window_set_position(CGL_window* window, CGL_int x, CGL_int y)
    void CGL_window_set_hidden(CGL_window* window, bool hidden)
    void CGL_window_set_user_data(CGL_window* window, void* user_data)
    void* CGL_window_get_user_data(CGL_window* window)
    void CGL_window_get_size(CGL_window* window, CGL_int* width, CGL_int* height)
    void CGL_window_get_position(CGL_window* window, CGL_int* x, CGL_int* y)
    void CGL_window_get_framebuffer_size(CGL_window* window, CGL_int* width, CGL_int* height)
    void CGL_window_set_key_callback(CGL_window* window, CGL_window_key_callback callback)
    void CGL_window_set_mouse_button_callback(CGL_window* window, CGL_window_mouse_button_callback callback)
    void CGL_window_set_mouse_position_callback(CGL_window* window, CGL_window_mouse_position_callback callback)
    void CGL_window_set_mouse_scroll_callback(CGL_window* window, CGL_window_mouse_scroll_callback callback)
    void CGL_window_set_framebuffer_size_callback(CGL_window* window, CGL_window_framebuffer_size_callback callback)
    void CGL_window_set_close_callback(CGL_window* window, CGL_window_close_callback callback)
    void CGL_window_set_drag_n_drop_callback(CGL_window* window, CGL_window_drag_n_drop_callback callback)
    void CGL_window_resecure_callbacks(CGL_window* window)
    void CGL_window_make_context_current(CGL_window* window)
    GLFWwindow* CGL_window_get_glfw_handle(CGL_window* window)
    CGL_int CGL_window_get_key(CGL_window* window, CGL_int key)
    CGL_bool CGL_window_is_key_pressed(CGL_window* window, CGL_int key)
    CGL_int CGL_window_get_mouse_button(CGL_window* window, CGL_int button)
    void CGL_window_get_mouse_position(CGL_window* window, double* xpos, double* ypos)


CGL_TRUE = 1
CGL_FALSE = 0

def free(ptr):
    stdlib.free(<void*>ptr)

def init() -> bool:
    return CGL_init()

def shutdown():
    CGL_shutdown()

def utils_sleep(milliseconds: int):
    CGL_utils_sleep(milliseconds)

def utils_read_file(path: str) -> bytes:
    cdef CGL_sizei size
    cdef CGL_byte* data = CGL_utils_read_file(path.encode(), &size)
    if data is NULL:
        raise MemoryError()
    result = copy.deepcopy(data[:size])
    with nogil:
        stdlib.free(data)
    return result

def utils_get_file_size(path: str) -> int:
    return CGL_utils_get_file_size(path.encode())

def utils_append_file(path: str, data: bytes) -> bool:
    return CGL_utils_append_file(path.encode(), data, len(data))

def utils_write_file(path : str, data : bytes) -> bool:
    return CGL_utils_write_file(path.encode(), data, len(data))

def utils_get_time() -> float:
    return CGL_utils_get_time()

def utils_get_timestamp() -> str:
    cdef CGL_byte buffer[128]
    CGL_utils_get_timestamp(buffer)
    return buffer.decode()

def utils_is_little_endian() -> bool:
    return CGL_utils_is_little_endian()

def utils_is_big_endian() -> bool:
    return not CGL_utils_is_little_endian()

def utils_get_random_with_probability(probabilities) -> int:
    cdef CGL_float* c_probabilities
    c_probabilities = <CGL_float*>stdlib.malloc(len(probabilities) * sizeof(CGL_float))
    if c_probabilities is NULL:
        raise MemoryError()
    for i in range(len(probabilities)):
        c_probabilities[i] = probabilities[i]
    result = CGL_utils_get_random_with_probability(c_probabilities, len(probabilities))
    with nogil:
        stdlib.free(c_probabilities)
    return result

def utils_reverse_bytes(data: bytes):
    CGL_utils_reverse_bytes(<void*>data, len(data))

def utils_little_endian_to_current(data : bytes):
    CGL_utils_little_endian_to_current(<void*>data, len(data))

def utils_big_endian_to_current(data : bytes):
    CGL_utils_big_endian_to_current(<void*>data, len(data))

def utils_fast_srand(seed: int):
    CGL_utils_fast_srand(seed)

def utils_fast_rand() -> int:
    return CGL_utils_fast_rand()

def utils_xorshf96() -> int:
    return CGL_utils_xorshf96()

def utils_srand31(seed: int):
    CGL_utils_srand31(seed)

def utils_rand31() -> int:
    return CGL_utils_rand31()

def utils_sigmoid(x: float) -> float:
    CGL_utils_sigmoid(x)

def utils_sigmoid_derivative(x: float) -> float:
    CGL_utils_sigmoid_derivative(x)

def utils_relu(x: float) -> float:
    CGL_utils_relu(x)

def utils_relu_derivative(x: float) -> float:
    CGL_utils_relu_derivative(x)

def utils_tanh(x: float) -> float:
    CGL_utils_tanh(x)

def utils_tanh_derivative(x: float) -> float:
    CGL_utils_tanh_derivative(x)

def utils_step(x: float) -> float:
    CGL_utils_step(x)

def utils_step_derivative(x: float) -> float:
    CGL_utils_step_derivative(x)

def utils_relu_leaky(x: float) -> float:
    CGL_utils_relu_leaky(x)

def utils_relu_leaky_derivative(x: float) -> float:
    CGL_utils_relu_leaky_derivative(x)

def utils_relu_smooth(x: float) -> float:
    CGL_utils_relu_smooth(x)

def utils_relu_smooth_derivative(x: float) -> float:
    CGL_utils_relu_smooth_derivative(x)


CONSOLE_COLOR_RESET = 0
CONSOLE_COLOR_RED = 1
CONSOLE_COLOR_GREEN = 2
CONSOLE_COLOR_GRAY = 3
CONSOLE_COLOR_BLUE = 4

def console_set_color(color: int):
    if color < 0 or color > 4:
        raise ValueError()
    CGL_console_set_color(color)

def printf_red(format: str):
    CGL_printf_red(format.encode())

def printf_green(format: str):
    CGL_printf_green(format.encode())

def printf_gray(format: str):
    CGL_printf_gray(format.encode())

def printf_blue(format: str):
    CGL_printf_blue(format.encode())

def console_progress_bar(progress: float, width: int, prefix: str, suffix: str, complete_char: str = "#", incomplete_char: str = "_"):
    CGL_console_progress_bar(progress, width, prefix.encode(), suffix.encode(), complete_char[0], incomplete_char[0])

LOG_LEVEL_TRACE        = 0
LOG_LEVEL_INFO         = 1
LOG_LEVEL_WARN         = 2
LOG_LEVEL_ERROR        = 3 
LOG_LEVEL_INTERNAL     = 4 
LOGGER_MAX_LOG_FILES   = 32
LOGGER_LOG_BUFFER_SIZE = (1024 * 4)


def logger_init(enable_console_logging: bool):
    CGL_logger_init(enable_console_logging)

def logger_shutdown():
    CGL_logger_shutdown()

def logger_attach_log_file(path: str) -> bool:
    return CGL_logger_attach_log_file(path.encode())

def logger_detach_log_file(path: str) -> bool:
    return CGL_logger_detach_log_file(path.encode())

def logger_flush():
    CGL_logger_flush()
    
def logger_disable_console_logging():
    CGL_logger_disable_console_logging()

def logger_enable_console_logging():
    CGL_logger_enable_console_logging()

def logger_log(level: int, log_format: str):
    CGL_logger_log(level, log_format.encode())

def log_trace(log_format: str):
    CGL_logger_log(LOG_LEVEL_TRACE, log_format.encode())

def log_info(log_format: str):
    CGL_logger_log(LOG_LEVEL_INFO, log_format.encode())

def log_warn(log_format: str):
    CGL_logger_log(LOG_LEVEL_WARN, log_format.encode())

def log_error(log_format: str):
    CGL_logger_log(LOG_LEVEL_ERROR, log_format.encode())

def log_internal(log_format: str):
    CGL_logger_log(LOG_LEVEL_INTERNAL, log_format.encode())

def utils_crc32(data: bytes) -> int:
    return CGL_utils_crc32(<void*>data, len(data))

def utils_crc64(data: bytes) -> int:
    return CGL_utils_crc64(<void*>data, len(data))

def utils_rot13(data: bytes) -> bytes:
    result = bytes(len(data))
    CGL_utils_rot13(<char*>data, <char*>result)
    return result

def utils_super_fast_hash(data: bytes) -> int:
    return CGL_utils_super_fast_hash(<void*>data, len(data))


PI = (3.14159265358979323846)
TWO_PI = (6.28318530717958647692)
PI_2 = (1.57079632679489661923)
E = (2.71828182845904523536)
SQRT2 = (1.41421356237309504880)
SQRT3 = (1.73205080756887729352)
SQRT5 = (2.23606797749978969640)
SQRT6 = (2.44948974278317809820)
SQRT7 = (2.64575131106459059050)
SQRT8 = (2.82842712474619009760)

def deg_to_rad(deg):
    return ((deg) * (PI / 180.0))

def rad_to_deg(rad):
    return ((rad) * (180.0 / PI))

def float_lerp(a: float, b: float, t: float):
    return ((a) + ((b) - (a)) * (t))

def float_quadratic_lerp(a: float, b: float, c: float, t: float) -> float:
    return CGL_float_quadratic_lerp(a, b, c, t)

def float_cubic_lerp(a: float, b: float, c: float, d: float, t: float) -> float:
    return CGL_float_cubic_lerp(a, b, c, d, t)

cdef class vec2:
    cdef CGL_vec2 c_vec2

    def __init__(self, x: float = 0.0, y: float = 0.0):
        self.c_vec2 = CGL_vec2(x, y)
    
    def __repr__(self):
        return f"vec2({self.x}, {self.y})"
    
    def __str__(self):
        return f"vec2({self.x}, {self.y})"

    def __bytes__(self):
        return bytes(self.c_vec2)
    
    def __len__(self):
        return 2
    
    def __getitem__(self, index: int):
        if index == 0:
            return self.c_vec2.x
        elif index == 1:
            return self.c_vec2.y
        else:
            raise IndexError()
    
    def __setitem__(self, index: int, value: int):
        if index == 0:
            self.c_vec2.x = value
        elif index == 1:
            self.c_vec2.y = value
        else:
            raise IndexError()
    
    def __eq__(self, other: vec2):
        return self.c_vec2.x == other.c_vec2.x and self.c_vec2.y == other.c_vec2.y
    
    def __ne__(self, other: vec2):
        return self.c_vec2.x != other.c_vec2.x or self.c_vec2.y != other.c_vec2.y
    
    def __hash__(self):
        return hash((self.c_vec2.x, self.c_vec2.y))
    
    def __getattr__(self, name: str):
        if name == "x":
            return self.c_vec2.x
        elif name == "y":
            return self.c_vec2.y
        else:
            raise AttributeError()
    
    def __setattr__(self, name: str, value):
        if name == "x":
            self.c_vec2.x = value
        elif name == "y":
            self.c_vec2.y = value
        else:
            raise AttributeError()
    
    def __add__(self, other: vec2):
        return vec2(self.x + other.x, self.y + other.y)
    
    def __sub__(self, other: vec2):
        return vec2(self.x - other.x, self.y - other.y)
    
    def __mul__(self, other: vec2):
        return vec2(self.x * other.x, self.y * other.y)
    
    def __truediv__(self, other: vec2):
        return vec2(self.x / other.x, self.y / other.y)
    
    def __floordiv__(self, other: vec2):
        return vec2(self.x // other.x, self.y // other.y)
    
    def dot(self, other: vec2):
        return (self.x * other.x) + (self.y * other.y)
    
    def cross(self, other: vec2):
        return (self.x * other.y) - (self.y * other.x)
    
    def length(self):
        return math.sqrt((self.x * self.x) + (self.y * self.y))
    
    def length_squared(self):
        return (self.x * self.x) + (self.y * self.y)
    
    def normalized(self):
        length = self.length()
        return vec2(self.x / length, self.y / length)
    
def vec2_triple_product(a: vec2, b: vec2, c: vec2) -> vec2:
    cdef CGL_vec2 obj = vec2(CGL_vec2_triple_product(a.c_vec2, b.c_vec2, c.c_vec2))
    return vec2(obj.x, obj.y)


cdef class vec3:
    cdef CGL_vec3 c_vec3

    def __init__(self, x: float = 0.0, y: float = 0.0, z: float = 0.0):
        self.c_vec3 = CGL_vec3(x, y, z)
    
    def __repr__(self):
        return f"vec3({self.x}, {self.y}, {self.z})"
    
    def __str__(self):
        return f"vec3({self.x}, {self.y}, {self.z})"

    def __bytes__(self):
        return bytes(self.c_vec3)
    
    def __len__(self):
        return 3
    
    def __getitem__(self, index: int):
        if index == 0:
            return self.c_vec3.x
        elif index == 1:
            return self.c_vec3.y
        elif index == 2:
            return self.c_vec3.z
        else:
            raise IndexError()
    
    def __setitem__(self, index: int, value: int):
        if index == 0:
            self.c_vec3.x = value
        elif index == 1:
            self.c_vec3.y = value
        elif index == 2:
            self.c_vec3.z = value
        else:
            raise IndexError()
    
    def __eq__(self, other: vec3):
        return self.c_vec3.x == other.c_vec3.x and self.c_vec3.y == other.c_vec3.y and self.c_vec3.z == other.c_vec3.z
    
    def __ne__(self, other: vec3):
        return self.c_vec3.x != other.c_vec3.x or self.c_vec3.y != other.c_vec3.y or self.c_vec3.z != other.c_vec3.z
    
    
    def __hash__(self):
        return hash((self.c_vec3.x, self.c_vec3.y, self.c_vec3.z))
    
    def __getattr__(self, name: str):
        if name == "x":
            return self.c_vec3.x
        elif name == "y":
            return self.c_vec3.y
        elif name == "z":
            return self.c_vec3.z
        else:
            raise AttributeError()
    
    def __setattr__(self, name: str, value):
        if name == "x":
            self.c_vec3.x = value
        elif name == "y":
            self.c_vec3.y = value
        elif name == "z":
            self.c_vec3.z = value
        else:
            raise AttributeError()
    
    def __add__(self, other: vec3):
        return vec3(self.x + other.x, self.y + other.y, self.z + other.z)
    
    def __sub__(self, other: vec3):
        return vec3(self.x - other.x, self.y - other.y, self.z - other.z)
    
    def __mul__(self, other: vec3):
        return vec3(self.x * other.x, self.y * other.y, self.z * other.z)
    
    def __truediv__(self, other: vec3):
        return vec3(self.x / other.x, self.y / other.y, self.z / other.z)
    
    def __floordiv__(self, other: vec3):
        return vec3(self.x // other.x, self.y // other.y, self.z // other.z)
    
    def dot(self, other: vec3):
        return (self.x * other.x) + (self.y * other.y) + (self.z * other.z)
    
    def cross(self, other: vec3):
        return vec3((self.y * other.z) - (self.z * other.y), (self.z * other.x) - (self.x * other.z), (self.x * other.y) - (self.y * other.x))
    
    def length(self):
        return math.sqrt((self.x * self.x) + (self.y * self.y) + (self.z * self.z))
    
    def length_squared(self):
        return (self.x * self.x) + (self.y * self.y) + (self.z * self.z)
    
    def normalized(self):
        cdef float length = self.length()
        return vec3(self.x / length, self.y / length, self.z / length)
    
def vec3_reflect(a: vec3, n: vec3) -> vec3:
    cdef CGL_vec3 obj = vec3(CGL_vec3_reflect(a.c_vec3, n.c_vec3))
    return vec3(obj.x, obj.y, obj.z)

def vec3_rotate_about_axis(v: vec3, axis: vec3, theta: float) -> vec3:
    cdef CGL_vec3 obj = vec3(CGL_vec3_rotate_about_axis(v.c_vec3, axis.c_vec3, theta))
    return vec3(obj.x, obj.y, obj.z)

def vec3_triple_product(a: vec3, b: vec3, c: vec3) -> vec3:
    cdef CGL_vec3 obj = vec3(CGL_vec3_triple_product(a.c_vec3, b.c_vec3, c.c_vec3))
    return vec3(obj.x, obj.y, obj.z)

cdef class vec4:
    cdef CGL_vec4 c_vec4

    def __init__(self, x: float = 0.0, y: float = 0.0, z: float = 0.0, w: float = 0.0):
        self.c_vec4 = CGL_vec4(x, y, z, w)
    
    def __repr__(self):
        return f"vec4({self.x}, {self.y}, {self.z}, {self.w})"
    
    def __str__(self):
        return f"vec4({self.x}, {self.y}, {self.z}, {self.w})"

    def __bytes__(self):
        return bytes(self.c_vec4)
    
    def __len__(self):
        return 4
    
    def __getitem__(self, index: int):
        if index == 0:
            return self.c_vec4.x
        elif index == 1:
            return self.c_vec4.y
        elif index == 2:
            return self.c_vec4.z
        elif index == 3:
            return self.c_vec4.w
        else:
            raise IndexError()
    
    def __setitem__(self, index: int, value: int):
        if index == 0:
            self.c_vec4.x = value
        elif index == 1:
            self.c_vec4.y = value
        elif index == 2:
            self.c_vec4.z = value
        elif index == 3:
            self.c_vec4.w = value
        else:
            raise IndexError()
    
    def __eq__(self, other: vec4):
        return self.c_vec4.x == other.c_vec4.x and self.c_vec4.y == other.c_vec4.y and self.c_vec4.z == other.c_vec4.z and self.c_vec4.w == other.c_vec4.w
    
    def __ne__(self, other: vec4):
        return self.c_vec4.x != other.c_vec4.x or self.c_vec4.y != other.c_vec4.y or self.c_vec4.z != other.c_vec4.z or self.c_vec4.w != other.c_vec4.w
    
    def __hash__(self):
        return hash((self.c_vec4.x, self.c_vec4.y, self.c_vec4.z, self.c_vec4.w))
    
    def __getattr__(self, name: str):
        if name == "x":
            return self.c_vec4.x
        elif name == "y":
            return self.c_vec4.y
        elif name == "z":
            return self.c_vec4.z
        elif name == "w":
            return self.c_vec4.w
        else:
            raise AttributeError()
    
    def __setattr__(self, name: str, value):
        if name == "x":
            self.c_vec4.x = value
        elif name == "y":
            self.c_vec4.y = value
        elif name == "z":
            self.c_vec4.z = value
        elif name == "w":
            self.c_vec4.w = value
        else:
            raise AttributeError()
    
    def __add__(self, other: vec4):
        return vec4(self.x + other.x, self.y + other.y, self.z + other.z, self.w + other.w)
    
    def __sub__(self, other: vec4):
        return vec4(self.x - other.x, self.y - other.y, self.z - other.z, self.w - other.w)
    
    def __mul__(self, other: vec4):
        return vec4(self.x * other.x, self.y * other.y, self.z * other.z, self.w * other.w)
    
    def __truediv__(self, other: vec4):
        return vec4(self.x / other.x, self.y / other.y, self.z / other.z, self.w / other.w)
    
    def __floordiv__(self, other: vec3):
        return vec4(self.x // other.x, self.y // other.y, self.z // other.z, self.w // other.w)
    
    def dot(self, other: vec3):
        return (self.x * other.x) + (self.y * other.y) + (self.z * other.z) + (self.w * other.w)
    
    def cross(self, other: vec3):
        return vec4(self.y * other.z - self.z * other.y, self.z * other.x - self.x * other.z, self.x * other.y - self.y * other.x, 0.0)
    
    def length(self):
        return math.sqrt(self.length_squared())
    
    def length_squared(self):
        return (self.x * self.x) + (self.y * self.y) + (self.z * self.z) + (self.w * self.w)

    def normalized(self):
        length = self.length()
        return vec4(self.x / length, self.y / length, self.z / length, self.w / length)

def vec4_triple_product(a: vec4, b: vec4, c: vec4) -> vec4:
    cdef CGL_vec4 obj = vec4(CGL_vec4_triple_product(a.c_vec4, b.c_vec4, c.c_vec4))
    return vec4(obj.x, obj.y, obj.z, obj.w)

cdef class mat4:
    cdef CGL_mat4 c_mat4

    def __init__(self):
        self.c_mat4 = CGL_mat4()
    
    def __repr__(self):
        result = "mat4x4(\n"
        for i in range(4):
            result += "    "
            for j in range(4):
                result += f"{self.c_mat4.m[i * 4 + j]}, "
            result += "\n"
        result += ")"
        return result
    
    def __str__(self):
        result = "mat4x4(\n"
        for i in range(4):
            result += "    "
            for j in range(4):
                result += f"{self.c_mat4.m[i * 4 + j]}, "
            result += "\n"
        result += ")"
        return result
    
    cdef set_c_mat4(self, c_mat4: CGL_mat4):
        self.c_mat4 = c_mat4
    
    def __mul__(self, other: mat4):
        return mat4_mul(self, other)

def mat4_mul(a: mat4, b: mat4) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_mul(a.c_mat4, b.c_mat4)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_det(m: mat4) -> float:
    return CGL_mat4_det(m.c_mat4)
    
def mat4_det_by_lu(m: mat4) -> float:
    return CGL_mat4_det_by_lu(m.c_mat4)
    
def mat4_det_by_gauss(m: mat4) -> float:
    return CGL_mat4_det_by_gauss(m.c_mat4)
    
def mat4_mul_vec4(m: mat4, v: vec4) -> vec4:
    cdef CGL_vec4 obj = CGL_mat4_mul_vec4(m.c_mat4, v.c_vec4)
    return vec4(obj.x, obj.y, obj.z, obj.w)
    
def mat4_inverse(m: mat4) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_inverse(m.c_mat4)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_transpose(m: mat4) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_transpose(m.c_mat4)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_adjoint(m: mat4) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_adjoint(m.c_mat4)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_gauss_elim(m: mat4) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_gauss_elim(m.c_mat4)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_rank(m: mat4) -> int:
    return CGL_mat4_rank(m.c_mat4)

def mat4_trace(m: mat4) -> float:
    return CGL_mat4_trace(m.c_mat4)
    
def mat4_rotate_about_axis(axis: vec3, angle: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_rotate_about_axis(axis.c_vec3, angle)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_look_at(eye: vec3, target: vec3, up: vec3) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_look_at(eye.c_vec3, target.c_vec3, up.c_vec3)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_lerp(a: mat4, b: mat4, t: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_lerp(a.c_mat4, b.c_mat4, t)
    result = mat4()
    result.set_c_mat4(obj)
    return result
    
def mat4_decompose_lu(m: mat4, l: mat4, u: mat4):
    cdef CGL_mat4 l_obj = CGL_mat4()
    cdef CGL_mat4 u_obj = CGL_mat4()
    CGL_mat4_decompose_lu(m.c_mat4, &l_obj, &u_obj)
    l = mat4()
    u = mat4()
    l.set_c_mat4(l_obj)
    u.set_c_mat4(u_obj)
    return (l, u)

def mat4_perspective(fov: float, aspect: float, near: float, far: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_perspective(fov, aspect, near, far)
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_orthographic(left: float, right: float, bottom: float, top: float, near: float, far: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_orthographic(left, right, bottom, top, near, far)
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_zero() -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_zero()
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_identity() -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_identity()
    result = mat4()
    result.set_c_mat4(obj)
    return result


def mat4_scale(x: float, y: float, z: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_scale(x, y, z)
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_translate(x: float, y: float, z: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_translate(x, y, z)
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_rotate_x(angle: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_rotate_x(angle)
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_rotate_y(angle: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_rotate_y(angle)
    result = mat4()
    result.set_c_mat4(obj)
    return result

def mat4_rotate_z(angle: float) -> mat4:
    cdef CGL_mat4 obj = CGL_mat4_rotate_z(angle)
    result = mat4()
    result.set_c_mat4(obj)
    return result

# TODO: Quaterneon Lib

# TODO: Linear algebra matrix lib


KEY_UNKNOWN            = -1
KEY_SPACE              = 32
KEY_APOSTROPHE         = 39 
KEY_COMMA              = 44 
KEY_MINUS              = 45 
KEY_PERIOD             = 46 
KEY_SLASH              = 47 
KEY_0                  = 48
KEY_1                  = 49
KEY_2                  = 50
KEY_3                  = 51
KEY_4                  = 52
KEY_5                  = 53
KEY_6                  = 54
KEY_7                  = 55
KEY_8                  = 56
KEY_9                  = 57
KEY_SEMICOLON          = 59  
KEY_EQUAL              = 61  
KEY_A                  = 65
KEY_B                  = 66
KEY_C                  = 67
KEY_D                  = 68
KEY_E                  = 69
KEY_F                  = 70
KEY_G                  = 71
KEY_H                  = 72
KEY_I                  = 73
KEY_J                  = 74
KEY_K                  = 75
KEY_L                  = 76
KEY_M                  = 77
KEY_N                  = 78
KEY_O                  = 79
KEY_P                  = 80
KEY_Q                  = 81
KEY_R                  = 82
KEY_S                  = 83
KEY_T                  = 84
KEY_U                  = 85
KEY_V                  = 86
KEY_W                  = 87
KEY_X                  = 88
KEY_Y                  = 89
KEY_Z                  = 90
KEY_LEFT_BRACKET       = 91  
KEY_BACKSLASH          = 92  
KEY_RIGHT_BRACKET      = 93  
KEY_GRAVE_ACCENT       = 96  
KEY_WORLD_1            = 161 
KEY_WORLD_2            = 162 
KEY_ESCAPE             = 256
KEY_ENTER              = 257
KEY_TAB                = 258
KEY_BACKSPACE          = 259
KEY_INSERT             = 260
KEY_DELETE             = 261
KEY_RIGHT              = 262
KEY_LEFT               = 263
KEY_DOWN               = 264
KEY_UP                 = 265
KEY_PAGE_UP            = 266
KEY_PAGE_DOWN          = 267
KEY_HOME               = 268
KEY_END                = 269
KEY_CAPS_LOCK          = 280
KEY_SCROLL_LOCK        = 281
KEY_NUM_LOCK           = 282
KEY_PRINT_SCREEN       = 283
KEY_PAUSE              = 284
KEY_F1                 = 290
KEY_F2                 = 291
KEY_F3                 = 292
KEY_F4                 = 293
KEY_F5                 = 294
KEY_F6                 = 295
KEY_F7                 = 296
KEY_F8                 = 297
KEY_F9                 = 298
KEY_F10                = 299
KEY_F11                = 300
KEY_F12                = 301
KEY_F13                = 302
KEY_F14                = 303
KEY_F15                = 304
KEY_F16                = 305
KEY_F17                = 306
KEY_F18                = 307
KEY_F19                = 308
KEY_F20                = 309
KEY_F21                = 310
KEY_F22                = 311
KEY_F23                = 312
KEY_F24                = 313
KEY_F25                = 314
KEY_KP_0               = 320
KEY_KP_1               = 321
KEY_KP_2               = 322
KEY_KP_3               = 323
KEY_KP_4               = 324
KEY_KP_5               = 325
KEY_KP_6               = 326
KEY_KP_7               = 327
KEY_KP_8               = 328
KEY_KP_9               = 329
KEY_KP_DECIMAL         = 330
KEY_KP_DIVIDE          = 331
KEY_KP_MULTIPLY        = 332
KEY_KP_SUBTRACT        = 333
KEY_KP_ADD             = 334
KEY_KP_ENTER           = 335
KEY_KP_EQUAL           = 336
KEY_LEFT_SHIFT         = 340
KEY_LEFT_CONTROL       = 341
KEY_LEFT_ALT           = 342
KEY_LEFT_SUPER         = 343
KEY_RIGHT_SHIFT        = 344
KEY_RIGHT_CONTROL      = 345
KEY_RIGHT_ALT          = 346
KEY_RIGHT_SUPER        = 347
KEY_MENU               = 348
MOUSE_BUTTON_1         = 0
MOUSE_BUTTON_2         = 1
MOUSE_BUTTON_3         = 2
MOUSE_BUTTON_4         = 3
MOUSE_BUTTON_5         = 4
MOUSE_BUTTON_6         = 5
MOUSE_BUTTON_7         = 6
MOUSE_BUTTON_8         = 7
MOUSE_BUTTON_LAST      = MOUSE_BUTTON_8
MOUSE_BUTTON_LEFT      = MOUSE_BUTTON_1
MOUSE_BUTTON_RIGHT     = MOUSE_BUTTON_2
MOUSE_BUTTON_MIDDLE    = MOUSE_BUTTON_3
RELEASE                = 0
PRESS                  = 1
REPEAT                 = 2

cdef class window:
    cdef CGL_window* c_window
    cdef bool is_decorated
    cdef bool has_been_destroyed
    cdef str title

    def __init__(self, width: int, height: int, title: str = "CGL Window", decorated: bool = True):
        self.is_decorated = decorated
        self.title = title
        if decorated:
            self.c_window = CGL_window_create(width, height, title.encode('utf-8'))
        else:
            self.c_window = CGL_window_create_undecorated(width, height, title.encode('utf-8'))
        if self.c_window == NULL:
            raise RuntimeError("Failed to create window")
        self.has_been_destroyed = False
        
    def __dealloc__(self):
        self.destroy()
    
    def destroy(self):
        if self.has_been_destroyed:
            return
        CGL_window_destroy(self.c_window)
        self.has_been_destroyed = True
    
    def poll_events(self):
        CGL_window_poll_events(self.c_window)
    
    def should_close(self) -> bool:
        return CGL_window_should_close(self.c_window)
    
    def swap_buffers(self):
        CGL_window_swap_buffers(self.c_window)
    
    def set_title(self, title: str):
        CGL_window_set_title(self.c_window, title.encode('utf-8'))
    
    def set_size(self, width: int, height: int):
        CGL_window_set_size(self.c_window, width, height)
    
    def set_position(self, x: int, y: int):
        CGL_window_set_position(self.c_window, x, y)
    
    def set_hidden(self, hidden: bool):
        CGL_window_set_hidden(self.c_window, hidden)
    
    def set_user_data(self, data: object):
        CGL_window_set_user_data(self.c_window, <void*>data)
    
    def get_user_data(self) -> object:
        return <object>CGL_window_get_user_data(self.c_window)
    
    def get_size(self) -> tuple:
        cdef int width, height
        CGL_window_get_size(self.c_window, &width, &height)
        return (width, height)
    
    def get_position(self) -> tuple:
        cdef int x, y
        CGL_window_get_position(self.c_window, &x, &y)
        return (x, y)
    
    def get_framebuffer_size(self) -> tuple:
        cdef int width, height
        CGL_window_get_framebuffer_size(self.c_window, &width, &height)
        return (width, height)

    def resecure_callbaks(self):
        CGL_window_resecure_callbacks(self.c_window)
    
    def make_context_current(self):
        CGL_window_make_context_current(self.c_window)
    
    def get_glfw_handle(self) -> object:
        return <object>CGL_window_get_glfw_handle(self.c_window)
    
    def get_key(self, key: int) -> int:
        return CGL_window_get_key(self.c_window, key)
    
    def is_key_presswd(self, key: int) -> bool:
        return CGL_window_is_key_pressed(self.c_window, key)

    def get_mouse_button(self, button: int) -> int:
        return CGL_window_get_mouse_button(self.c_window, button)
    
    def get_cursor_pos(self) -> tuple:
        cdef double x, y
        CGL_window_get_mouse_position(self.c_window, &x, &y)
        return (x, y)
    
    def __repr__(self):
        return f"window({self.width}, {self.height}, {self.title})"
    
    def __str__(self):
        return f"window({self.width}, {self.height}, {self.title})"
    
    @property
    def width(self) -> int:
        return self.get_size()[0]
    
    @property
    def height(self) -> int:
        return self.get_size()[1]
    
    @property
    def x(self) -> int:
        return self.get_position()[0]
    
    @property
    def y(self) -> int:
        return self.get_position()[1]

