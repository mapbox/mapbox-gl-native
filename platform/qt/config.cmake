include(platform/qt/qt.cmake)

mason_use(sqlite VERSION 3.9.1)
mason_use(gtest VERSION 1.7.0${MASON_CXXABI_SUFFIX})

if(NOT WITH_QT_DECODERS)
    mason_use(libjpeg-turbo VERSION 1.4.2)
    mason_use(libpng VERSION 1.6.20)
    mason_use(webp VERSION 0.5.0)
endif()

macro(mbgl_platform_core)
    target_sources(mbgl-core
        ${MBGL_QT_FILES}
    )

    target_include_directories(mbgl-core
        PRIVATE platform/default
        PRIVATE platform/qt/include
    )

    target_add_mason_package(mbgl-core PRIVATE sqlite)

    target_link_libraries(mbgl-core
        ${MBGL_QT_LIBRARIES}
    )

    if(NOT WITH_QT_DECODERS)
        target_sources(mbgl-core
            PRIVATE platform/default/jpeg_reader.cpp
            PRIVATE platform/default/png_reader.cpp
            PRIVATE platform/default/webp_reader.cpp
        )

        target_add_mason_package(mbgl-core PRIVATE libjpeg-turbo)
        target_add_mason_package(mbgl-core PRIVATE libpng)
        target_add_mason_package(mbgl-core PRIVATE webp)
    else()
        add_definitions(-DQT_IMAGE_DECODERS)
    endif()
endmacro()

# TODO: Implement a Qt headless view, using offscreen
# graphics system.
macro(mbgl_platform_test)
    target_sources(mbgl-test
        PRIVATE test/src/main.cpp
    )

    set_source_files_properties(
        test/src/main.cpp
        PROPERTIES COMPILE_FLAGS -DWORK_DIRECTORY="${CMAKE_SOURCE_DIR}"
    )
endmacro()
