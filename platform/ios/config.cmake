add_definitions(-DMBGL_USE_GLES2=1)

mason_use(icu VERSION 58.1)

macro(mbgl_platform_core)
    set_xcode_property(mbgl-core IPHONEOS_DEPLOYMENT_TARGET "8.0")
    set_xcode_property(mbgl-core ENABLE_BITCODE "YES")
    set_xcode_property(mbgl-core BITCODE_GENERATION_MODE bitcode)
    set_xcode_property(mbgl-core ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)

    target_sources(mbgl-core
        # Loop
        PRIVATE platform/darwin/src/async_task.cpp
        PRIVATE platform/darwin/src/run_loop.cpp
        PRIVATE platform/darwin/src/timer.cpp

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
        PRIVATE platform/default/thread_local.cpp
        PRIVATE platform/default/utf.cpp

        # Image handling
        PRIVATE platform/darwin/src/image.mm

        # Headless view
        PRIVATE platform/default/mbgl/gl/headless_backend.cpp
        PRIVATE platform/default/mbgl/gl/headless_backend.hpp
        PRIVATE platform/darwin/src/headless_backend_eagl.mm
        PRIVATE platform/default/mbgl/gl/headless_display.cpp
        PRIVATE platform/default/mbgl/gl/headless_display.hpp
        PRIVATE platform/default/mbgl/gl/offscreen_view.cpp
        PRIVATE platform/default/mbgl/gl/offscreen_view.hpp

        # Thread pool
        PRIVATE platform/default/mbgl/util/default_thread_pool.cpp
        PRIVATE platform/default/mbgl/util/default_thread_pool.cpp
    )

    target_add_mason_package(mbgl-core PUBLIC geojson)
    target_add_mason_package(mbgl-core PUBLIC icu)

    target_compile_options(mbgl-core
        PRIVATE -fobjc-arc
    )

    # TODO: Remove this by converting to ARC
    set_source_files_properties(
        platform/darwin/src/headless_backend_eagl.mm
            PROPERTIES
        COMPILE_FLAGS -fno-objc-arc
    )

    target_include_directories(mbgl-core
        PUBLIC platform/darwin
        PUBLIC platform/default
    )

    target_link_libraries(mbgl-core
        PUBLIC -lz
    )
endmacro()
