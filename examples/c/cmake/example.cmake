function(cgl_add_example target_name source_file)
    add_executable(${target_name} ${source_file})

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
endfunction()