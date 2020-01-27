find_package(OpenGL REQUIRED)

target_compile_definitions(
    mbgl-core
    PUBLIC GL_SILENCE_DEPRECATION
)

target_sources(
    mbgl-core
    PRIVATE
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLAccountManager.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLAttributedExpression.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLAttributionInfo.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLBackgroundStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLCircleStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLClockDirectionFormatter.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLCompassDirectionFormatter.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLComputedShapeSource.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLCoordinateFormatter.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLDistanceFormatter.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFeature.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFillExtrusionStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFillStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLForegroundStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFoundation.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLGeometry.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLHeatmapStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLHillshadeStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLImageSource.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLight.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLineStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLoggingConfiguration.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLMapCamera.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLMapSnapshotter.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLMultiPoint.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLNetworkConfiguration.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOfflinePack.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOfflineStorage.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOpenGLStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPointAnnotation.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPointCollection.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPolygon.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPolyline.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRasterDEMSource.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRasterStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRasterTileSource.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRendererConfiguration.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLSDKMetricsManager.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShape.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShapeCollection.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShapeOfflineRegion.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShapeSource.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLSource.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyle.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyleLayerManager.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyleValue.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLSymbolStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLTilePyramidOfflineRegion.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLTileSource.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLTypes.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLVectorStyleLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLVectorTileSource.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSArray+MGLAdditions.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSBundle+MGLAdditions.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSCoder+MGLAdditions.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSComparisonPredicate+MGLAdditions.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSCompoundPredicate+MGLAdditions.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSDate+MGLAdditions.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSDictionary+MGLAdditions.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSExpression+MGLAdditions.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSPredicate+MGLAdditions.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSString+MGLAdditions.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSURL+MGLAdditions.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSValue+MGLAdditions.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSValue+MGLStyleAttributeAdditions.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/async_task.cpp
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/collator.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/gl_functions.cpp
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/headless_backend_cgl.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/http_file_source.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/image.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/local_glyph_rasterizer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/logging_nslog.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/nsthread.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/number_format.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/reachability.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/run_loop.cpp
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/string_nsstring.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/timer.cpp
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/native_apple_interface.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLNetworkIntegrationManager.m
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
        ${PROJECT_SOURCE_DIR}/platform/macos/src/MGLAnnotationImage.m
        ${PROJECT_SOURCE_DIR}/platform/macos/src/MGLAttributionButton.mm
        ${PROJECT_SOURCE_DIR}/platform/macos/src/MGLCompassCell.m
        ${PROJECT_SOURCE_DIR}/platform/macos/src/MGLMapView+IBAdditions.mm
        ${PROJECT_SOURCE_DIR}/platform/macos/src/MGLMapView+Impl.mm
        ${PROJECT_SOURCE_DIR}/platform/macos/src/MGLMapView+OpenGL.mm
        ${PROJECT_SOURCE_DIR}/platform/macos/src/MGLMapView.mm
        ${PROJECT_SOURCE_DIR}/platform/macos/src/MGLOpenGLLayer.mm
        ${PROJECT_SOURCE_DIR}/platform/macos/src/NSColor+MGLAdditions.mm
        ${PROJECT_SOURCE_DIR}/platform/macos/src/NSImage+MGLAdditions.mm
        ${PROJECT_SOURCE_DIR}/platform/macos/src/NSProcessInfo+MGLAdditions.m
)

set_target_properties(mbgl-core PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)

target_compile_options(mbgl-core PRIVATE -fobjc-arc)

# FIXME: Should not be needed, but now needed by node because of the headless frontend.
target_include_directories(
    mbgl-core
    PUBLIC ${PROJECT_SOURCE_DIR}/platform/default/include
)

target_include_directories(
    mbgl-core
    PRIVATE
        ${PROJECT_SOURCE_DIR}/platform/darwin/include ${PROJECT_SOURCE_DIR}/platform/darwin/src ${PROJECT_SOURCE_DIR}/platform/macos/src
)

include(${PROJECT_SOURCE_DIR}/vendor/icu.cmake)

target_link_libraries(
    mbgl-core
    PRIVATE
        "-framework AppKit"
        "-framework CoreGraphics"
        "-framework CoreLocation"
        "-framework SystemConfiguration"
        OpenGL::GL
        mbgl-vendor-icu
        sqlite3
        z
)

add_subdirectory(${PROJECT_SOURCE_DIR}/bin)
add_subdirectory(${PROJECT_SOURCE_DIR}/expression-test)
add_subdirectory(${PROJECT_SOURCE_DIR}/platform/glfw)
add_subdirectory(${PROJECT_SOURCE_DIR}/platform/node)

add_executable(
    mbgl-test-runner
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/test/main.cpp
)

target_compile_definitions(
    mbgl-test-runner
    PRIVATE WORK_DIRECTORY=${PROJECT_SOURCE_DIR}
)

target_link_libraries(
    mbgl-test-runner
    PRIVATE mbgl-compiler-options mbgl-test
)

add_executable(
    mbgl-benchmark-runner
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/benchmark/main.cpp
)

target_link_libraries(
    mbgl-benchmark-runner
    PRIVATE mbgl-compiler-options mbgl-benchmark
)

add_executable(
    mbgl-render-test-runner
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/render-test/main.cpp
)

target_link_libraries(
    mbgl-render-test-runner
    PRIVATE mbgl-compiler-options mbgl-render-test
)

set_property(TARGET mbgl-benchmark-runner PROPERTY FOLDER Executables)
set_property(TARGET mbgl-test-runner PROPERTY FOLDER Executables)
set_property(TARGET mbgl-render-test-runner PROPERTY FOLDER Executables)

add_test(NAME mbgl-benchmark-runner COMMAND mbgl-benchmark-runner WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_test(NAME mbgl-test-runner COMMAND mbgl-test-runner WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
