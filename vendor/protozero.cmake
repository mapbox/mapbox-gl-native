add_library(protozero INTERFACE)

target_include_directories(protozero SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/protozero/include
)
