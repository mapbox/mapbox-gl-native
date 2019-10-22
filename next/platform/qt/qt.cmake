# Note: Using Sqlite instead of QSqlDatabase for better compatibility.

find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions("-DQT_COMPILING_QIMAGE_COMPAT_CPP")
    add_definitions("-D_USE_MATH_DEFINES")
    add_definitions("-Wno-deprecated-declarations")
    add_definitions("-Wno-inconsistent-missing-override")
    add_definitions("-Wno-macro-redefined")
    add_definitions("-Wno-microsoft-exception-spec")
    add_definitions("-Wno-unknown-argument")
    add_definitions("-Wno-unknown-warning-option")
    add_definitions("-Wno-unused-command-line-argument")
    add_definitions("-Wno-unused-local-typedef")
    add_definitions("-Wno-unused-private-field")
endif()

target_sources(
    mbgl-core
    PRIVATE
        ${MBGL_ROOT}/platform/$<IF:$<PLATFORM_ID:Windows>,qt/src/bidi.cpp,default/src/mbgl/text/bidi.cpp>
        ${MBGL_ROOT}/platform/default/include/mbgl/gfx/headless_backend.hpp
        ${MBGL_ROOT}/platform/default/include/mbgl/gfx/headless_frontend.hpp
        ${MBGL_ROOT}/platform/default/include/mbgl/gl/headless_backend.hpp
        ${MBGL_ROOT}/platform/default/src/mbgl/gfx/headless_backend.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/gfx/headless_frontend.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/gl/headless_backend.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/i18n/collator.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/layermanager/layer_manager.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/asset_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/default_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/file_source_request.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/local_file_request.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/local_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/offline.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/offline_database.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/offline_download.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/online_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/sqlite3.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/compression.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/monotonic_timer.cpp
        ${MBGL_ROOT}/platform/qt/src/async_task.cpp
        ${MBGL_ROOT}/platform/qt/src/async_task_impl.hpp
        ${MBGL_ROOT}/platform/qt/src/number_format.cpp
        ${MBGL_ROOT}/platform/qt/src/gl_functions.cpp
        ${MBGL_ROOT}/platform/qt/src/headless_backend_qt.cpp
        ${MBGL_ROOT}/platform/qt/src/http_file_source.cpp
        ${MBGL_ROOT}/platform/qt/src/http_file_source.hpp
        ${MBGL_ROOT}/platform/qt/src/http_request.cpp
        ${MBGL_ROOT}/platform/qt/src/http_request.hpp
        ${MBGL_ROOT}/platform/qt/src/local_glyph_rasterizer.cpp
        ${MBGL_ROOT}/platform/qt/src/qt_image.cpp
        ${MBGL_ROOT}/platform/qt/src/qt_logging.cpp
        ${MBGL_ROOT}/platform/qt/src/run_loop.cpp
        ${MBGL_ROOT}/platform/qt/src/run_loop_impl.hpp
        ${MBGL_ROOT}/platform/qt/src/string_stdlib.cpp
        ${MBGL_ROOT}/platform/qt/src/thread.cpp
        ${MBGL_ROOT}/platform/qt/src/thread_local.cpp
        ${MBGL_ROOT}/platform/qt/src/timer.cpp
        ${MBGL_ROOT}/platform/qt/src/timer_impl.hpp
        ${MBGL_ROOT}/platform/qt/src/utf.cpp
)

target_compile_definitions(
    mbgl-core
    PRIVATE QT_IMAGE_DECODERS
    PUBLIC __QT__ MBGL_USE_GLES2
)

target_include_directories(
    mbgl-core
    PRIVATE ${MBGL_ROOT}/platform/default/include
)

include(${PROJECT_SOURCE_DIR}/vendor/icu.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/nunicode.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/sqlite.cmake)

target_link_libraries(
    mbgl-core
    PRIVATE
        $<$<NOT:$<PLATFORM_ID:Windows>>:z>
        $<$<NOT:$<PLATFORM_ID:Windows>>:mbgl-vendor-icu>
        Qt5::Core
        Qt5::Gui
        Qt5::Network
        Qt5::OpenGL
        mbgl-vendor-nunicode
        mbgl-vendor-sqlite
)

add_library(
    qmapboxgl SHARED
    ${MBGL_ROOT}/platform/qt/include/qmapbox.hpp
    ${MBGL_ROOT}/platform/qt/include/qmapboxgl.hpp
    ${MBGL_ROOT}/platform/qt/src/qmapbox.cpp
    ${MBGL_ROOT}/platform/qt/src/qmapboxgl.cpp
    ${MBGL_ROOT}/platform/qt/src/qmapboxgl_map_observer.cpp
    ${MBGL_ROOT}/platform/qt/src/qmapboxgl_map_observer.hpp
    ${MBGL_ROOT}/platform/qt/src/qmapboxgl_map_renderer.cpp
    ${MBGL_ROOT}/platform/qt/src/qmapboxgl_map_renderer.hpp
    ${MBGL_ROOT}/platform/qt/src/qmapboxgl_p.hpp
    ${MBGL_ROOT}/platform/qt/src/qmapboxgl_renderer_backend.cpp
    ${MBGL_ROOT}/platform/qt/src/qmapboxgl_renderer_backend.hpp
    ${MBGL_ROOT}/platform/qt/src/qmapboxgl_scheduler.cpp
    ${MBGL_ROOT}/platform/qt/src/qmapboxgl_scheduler.hpp
    ${MBGL_ROOT}/platform/qt/src/qt_conversion.hpp
    ${MBGL_ROOT}/platform/qt/src/qt_geojson.cpp
    ${MBGL_ROOT}/platform/qt/src/qt_geojson.hpp
)

# FIXME: Because of rapidjson conversion
target_include_directories(
    qmapboxgl
    PRIVATE ${MBGL_ROOT}/src
)

target_include_directories(
    qmapboxgl
    PUBLIC ${MBGL_ROOT}/platform/qt/include
)

target_compile_definitions(
    qmapboxgl
    PRIVATE QT_BUILD_MAPBOXGL_LIB
)

target_link_libraries(
    qmapboxgl
    Qt5::Core
    Qt5::Gui
    mbgl-core
)

add_executable(
    mbgl-qt
    ${MBGL_ROOT}/platform/qt/app/main.cpp
    ${MBGL_ROOT}/platform/qt/app/mapwindow.cpp
    ${MBGL_ROOT}/platform/qt/app/mapwindow.hpp
    ${MBGL_ROOT}/platform/qt/resources/common.qrc
)

# Qt public API should keep compatibility with old compilers for legacy systems
set_property(TARGET mbgl-qt PROPERTY CXX_STANDARD 98)

target_link_libraries(
    mbgl-qt
    PRIVATE Qt5::Widgets Qt5::Gui qmapboxgl
)

add_executable(
    mbgl-test-runner
    ${MBGL_ROOT}/platform/qt/test/main.cpp
)

target_compile_definitions(
    mbgl-test-runner
    PRIVATE WORK_DIRECTORY=${MBGL_ROOT}
)

target_link_libraries(
    mbgl-test-runner
    PRIVATE
        Qt5::Gui
        Qt5::OpenGL
        mbgl-test
        pthread
)

find_program(MBGL_QDOC NAMES qdoc)

if(MBGL_QDOC)
    add_custom_target(mbgl-qt-docs)

    add_custom_command(
        TARGET mbgl-qt-docs PRE_BUILD
        COMMAND
            ${MBGL_QDOC}
            ${MBGL_ROOT}/platform/qt/config.qdocconf
            -outputdir
            ${CMAKE_BINARY_DIR}/docs
    )
endif()

add_test(NAME mbgl-test-runner COMMAND mbgl-test-runner WORKING_DIRECTORY ${MBGL_ROOT})
