mason_use(glfw VERSION 2017-02-09-77a8f10)
mason_use(mesa VERSION 13.0.4)
mason_use(boost_libprogram_options VERSION 1.62.0${MASON_CXXABI_SUFFIX})
mason_use(sqlite VERSION 3.14.2)
mason_use(libuv VERSION 1.9.1)
mason_use(nunicode VERSION 1.7.1)
mason_use(libpng VERSION 1.6.25)
mason_use(libjpeg-turbo VERSION 1.5.0)
mason_use(webp VERSION 0.5.1)
mason_use(gtest VERSION 1.8.0${MASON_CXXABI_SUFFIX})
mason_use(benchmark VERSION 1.0.0-1)
mason_use(icu VERSION 58.1-min-size)

# Link with libuv. This is not part of loop-uv.cmake because loop-uv.cmake is also
# used by node.cmake, where we want to link with the libuv provided by node itself.
target_add_mason_package(mbgl-loop-uv PUBLIC libuv)

macro(mbgl_platform_core)
    target_add_mason_package(mbgl-core PUBLIC mesa)

    if(WITH_OSMESA)
        target_sources(mbgl-core
            PRIVATE platform/default/headless_backend_osmesa.cpp
            PRIVATE platform/default/mbgl/gl/headless_display.cpp
        )
        target_link_libraries(mbgl-core
            PUBLIC -lOSMesa
        )
    elseif(WITH_EGL)
        target_sources(mbgl-core
            PRIVATE platform/linux/src/headless_backend_egl.cpp
            PRIVATE platform/linux/src/headless_display_egl.cpp
        )
        target_link_libraries(mbgl-core
            PUBLIC -lGLESv2
            PUBLIC -lEGL
            PUBLIC -lgbm
        )
    else()
        target_sources(mbgl-core
            PRIVATE platform/linux/src/headless_backend_glx.cpp
            PRIVATE platform/linux/src/headless_display_glx.cpp
        )
        target_link_libraries(mbgl-core
            PUBLIC -lGL
            PUBLIC -lX11
        )
    endif()

    target_sources(mbgl-core
        # File source
        PRIVATE platform/default/asset_file_source.cpp
        PRIVATE platform/default/default_file_source.cpp
        PRIVATE platform/default/local_file_source.cpp
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
        PRIVATE platform/default/logging_stderr.cpp
        PRIVATE platform/default/string_stdlib.cpp
        PRIVATE platform/default/thread.cpp
        PRIVATE platform/default/bidi.cpp
        PRIVATE platform/default/utf.cpp

        # Image handling
        PRIVATE platform/default/image.cpp
        PRIVATE platform/default/jpeg_reader.cpp
        PRIVATE platform/default/png_writer.cpp
        PRIVATE platform/default/png_reader.cpp
        PRIVATE platform/default/webp_reader.cpp

        # Headless view
        PRIVATE platform/default/mbgl/gl/headless_backend.cpp
        PRIVATE platform/default/mbgl/gl/headless_backend.hpp
        PRIVATE platform/default/mbgl/gl/headless_display.hpp
        PRIVATE platform/default/mbgl/gl/offscreen_view.cpp
        PRIVATE platform/default/mbgl/gl/offscreen_view.hpp

        # Thread pool
        PRIVATE platform/default/mbgl/util/default_thread_pool.cpp
        PRIVATE platform/default/mbgl/util/default_thread_pool.cpp
    )

    target_include_directories(mbgl-core
        PRIVATE platform/default
    )

    target_add_mason_package(mbgl-core PUBLIC sqlite)
    target_add_mason_package(mbgl-core PUBLIC nunicode)
    target_add_mason_package(mbgl-core PUBLIC libpng)
    target_add_mason_package(mbgl-core PUBLIC libjpeg-turbo)
    target_add_mason_package(mbgl-core PUBLIC webp)
    target_add_mason_package(mbgl-core PUBLIC icu)

    target_link_libraries(mbgl-core
        PUBLIC -lz
        PUBLIC -lcurl
    )
endmacro()


macro(mbgl_platform_glfw)
    target_link_libraries(mbgl-glfw
        PRIVATE mbgl-loop-uv
    )

    add_custom_command(
        TARGET mbgl-glfw POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/common/ca-bundle.crt
                ${CMAKE_CURRENT_BINARY_DIR}/ca-bundle.crt
    )
endmacro()


macro(mbgl_platform_render)
    target_link_libraries(mbgl-render
        PRIVATE mbgl-loop-uv
    )
endmacro()


macro(mbgl_platform_offline)
    target_link_libraries(mbgl-offline
        PRIVATE mbgl-loop-uv
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

    target_link_libraries(mbgl-test
        PRIVATE mbgl-loop-uv
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
        PRIVATE mbgl-loop-uv
    )
endmacro()


macro(mbgl_platform_node)
    # Enabling node module by defining this macro
endmacro()
