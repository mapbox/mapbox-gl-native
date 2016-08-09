add_executable(mbgl-offline
    bin/offline.cpp
)

target_compile_options(mbgl-offline
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-offline
    PRIVATE include
    PRIVATE src # TODO: eliminate
)

target_link_libraries(mbgl-offline
    PRIVATE mbgl-core
)

target_add_mason_package(mbgl-offline PRIVATE boost)
target_add_mason_package(mbgl-offline PRIVATE boost_libprogram_options)

mbgl_platform_offline()
