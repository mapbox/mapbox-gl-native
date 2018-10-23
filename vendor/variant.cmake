add_library(variant INTERFACE)

target_include_directories(variant SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/variant/include
)
