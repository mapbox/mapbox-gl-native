add_executable(mbgl-render-test
    render-test/main.cpp
    render-test/parser.cpp
    render-test/runner.cpp
)

if(APPLE)
    target_link_libraries(mbgl-render-test PRIVATE mbgl-loop-darwin)
else()
    target_link_libraries(mbgl-render-test PRIVATE mbgl-loop-uv)
endif()

target_include_directories(mbgl-render-test
    PRIVATE src
    PRIVATE platform/default/include
    PRIVATE render-test
)

target_link_libraries(mbgl-render-test PRIVATE
    mbgl-core
    mbgl-filesource
    args
    expected
    filesystem
    pixelmatch-cpp
    rapidjson
)

add_definitions(-DTEST_RUNNER_ROOT_PATH="${CMAKE_SOURCE_DIR}")
