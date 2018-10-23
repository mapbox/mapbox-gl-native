add_library(expected INTERFACE)

target_include_directories(expected SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/expected/include
)
