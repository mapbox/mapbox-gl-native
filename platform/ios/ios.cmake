if(NOT DEFINED IOS_DEPLOYMENT_TARGET)
    set(IOS_DEPLOYMENT_TARGET "12.0")
endif()

macro(initialize_ios_target target)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "YES")
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE bitcode)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)
endmacro()

set_target_properties(mbgl-core PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)

if(MBGL_WITH_OPENGL)
    target_compile_definitions(
        mbgl-core
        PUBLIC MBGL_USE_GLES2 GLES_SILENCE_DEPRECATION
    )
    target_sources(
        mbgl-core
        PRIVATE
            ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gl/headless_backend.cpp
            ${PROJECT_SOURCE_DIR}/platform/darwin/src/gl_functions.cpp ${PROJECT_SOURCE_DIR}/platform/darwin/src/headless_backend_eagl.mm
    )
    target_link_libraries(
        mbgl-core
        PRIVATE "-framework GLKit" "-framework OpenGLES"
    )
endif()

target_sources(
    mbgl-core
    PRIVATE
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/async_task.cpp
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/collator.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/native_apple_interface.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/http_file_source.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/native_apple_interface.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/image.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/local_glyph_rasterizer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/logging_nslog.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/nsthread.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/number_format.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/reachability.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/run_loop.cpp
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/string_nsstring.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/timer.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_backend.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_frontend.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/map/map_snapshotter.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/asset_file_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/database_file_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/file_source_manager.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/file_source_request.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/local_file_request.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/local_file_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/main_resource_loader.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline_database.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline_download.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/online_file_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/sqlite3.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/bidi.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/compression.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/monotonic_timer.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/png_writer.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/thread_local.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/utf.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/thread_local.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/layermanager/layer_manager.cpp
)

target_include_directories(
    mbgl-core
    PRIVATE ${PROJECT_SOURCE_DIR}/platform/darwin/include ${PROJECT_SOURCE_DIR}/platform/default/include
)

include(${PROJECT_SOURCE_DIR}/vendor/icu.cmake)
include(${PROJECT_SOURCE_DIR}/platform/ios/ccache.cmake)

initialize_ios_target(mbgl-core)
initialize_ios_target(mbgl-vendor-icu)

target_link_libraries(
    mbgl-core
    PRIVATE
        "-framework CoreGraphics"
        "-framework CoreImage"
        "-framework CoreLocation"
        "-framework CoreServices"
        "-framework CoreText"
        "-framework Foundation"
        "-framework ImageIO"
        "-framework QuartzCore"
        "-framework Security"
        "-framework SystemConfiguration"
        "-framework UIKit"
        "-framework WebKit"
        mbgl-vendor-icu
        sqlite3
        z
)

if(MBGL_IOS_RENDER_TEST)
    include(${PROJECT_SOURCE_DIR}/vendor/zip-archive.cmake)
    initialize_ios_target(mbgl-vendor-zip-archive)

    set(PREPARE_CMD "${PROJECT_SOURCE_DIR}/render-test/ios/setup_test_data.sh")
    execute_process(COMMAND ${PREPARE_CMD} RESULT_VARIABLE CMD_ERROR WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

    set(RESOURCES ${PROJECT_SOURCE_DIR}/platform/ios/test/common/Main.storyboard
                  ${PROJECT_SOURCE_DIR}/platform/ios/test/common/LaunchScreen.storyboard ${PROJECT_SOURCE_DIR}/test-data)

    add_executable(
        RenderTestApp
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/ios_test_runner.hpp
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/AppDelegate.h
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/AppDelegate.m
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/main.m
        ${PROJECT_SOURCE_DIR}/render-test/ios/ios_test_runner.cpp
        ${PROJECT_SOURCE_DIR}/render-test/ios/iosTestRunner.h
        ${PROJECT_SOURCE_DIR}/render-test/ios/iosTestRunner.mm
        ${RESOURCES}
    )
    initialize_ios_target(RenderTestApp)

    set_target_properties(
        RenderTestApp
        PROPERTIES
            MACOSX_BUNDLE
            TRUE
            MACOSX_BUNDLE_IDENTIFIER
            com.mapbox.RenderTestAPP
            MACOSX_BUNDLE_INFO_PLIST
            ${PROJECT_SOURCE_DIR}/render-test/ios/Info.plist
            RESOURCE
            "${RESOURCES}"
    )

    target_include_directories(
        RenderTestApp
        PUBLIC {PROJECT_SOURCE_DIR}/render-test/include ${PROJECT_SOURCE_DIR}/include
    )

    target_include_directories(
        RenderTestApp
        PRIVATE
            ${PROJECT_SOURCE_DIR}/platform/darwin/src
            ${PROJECT_SOURCE_DIR}/platform/darwin/include
            ${PROJECT_SOURCE_DIR}/platform/darwin/include/mbgl/interface/
            ${PROJECT_SOURCE_DIR}/platform/default/include
            ${PROJECT_SOURCE_DIR}/platform/ios/test/common
            ${PROJECT_SOURCE_DIR}/src
    )

    target_include_directories(
        RenderTestApp
        PUBLIC ${PROJECT_SOURCE_DIR}/render-test/ios
    )

    target_link_libraries(
        RenderTestApp
        PRIVATE
            "-framework CoreGraphics"
            "-framework CoreLocation"
            "-framework Foundation"
            "-framework OpenGLES"
            "-framework QuartzCore"
            "-framework UIKit"
            mbgl-render-test
            mbgl-vendor-zip-archive
    )

    set_target_properties(RenderTestApp PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
    set_target_properties(RenderTestApp PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")

    find_package(XCTest REQUIRED)

    xctest_add_bundle(RenderTestAppTests RenderTestApp ${PROJECT_SOURCE_DIR}/render-test/ios/tests/Tests.m)

    target_include_directories(
        RenderTestAppTests
        PUBLIC ${PROJECT_SOURCE_DIR}/render-test/ios
    )

    xctest_add_test(XCTest.RenderTestApp RenderTestAppTests)

    set_target_properties(RenderTestAppTests PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
    set_target_properties(RenderTestAppTests PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)
    set_target_properties(RenderTestAppTests PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/render-test/ios/tests/Info.plist)
    set_target_properties(RenderTestAppTests PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
    set_target_properties(RenderTestAppTests PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
endif()

if(MBGL_IOS_UNIT_TEST)
    execute_process(COMMAND ditto ${PROJECT_SOURCE_DIR}/test/fixtures ${CMAKE_CURRENT_BINARY_DIR}/test-data/test/fixtures)
    execute_process(
        COMMAND
            ditto ${PROJECT_SOURCE_DIR}/mapbox-gl-js/src/style-spec/reference
            ${CMAKE_CURRENT_BINARY_DIR}/test-data/mapbox-gl-js/src/style-spec/reference
    )

    set(RESOURCES ${PROJECT_SOURCE_DIR}/platform/ios/test/common/Main.storyboard
                  ${PROJECT_SOURCE_DIR}/platform/ios/test/common/LaunchScreen.storyboard ${CMAKE_CURRENT_BINARY_DIR}/test-data)

    add_executable(
        UnitTestsApp
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/ios_test_runner.hpp
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/AppDelegate.h
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/AppDelegate.m
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/iosTestRunner.mm
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/main.m
        ${PROJECT_SOURCE_DIR}/test/ios/ios_test_runner.cpp
        ${PROJECT_SOURCE_DIR}/test/ios/iosTestRunner.h
        ${RESOURCES}
    )
    initialize_ios_target(UnitTestsApp)

    set_target_properties(
        UnitTestsApp
        PROPERTIES
            MACOSX_BUNDLE
            TRUE
            MACOSX_BUNDLE_IDENTIFIER
            com.mapbox.UnitTestsApp
            MACOSX_BUNDLE_INFO_PLIST
            ${PROJECT_SOURCE_DIR}/test/ios/Info.plist
            RESOURCE
            "${RESOURCES}"
    )

    target_include_directories(
        UnitTestsApp
        PUBLIC
            ${PROJECT_SOURCE_DIR}/include
            ${PROJECT_SOURCE_DIR}/platform/ios/test/common
            ${PROJECT_SOURCE_DIR}/test/include
            ${PROJECT_SOURCE_DIR}/test/ios
    )

    target_link_libraries(
        UnitTestsApp
        PRIVATE
            "-framework CoreGraphics"
            "-framework CoreLocation"
            "-framework Foundation"
            "-framework OpenGLES"
            "-framework QuartzCore"
            "-framework UIKit"
            mbgl-compiler-options
            -Wl,-force_load
            mbgl-test
    )

    set_target_properties(UnitTestsApp PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
    set_target_properties(UnitTestsApp PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")

    find_package(XCTest REQUIRED)

    xctest_add_bundle(UnitTestsAppTests UnitTestsApp ${PROJECT_SOURCE_DIR}/platform/ios/test/common/Tests.m)

    target_include_directories(
        UnitTestsAppTests
        PUBLIC ${PROJECT_SOURCE_DIR}/test/ios
    )

    xctest_add_test(XCTest.UnitTestsApp UnitTestsAppTests)

    set_target_properties(UnitTestsAppTests PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
    set_target_properties(UnitTestsAppTests PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)
    set_target_properties(UnitTestsAppTests PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/test/ios/tests/Info.plist)
    set_target_properties(UnitTestsAppTests PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
    set_target_properties(UnitTestsAppTests PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
endif()

if(MBGL_IOS_BENCHMARK)
    execute_process(COMMAND ditto ${PROJECT_SOURCE_DIR}/benchmark/fixtures ${CMAKE_CURRENT_BINARY_DIR}/benchmark-data/benchmark/fixtures)
    execute_process(
        COMMAND
            ditto ${PROJECT_SOURCE_DIR}/test/fixtures/api/assets/streets
            ${CMAKE_CURRENT_BINARY_DIR}/benchmark-data/test/fixtures/api/assets/streets
    )

    set(RESOURCES ${PROJECT_SOURCE_DIR}/platform/ios/test/common/Main.storyboard
                  ${PROJECT_SOURCE_DIR}/platform/ios/test/common/LaunchScreen.storyboard ${CMAKE_CURRENT_BINARY_DIR}/benchmark-data)

    add_executable(
        BenchmarkApp
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/ios_test_runner.hpp
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/AppDelegate.h
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/AppDelegate.m
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/iosTestRunner.mm
        ${PROJECT_SOURCE_DIR}/platform/ios/test/common/main.m
        ${PROJECT_SOURCE_DIR}/benchmark/ios/ios_test_runner.cpp
        ${PROJECT_SOURCE_DIR}/benchmark/ios/iosTestRunner.h
        ${RESOURCES}
    )
    initialize_ios_target(BenchmarkApp)

    target_include_directories(
        BenchmarkApp
        PUBLIC
            ${PROJECT_SOURCE_DIR}/include
            ${PROJECT_SOURCE_DIR}/platform/ios/test/common
            ${PROJECT_SOURCE_DIR}/benchmark/include
            ${PROJECT_SOURCE_DIR}/benchmark/ios
    )

    target_link_libraries(
        BenchmarkApp
        PRIVATE
            "-framework CoreGraphics"
            "-framework CoreLocation"
            "-framework Foundation"
            "-framework OpenGLES"
            "-framework QuartzCore"
            "-framework UIKit"
            mbgl-compiler-options
            -Wl,-force_load
            mbgl-benchmark
    )

    set_target_properties(BenchmarkApp PROPERTIES MACOSX_BUNDLE TRUE)
    set_target_properties(BenchmarkApp PROPERTIES MACOSX_BUNDLE_IDENTIFIER com.mapbox.BenchmarkApp)
    set_target_properties(BenchmarkApp PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/benchmark/ios/Info.plist)
    set_target_properties(BenchmarkApp PROPERTIES RESOURCE "${RESOURCES}")
    set_target_properties(BenchmarkApp PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
    set_target_properties(BenchmarkApp PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")

    find_package(XCTest REQUIRED)

    xctest_add_bundle(BenchmarkAppTests BenchmarkApp ${PROJECT_SOURCE_DIR}/platform/ios/test/common/Tests.m)

    target_include_directories(
        BenchmarkAppTests
        PUBLIC ${PROJECT_SOURCE_DIR}/benchmark/ios
    )

    xctest_add_test(XCTest.BenchmarkApp BenchmarkAppTests)

    set_target_properties(BenchmarkAppTests PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
    set_target_properties(BenchmarkAppTests PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)
    set_target_properties(BenchmarkAppTests PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/benchmark/ios/tests/Info.plist)
    set_target_properties(BenchmarkAppTests PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")
    set_target_properties(BenchmarkAppTests PROPERTIES XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
endif()

unset(IOS_DEPLOYMENT_TARGET CACHE)
