mason_use(glfw 3.1.2)
mason_use(boost_libprogram_options 1.60.0)
mason_use(sqlite 3.9.1)
mason_use(libuv 1.7.5)
mason_use(nunicode 1.6)
mason_use(libpng 1.6.20)
mason_use(libjpeg-turbo 1.4.2)
mason_use(webp 0.5.0)

include(cmake/loop-uv.cmake)

macro(mbgl_platform_core)
    target_sources(mbgl-core
        # File source
        PRIVATE platform/default/asset_file_source.cpp
        PRIVATE platform/default/default_file_source.cpp
        PRIVATE platform/default/http_file_source.cpp
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
        PRIVATE platform/default/log_stderr.cpp
        PRIVATE platform/default/string_stdlib.cpp
        PRIVATE platform/default/thread.cpp

        # Image handling
        PRIVATE platform/default/image.cpp
        PRIVATE platform/default/jpeg_reader.cpp
        PRIVATE platform/default/png_reader.cpp
        PRIVATE platform/default/webp_reader.cpp

        # Headless view
        PRIVATE platform/default/headless_display.cpp
        PRIVATE platform/default/headless_view.cpp
        PRIVATE platform/default/headless_view_glx.cpp
    )

    target_include_directories(mbgl-core
        PRIVATE platform/default
    )

    target_add_mason_package(mbgl-core PUBLIC sqlite)
    target_add_mason_package(mbgl-core PUBLIC nunicode)
    target_add_mason_package(mbgl-core PUBLIC libpng)
    target_add_mason_package(mbgl-core PUBLIC libjpeg-turbo)
    target_add_mason_package(mbgl-core PUBLIC webp)

    target_link_libraries(mbgl-core
        PUBLIC -lz
        PUBLIC -lcurl
        PUBLIC -lGL
        PUBLIC -lX11
    )
endmacro()


macro(mbgl_platform_glfw)
    target_link_libraries(mbgl-glfw
        PRIVATE mbgl-loop
    )
endmacro()


macro(mbgl_platform_render)
    target_link_libraries(mbgl-render
        PRIVATE mbgl-loop
    )
endmacro()


macro(mbgl_platform_offline)
    target_link_libraries(mbgl-offline
        PRIVATE mbgl-loop
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
    )
endmacro()


macro(mbgl_platform_node)
    # Enabling node module by defining this macro
endmacro()
