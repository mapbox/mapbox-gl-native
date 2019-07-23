add_executable(mbgl-render-test
    ${CMAKE_CURRENT_LIST_DIR}/allocation_index.cpp
    ${CMAKE_CURRENT_LIST_DIR}/allocation_index.hpp
    ${CMAKE_CURRENT_LIST_DIR}/filesystem.hpp
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/metadata.hpp
    ${CMAKE_CURRENT_LIST_DIR}/parser.cpp
    ${CMAKE_CURRENT_LIST_DIR}/parser.hpp
    ${CMAKE_CURRENT_LIST_DIR}/runner.cpp
    ${CMAKE_CURRENT_LIST_DIR}/runner.hpp
)

target_compile_definitions(mbgl-render-test PRIVATE
    TEST_RUNNER_ROOT_PATH="${PROJECT_SOURCE_DIR}"
)

# FIXME: Should not use core private interface
target_include_directories(mbgl-render-test PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

include(${PROJECT_SOURCE_DIR}/vendor/boost.cmake)

target_link_libraries(mbgl-render-test PRIVATE
    Mapbox::Base::Extras::args
    Mapbox::Base::Extras::filesystem
    Mapbox::Base::pixelmatch-cpp
    mbgl-core
    mbgl-vendor-boost
)

set_property(TARGET mbgl-render-test PROPERTY FOLDER Executables)

string(RANDOM LENGTH 5 ALPHABET 0123456789 MBGL_RENDER_TEST_SEED)
add_test(NAME mbgl-render-test COMMAND mbgl-render-test --recycle-map --shuffle --seed ${MBGL_RENDER_TEST_SEED})
