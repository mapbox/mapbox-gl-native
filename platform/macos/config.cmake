set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)

mason_use(glfw VERSION 2017-02-09-77a8f10)
mason_use(boost_libprogram_options VERSION 1.62.0)
mason_use(gtest VERSION 1.8.0)
mason_use(benchmark VERSION 1.0.0-1)
mason_use(icu VERSION 58.1-min-size)

include(cmake/loop-darwin.cmake)

macro(mbgl_platform_core)
    target_sources(mbgl-core
        # File source
        PRIVATE platform/darwin/src/http_file_source.mm
        PRIVATE platform/default/asset_file_source.cpp
        PRIVATE platform/default/default_file_source.cpp
        PRIVATE platform/default/local_file_source.cpp
        PRIVATE platform/default/online_file_source.cpp

        # Default styles
        PRIVATE platform/default/mbgl/util/default_styles.hpp
        PRIVATE platform/default/mbgl/util/default_styles.cpp

        # Offline
        PRIVATE platform/default/mbgl/storage/offline.cpp
        PRIVATE platform/default/mbgl/storage/offline_database.cpp
        PRIVATE platform/default/mbgl/storage/offline_database.hpp
        PRIVATE platform/default/mbgl/storage/offline_download.cpp
        PRIVATE platform/default/mbgl/storage/offline_download.hpp
        PRIVATE platform/default/sqlite3.cpp
        PRIVATE platform/default/sqlite3.hpp

        # Misc
        PRIVATE platform/darwin/mbgl/storage/reachability.h
        PRIVATE platform/darwin/mbgl/storage/reachability.m
        PRIVATE platform/darwin/src/logging_nslog.mm
        PRIVATE platform/darwin/src/nsthread.mm
        PRIVATE platform/darwin/src/string_nsstring.mm
        PRIVATE platform/default/bidi.cpp
        PRIVATE platform/default/utf.cpp

        # Image handling
        PRIVATE platform/darwin/mbgl/util/image+MGLAdditions.hpp
        PRIVATE platform/darwin/src/image.mm
        PRIVATE platform/default/png_writer.cpp

        # Headless view
        PRIVATE platform/default/mbgl/gl/headless_backend.cpp
        PRIVATE platform/default/mbgl/gl/headless_backend.hpp
        PRIVATE platform/darwin/src/headless_backend_cgl.cpp
        PRIVATE platform/default/mbgl/gl/headless_display.hpp
        PRIVATE platform/darwin/src/headless_display_cgl.cpp
        PRIVATE platform/default/mbgl/gl/offscreen_view.cpp
        PRIVATE platform/default/mbgl/gl/offscreen_view.hpp

        # Thread pool
        PRIVATE platform/default/mbgl/util/shared_thread_pool.cpp
        PRIVATE platform/default/mbgl/util/shared_thread_pool.hpp
        PRIVATE platform/default/mbgl/util/default_thread_pool.cpp
        PRIVATE platform/default/mbgl/util/default_thread_pool.cpp
    )

    target_add_mason_package(mbgl-core PUBLIC geojson)
    target_add_mason_package(mbgl-core PUBLIC icu)

    target_compile_options(mbgl-core
        PRIVATE -fobjc-arc
        PRIVATE -fvisibility=hidden
    )

    target_include_directories(mbgl-core
        PUBLIC platform/darwin
        PUBLIC platform/default
    )

    target_link_libraries(mbgl-core
        PUBLIC -lz
    )
endmacro()


macro(mbgl_platform_glfw)
    target_link_libraries(mbgl-glfw
        PRIVATE mbgl-loop-darwin
        PRIVATE "-framework OpenGL"
        PRIVATE "-lsqlite3"
    )
endmacro()


macro(mbgl_platform_render)
    target_link_libraries(mbgl-render
        PRIVATE mbgl-loop-darwin
        PRIVATE "-framework Foundation"
        PRIVATE "-framework CoreGraphics"
        PRIVATE "-framework OpenGL"
        PRIVATE "-framework ImageIO"
        PRIVATE "-framework CoreServices"
        PRIVATE "-lsqlite3"
    )
endmacro()


macro(mbgl_platform_offline)
    target_link_libraries(mbgl-offline
        PRIVATE mbgl-loop-darwin
        PRIVATE "-framework Foundation"
        PRIVATE "-framework CoreGraphics"
        PRIVATE "-framework OpenGL"
        PRIVATE "-framework ImageIO"
        PRIVATE "-framework CoreServices"
        PRIVATE "-lsqlite3"
    )
endmacro()


macro(mbgl_platform_test)
    target_sources(mbgl-test
        PRIVATE platform/default/mbgl/test/main.cpp
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
        PRIVATE mbgl-loop-darwin
        PRIVATE "-framework Foundation"
        PRIVATE "-framework CoreGraphics"
        PRIVATE "-framework OpenGL"
        PRIVATE "-framework ImageIO"
        PRIVATE "-framework CoreServices"
        PRIVATE "-lsqlite3"
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
        PRIVATE mbgl-loop-darwin
        PRIVATE "-framework Foundation"
        PRIVATE "-framework CoreGraphics"
        PRIVATE "-framework OpenGL"
        PRIVATE "-framework ImageIO"
        PRIVATE "-framework CoreServices"
        PRIVATE "-lsqlite3"
    )
endmacro()

macro(mbgl_platform_node)
    target_compile_options(mbgl-node
        PRIVATE -fvisibility=hidden
    )

    target_link_libraries(mbgl-node
        PRIVATE "-framework Foundation"
        PRIVATE "-framework OpenGL"
        PRIVATE "-Wl,-bind_at_load"
    )
endmacro()
