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
