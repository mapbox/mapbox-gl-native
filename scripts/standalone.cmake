include(cmake/mason.cmake)

# include once for every platform to get all dependency defines
set(MBGL_PLATFORM "android")
include(cmake/mason-dependencies.cmake)
set(MBGL_PLATFORM "ios")
include(cmake/mason-dependencies.cmake)

get_cmake_property(_VARS VARIABLES)
foreach (_VAR ${_VARS})
    if (_VAR MATCHES "^MASON_PACKAGE_.+_NAME$")
        set(_PACKAGE "${${_VAR}}")
        if (MASON_PACKAGE_${_PACKAGE}_HEADER_ONLY)
            file(RELATIVE_PATH _PREFIX "${CMAKE_CURRENT_LIST_DIR}/.." "${MASON_PACKAGE_${_PACKAGE}_PREFIX}")
            message("${_PACKAGE} ${_PREFIX} ${MASON_PACKAGE_${_PACKAGE}_VERSION}")
        endif()
    endif()
endforeach()
