add_library(polylabel INTERFACE)

target_include_directories(polylabel SYSTEM INTERFACE
    ${GIT_ROOT}/vendor/polylabel/include
)
