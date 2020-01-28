target_compile_definitions(
    mbgl-core
    PUBLIC MBGL_USE_GLES2 GLES_SILENCE_DEPRECATION
)

if(NOT DEFINED IOS_DEPLOYMENT_TARGET)
    set(IOS_DEPLOYMENT_TARGET "9.0")
endif()

macro(initialize_ios_target target)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "YES")
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE bitcode)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)
endmacro()

set_target_properties(mbgl-core PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)

target_sources(
    mbgl-core
    PRIVATE
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/async_task.cpp
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/collator.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/gl_functions.cpp
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/headless_backend_eagl.mm
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
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gl/headless_backend.cpp
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
        "-framework GLKit"
        "-framework ImageIO"
        "-framework OpenGLES"
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
    set(CMAKE_OSX_ARCHITECTURES "armv7;i386;x86_64;arm64")
    set(PREPARE_CMD "${PROJECT_SOURCE_DIR}/render-test/ios/setup_test_data.sh")
    message("COMMAND: ${PREPARE_CMD}")
    execute_process(COMMAND ${PREPARE_CMD} RESULT_VARIABLE CMD_ERROR WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    message(STATUS "CMD_ERROR:" ${CMD_ERROR})

    set(RESOURCES ${PROJECT_SOURCE_DIR}/render-test/ios/Main.storyboard ${PROJECT_SOURCE_DIR}/render-test/ios/LaunchScreen.storyboard
                  ${PROJECT_SOURCE_DIR}/test-data)

    add_executable(
        RenderTestApp
        ${PROJECT_SOURCE_DIR}/render-test/ios/ios_test_runner.hpp
        ${PROJECT_SOURCE_DIR}/render-test/ios/ios_test_runner.cpp
        ${PROJECT_SOURCE_DIR}/render-test/ios/AppDelegate.h
        ${PROJECT_SOURCE_DIR}/render-test/ios/AppDelegate.m
        ${PROJECT_SOURCE_DIR}/render-test/ios/ViewController.h
        ${PROJECT_SOURCE_DIR}/render-test/ios/ViewController.m
        ${PROJECT_SOURCE_DIR}/render-test/ios/iosTestRunner.h
        ${PROJECT_SOURCE_DIR}/render-test/ios/iosTestRunner.mm
        ${PROJECT_SOURCE_DIR}/render-test/ios/main.m
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
    )

    find_package(XCTest REQUIRED)

    xctest_add_bundle(RenderTestAppTests RenderTestApp ${PROJECT_SOURCE_DIR}/render-test/ios/tests/Tests.m)

    set_target_properties(
        RenderTestAppTests
        PROPERTIES
            XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET
            "${IOS_DEPLOYMENT_TARGET}"
            XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH
            $<$<CONFIG:Debug>:YES>
    )

    target_include_directories(
        RenderTestAppTests
        PUBLIC ${PROJECT_SOURCE_DIR}/render-test/ios
    )

    xctest_add_test(XCTest.RenderTestApp RenderTestAppTests)

    set_target_properties(RenderTestAppTests PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/render-test/ios/tests/Info.plist)
endif()

unset(IOS_DEPLOYMENT_TARGET CACHE)
