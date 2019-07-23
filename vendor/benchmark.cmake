if(TARGET mbgl-vendor-benchmark)
    return()
endif()

add_library(mbgl-vendor-benchmark STATIC EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/commandlineflags.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/console_reporter.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/complexity.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/csv_reporter.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/colorprint.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/sleep.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/benchmark.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/counter.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/benchmark_register.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/statistics.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/json_reporter.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/reporter.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/string_util.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/sysinfo.cc
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/src/timers.cc
)

target_compile_definitions(mbgl-vendor-benchmark PRIVATE
    HAVE_STEADY_CLOCK
)

target_include_directories(mbgl-vendor-benchmark SYSTEM PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/benchmark/include
)

set_property(TARGET mbgl-vendor-benchmark PROPERTY FOLDER Core)
