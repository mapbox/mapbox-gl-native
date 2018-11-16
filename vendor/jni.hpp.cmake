add_library(jni.hpp INTERFACE)

target_include_directories(jni.hpp SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/jni.hpp/include
)
