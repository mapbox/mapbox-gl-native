function(cache_executable_path name variable)
    # Obtain paths to node and npm and cache it, so when e.g. Xcode reruns CMake, we still have the
    # variables from the original invocation
    if(NOT ${variable})
        set(_which_output)
        execute_process(
            COMMAND which ${name}
            OUTPUT_VARIABLE _which_output
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        if(_which_output STREQUAL "")
            message(STATUS "Using cached ${name}: ${${variable}}")
        else()
            set(${variable} "${_which_output}" CACHE PATH "Path to ${name} executable")
            message(STATUS "Using ${name}: ${${variable}}")
        endif()
    endif()
endfunction()

cache_executable_path(node _node)
cache_executable_path(npm _npm)
