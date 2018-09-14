include(cmake/nunicode.cmake)
include(cmake/sqlite.cmake)
include(cmake/icu.cmake)

add_library(mbgl-loop-uv STATIC
    platform/default/async_task.cpp
    platform/default/run_loop.cpp
    platform/default/timer.cpp
)

target_include_directories(mbgl-loop-uv
    PRIVATE include
    PRIVATE src
)

target_link_libraries(mbgl-loop-uv
    PRIVATE mbgl-core
)

target_add_mason_package(mbgl-loop-uv PUBLIC libuv)

macro(mbgl_platform_core)
    if(WITH_OSMESA)
        target_sources(mbgl-core
            PRIVATE platform/default/headless_backend_osmesa.cpp
        )
        target_link_libraries(mbgl-core
            PUBLIC -lOSMesa
        )
    elseif(WITH_EGL)
        target_sources(mbgl-core
            PRIVATE platform/linux/src/headless_backend_egl.cpp
        )
        target_add_mason_package(mbgl-core PUBLIC swiftshader)
    else()
        target_sources(mbgl-core
            PRIVATE platform/linux/src/headless_backend_glx.cpp
        )
        target_link_libraries(mbgl-core
            PUBLIC -lGL
            PUBLIC -lX11
        )
    endif()

    target_sources(mbgl-core
        # Misc
        PRIVATE platform/default/logging_stderr.cpp
        PRIVATE platform/default/string_stdlib.cpp
        PRIVATE platform/default/thread.cpp
        PRIVATE platform/default/bidi.cpp
        PRIVATE platform/default/collator.cpp
        PRIVATE platform/default/local_glyph_rasterizer.cpp
        PRIVATE platform/default/thread_local.cpp
        PRIVATE platform/default/unaccent.cpp
        PRIVATE platform/default/unaccent.hpp
        PRIVATE platform/default/utf.cpp

        # Image handling
        PRIVATE platform/default/image.cpp
        PRIVATE platform/default/jpeg_reader.cpp
        PRIVATE platform/default/png_writer.cpp
        PRIVATE platform/default/png_reader.cpp

        # Headless view
        PRIVATE platform/default/mbgl/gl/headless_frontend.cpp
        PRIVATE platform/default/mbgl/gl/headless_frontend.hpp
        PRIVATE platform/default/mbgl/gl/headless_backend.cpp
        PRIVATE platform/default/mbgl/gl/headless_backend.hpp

        # Thread pool
        PRIVATE platform/default/mbgl/util/default_thread_pool.cpp
        PRIVATE platform/default/mbgl/util/default_thread_pool.cpp
        PRIVATE platform/default/mbgl/util/shared_thread_pool.cpp
    )

    target_include_directories(mbgl-core
        PRIVATE platform/default
        PRIVATE platform/linux
    )

    target_add_mason_package(mbgl-core PUBLIC libpng)
    target_add_mason_package(mbgl-core PUBLIC libjpeg-turbo)
    target_add_mason_package(mbgl-core PUBLIC geojson)

    target_link_libraries(mbgl-core
        PRIVATE nunicode
        PRIVATE icu
        PUBLIC -lz
    )

    if(WITH_CXX11ABI)
        # Statically link libstdc++ when we're using the new STL ABI
        target_link_libraries(mbgl-core
            PUBLIC -static-libstdc++
            PUBLIC -Wl,-Bsymbolic-functions
        )
    endif()
endmacro()


macro(mbgl_filesource)
    target_sources(mbgl-filesource
        # File source
        PRIVATE platform/default/http_file_source.cpp

        # Database
        PRIVATE platform/default/sqlite3.cpp
    )

    # We're not referencing any cURL symbols since we're dynamically loading it. However, we want to
    # link the library anyway since we're definitely going to load it on startup anyway.
    target_link_libraries(mbgl-filesource
        PUBLIC sqlite
        PUBLIC -Wl,--no-as-needed -lcurl -Wl,--as-needed
    )
endmacro()


macro(mbgl_platform_glfw)
    target_link_libraries(mbgl-glfw
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-uv
    )

    add_custom_command(
        TARGET mbgl-glfw POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/misc/ca-bundle.crt
                ${CMAKE_CURRENT_BINARY_DIR}/ca-bundle.crt
    )
endmacro()


macro(mbgl_platform_render)
    target_link_libraries(mbgl-render
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-uv
    )
endmacro()


macro(mbgl_platform_offline)
    target_link_libraries(mbgl-offline
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-uv
    )
endmacro()


macro(mbgl_platform_test)
    target_sources(mbgl-test
        PRIVATE platform/default/mbgl/test/main.cpp
    )

    target_include_directories(mbgl-test
        PRIVATE platform/linux
    )

    set_source_files_properties(
        platform/default/mbgl/test/main.cpp
            PROPERTIES
        COMPILE_FLAGS -DWORK_DIRECTORY="${CMAKE_SOURCE_DIR}"
    )

    target_link_libraries(mbgl-test
        PRIVATE mbgl-filesource
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
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-uv
    )
endmacro()


macro(mbgl_platform_node)
    target_link_libraries(mbgl-node INTERFACE
        -Wl,--version-script=${CMAKE_SOURCE_DIR}/platform/node/version-script
    )
endmacro()
