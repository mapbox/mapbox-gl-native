add_library(supercluster.hpp INTERFACE)

target_include_directories(supercluster.hpp SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/supercluster.hpp/include
)
