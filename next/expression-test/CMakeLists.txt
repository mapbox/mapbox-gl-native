add_executable(
    mbgl-expression-test
    ${MBGL_ROOT}/expression-test/expression_test_logger.cpp
    ${MBGL_ROOT}/expression-test/expression_test_logger.hpp
    ${MBGL_ROOT}/expression-test/expression_test_parser.cpp
    ${MBGL_ROOT}/expression-test/expression_test_parser.hpp
    ${MBGL_ROOT}/expression-test/expression_test_runner.cpp
    ${MBGL_ROOT}/expression-test/expression_test_runner.hpp
    ${MBGL_ROOT}/expression-test/test_runner_common.cpp
    ${MBGL_ROOT}/expression-test/test_runner_common.hpp
    ${MBGL_ROOT}/expression-test/main.cpp
)

target_compile_definitions(
    mbgl-expression-test
    PRIVATE TEST_RUNNER_ROOT_PATH="${MBGL_ROOT}"
)

# FIXME: Should not use core private interface
target_include_directories(
    mbgl-expression-test
    PRIVATE ${MBGL_ROOT}/src
)

target_link_libraries(
    mbgl-expression-test
    PRIVATE
        Mapbox::Base::Extras::args
        Mapbox::Base::Extras::filesystem
        Mapbox::Base::io
        mbgl-core
)

set_property(TARGET mbgl-expression-test PROPERTY FOLDER Executables)

string(RANDOM LENGTH 5 ALPHABET 0123456789 MBGL_EXPRESSION_TEST_SEED)

add_test(NAME mbgl-expression-test COMMAND mbgl-expression-test -s --seed=${MBGL_EXPRESSION_TEST_SEED} WORKING_DIRECTORY ${MBGL_ROOT})
