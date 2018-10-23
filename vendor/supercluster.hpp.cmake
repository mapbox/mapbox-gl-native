add_library(supercluster.hpp INTERFACE)

target_include_directories(supercluster.hpp SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/supercluster.hpp/include
)
