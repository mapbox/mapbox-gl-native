add_library(shelf-pack-cpp INTERFACE)

target_include_directories(shelf-pack-cpp SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/shelf-pack-cpp/include
)
