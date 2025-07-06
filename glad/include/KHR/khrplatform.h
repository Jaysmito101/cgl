#ifndef __khrplatform_h_
#define __khrplatform_h_

/* Stub file for GLAD - actual implementation assumed to be present */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Basic platform definitions */
#define KHRONOS_SUPPORT_INT64   1
#define KHRONOS_SUPPORT_FLOAT   1

#if defined(__cplusplus)
#define KHRONOS_APICALL extern "C"
#else
#define KHRONOS_APICALL extern
#endif

#define KHRONOS_APIENTRY
#define KHRONOS_APIATTRIBUTES

#ifdef __cplusplus
}
#endif

#endif /* __khrplatform_h_ */