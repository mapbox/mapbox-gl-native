add_library(kdbush.hpp INTERFACE)

target_include_directories(kdbush.hpp SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/kdbush.hpp/include
)
