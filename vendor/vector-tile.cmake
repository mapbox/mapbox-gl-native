add_library(vector-tile INTERFACE)

target_include_directories(vector-tile SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/vector-tile/include
)
