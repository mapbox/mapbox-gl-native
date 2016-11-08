set(CMAKE_OSX_DEPLOYMENT_TARGET 10.10)

mason_use(glfw VERSION 3.2.1)
mason_use(boost_libprogram_options VERSION 1.60.0)
mason_use(gtest VERSION 1.7.0${MASON_CXXABI_SUFFIX})
mason_use(benchmark VERSION 1.0.0)

include(cmake/loop-darwin.cmake)

macro(mbgl_platform_core)
    target_sources(mbgl-core
        # File source
        PRIVATE platform/darwin/src/http_file_source.mm
        PRIVATE platform/default/asset_file_source.cpp
        PRIVATE platform/default/default_file_source.cpp
        PRIVATE platform/default/local_file_source.cpp
        PRIVATE platform/default/online_file_source.cpp

        # Offline
        PRIVATE platform/default/mbgl/storage/offline.cpp
        PRIVATE platform/default/mbgl/storage/offline_database.cpp
        PRIVATE platform/default/mbgl/storage/offline_database.hpp
        PRIVATE platform/default/mbgl/storage/offline_download.cpp
        PRIVATE platform/default/mbgl/storage/offline_download.hpp
        PRIVATE platform/default/sqlite3.cpp
        PRIVATE platform/default/sqlite3.hpp

        # Misc
        PRIVATE platform/darwin/src/log_nslog.mm
        PRIVATE platform/darwin/src/nsthread.mm
        PRIVATE platform/darwin/src/reachability.m
        PRIVATE platform/darwin/src/string_nsstring.mm

        # Image handling
        PRIVATE platform/darwin/src/image.mm

        # Headless view
        PRIVATE platform/darwin/src/headless_backend_cgl.cpp
        PRIVATE platform/darwin/src/headless_display_cgl.cpp
        PRIVATE platform/default/headless_backend.cpp
        PRIVATE platform/default/offscreen_view.cpp

        # Thread pool
        PRIVATE platform/default/thread_pool.cpp
    )

    target_add_mason_package(mbgl-core PUBLIC geojson)

    target_compile_options(mbgl-core
        PRIVATE -fobjc-arc
    )

    target_include_directories(mbgl-core
        PRIVATE platform/default
    )

    target_link_libraries(mbgl-core
        PUBLIC -lz
    )
endmacro()


macro(mbgl_platform_glfw)
    target_link_libraries(mbgl-glfw
        PRIVATE mbgl-loop
        PRIVATE "-framework OpenGL"
        PRIVATE "-lsqlite3"
    )
endmacro()


macro(mbgl_platform_render)
    target_link_libraries(mbgl-render
        PRIVATE mbgl-loop
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
        PRIVATE mbgl-loop
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
        PRIVATE test/src/main.cpp
    )

    set_source_files_properties(
        test/src/main.cpp
            PROPERTIES
        COMPILE_FLAGS -DWORK_DIRECTORY="${CMAKE_SOURCE_DIR}"
    )

    target_link_libraries(mbgl-test
        PRIVATE mbgl-loop
        PRIVATE "-framework Foundation"
        PRIVATE "-framework CoreGraphics"
        PRIVATE "-framework OpenGL"
        PRIVATE "-framework ImageIO"
        PRIVATE "-framework CoreServices"
        PRIVATE "-lsqlite3"
    )
endmacro()

macro(mbgl_platform_benchmark)
    target_sources(mbgl-benchmark
        PRIVATE benchmark/src/main.cpp
    )

    set_source_files_properties(
        benchmark/src/main.cpp
            PROPERTIES
        COMPILE_FLAGS -DWORK_DIRECTORY="${CMAKE_SOURCE_DIR}"
    )

    target_link_libraries(mbgl-benchmark
        PRIVATE mbgl-loop
        PRIVATE "-framework Foundation"
        PRIVATE "-framework CoreGraphics"
        PRIVATE "-framework OpenGL"
        PRIVATE "-framework ImageIO"
        PRIVATE "-framework CoreServices"
        PRIVATE "-lsqlite3"
    )
endmacro()

macro(mbgl_platform_node)
    target_link_libraries(mbgl-node
        PRIVATE "-framework Foundation"
        PRIVATE "-framework OpenGL"
    )
endmacro()
