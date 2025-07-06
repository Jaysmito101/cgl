#ifndef _glfw3_h_
#define _glfw3_h_

/* Stub file for GLFW - actual implementation assumed to be present */

#ifdef __cplusplus
extern "C" {
#endif

#define GLFW_VERSION_MAJOR          3
#define GLFW_VERSION_MINOR          3
#define GLFW_VERSION_REVISION       0

#define GLFW_TRUE                   1
#define GLFW_FALSE                  0

#define GLFW_RELEASE                0
#define GLFW_PRESS                  1
#define GLFW_REPEAT                 2

/* Key codes */
#define GLFW_KEY_SPACE              32
#define GLFW_KEY_APOSTROPHE         39
#define GLFW_KEY_COMMA              44
#define GLFW_KEY_MINUS              45
#define GLFW_KEY_PERIOD             46
#define GLFW_KEY_SLASH              47
#define GLFW_KEY_0                  48
#define GLFW_KEY_1                  49
#define GLFW_KEY_2                  50
#define GLFW_KEY_3                  51
#define GLFW_KEY_4                  52
#define GLFW_KEY_5                  53
#define GLFW_KEY_6                  54
#define GLFW_KEY_7                  55
#define GLFW_KEY_8                  56
#define GLFW_KEY_9                  57
#define GLFW_KEY_A                  65
#define GLFW_KEY_R                  82
#define GLFW_KEY_UP                 265
#define GLFW_KEY_DOWN               264
#define GLFW_KEY_LEFT               263
#define GLFW_KEY_RIGHT              262
#define GLFW_KEY_ESCAPE             256

#define GLFW_MOUSE_BUTTON_1         0
#define GLFW_MOUSE_BUTTON_2         1
#define GLFW_MOUSE_BUTTON_3         2
#define GLFW_MOUSE_BUTTON_LEFT      GLFW_MOUSE_BUTTON_1
#define GLFW_MOUSE_BUTTON_RIGHT     GLFW_MOUSE_BUTTON_2
#define GLFW_MOUSE_BUTTON_MIDDLE    GLFW_MOUSE_BUTTON_3

typedef struct GLFWwindow GLFWwindow;
typedef void* GLFWmonitor;

typedef void (*GLFWkeyfun)(GLFWwindow*, int, int, int, int);
typedef void (*GLFWmousebuttonfun)(GLFWwindow*, int, int, int);
typedef void (*GLFWcursorposfun)(GLFWwindow*, double, double);
typedef void (*GLFWscrollfun)(GLFWwindow*, double, double);
typedef void (*GLFWframebuffersizefun)(GLFWwindow*, int, int);
typedef void (*GLFWwindowclosefun)(GLFWwindow*);
typedef void (*GLFWdropfun)(GLFWwindow*, int, const char**);
typedef void (*GLFWwindowsizefun)(GLFWwindow*, int, int);
typedef void (*GLFWwindowposfun)(GLFWwindow*, int, int);
typedef void (*GLFWwindowiconifyfun)(GLFWwindow*, int);
typedef void (*GLFWwindowfocusfun)(GLFWwindow*, int);
typedef void (*GLFWwindowrefreshfun)(GLFWwindow*);
typedef void (*GLFWcharfun)(GLFWwindow*, unsigned int);

extern int glfwInit(void);
extern void glfwTerminate(void);
extern GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share);
extern void glfwDestroyWindow(GLFWwindow* window);
extern int glfwWindowShouldClose(GLFWwindow* window);
extern void glfwSetWindowShouldClose(GLFWwindow* window, int value);
extern void glfwPollEvents(void);
extern void glfwSwapBuffers(GLFWwindow* window);
extern void glfwMakeContextCurrent(GLFWwindow* window);
extern void glfwGetFramebufferSize(GLFWwindow* window, int* width, int* height);
extern void glfwGetWindowSize(GLFWwindow* window, int* width, int* height);
extern int glfwGetKey(GLFWwindow* window, int key);
extern int glfwGetMouseButton(GLFWwindow* window, int button);
extern void glfwGetCursorPos(GLFWwindow* window, double* xpos, double* ypos);
extern GLFWkeyfun glfwSetKeyCallback(GLFWwindow* window, GLFWkeyfun callback);
extern GLFWmousebuttonfun glfwSetMouseButtonCallback(GLFWwindow* window, GLFWmousebuttonfun callback);
extern GLFWcursorposfun glfwSetCursorPosCallback(GLFWwindow* window, GLFWcursorposfun callback);
extern GLFWscrollfun glfwSetScrollCallback(GLFWwindow* window, GLFWscrollfun callback);
extern GLFWframebuffersizefun glfwSetFramebufferSizeCallback(GLFWwindow* window, GLFWframebuffersizefun callback);
extern GLFWwindowclosefun glfwSetWindowCloseCallback(GLFWwindow* window, GLFWwindowclosefun callback);
extern GLFWdropfun glfwSetDropCallback(GLFWwindow* window, GLFWdropfun callback);
extern GLFWwindowsizefun glfwSetWindowSizeCallback(GLFWwindow* window, GLFWwindowsizefun callback);
extern GLFWwindowposfun glfwSetWindowPosCallback(GLFWwindow* window, GLFWwindowposfun callback);
extern GLFWwindowiconifyfun glfwSetWindowIconifyCallback(GLFWwindow* window, GLFWwindowiconifyfun callback);
extern GLFWwindowfocusfun glfwSetWindowFocusCallback(GLFWwindow* window, GLFWwindowfocusfun callback);
extern GLFWwindowrefreshfun glfwSetWindowRefreshCallback(GLFWwindow* window, GLFWwindowrefreshfun callback);
extern GLFWcharfun glfwSetCharCallback(GLFWwindow* window, GLFWcharfun callback);
extern void* glfwGetProcAddress(const char* procname);

#ifdef __cplusplus
}
#endif

#endif /* _glfw3_h_ */