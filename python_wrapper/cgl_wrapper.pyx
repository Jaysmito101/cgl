from libcpp cimport bool
from libc.stdint cimport uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, uintptr_t, intptr_t
cimport libc.stdlib as stdlib
import copy

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
    ctypedef CGL_logger_context* CGL_logger_context_ptr

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

def CGL_printf_blue(format: str):
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

