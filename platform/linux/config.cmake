mason_use(glfw VERSION 3.2.1)
if(IS_CI_BUILD AND NOT WITH_EGL)
    mason_use(mesa VERSION 13.0.0${MASON_MESA_SUFFIX}${MASON_CXXABI_SUFFIX})
endif()
mason_use(boost_libprogram_options VERSION 1.60.0)
mason_use(sqlite VERSION 3.14.2)
mason_use(libuv VERSION 1.9.1)
mason_use(nunicode VERSION 1.7.1)
mason_use(libpng VERSION 1.6.25)
mason_use(libjpeg-turbo VERSION 1.5.0)
mason_use(webp VERSION 0.5.1)
mason_use(gtest VERSION 1.7.0${MASON_CXXABI_SUFFIX})
mason_use(benchmark VERSION 1.0.0)

include(cmake/loop-uv.cmake)

macro(mbgl_platform_core)
    if(WITH_OSMESA)
        target_sources(mbgl-core
            PRIVATE platform/default/headless_backend_osmesa.cpp
            PRIVATE platform/default/headless_display.cpp
        )
        target_add_mason_package(mbgl-core PUBLIC mesa)
    elseif(WITH_EGL)
        target_sources(mbgl-core
            PRIVATE platform/linux/src/headless_backend_egl.cpp
            PRIVATE platform/linux/src/headless_display_egl.cpp
        )
        # TODO: Provide surface-less EGL mesa for CI builds.
        # https://github.com/mapbox/mapbox-gl-native/issues/7020
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
        if (IS_CI_BUILD)
            target_add_mason_package(mbgl-core PUBLIC mesa)
            target_link_libraries(mbgl-core PUBLIC -lX11)
        else()
            target_link_libraries(mbgl-core
                PUBLIC -lGL
                PUBLIC -lX11
            )
        endif()
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
        PRIVATE platform/default/log_stderr.cpp
        PRIVATE platform/default/string_stdlib.cpp
        PRIVATE platform/default/thread.cpp

        # Image handling
        PRIVATE platform/default/image.cpp
        PRIVATE platform/default/jpeg_reader.cpp
        PRIVATE platform/default/png_reader.cpp
        PRIVATE platform/default/webp_reader.cpp

        # Headless view
        PRIVATE platform/default/headless_backend.cpp
        PRIVATE platform/default/offscreen_view.cpp

        # Thread pool
        PRIVATE platform/default/thread_pool.cpp
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
    )
endmacro()


macro(mbgl_platform_node)
    # Enabling node module by defining this macro
endmacro()
