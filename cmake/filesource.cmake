add_library(mbgl-filesource STATIC
    # File source
    include/mbgl/storage/default_file_source.hpp
    platform/default/default_file_source.cpp
    platform/default/mbgl/storage/file_source_request.hpp
    platform/default/file_source_request.cpp
    include/mbgl/storage/online_file_source.hpp
    platform/default/online_file_source.cpp
    src/mbgl/storage/http_file_source.hpp
    src/mbgl/storage/asset_file_source.hpp
    platform/default/asset_file_source.cpp
    src/mbgl/storage/local_file_source.hpp
    platform/default/local_file_source.cpp

    # Offline
    include/mbgl/storage/offline.hpp
    platform/default/mbgl/storage/offline.cpp
    platform/default/mbgl/storage/offline_database.hpp
    platform/default/mbgl/storage/offline_database.cpp
    platform/default/mbgl/storage/offline_download.hpp
    platform/default/mbgl/storage/offline_download.cpp

    # Database
    platform/default/sqlite3.hpp
)

target_add_mason_package(mbgl-filesource PUBLIC geometry)
target_add_mason_package(mbgl-filesource PUBLIC variant)
target_add_mason_package(mbgl-filesource PRIVATE rapidjson)
target_add_mason_package(mbgl-filesource PRIVATE boost)
target_add_mason_package(mbgl-filesource PRIVATE geojson)

target_include_directories(mbgl-filesource
    PRIVATE include
    PRIVATE src
    PRIVATE platform/default
)

target_link_libraries(mbgl-filesource
    PUBLIC mbgl-core
)

mbgl_filesource()

create_source_groups(mbgl-filesource)

xcode_create_scheme(TARGET mbgl-filesource)

initialize_xcode_cxx_build_settings(mbgl-filesource)
