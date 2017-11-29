# This file is to be reused by target platforms that don't
# support `mason` (i.e. Yocto). Do not add any `mason` macro.

option(WITH_QT_DECODERS "Use builtin Qt image decoders" OFF)
option(WITH_QT_I18N     "Use builtin Qt i18n support"   OFF)
option(WITH_QT_4        "Use Qt4 instead of Qt5"        OFF)

add_definitions("-D__QT__")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(MBGL_QT_CORE_FILES
    # Headless view
    PRIVATE platform/default/mbgl/gl/headless_frontend.cpp
    PRIVATE platform/default/mbgl/gl/headless_frontend.hpp
    PRIVATE platform/default/mbgl/gl/headless_backend.cpp
    PRIVATE platform/default/mbgl/gl/headless_backend.hpp
    PRIVATE platform/qt/src/headless_backend_qt.cpp

    # Thread pool
    PRIVATE platform/default/mbgl/util/shared_thread_pool.cpp
    PRIVATE platform/default/mbgl/util/shared_thread_pool.hpp
    PRIVATE platform/default/mbgl/util/default_thread_pool.cpp
    PRIVATE platform/default/mbgl/util/default_thread_pool.hpp

    # Thread
    PRIVATE platform/qt/src/thread_local.cpp

    # Platform integration
    PRIVATE platform/qt/src/async_task.cpp
    PRIVATE platform/qt/src/async_task_impl.hpp
    PRIVATE platform/qt/src/qt_logging.cpp
    PRIVATE platform/qt/src/qt_image.cpp
    PRIVATE platform/qt/src/run_loop.cpp
    PRIVATE platform/qt/src/run_loop_impl.hpp
    PRIVATE platform/qt/src/string_stdlib.cpp
    PRIVATE platform/qt/src/timer.cpp
    PRIVATE platform/qt/src/timer_impl.hpp
    PRIVATE platform/qt/src/utf.cpp
)

set(MBGL_QT_FILESOURCE_FILES
    # File source
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
    platform/qt/src/qmapboxgl_renderer_frontend_p.hpp
    platform/qt/src/qmapboxgl_renderer_frontend_p.cpp
    platform/default/mbgl/util/default_styles.hpp
)

target_include_directories(qmapboxgl
    PUBLIC platform/qt/include
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

if(WITH_QT_4)
    include(platform/qt/qt4.cmake)
else()
    include(platform/qt/qt5.cmake)
endif()

xcode_create_scheme(TARGET mbgl-qt)

# OS specific configurations
if (MASON_PLATFORM STREQUAL "osx" OR MASON_PLATFORM STREQUAL "ios")
    list(APPEND MBGL_QT_CORE_FILES
        PRIVATE platform/darwin/src/nsthread.mm
    )
    list(APPEND MBGL_QT_CORE_LIBRARIES
        PRIVATE "-framework Foundation"
    )
    if(WITH_QT_4)
        list(APPEND MBGL_QT_CORE_LIBRARIES
            PRIVATE "-framework OpenGL"
        )
    endif()
elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND MBGL_QT_CORE_FILES
        PRIVATE platform/default/thread.cpp
    )
    if(WITH_QT_4)
        list(APPEND MBGL_QT_CORE_LIBRARIES
            PRIVATE "-lGL"
        )
    endif()
elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND MBGL_QT_CORE_FILES
        PRIVATE platform/qt/src/thread.cpp
    )
endif()

add_custom_command(
    TARGET qmapboxgl
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/platform/qt/include
            ${CMAKE_CURRENT_BINARY_DIR}/platform/qt/include
)

xcode_create_scheme(TARGET qmapboxgl)
