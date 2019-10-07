add_executable(mbgl-expression-test
    expression-test/main.cpp
    expression-test/expression_test_parser.cpp
    expression-test/expression_test_runner.cpp
    expression-test/expression_test_logger.cpp
    expression-test/test_runner_common.cpp
    expression-test/test_runner_common.hpp
)

if(APPLE)
    target_link_libraries(mbgl-expression-test PRIVATE mbgl-loop-darwin)
else()
    target_link_libraries(mbgl-expression-test PRIVATE mbgl-loop-uv)
endif()

target_include_directories(mbgl-expression-test
    PRIVATE src
    PRIVATE expression-test
    PRIVATE render-test
)

target_link_libraries(mbgl-expression-test PRIVATE
    mbgl-core
    Mapbox::Base::Extras::args
    Mapbox::Base::Extras::filesystem
    Mapbox::Base::Extras::rapidjson
)

add_definitions(-DTEST_RUNNER_ROOT_PATH="${CMAKE_SOURCE_DIR}")
