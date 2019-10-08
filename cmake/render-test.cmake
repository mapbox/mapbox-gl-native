add_executable(mbgl-render-test
    render-test/src/mbgl/allocation_index.cpp
    render-test/src/mbgl/render_test_runner.cpp
    render-test/src/mbgl/parser.cpp
    render-test/src/mbgl/runner.cpp
    expression-test/test_runner_common.cpp
    expression-test/test_runner_common.hpp
)

target_include_directories(mbgl-render-test
    PRIVATE src
    PRIVATE platform/default/include
    PRIVATE render-test/include
    PRIVATE render-test/src
)

target_link_libraries(mbgl-render-test PRIVATE
    mbgl-core
    mbgl-filesource
    Mapbox::Base::Extras::args
    mbgl-vendor-expected
    Mapbox::Base::Extras::filesystem
    Mapbox::Base::pixelmatch-cpp
    Mapbox::Base::Extras::rapidjson
)

mbgl_platform_test()

set_target_properties(mbgl-render-test PROPERTIES FOLDER "Executables")

add_definitions(-DTEST_RUNNER_ROOT_PATH="${CMAKE_SOURCE_DIR}")
