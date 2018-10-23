add_library(wagyu INTERFACE)

target_include_directories(wagyu SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/wagyu/include
)
