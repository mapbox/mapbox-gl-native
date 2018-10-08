add_library(optional INTERFACE)

# This polyfill is needed for Windows and Android since these standard libraries don't ship with
# usable versions of <experimental/optional>
if(WIN32 OR MBGL_PLATFORM STREQUAL "android")
    target_include_directories(optional SYSTEM INTERFACE
        ${CMAKE_SOURCE_DIR}/vendor/optional/include
    )
endif()
