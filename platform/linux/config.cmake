include(cmake/sqlite.cmake)

add_library(mbgl-loop-uv STATIC
    platform/default/src/mbgl/util/async_task.cpp
    platform/default/src/mbgl/util/run_loop.cpp
    platform/default/src/mbgl/util/timer.cpp
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
            PRIVATE platform/default/src/mbgl/gl/headless_backend_osmesa.cpp
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
        # GL
        PRIVATE platform/linux/src/gl_functions.cpp

        # Misc
        PRIVATE platform/default/src/mbgl/util/logging_stderr.cpp
        PRIVATE platform/default/src/mbgl/util/string_stdlib.cpp
        PRIVATE platform/default/src/mbgl/util/thread.cpp
        PRIVATE platform/default/src/mbgl/text/bidi.cpp
        PRIVATE platform/default/src/mbgl/text/collator.cpp
        PRIVATE platform/default/src/mbgl/layermanager/layer_manager.cpp
        PRIVATE platform/default/src/mbgl/text/local_glyph_rasterizer.cpp
        PRIVATE platform/default/src/mbgl/util/thread_local.cpp
        PRIVATE platform/default/src/mbgl/text/unaccent.cpp
        PRIVATE platform/default/include/mbgl/text/unaccent.hpp
        PRIVATE platform/default/src/mbgl/util/utf.cpp
        PRIVATE platform/default/src/mbgl/util/format_number.cpp

        # Image handling
        PRIVATE platform/default/src/mbgl/util/image.cpp
        PRIVATE platform/default/src/mbgl/util/jpeg_reader.cpp
        PRIVATE platform/default/src/mbgl/util/png_writer.cpp
        PRIVATE platform/default/src/mbgl/util/png_reader.cpp

        # Headless view
        PRIVATE platform/default/src/mbgl/gfx/headless_frontend.cpp
        PRIVATE platform/default/include/mbgl/gfx/headless_frontend.hpp
        PRIVATE platform/default/src/mbgl/gfx/headless_backend.cpp
        PRIVATE platform/default/include/mbgl/gfx/headless_backend.hpp
        PRIVATE platform/default/src/mbgl/gl/headless_backend.cpp
        PRIVATE platform/default/include/mbgl/gl/headless_backend.hpp

        # Snapshotting
        PRIVATE platform/default/src/mbgl/map/map_snapshotter.cpp
        PRIVATE platform/default/include/mbgl/map/map_snapshotter.hpp
    )

    target_include_directories(mbgl-core
        PRIVATE platform/default/include
        PRIVATE platform/linux
    )

    target_add_mason_package(mbgl-core PUBLIC libpng)
    target_add_mason_package(mbgl-core PUBLIC libjpeg-turbo)
    target_add_mason_package(mbgl-core PRIVATE icu)

    # Ignore warning caused by ICU header unistr.h in some CI environments
    set_source_files_properties(platform/default/src/mbgl/util/format_number.cpp PROPERTIES COMPILE_FLAGS -Wno-error=shadow)

    # Link all ICU libraries (by default only libicuuc is linked)
    find_library(LIBICUI18N NAMES icui18n HINTS ${MASON_PACKAGE_icu_INCLUDE_DIRS}/../lib)
    find_library(LIBICUUC NAMES icuuc HINTS ${MASON_PACKAGE_icu_INCLUDE_DIRS}/../lib)
    find_library(LIBICUDATA NAMES icudata HINTS ${MASON_PACKAGE_icu_INCLUDE_DIRS}/../lib)

    target_link_libraries(mbgl-core
        PRIVATE ${LIBICUI18N}
        PRIVATE ${LIBICUUC}
        PRIVATE ${LIBICUDATA}
        PRIVATE nunicode
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
    # Modify platform/linux/filesource-files.json to change the source files for this target.
    target_sources_from_file(mbgl-filesource PRIVATE platform/linux/filesource-files.json)

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
        PRIVATE platform/default/src/mbgl/test/main.cpp
    )

    target_include_directories(mbgl-test
        PRIVATE platform/linux
    )

    set_source_files_properties(
        platform/default/src/mbgl/test/main.cpp
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
        PRIVATE platform/default/src/mbgl/benchmark/main.cpp
    )

    set_source_files_properties(
        platform/default/src/mbgl/benchmark/main.cpp
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
