include(${PROJECT_SOURCE_DIR}/platform/gfx/gl/gl.cmake)

target_compile_definitions(mbgl-core PUBLIC
    MBGL_USE_GLES2
)

find_package(Qt5Core    REQUIRED)
find_package(Qt5Gui     REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5OpenGL  REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql     REQUIRED)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions("-DQT_COMPILING_QIMAGE_COMPAT_CPP")
    add_definitions("-DRAPIDJSON_HAS_CXX11_RVALUE_REFS")
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

target_sources(mbgl-core PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/async_task.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/async_task_impl.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/bidi.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/format_number.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gl_functions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/headless_backend_qt.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/http_file_source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/http_file_source.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/http_request.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/http_request.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/local_glyph_rasterizer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qt_image.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qt_logging.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/run_loop.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/run_loop_impl.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/sqlite3.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/string_stdlib.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/thread.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/thread_local.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/timer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/timer_impl.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/utf.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/include/mbgl/gfx/headless_backend.hpp
    ${PROJECT_SOURCE_DIR}/platform/default/include/mbgl/gfx/headless_frontend.hpp
    ${PROJECT_SOURCE_DIR}/platform/default/include/mbgl/gl/headless_backend.hpp
    ${PROJECT_SOURCE_DIR}/platform/default/include/mbgl/text/unaccent.hpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_backend.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_frontend.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gl/headless_backend.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/layermanager/layer_manager.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/asset_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/default_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/file_source_request.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/local_file_request.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/local_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline_database.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline_download.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/online_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/collator.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/unaccent.cpp
)

target_compile_definitions(mbgl-core PRIVATE
    QT_IMAGE_DECODERS
)

target_compile_definitions(mbgl-core PUBLIC
    __QT__
)

target_include_directories(mbgl-core PRIVATE
    ${PROJECT_SOURCE_DIR}/platform/default/include
)

include(${PROJECT_SOURCE_DIR}/vendor/nunicode.cmake)

target_link_libraries(mbgl-core PRIVATE
    $<$<NOT:$<PLATFORM_ID:Windows>>:z>
    Qt5::Core
    Qt5::Gui
    Qt5::Network
    Qt5::OpenGL
    Qt5::Sql
    mbgl-vendor-nunicode
)

add_library(qmapboxgl SHARED
    ${CMAKE_CURRENT_LIST_DIR}/include/qmapbox.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/qmapboxgl.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qmapbox.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qmapboxgl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qmapboxgl_map_observer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qmapboxgl_map_observer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qmapboxgl_map_renderer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qmapboxgl_map_renderer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qmapboxgl_p.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qmapboxgl_renderer_backend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qmapboxgl_renderer_backend.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qmapboxgl_scheduler.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qmapboxgl_scheduler.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qt_conversion.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qt_geojson.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/qt_geojson.hpp
)

# FIXME: Because of rapidjson conversion
target_include_directories(qmapboxgl PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_include_directories(qmapboxgl PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_compile_definitions(qmapboxgl PRIVATE
    QT_BUILD_MAPBOXGL_LIB
)

target_link_libraries(qmapboxgl
    Qt5::Core
    Qt5::Gui
    mbgl-core
)

add_executable(mbgl-qt
    ${CMAKE_CURRENT_LIST_DIR}/app/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/app/mapwindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/app/mapwindow.hpp
    ${CMAKE_CURRENT_LIST_DIR}/resources/common.qrc
)

# Qt public API should keep compatibility
# with old compilers for legacy systems
set_property(TARGET mbgl-qt PROPERTY CXX_STANDARD 98)

target_link_libraries(mbgl-qt PRIVATE
    Qt5::Widgets
    Qt5::Gui
    qmapboxgl
)

add_executable(mbgl-test-runner
    ${CMAKE_CURRENT_LIST_DIR}/test/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test/qmapboxgl.test.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test/qmapboxgl.test.hpp
)

target_compile_definitions(mbgl-test-runner PRIVATE
    WORK_DIRECTORY=${PROJECT_SOURCE_DIR}
)

target_link_libraries(mbgl-test-runner PRIVATE
    Qt5::Gui
    Qt5::OpenGL
    qmapboxgl
    mbgl-test
    pthread
)

add_executable(mbgl-benchmark-runner
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/benchmark/main.cpp
)

target_link_libraries(mbgl-benchmark-runner PRIVATE
    mbgl-benchmark
)

add_test(NAME mbgl-benchmark-runner COMMAND mbgl-benchmark-runner WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_test(NAME mbgl-test-runner COMMAND mbgl-test-runner WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
