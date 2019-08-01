# scripts/generate-file-lists.js to change the source files for this target.
load_sources_list(MBGL_TEST_FILES test/test-files.json)
if (MBGL_TEST_TARGET_TYPE STREQUAL "library")
    add_library(mbgl-test SHARED ${MBGL_TEST_FILES})
else()
    add_executable(mbgl-test ${MBGL_TEST_FILES})
endif()

# GCC 8+ trips over GTest's way of defining Test functions
target_compile_options(mbgl-test PRIVATE -Wno-shadow)

if(NOT WITH_NODEJS)
    target_compile_definitions(mbgl-test PRIVATE "-DTEST_HAS_SERVER=0")
endif()

set_source_files_properties(test/src/mbgl/test/util.cpp PROPERTIES COMPILE_FLAGS -DNODE_EXECUTABLE="${NodeJS_EXECUTABLE}")

target_include_directories(mbgl-test
    PRIVATE src
    PRIVATE test/include
    PRIVATE test/src
    PRIVATE platform/default/include
)

target_link_libraries(mbgl-test PRIVATE
    googletest
    args
    mbgl-core
    shelf-pack-cpp
    unique_resource
    pixelmatch-cpp
)

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
