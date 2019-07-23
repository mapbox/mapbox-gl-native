add_executable(mbgl-expression-test
    ${CMAKE_CURRENT_LIST_DIR}/expression_test_logger.cpp
    ${CMAKE_CURRENT_LIST_DIR}/expression_test_logger.hpp
    ${CMAKE_CURRENT_LIST_DIR}/expression_test_parser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/expression_test_parser.hpp
    ${CMAKE_CURRENT_LIST_DIR}/expression_test_runner.cpp
    ${CMAKE_CURRENT_LIST_DIR}/expression_test_runner.hpp
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

target_compile_definitions(mbgl-expression-test PRIVATE
    TEST_RUNNER_ROOT_PATH="${PROJECT_SOURCE_DIR}"
)

## FIXME: Should not use core private interface
target_include_directories(mbgl-expression-test PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(mbgl-expression-test PRIVATE
    Mapbox::Base::Extras::args
    Mapbox::Base::Extras::filesystem
    Mapbox::Base::io
    mbgl-core
)

set_property(TARGET mbgl-expression-test PROPERTY FOLDER Executables)

string(RANDOM LENGTH 5 ALPHABET 0123456789 MBGL_EXPRESSION_TEST_SEED)
add_test(NAME mbgl-expression-test COMMAND mbgl-expression-test -s --seed=${MBGL_EXPRESSION_TEST_SEED})
