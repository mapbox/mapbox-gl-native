add_library(
    mbgl-render-test STATIC EXCLUDE_FROM_ALL
    ${PROJECT_SOURCE_DIR}/expression-test/test_runner_common.cpp
    ${PROJECT_SOURCE_DIR}/expression-test/test_runner_common.hpp
    ${PROJECT_SOURCE_DIR}/render-test/allocation_index.cpp
    ${PROJECT_SOURCE_DIR}/render-test/allocation_index.hpp
    ${PROJECT_SOURCE_DIR}/render-test/file_source.cpp
    ${PROJECT_SOURCE_DIR}/render-test/file_source.hpp
    ${PROJECT_SOURCE_DIR}/render-test/filesystem.hpp
    ${PROJECT_SOURCE_DIR}/render-test/include/mbgl/render_test.hpp
    ${PROJECT_SOURCE_DIR}/render-test/manifest_parser.cpp
    ${PROJECT_SOURCE_DIR}/render-test/manifest_parser.hpp
    ${PROJECT_SOURCE_DIR}/render-test/metadata.hpp
    ${PROJECT_SOURCE_DIR}/render-test/parser.cpp
    ${PROJECT_SOURCE_DIR}/render-test/parser.hpp
    ${PROJECT_SOURCE_DIR}/render-test/render_test.cpp
    ${PROJECT_SOURCE_DIR}/render-test/runner.cpp
    ${PROJECT_SOURCE_DIR}/render-test/runner.hpp
)

if(CMAKE_SYSTEM_NAME STREQUAL iOS)
    set_target_properties(mbgl-render-test PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
    set_target_properties(mbgl-render-test PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "YES")
    set_target_properties(mbgl-render-test PROPERTIES XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE bitcode)
    set_target_properties(mbgl-render-test PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)
endif()

# FIXME: Should not use core private interface
target_include_directories(
    mbgl-render-test
    PRIVATE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/platform/default/include
)

target_include_directories(
    mbgl-render-test
    PUBLIC ${PROJECT_SOURCE_DIR}/render-test/include ${PROJECT_SOURCE_DIR}/include
)

include(${PROJECT_SOURCE_DIR}/vendor/boost.cmake)

target_link_libraries(
    mbgl-render-test
    PRIVATE
        Mapbox::Base::Extras::args
        Mapbox::Base::Extras::filesystem
        Mapbox::Base::pixelmatch-cpp
        mbgl-compiler-options
        mbgl-vendor-boost
    PUBLIC mbgl-core
)

set_property(TARGET mbgl-render-test PROPERTY FOLDER Core)
