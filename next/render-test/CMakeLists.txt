add_executable(
    mbgl-render-test
    ${MBGL_ROOT}/render-test/allocation_index.cpp
    ${MBGL_ROOT}/render-test/allocation_index.hpp
    ${MBGL_ROOT}/render-test/filesystem.hpp
    ${MBGL_ROOT}/render-test/main.cpp
    ${MBGL_ROOT}/render-test/metadata.hpp
    ${MBGL_ROOT}/render-test/parser.cpp
    ${MBGL_ROOT}/render-test/parser.hpp
    ${MBGL_ROOT}/render-test/runner.cpp
    ${MBGL_ROOT}/render-test/runner.hpp
)

target_compile_definitions(
    mbgl-render-test
    PRIVATE TEST_RUNNER_ROOT_PATH="${MBGL_ROOT}"
)

# FIXME: Should not use core private interface
target_include_directories(
    mbgl-render-test
    PRIVATE ${MBGL_ROOT}/src
)

include(${PROJECT_SOURCE_DIR}/vendor/boost.cmake)

target_link_libraries(
    mbgl-render-test
    PRIVATE
        Mapbox::Base::Extras::args
        Mapbox::Base::Extras::filesystem
        Mapbox::Base::pixelmatch-cpp
        mbgl-core
        mbgl-vendor-boost
)

set_property(TARGET mbgl-render-test PROPERTY FOLDER Executables)

string(RANDOM LENGTH 5 ALPHABET 0123456789 MBGL_RENDER_TEST_SEED)

add_test(
    NAME mbgl-render-test
    COMMAND
        mbgl-render-test
        render-tests
        --recycle-map
        --shuffle
        --seed
        ${MBGL_RENDER_TEST_SEED}
    WORKING_DIRECTORY ${MBGL_ROOT}
)

add_test(NAME mbgl-query-test COMMAND mbgl-render-test query-tests WORKING_DIRECTORY ${MBGL_ROOT})
