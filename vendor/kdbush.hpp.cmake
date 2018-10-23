add_library(kdbush.hpp INTERFACE)

target_include_directories(kdbush.hpp SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/kdbush.hpp/include
)
