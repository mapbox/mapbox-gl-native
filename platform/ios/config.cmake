add_definitions(-DMBGL_USE_GLES2=1)

mason_use(icu VERSION 58.1-min-size)

macro(initialize_ios_target target)
    set_xcode_property(${target} IPHONEOS_DEPLOYMENT_TARGET "8.0")
    set_xcode_property(${target} ENABLE_BITCODE "YES")
    set_xcode_property(${target} BITCODE_GENERATION_MODE bitcode)
    set_xcode_property(${target} ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)

    target_compile_options(${target}
        PRIVATE -fobjc-arc
    )
endmacro()


include(cmake/loop-darwin.cmake)
initialize_ios_target(mbgl-loop-darwin)


macro(mbgl_platform_core)
    initialize_ios_target(mbgl-core)

    target_sources(mbgl-core
        # Default styles
        PRIVATE platform/default/mbgl/util/default_styles.hpp

        # Misc
        PRIVATE platform/darwin/mbgl/storage/reachability.h
        PRIVATE platform/darwin/mbgl/storage/reachability.m
        PRIVATE platform/darwin/src/CFHandle.hpp
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
        PRIVATE platform/darwin/src/headless_backend_eagl.mm

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

    target_include_directories(mbgl-core
        PUBLIC platform/darwin
        PUBLIC platform/default
    )

    target_link_libraries(mbgl-core
        PUBLIC "-lz"
        PUBLIC "-framework Foundation"
        PUBLIC "-framework CoreText"
        PUBLIC "-framework CoreGraphics"
        PUBLIC "-framework OpenGLES"
        PUBLIC "-framework ImageIO"
        PUBLIC "-framework MobileCoreServices"
        PUBLIC "-framework SystemConfiguration"
    )
endmacro()


macro(mbgl_filesource)
    initialize_ios_target(mbgl-filesource)

    target_sources(mbgl-filesource
        # File source
        PRIVATE platform/darwin/src/http_file_source.mm

        # Database
        PRIVATE platform/default/sqlite3.cpp
    )

    target_link_libraries(mbgl-filesource
        PUBLIC "-lsqlite3"
        PUBLIC "-framework Foundation"
    )
endmacro()
