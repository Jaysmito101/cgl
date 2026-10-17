function(cgl_add_example target_name source_file)
    if(CGL_BUILD_EXAMPLE AND NOT "${target_name}" STREQUAL "${CGL_BUILD_EXAMPLE}")
        return()
    endif()

    set(options REQUIRES_FREETYPE REQUIRES_OPENSSL)
    set(oneValueArgs "")
    set(multiValueArgs "")
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(ARG_REQUIRES_FREETYPE AND NOT FREETYPE_FOUND)
        if("${CGL_BUILD_EXAMPLE}" STREQUAL "${target_name}")
            message(FATAL_ERROR "Example ${target_name} requires FreeType, but FreeType was not found.")
        endif()
        message(WARNING "Skipping example ${target_name} because FreeType was not found.")
        return()
    endif()

    if(ARG_REQUIRES_OPENSSL AND NOT OPENSSL_FOUND)
        if("${CGL_BUILD_EXAMPLE}" STREQUAL "${target_name}")
            message(FATAL_ERROR "Example ${target_name} requires OpenSSL, but OpenSSL was not found.")
        endif()
        message(WARNING "Skipping example ${target_name} because OpenSSL was not found.")
        return()
    endif()

    add_executable(${target_name} ${source_file})

    if("${CGL_BUILD_EXAMPLE}" STREQUAL "${target_name}")
        if(CGL_EXAMPLE_OUTPUT_NAME)
            set_target_properties(${target_name} PROPERTIES
                OUTPUT_NAME "${CGL_EXAMPLE_OUTPUT_NAME}"
            )
        endif()

        if(CGL_EXAMPLE_OUTPUT_DIRECTORY)
            set_target_properties(${target_name} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CGL_EXAMPLE_OUTPUT_DIRECTORY}"
            )

            foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
                set_target_properties(${target_name} PROPERTIES
                    RUNTIME_OUTPUT_DIRECTORY_${config} "${CGL_EXAMPLE_OUTPUT_DIRECTORY}"
                )
            endforeach()
        endif()
    endif()

    if(DEFINED EMSCRIPTEN)
        set_target_properties(${target_name} PROPERTIES 
            SUFFIX ".js"
            COMPILE_FLAGS "-O2"
            LINK_FLAGS "-O2 -s USE_GLFW=3 -sFULL_ES3 -lopenal -sALLOW_MEMORY_GROWTH=1 -sEXPORTED_RUNTIME_METHODS=ccall,cwrap -sEXPORTED_FUNCTIONS=_main"
        )
    endif()

    target_link_libraries(${target_name}
        glfw
        ${GLFW_LIBRARIES}
        glad
    )

    
    if(NOT DEFINED EMSCRIPTEN)
        target_link_libraries(${target_name} OpenAL)
    endif()

    if(ARG_REQUIRES_FREETYPE AND FREETYPE_FOUND)
        target_include_directories(${target_name} PRIVATE ${FREETYPE_INCLUDE_DIRS})
        target_link_libraries(${target_name} ${FREETYPE_LIBRARIES})
    endif()

    if(ARG_REQUIRES_OPENSSL AND OPENSSL_FOUND)
        target_include_directories(${target_name} PRIVATE ${OPENSSL_INCLUDE_DIR})
        target_link_libraries(${target_name} OpenSSL::SSL OpenSSL::Crypto)
    endif()
endfunction()
