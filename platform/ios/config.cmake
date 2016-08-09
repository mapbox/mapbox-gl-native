macro(mbgl_platform_core)
    set_xcode_property(mbgl-core IPHONEOS_DEPLOYMENT_TARGET "8.0")
    set_xcode_property(mbgl-core ENABLE_BITCODE "YES")
    set_xcode_property(mbgl-core BITCODE_GENERATION_MODE bitcode)

    target_sources(mbgl-core
        # Loop
        PRIVATE platform/darwin/src/async_task.cpp
        PRIVATE platform/darwin/src/run_loop.cpp
        PRIVATE platform/darwin/src/timer.cpp

        # File source
        PRIVATE platform/darwin/src/http_file_source.mm
        PRIVATE platform/default/asset_file_source.cpp
        PRIVATE platform/default/default_file_source.cpp
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
        PRIVATE platform/darwin/src/headless_view_eagl.mm
        PRIVATE platform/default/headless_display.cpp
        PRIVATE platform/default/headless_view.cpp
    )

    target_compile_options(mbgl-core
        PRIVATE -fobjc-arc
    )

    # TODO: Remove this by converting to ARC
    set_source_files_properties(
        platform/darwin/src/headless_view_eagl.mm
            PROPERTIES
        COMPILE_FLAGS -fno-objc-arc
    )

    target_include_directories(mbgl-core
        PRIVATE platform/default
    )

    target_link_libraries(mbgl-core
        PUBLIC -lz
    )
endmacro()