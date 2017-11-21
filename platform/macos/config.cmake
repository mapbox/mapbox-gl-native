set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)

mason_use(glfw VERSION 2017-07-13-67c9155)
mason_use(boost_libprogram_options VERSION 1.62.0)
mason_use(gtest VERSION 1.8.0)
mason_use(benchmark VERSION 1.2.0)
mason_use(icu VERSION 58.1-min-size)

include(cmake/loop-uv.cmake)
include(cmake/loop-darwin.cmake)

macro(mbgl_platform_core)
    target_sources(mbgl-core
        # Misc
        PRIVATE platform/darwin/mbgl/storage/reachability.h
        PRIVATE platform/darwin/mbgl/storage/reachability.m
        PRIVATE platform/darwin/src/logging_nslog.mm
        PRIVATE platform/darwin/src/nsthread.mm
        PRIVATE platform/darwin/src/string_nsstring.mm
        PRIVATE platform/default/bidi.cpp
        PRIVATE platform/default/local_glyph_rasterizer.cpp
        PRIVATE platform/default/thread_local.cpp
        PRIVATE platform/default/utf.cpp

        # Image handling
        PRIVATE platform/darwin/mbgl/util/image+MGLAdditions.hpp
        PRIVATE platform/darwin/src/image.mm
        PRIVATE platform/default/png_writer.cpp

        # Headless view
        PRIVATE platform/default/mbgl/gl/headless_frontend.cpp
        PRIVATE platform/default/mbgl/gl/headless_frontend.hpp
        PRIVATE platform/default/mbgl/gl/headless_backend.cpp
        PRIVATE platform/default/mbgl/gl/headless_backend.hpp
        PRIVATE platform/darwin/src/headless_backend_cgl.cpp
        PRIVATE platform/default/mbgl/gl/headless_display.hpp
        PRIVATE platform/darwin/src/headless_display_cgl.cpp

        # Snapshotting
        PRIVATE platform/default/mbgl/map/map_snapshotter.cpp
        PRIVATE platform/default/mbgl/map/map_snapshotter.hpp

        # Thread pool
        PRIVATE platform/default/mbgl/util/shared_thread_pool.cpp
        PRIVATE platform/default/mbgl/util/shared_thread_pool.hpp
        PRIVATE platform/default/mbgl/util/default_thread_pool.cpp
        PRIVATE platform/default/mbgl/util/default_thread_pool.cpp
    )

    target_add_mason_package(mbgl-core PUBLIC geojson)
    target_add_mason_package(mbgl-core PUBLIC polylabel)
    target_add_mason_package(mbgl-core PRIVATE icu)

    target_compile_options(mbgl-core
        PRIVATE -fobjc-arc
        PRIVATE -fvisibility=hidden
    )

    target_include_directories(mbgl-core
        PUBLIC platform/darwin
        PUBLIC platform/default
    )

    target_link_libraries(mbgl-core
        PUBLIC "-lz"
        PUBLIC "-framework Foundation"
        PUBLIC "-framework CoreGraphics"
        PUBLIC "-framework OpenGL"
        PUBLIC "-framework ImageIO"
        PUBLIC "-framework CoreServices"
        PUBLIC "-framework SystemConfiguration"
    )
endmacro()


macro(mbgl_filesource)
    target_sources(mbgl-filesource
        # File source
        PRIVATE platform/darwin/src/http_file_source.mm

        # Database
        PRIVATE platform/default/sqlite3.cpp
    )

    target_compile_options(mbgl-filesource
        PRIVATE -fobjc-arc
        PRIVATE -fvisibility=hidden
    )

    target_link_libraries(mbgl-filesource
        PUBLIC "-lsqlite3"
        PUBLIC "-framework Foundation"
    )
endmacro()


macro(mbgl_platform_glfw)
    target_link_libraries(mbgl-glfw
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-darwin
    )

    target_compile_options(mbgl-glfw
        PRIVATE -fvisibility=hidden
    )

endmacro()


macro(mbgl_platform_render)
    target_link_libraries(mbgl-render
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-darwin
    )
    target_compile_options(mbgl-render
        PRIVATE -fvisibility=hidden
    )
endmacro()


macro(mbgl_platform_offline)
    target_link_libraries(mbgl-offline
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-darwin
    )
    target_compile_options(mbgl-offline
        PRIVATE -fvisibility=hidden
    )
endmacro()


macro(mbgl_platform_test)
    target_sources(mbgl-test
        PRIVATE platform/default/mbgl/test/main.cpp
    )

    target_include_directories(mbgl-test
        PRIVATE platform/macos
    )

    set_source_files_properties(
        platform/default/mbgl/test/main.cpp
            PROPERTIES
        COMPILE_FLAGS -DWORK_DIRECTORY="${CMAKE_SOURCE_DIR}"
    )

    target_compile_options(mbgl-test
        PRIVATE -fvisibility=hidden
    )

    target_link_libraries(mbgl-test
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-darwin
    )
endmacro()

macro(mbgl_platform_benchmark)
    target_compile_options(mbgl-benchmark
        PRIVATE -fvisibility=hidden
    )

    target_sources(mbgl-benchmark
        PRIVATE benchmark/src/main.cpp
    )

    set_source_files_properties(
        benchmark/src/main.cpp
            PROPERTIES
        COMPILE_FLAGS -DWORK_DIRECTORY="${CMAKE_SOURCE_DIR}"
    )

    target_link_libraries(mbgl-benchmark
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-darwin
    )
endmacro()

macro(mbgl_platform_node)
    target_compile_options(mbgl-node
        PRIVATE -fvisibility=hidden
    )

    target_link_libraries(mbgl-node
        PRIVATE "-Wl,-bind_at_load"
    )
endmacro()
