cmake_minimum_required(VERSION 3.10)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) # Use folders for IDE

project(cgl)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REUIRED True)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Freetype QUIET)
if (NOT FREETYPE_FOUND)
    message(WARNING "FreeType not found. Some examples will be skipped.")
endif()

find_package(OpenSSL QUIET)
if (NOT OPENSSL_FOUND)
    message(WARNING "OpenSSL not found. Some examples will be skipped.")
endif()

include_directories(
    ./
    ./include
    ./lib/stb
    ./lib/nuklear
    ./lib/glad/include
    ./lib/glfw/include
    ./lib/openal/include
)

set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory(lib/glad/cmake)
glad_add_library(glad STATIC API gl:core=4.3)

if (NOT DEFINED EMSCRIPTEN)
    add_subdirectory(lib/glfw)
    add_subdirectory(lib/openal)
endif()

if (MSVC)
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(examples/c)


