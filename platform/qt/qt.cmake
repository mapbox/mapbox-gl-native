# This file is to be reused by target platforms that don't
# support `mason` (i.e. Yocto). Do not add any `mason` macro.

option(WITH_QT_DECODERS "Use builtin Qt image decoders" OFF)
option(WITH_QT_I18N     "Use builtin Qt i18n support"   OFF)

add_definitions("-D__QT__")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(MBGL_QT_CORE_FILES
    # Headless view
    PRIVATE platform/default/src/mbgl/gfx/headless_frontend.cpp
    PRIVATE platform/default/include/mbgl/gfx/headless_frontend.hpp
    PRIVATE platform/default/src/mbgl/gfx/headless_backend.cpp
    PRIVATE platform/default/include/mbgl/gfx/headless_backend.hpp
    PRIVATE platform/default/src/mbgl/gl/headless_backend.cpp
    PRIVATE platform/default/include/mbgl/gl/headless_backend.hpp
    PRIVATE platform/qt/src/headless_backend_qt.cpp

    # Thread
    PRIVATE platform/qt/src/thread_local.cpp

    # Platform integration
    PRIVATE platform/qt/src/async_task.cpp
    PRIVATE platform/qt/src/async_task_impl.hpp
    PRIVATE platform/qt/src/local_glyph_rasterizer.cpp
    PRIVATE platform/qt/src/qt_logging.cpp
    PRIVATE platform/qt/src/qt_image.cpp
    PRIVATE platform/qt/src/run_loop.cpp
    PRIVATE platform/qt/src/run_loop_impl.hpp
    PRIVATE platform/qt/src/string_stdlib.cpp
    PRIVATE platform/qt/src/timer.cpp
    PRIVATE platform/qt/src/timer_impl.hpp
    PRIVATE platform/qt/src/utf.cpp
    PRIVATE platform/qt/src/gl_functions.cpp
    PRIVATE platform/qt/src/format_number.cpp

    PRIVATE platform/default/src/mbgl/text/collator.cpp
    PRIVATE platform/default/src/mbgl/text/unaccent.cpp
    PRIVATE platform/default/include/mbgl/text/unaccent.hpp

    #Layer manager
    PRIVATE platform/default/src/mbgl/layermanager/layer_manager.cpp
)

set(MBGL_QT_FILESOURCE_FILES
    # File source
    PRIVATE platform/default/src/mbgl/storage/file_source.cpp
    PRIVATE platform/qt/src/http_file_source.cpp
    PRIVATE platform/qt/src/http_file_source.hpp
    PRIVATE platform/qt/src/http_request.cpp
    PRIVATE platform/qt/src/http_request.hpp

    # Database
    PRIVATE platform/qt/src/sqlite3.cpp
)

# Shared library
add_library(qmapboxgl SHARED
    platform/qt/include/qmapbox.hpp
    platform/qt/include/qmapboxgl.hpp
    platform/qt/src/qt_conversion.hpp
    platform/qt/src/qt_geojson.cpp
    platform/qt/src/qt_geojson.hpp
    platform/qt/src/qmapbox.cpp
    platform/qt/src/qmapboxgl.cpp
    platform/qt/src/qmapboxgl_p.hpp
    platform/qt/src/qmapboxgl_map_observer.cpp
    platform/qt/src/qmapboxgl_map_observer.hpp
    platform/qt/src/qmapboxgl_map_renderer.cpp
    platform/qt/src/qmapboxgl_map_renderer.hpp
    platform/qt/src/qmapboxgl_renderer_backend.cpp
    platform/qt/src/qmapboxgl_renderer_backend.hpp
    platform/qt/src/qmapboxgl_scheduler.cpp
    platform/qt/src/qmapboxgl_scheduler.hpp
    platform/default/include/mbgl/util/default_styles.hpp
)

target_include_directories(qmapboxgl
    PUBLIC platform/qt/include
    PRIVATE src
)

target_compile_definitions(qmapboxgl
    PRIVATE "-DQT_BUILD_MAPBOXGL_LIB"
)

target_link_libraries(qmapboxgl
    PRIVATE mbgl-core
    PRIVATE mbgl-filesource
    ${MBGL_QT_CORE_LIBRARIES}
    ${MBGL_QT_FILESOURCE_LIBRARIES}
)

# C++ app
add_executable(mbgl-qt
    platform/qt/app/main.cpp
    platform/qt/app/mapwindow.cpp
    platform/qt/app/mapwindow.hpp
    platform/qt/resources/common.qrc
)

target_compile_options(qmapboxgl
    PRIVATE -std=c++03
)

target_link_libraries(mbgl-qt
    PRIVATE qmapboxgl
)

find_package(Qt5Core     REQUIRED)
find_package(Qt5Gui      REQUIRED)
find_package(Qt5Network  REQUIRED)
find_package(Qt5OpenGL   REQUIRED)
find_package(Qt5Widgets  REQUIRED)
find_package(Qt5Sql      REQUIRED)

# Qt5 always build OpenGL ES2 which is the compatibility
# mode. Qt5 will take care of translating the desktop
# version of OpenGL to ES2.
add_definitions("-DMBGL_USE_GLES2")

set(MBGL_QT_CORE_LIBRARIES
    PUBLIC Qt5::Core
    PUBLIC Qt5::Gui
    PUBLIC Qt5::OpenGL
)

set(MBGL_QT_FILESOURCE_LIBRARIES
    PUBLIC Qt5::Network
    PUBLIC Qt5::Sql
)

target_link_libraries(mbgl-qt
    PRIVATE Qt5::Widgets
)

xcode_create_scheme(TARGET mbgl-qt)

# OS specific configurations
if (MASON_PLATFORM STREQUAL "osx" OR MASON_PLATFORM STREQUAL "ios")
    list(APPEND MBGL_QT_CORE_FILES
        PRIVATE platform/darwin/src/nsthread.mm
    )
    list(APPEND MBGL_QT_CORE_LIBRARIES
        PRIVATE "-framework Foundation"
    )
elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND MBGL_QT_CORE_FILES
        PRIVATE platform/default/src/mbgl/util/thread.cpp
    )
elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    add_definitions("-DQT_COMPILING_QIMAGE_COMPAT_CPP")
    add_definitions("-DRAPIDJSON_HAS_CXX11_RVALUE_REFS")
    add_definitions("-D_USE_MATH_DEFINES")

    add_definitions("-Wno-deprecated-declarations")
    add_definitions("-Wno-macro-redefined")
    add_definitions("-Wno-microsoft-exception-spec")
    add_definitions("-Wno-unknown-argument")
    add_definitions("-Wno-unknown-warning-option")
    add_definitions("-Wno-unused-command-line-argument")
    add_definitions("-Wno-unused-local-typedef")
    add_definitions("-Wno-unused-private-field")
    add_definitions("-Wno-inconsistent-missing-override")

    list(APPEND MBGL_QT_CORE_FILES
        PRIVATE platform/qt/src/thread.cpp
    )
elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "QNX")
    list(APPEND MBGL_QT_CORE_FILES
        PRIVATE platform/qt/src/thread.cpp
    )
    add_definitions("-Wno-narrowing")
endif()

add_custom_command(
    TARGET qmapboxgl
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/platform/qt/include
            ${CMAKE_CURRENT_BINARY_DIR}/platform/qt/include
)

xcode_create_scheme(TARGET qmapboxgl)
