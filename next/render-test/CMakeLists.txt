
add_library(
    mbgl-render-test SHARED EXCLUDE_FROM_ALL
    ${MBGL_ROOT}/render-test/filesystem.hpp
    ${MBGL_ROOT}/render-test/src/mbgl/render_test_runner.cpp
    ${MBGL_ROOT}/render-test/src/test-runner/allocation_index.cpp
    ${MBGL_ROOT}/render-test/src/test-runner/allocation_index.hpp
    ${MBGL_ROOT}/render-test/src/test-runner/filesystem.hpp
    ${MBGL_ROOT}/render-test/src/test-runner/metadata.hpp
    ${MBGL_ROOT}/render-test/src/test-runner/parser.cpp
    ${MBGL_ROOT}/render-test/src/test-runner/parser.hpp
    ${MBGL_ROOT}/render-test/src/test-runner/runner.cpp
    ${MBGL_ROOT}/render-test/src/test-runner/runner.hpp
    ${MBGL_ROOT}/render-test/src/test-runner/test_runner.hpp
    ${MBGL_ROOT}/render-test/src/test-runner/test_runner.cpp
    ${MBGL_ROOT}/expression-test/test_runner_common.cpp
    ${MBGL_ROOT}/expression-test/test_runner_common.hpp
)

target_compile_definitions(
    mbgl-render-test
    PRIVATE TEST_RUNNER_ROOT_PATH="${MBGL_ROOT}"
)

# FIXME: Should not use core private interface
target_include_directories(
    mbgl-render-test
    PRIVATE 
        ${MBGL_ROOT}/src 
        ${MBGL_ROOT}/platform/default/include
        ${MBGL_ROOT}/platform/gfx/gl/src
        ${MBGL_ROOT}/render-test/src
)

target_include_directories(
    mbgl-render-test
    PUBLIC ${MBGL_ROOT}/render-test/include ${MBGL_ROOT}/include
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

if(CMAKE_SYSTEM_NAME STREQUAL Android)
    set_target_properties(mbgl-render-test PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=safe")
endif()

set_property(TARGET mbgl-render-test PROPERTY FOLDER Core)

