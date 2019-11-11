add_executable(
    mbgl-render-test
    expression-test/test_runner_common.cpp
    expression-test/test_runner_common.hpp
    platform/default/src/mbgl/render-test/main.cpp
    render-test/file_source.cpp
    render-test/file_source.hpp
    render-test/allocation_index.cpp
    render-test/allocation_index.hpp
    render-test/filesystem.hpp
    render-test/filesystem.hpp
    render-test/include/mbgl/render_test.hpp
    render-test/manifest_parser.cpp
    render-test/manifest_parser.hpp
    render-test/metadata.hpp
    render-test/parser.cpp
    render-test/parser.hpp
    render-test/render_test.cpp
    render-test/runner.cpp
    render-test/runner.hpp
)

if(APPLE)
    target_link_libraries(
        mbgl-render-test
        PRIVATE mbgl-loop-darwin
    )
else()
    target_link_libraries(
        mbgl-render-test
        PRIVATE mbgl-loop-uv
    )
endif()

target_include_directories(
    mbgl-render-test
    PRIVATE src
    PRIVATE platform/default/include
    PRIVATE render-test
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
        Mapbox::Base::Extras::expected-lite
        Mapbox::Base::Extras::filesystem
        Mapbox::Base::pixelmatch-cpp
        Mapbox::Base::Extras::rapidjson
)

create_source_groups(mbgl-render-test)

set_target_properties(mbgl-render-test PROPERTIES FOLDER "Executables")

add_definitions(-DTEST_RUNNER_ROOT_PATH="${CMAKE_SOURCE_DIR}")
