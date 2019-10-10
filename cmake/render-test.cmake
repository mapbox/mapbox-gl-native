add_executable(
    mbgl-render-test
    expression-test/test_runner_common.cpp
    expression-test/test_runner_common.hpp
    render-test/allocation_index.cpp
    render-test/allocation_index.hpp
    render-test/filesystem.hpp
    render-test/filesystem.hpp
    render-test/include/mbgl/render_test.hpp
    render-test/metadata.hpp
    render-test/parser.cpp
    render-test/parser.hpp
    render-test/render_test.cpp
    render-test/runner.cpp
    render-test/runner.hpp
)

target_include_directories(
    mbgl-render-test
    PRIVATE src
    PRIVATE platform/default/include
    PRIVATE render-test/src
)

target_include_directories(
    mbgl-render-test
    PUBLIC render-test/include
    PUBLIC include
)

target_link_libraries(
    mbgl-render-test
    PRIVATE
        mbgl-core
        mbgl-filesource
        Mapbox::Base::Extras::args
        mbgl-vendor-expected
        Mapbox::Base::Extras::filesystem
        Mapbox::Base::pixelmatch-cpp
        Mapbox::Base::Extras::rapidjson
)

mbgl_platform_render_test()

create_source_groups(mbgl-render-test)

set_target_properties(mbgl-render-test PROPERTIES FOLDER "Executables")

add_definitions(-DTEST_RUNNER_ROOT_PATH="${CMAKE_SOURCE_DIR}")
