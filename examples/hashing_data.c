#define CGL_LOGGING_ENABLED
#define CGL_IMPLEMENTATION
#include "cgl.h"

int main()
{
    char buffer[1024];
    const char* data = "Hello World!";
    size_t size = strlen(data);
    printf("CRC32: %d\n", CGL_utils_crc32(data, size));
    printf("CRC64: %llu\n", CGL_utils_crc64(data, size));
    printf("SFH  : %d\n", CGL_utils_super_fast_hash(data, size));
    CGL_utils_rot13(data, buffer);
    printf("ROT13: %s\n", buffer);
    return 0;
}
