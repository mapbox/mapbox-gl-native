add_library(wagyu INTERFACE)

target_include_directories(wagyu SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/wagyu/include
)
