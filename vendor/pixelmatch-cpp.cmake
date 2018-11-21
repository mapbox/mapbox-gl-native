add_library(pixelmatch-cpp INTERFACE)

target_include_directories(pixelmatch-cpp SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/pixelmatch-cpp/include
)
