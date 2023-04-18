cdef extern from "cgl.h":
    unsigned int CGL_utils_super_fast_hash(const void* data, unsigned long long size)

def hash(data) -> int:
    return CGL_utils_super_fast_hash(<void *>data, len(data))
