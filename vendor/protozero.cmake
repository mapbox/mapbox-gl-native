add_library(protozero INTERFACE)

target_include_directories(protozero SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/protozero/include
)
