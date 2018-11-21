add_library(polylabel INTERFACE)

target_include_directories(polylabel SYSTEM INTERFACE
    ${CMAKE_SOURCE_DIR}/vendor/polylabel/include
)
