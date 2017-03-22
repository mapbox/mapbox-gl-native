if (MBGL_TEST_TARGET_TYPE STREQUAL "library")
    add_library(mbgl-test SHARED
        ${MBGL_TEST_FILES}
    )
else()
    add_executable(mbgl-test
        ${MBGL_TEST_FILES}
    )
endif()

target_compile_options(mbgl-test
    PRIVATE -fvisibility-inlines-hidden
)

set_source_files_properties(test/src/mbgl/test/util.cpp PROPERTIES COMPILE_FLAGS -DNODE_EXECUTABLE="${NodeJS_EXECUTABLE}")

target_include_directories(mbgl-test
    PRIVATE src
    PRIVATE test/include
    PRIVATE test/src
    PRIVATE platform/default
)

target_link_libraries(mbgl-test
    PRIVATE mbgl-core
)

target_add_mason_package(mbgl-test PRIVATE geometry)
target_add_mason_package(mbgl-test PRIVATE variant)
target_add_mason_package(mbgl-test PRIVATE unique_resource)
target_add_mason_package(mbgl-test PRIVATE rapidjson)
target_add_mason_package(mbgl-test PRIVATE gtest)
target_add_mason_package(mbgl-test PRIVATE pixelmatch)
target_add_mason_package(mbgl-test PRIVATE boost)
target_add_mason_package(mbgl-test PRIVATE geojson)
target_add_mason_package(mbgl-test PRIVATE geojsonvt)

mbgl_platform_test()

create_source_groups(mbgl-test)
