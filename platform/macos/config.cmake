include(cmake/loop-darwin.cmake)

macro(mbgl_platform_core)
    target_sources_from_file(mbgl-core PRIVATE platform/macos/core-files.json)

    if(WITH_EGL)
        target_sources(mbgl-core
            PRIVATE platform/linux/src/headless_backend_egl.cpp
        )
        target_add_mason_package(mbgl-core PUBLIC swiftshader)
    else()
        target_sources(mbgl-core
            PRIVATE platform/darwin/src/headless_backend_cgl.mm
        )
        target_link_libraries(mbgl-core
            PUBLIC "-framework OpenGL"
        )
    endif()

    target_compile_options(mbgl-core
        PRIVATE -fobjc-arc
    )

    target_include_directories(mbgl-core
        PUBLIC platform/darwin/include
        PUBLIC platform/default/include
    )

    target_link_libraries(mbgl-core
        PUBLIC "-lz"
        PUBLIC "-framework Foundation"
        PUBLIC "-framework CoreText"
        PUBLIC "-framework CoreGraphics"
        PUBLIC "-framework ImageIO"
        PUBLIC "-framework CoreServices"
        PUBLIC "-framework SystemConfiguration"
    )
endmacro()


macro(mbgl_filesource)
    # Modify platform/darwin/filesource-files.json to change the source files for this target.
    target_sources_from_file(mbgl-filesource PRIVATE platform/darwin/filesource-files.json)

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
    target_sources(mbgl-offline
        PRIVATE platform/default/src/mbgl/layermanager/layer_manager.cpp
    )

    target_link_libraries(mbgl-offline
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-darwin
    )
endmacro()


macro(mbgl_platform_test)
    target_sources(mbgl-test
        PRIVATE platform/default/src/mbgl/layermanager/layer_manager.cpp
        PRIVATE platform/default/src/mbgl/test/main.cpp
    )

    target_include_directories(mbgl-test
        PRIVATE platform/macos
    )

    set_source_files_properties(
        platform/default/src/mbgl/test/main.cpp
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
        PRIVATE platform/default/src/mbgl/layermanager/layer_manager.cpp
        PRIVATE platform/default/src/mbgl/benchmark/main.cpp
    )

    set_source_files_properties(
        platform/default/src/mbgl/benchmark/main.cpp
            PROPERTIES
        COMPILE_FLAGS -DWORK_DIRECTORY="${CMAKE_SOURCE_DIR}"
    )

    target_link_libraries(mbgl-benchmark
        PRIVATE mbgl-filesource
        PRIVATE mbgl-loop-darwin
    )
endmacro()

macro(mbgl_platform_node)
    target_sources(mbgl-core
        PRIVATE platform/default/src/mbgl/layermanager/layer_manager.cpp
    )
    target_link_libraries(mbgl-node INTERFACE
        -exported_symbols_list ${CMAKE_SOURCE_DIR}/platform/node/symbol-list
        -dead_strip
    )
endmacro()
