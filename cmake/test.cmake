add_vendor_target(gtest STATIC)

# Modify cmake/test-files.txt to change the source files for this target.
load_sources_list(MBGL_TEST_FILES cmake/test-files.txt)
if (MBGL_TEST_TARGET_TYPE STREQUAL "library")
    add_library(mbgl-test SHARED ${MBGL_TEST_FILES})
else()
    add_executable(mbgl-test ${MBGL_TEST_FILES})
endif()


if(NOT WITH_NODEJS)
    target_compile_definitions(mbgl-test PRIVATE "-DTEST_HAS_SERVER=0")
endif()

set_source_files_properties(test/src/mbgl/test/util.cpp PROPERTIES COMPILE_FLAGS -DNODE_EXECUTABLE="${NodeJS_EXECUTABLE}")

target_include_directories(mbgl-test
    PRIVATE src
    PRIVATE test/include
    PRIVATE test/src
    PRIVATE platform/default
)

target_link_libraries(mbgl-test
    PRIVATE mbgl-core
    PRIVATE gtest
)

target_add_mason_package(mbgl-test PRIVATE geometry)
target_add_mason_package(mbgl-test PRIVATE variant)
target_add_mason_package(mbgl-test PRIVATE unique_resource)
target_add_mason_package(mbgl-test PRIVATE rapidjson)
target_add_mason_package(mbgl-test PRIVATE pixelmatch)
target_add_mason_package(mbgl-test PRIVATE boost)
target_add_mason_package(mbgl-test PRIVATE geojson)
target_add_mason_package(mbgl-test PRIVATE geojsonvt)
target_add_mason_package(mbgl-test PRIVATE shelf-pack)

mbgl_platform_test()

create_source_groups(mbgl-test)

initialize_xcode_cxx_build_settings(mbgl-test)

set_target_properties(mbgl-test PROPERTIES FOLDER "Executables")

xcode_create_scheme(
    TARGET mbgl-test
    OPTIONAL_ARGS
        "--gtest_filter=Category.*"
        "--gtest_repeat=0"
        "--gtest_shuffle=0"
        "--gtest_break_on_failure=0"
        "--gtest_throw_on_failure=0"
        "--gtest_catch_exceptions=0"
    )
