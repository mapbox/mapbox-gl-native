add_library(geometry.hpp INTERFACE)

target_include_directories(geometry.hpp SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/geometry.hpp/include
)
