add_executable(
    mbgl-expression-test
    ${PROJECT_SOURCE_DIR}/expression-test/expression_test_logger.cpp
    ${PROJECT_SOURCE_DIR}/expression-test/expression_test_logger.hpp
    ${PROJECT_SOURCE_DIR}/expression-test/expression_test_parser.cpp
    ${PROJECT_SOURCE_DIR}/expression-test/expression_test_parser.hpp
    ${PROJECT_SOURCE_DIR}/expression-test/expression_test_runner.cpp
    ${PROJECT_SOURCE_DIR}/expression-test/expression_test_runner.hpp
    ${PROJECT_SOURCE_DIR}/expression-test/test_runner_common.cpp
    ${PROJECT_SOURCE_DIR}/expression-test/test_runner_common.hpp
    ${PROJECT_SOURCE_DIR}/expression-test/main.cpp
)

target_compile_definitions(
    mbgl-expression-test
    PRIVATE TEST_RUNNER_ROOT_PATH="${PROJECT_SOURCE_DIR}"
)

# FIXME: Should not use core private interface
target_include_directories(
    mbgl-expression-test
    PRIVATE ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    mbgl-expression-test
    PRIVATE
        Mapbox::Base
        Mapbox::Base::Extras::args
        Mapbox::Base::Extras::filesystem
        mbgl-core
)

set_property(TARGET mbgl-expression-test PROPERTY FOLDER Executables)

string(RANDOM LENGTH 5 ALPHABET 0123456789 MBGL_EXPRESSION_TEST_SEED)

add_test(
    NAME mbgl-expression-test
    COMMAND mbgl-expression-test -s --seed=${MBGL_EXPRESSION_TEST_SEED}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
