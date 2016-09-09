mason_use(libjpeg-turbo VERSION 1.4.2)
mason_use(libpng VERSION 1.6.20)
mason_use(libuv VERSION 1.7.5)
mason_use(nunicode VERSION 1.6)
mason_use(sqlite VERSION 3.9.1)
mason_use(webp VERSION 0.5.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fvisibility=hidden")

include(cmake/loop-uv.cmake)

macro(mbgl_platform_core)
    target_sources(mbgl-core
        # File source
        PRIVATE platform/default/asset_file_source.cpp
        PRIVATE platform/default/default_file_source.cpp
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
        PRIVATE platform/default/string_stdlib.cpp

        # Image handling
        PRIVATE platform/default/image.cpp
        PRIVATE platform/default/jpeg_reader.cpp
        PRIVATE platform/default/png_reader.cpp
        PRIVATE platform/default/webp_reader.cpp
    )

    target_include_directories(mbgl-core
        PRIVATE platform/default
    )

    target_add_mason_package(mbgl-core PRIVATE libjpeg-turbo)
    target_add_mason_package(mbgl-core PRIVATE libpng)
    target_add_mason_package(mbgl-core PRIVATE nunicode)
    target_add_mason_package(mbgl-core PRIVATE sqlite)
    target_add_mason_package(mbgl-core PRIVATE webp)
endmacro()

add_library(unity MODULE
    platform/unity/src/log_file.cpp
    platform/unity/src/plugin.cpp
)

target_include_directories(unity
    PRIVATE platform/unity/include
    PRIVATE ${MBGL_GENERATED}/include
)

target_link_libraries(unity
    PRIVATE mbgl-core
    PRIVATE mbgl-loop
)

set_target_properties(unity PROPERTIES
    OUTPUT_NAME "RenderingPlugin"
)

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    mason_use(libcrypto VERSION 1.0.1p)
    mason_use(libcurl VERSION 7.45.0)

    target_add_mason_package(unity PRIVATE libcrypto)
    target_add_mason_package(unity PRIVATE libcurl)

    set_target_properties(unity PROPERTIES
        LINK_FLAGS -Wl,--version-script=${CMAKE_SOURCE_DIR}/platform/unity/unity.version
    )

    target_sources(unity
        PRIVATE platform/default/http_file_source.cpp
        PRIVATE platform/default/thread.cpp
    )
elseif (CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(unity
        PRIVATE "-framework Foundation"
        PRIVATE "-framework OpenGL"
    )

    target_sources(unity
        PRIVATE platform/darwin/src/http_file_source.mm
        PRIVATE platform/darwin/src/nsthread.mm
    )

    target_compile_options(unity
        PRIVATE -fobjc-arc
    )
else()
    message(FATAL_ERROR "Target platform not supported.")
endif()
