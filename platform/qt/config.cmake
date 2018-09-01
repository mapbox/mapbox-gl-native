include(platform/qt/qt.cmake)
include(cmake/nunicode.cmake)

macro(mbgl_platform_core)
    target_sources(mbgl-core
        ${MBGL_QT_CORE_FILES}
    )

    target_include_directories(mbgl-core
        PUBLIC platform/default
        PRIVATE platform/qt
        PRIVATE platform/qt/include
    )

    target_link_libraries(mbgl-core
        ${MBGL_QT_CORE_LIBRARIES}
        PRIVATE nunicode
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

    if(NOT WITH_QT_I18N)
        target_sources(mbgl-core PRIVATE platform/default/bidi.cpp)
        target_add_mason_package(mbgl-core PRIVATE icu)
    else()
        target_sources(mbgl-core PRIVATE platform/qt/src/bidi.cpp)
    endif()

    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        target_add_mason_package(mbgl-core PRIVATE optional)
    endif()
endmacro()


macro(mbgl_filesource)
    target_sources(mbgl-filesource
        ${MBGL_QT_FILESOURCE_FILES}
    )

    target_link_libraries(mbgl-filesource
        ${MBGL_QT_FILESOURCE_LIBRARIES}
    )

    if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        target_add_mason_package(mbgl-filesource PRIVATE optional)
    endif()
endmacro()

# FIXME: For now tests are disabled on Windows until we
# get the node.js dependencies working.
if (NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    macro(mbgl_platform_test)
        target_sources(mbgl-test
            PRIVATE platform/qt/test/main.cpp
            PRIVATE platform/qt/test/qmapboxgl.test.cpp
            PRIVATE platform/qt/test/qmapboxgl.test.cpp
        )

        target_include_directories(mbgl-test
            PRIVATE platform/qt
        )

        set_source_files_properties(
            platform/qt/test/main.cpp
            PROPERTIES COMPILE_FLAGS -DWORK_DIRECTORY="${CMAKE_SOURCE_DIR}"
        )

        target_link_libraries(mbgl-test
            PRIVATE qmapboxgl
            PRIVATE mbgl-filesource
        )
    endmacro()
endif()

target_add_mason_package(qmapboxgl PRIVATE geojson)
target_add_mason_package(qmapboxgl PRIVATE rapidjson)
