add_library(earcut.hpp INTERFACE)

target_include_directories(earcut.hpp SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/earcut.hpp/include
)
