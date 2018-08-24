set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11)

include(cmake/loop-darwin.cmake)

macro(mbgl_platform_core)
    target_sources(mbgl-core
        # Misc
        PRIVATE platform/darwin/mbgl/storage/reachability.h
        PRIVATE platform/darwin/mbgl/storage/reachability.m
        PRIVATE platform/darwin/src/CFHandle.hpp
        PRIVATE platform/darwin/src/collator.mm
        PRIVATE platform/darwin/src/local_glyph_rasterizer.mm
        PRIVATE platform/darwin/src/logging_nslog.mm
        PRIVATE platform/darwin/src/nsthread.mm
        PRIVATE platform/darwin/src/string_nsstring.mm
        PRIVATE platform/default/bidi.cpp
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
    )

    target_include_directories(mbgl-core
        PUBLIC platform/darwin
        PUBLIC platform/default
    )

    target_link_libraries(mbgl-core
        PUBLIC "-lz"
        PUBLIC "-framework Foundation"
        PUBLIC "-framework CoreText"
        PUBLIC "-framework CoreGraphics"
        PUBLIC "-framework OpenGL"
        PUBLIC "-framework ImageIO"
        PUBLIC "-framework CoreServices"
        PUBLIC "-framework SystemConfiguration"
    )
endmacro()


macro(mbgl_filesource)
    # Modify platform/darwin/filesource-files.txt to change the source files for this target.
    target_sources_from_file(mbgl-filesource PRIVATE platform/darwin/filesource-files.txt)

    target_compile_options(mbgl-filesource
        PRIVATE -fobjc-arc
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
endmacro()


macro(mbgl_platform_render)
    target_link_libraries(mbgl-render
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-darwin
    )
endmacro()


macro(mbgl_platform_offline)
    target_link_libraries(mbgl-offline
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-darwin
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

    target_link_libraries(mbgl-test
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-darwin
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
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-darwin
    )
endmacro()

macro(mbgl_platform_node)
    target_link_libraries(mbgl-node INTERFACE
        -exported_symbols_list ${CMAKE_SOURCE_DIR}/platform/node/symbol-list
        -dead_strip
    )
endmacro()
