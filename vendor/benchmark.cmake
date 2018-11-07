add_library(benchmark STATIC
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/commandlineflags.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/console_reporter.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/complexity.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/csv_reporter.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/colorprint.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/sleep.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/benchmark.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/counter.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/benchmark_register.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/statistics.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/json_reporter.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/reporter.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/string_util.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/sysinfo.cc
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/src/timers.cc
)

target_compile_definitions(benchmark PRIVATE
    HAVE_STEADY_CLOCK
)

target_include_directories(benchmark SYSTEM PUBLIC
    ${CMAKE_SOURCE_DIR}/vendor/benchmark/include
)
