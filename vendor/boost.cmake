add_library(boost INTERFACE)

target_include_directories(boost SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/boost/include
)
