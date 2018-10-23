add_library(unique_resource INTERFACE)

target_include_directories(unique_resource SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/unique_resource
)
