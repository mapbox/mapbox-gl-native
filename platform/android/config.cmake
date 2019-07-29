set(USE_GLES2 ON)

include(cmake/sqlite.cmake)

# Build thin archives.
set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> cruT <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> cruT <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_APPEND "<CMAKE_AR> ruT <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_APPEND "<CMAKE_AR> ruT <TARGET> <LINK_FLAGS> <OBJECTS>")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")

if ((ANDROID_ABI STREQUAL "armeabi-v7a") OR (ANDROID_ABI STREQUAL "arm64-v8a") OR
    (ANDROID_ABI STREQUAL "x86") OR (ANDROID_ABI STREQUAL "x86_64"))
    # Use Identical Code Folding on platforms that support the gold linker.
    set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=gold -Wl,--icf=safe ${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=gold -Wl,--icf=safe ${CMAKE_SHARED_LINKER_FLAGS}")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -Wl,--version-script=${CMAKE_SOURCE_DIR}/platform/android/version-script")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl,--version-script=${CMAKE_SOURCE_DIR}/platform/android/version-script")

# Use LTO in Release builds. Due to a toolchain issue, -O2 is also required for the link step (https://github.com/android-ndk/ndk/issues/721)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -flto")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -flto")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -flto")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto -O2")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -flto -O2")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -flto -O2")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} -flto -O2")

## mbgl core ##

macro(mbgl_platform_core)
    # Modify platform/android/core-files.json to change the source files for this target.
    target_sources_from_file(mbgl-core PRIVATE platform/android/core-files.json)

    target_include_directories(mbgl-core
        PUBLIC platform/default/include
        PRIVATE platform/android/include
    )

    target_link_libraries(mbgl-core
        PUBLIC jni.hpp
        PUBLIC -llog
        PUBLIC -landroid
        PUBLIC -ljnigraphics
        PUBLIC -lEGL
        PUBLIC -lGLESv2
        PUBLIC -latomic
        PUBLIC -lz
    )
endmacro()


macro(mbgl_filesource)
    # Modify platform/android/filesource-files.json to change the source files for this target.
    target_sources_from_file(mbgl-filesource PRIVATE platform/android/filesource-files.json)

    target_link_libraries(mbgl-filesource
        PUBLIC sqlite
        PUBLIC jni.hpp
        PUBLIC -llog
        PUBLIC -landroid
        PUBLIC -latomic
    )
endmacro()

## Shared library

add_library(mapbox-gl SHARED
    platform/android/src/main.cpp
)

target_include_directories(mapbox-gl
    PRIVATE src
)

target_link_libraries(mapbox-gl
    PRIVATE mbgl-core
    PRIVATE mbgl-filesource
)

## Test executable ##
macro(mbgl_platform_test)
    target_sources(mbgl-test
        PRIVATE platform/android/src/test/test_runner.cpp
        PRIVATE platform/android/src/test/runtime.cpp
    )

    target_include_directories(mbgl-test
        PRIVATE platform/android/include
    )

    set_target_properties(mbgl-test
        PROPERTIES
        LINK_FLAGS
        "-fPIE -pie \
        -Wl,--export-dynamic \
        -Wl,--version-script=${CMAKE_SOURCE_DIR}/platform/android/src/test/version-script")

    target_link_libraries(mbgl-test
        PRIVATE mbgl-filesource
    )
endmacro()

## Benchmark ##
macro(mbgl_platform_benchmark)
    target_sources(mbgl-benchmark
        PRIVATE platform/android/src/test/benchmark_runner.cpp
        PRIVATE platform/android/src/test/runtime.cpp
    )

    set_target_properties(mbgl-benchmark
        PROPERTIES
        LINK_FLAGS
        "-fPIE -pie \
        -Wl,--export-dynamic \
        -Wl,--version-script=${CMAKE_SOURCE_DIR}/platform/android/src/test/version-script")

    target_link_libraries(mbgl-benchmark
        PRIVATE mbgl-filesource
    )
endmacro()


## Custom layer example ##

add_library(example-custom-layer SHARED
    platform/android/src/example_custom_layer.cpp
)

target_include_directories(example-custom-layer
    PRIVATE include
)

target_link_libraries(example-custom-layer PRIVATE
    mapbox-base
    -llog
    -lGLESv2
)
