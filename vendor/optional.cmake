add_library(optional INTERFACE)

# This polyfill is really only needed for
# Windows. We need to be careful to not leak
# it into any of our public interfaces.
if(WIN32)
    target_include_directories(optional SYSTEM INTERFACE
        ${CMAKE_SOURCE_DIR}/vendor/optional/include
    )
endif()
