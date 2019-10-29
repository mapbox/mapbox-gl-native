find_package(OpenGL REQUIRED)

target_compile_definitions(
    mbgl-core
    PUBLIC GL_SILENCE_DEPRECATION
)

target_sources(
    mbgl-core
    PRIVATE
        ${MBGL_ROOT}/platform/darwin/src/MGLAccountManager.m
        ${MBGL_ROOT}/platform/darwin/src/MGLAttributedExpression.m
        ${MBGL_ROOT}/platform/darwin/src/MGLAttributionInfo.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLBackgroundStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLCircleStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLClockDirectionFormatter.m
        ${MBGL_ROOT}/platform/darwin/src/MGLCompassDirectionFormatter.m
        ${MBGL_ROOT}/platform/darwin/src/MGLComputedShapeSource.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLCoordinateFormatter.m
        ${MBGL_ROOT}/platform/darwin/src/MGLDistanceFormatter.m
        ${MBGL_ROOT}/platform/darwin/src/MGLFeature.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLFillExtrusionStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLFillStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLForegroundStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLFoundation.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLGeometry.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLHeatmapStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLHillshadeStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLImageSource.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLLight.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLLineStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLLoggingConfiguration.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLMapCamera.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLMapSnapshotter.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLMultiPoint.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLNetworkConfiguration.m
        ${MBGL_ROOT}/platform/darwin/src/MGLOfflinePack.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLOfflineStorage.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLOpenGLStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLPointAnnotation.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLPointCollection.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLPolygon.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLPolyline.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLRasterDEMSource.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLRasterStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLRasterTileSource.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLRendererConfiguration.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLSDKMetricsManager.m
        ${MBGL_ROOT}/platform/darwin/src/MGLShape.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLShapeCollection.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLShapeOfflineRegion.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLShapeSource.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLSource.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLStyle.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLStyleLayerManager.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLStyleValue.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLSymbolStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLTilePyramidOfflineRegion.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLTileSource.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLTypes.m
        ${MBGL_ROOT}/platform/darwin/src/MGLVectorStyleLayer.mm
        ${MBGL_ROOT}/platform/darwin/src/MGLVectorTileSource.mm
        ${MBGL_ROOT}/platform/darwin/src/NSArray+MGLAdditions.mm
        ${MBGL_ROOT}/platform/darwin/src/NSBundle+MGLAdditions.m
        ${MBGL_ROOT}/platform/darwin/src/NSCoder+MGLAdditions.mm
        ${MBGL_ROOT}/platform/darwin/src/NSComparisonPredicate+MGLAdditions.mm
        ${MBGL_ROOT}/platform/darwin/src/NSCompoundPredicate+MGLAdditions.mm
        ${MBGL_ROOT}/platform/darwin/src/NSDate+MGLAdditions.mm
        ${MBGL_ROOT}/platform/darwin/src/NSDictionary+MGLAdditions.mm
        ${MBGL_ROOT}/platform/darwin/src/NSExpression+MGLAdditions.mm
        ${MBGL_ROOT}/platform/darwin/src/NSPredicate+MGLAdditions.mm
        ${MBGL_ROOT}/platform/darwin/src/NSString+MGLAdditions.m
        ${MBGL_ROOT}/platform/darwin/src/NSURL+MGLAdditions.m
        ${MBGL_ROOT}/platform/darwin/src/NSValue+MGLAdditions.m
        ${MBGL_ROOT}/platform/darwin/src/NSValue+MGLStyleAttributeAdditions.mm
        ${MBGL_ROOT}/platform/darwin/src/async_task.cpp
        ${MBGL_ROOT}/platform/darwin/src/collator.mm
        ${MBGL_ROOT}/platform/darwin/src/gl_functions.cpp
        ${MBGL_ROOT}/platform/darwin/src/headless_backend_cgl.mm
        ${MBGL_ROOT}/platform/darwin/src/http_file_source.mm
        ${MBGL_ROOT}/platform/darwin/src/image.mm
        ${MBGL_ROOT}/platform/darwin/src/local_glyph_rasterizer.mm
        ${MBGL_ROOT}/platform/darwin/src/logging_nslog.mm
        ${MBGL_ROOT}/platform/darwin/src/nsthread.mm
        ${MBGL_ROOT}/platform/darwin/src/number_format.mm
        ${MBGL_ROOT}/platform/darwin/src/reachability.m
        ${MBGL_ROOT}/platform/darwin/src/run_loop.cpp
        ${MBGL_ROOT}/platform/darwin/src/string_nsstring.mm
        ${MBGL_ROOT}/platform/darwin/src/timer.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/gfx/headless_backend.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/gfx/headless_frontend.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/gl/headless_backend.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/map/map_snapshotter.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/asset_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/default_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/file_source_request.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/local_file_request.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/local_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/offline.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/offline_database.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/offline_download.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/online_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/sqlite3.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/text/bidi.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/compression.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/monotonic_timer.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/png_writer.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/thread_local.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/util/utf.cpp
        ${MBGL_ROOT}/platform/macos/src/MGLAnnotationImage.m
        ${MBGL_ROOT}/platform/macos/src/MGLAttributionButton.mm
        ${MBGL_ROOT}/platform/macos/src/MGLCompassCell.m
        ${MBGL_ROOT}/platform/macos/src/MGLMapView+IBAdditions.mm
        ${MBGL_ROOT}/platform/macos/src/MGLMapView+Impl.mm
        ${MBGL_ROOT}/platform/macos/src/MGLMapView+OpenGL.mm
        ${MBGL_ROOT}/platform/macos/src/MGLMapView.mm
        ${MBGL_ROOT}/platform/macos/src/MGLOpenGLLayer.mm
        ${MBGL_ROOT}/platform/macos/src/NSColor+MGLAdditions.mm
        ${MBGL_ROOT}/platform/macos/src/NSImage+MGLAdditions.mm
        ${MBGL_ROOT}/platform/macos/src/NSProcessInfo+MGLAdditions.m
)

set_target_properties(mbgl-core PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)

target_compile_options(mbgl-core PRIVATE -fobjc-arc)

# FIXME: Should not be needed, but now needed by node because of the headless frontend.
target_include_directories(
    mbgl-core
    PUBLIC ${MBGL_ROOT}/platform/default/include
)

target_include_directories(
    mbgl-core
    PRIVATE ${MBGL_ROOT}/platform/darwin/include ${MBGL_ROOT}/platform/darwin/src ${MBGL_ROOT}/platform/macos/src
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
    ${MBGL_ROOT}/platform/default/src/mbgl/test/main.cpp
)

target_compile_definitions(
    mbgl-test-runner
    PRIVATE WORK_DIRECTORY=${MBGL_ROOT}
)

target_link_libraries(
    mbgl-test-runner
    PRIVATE mbgl-test
)

add_executable(
    mbgl-benchmark-runner
    ${MBGL_ROOT}/platform/default/src/mbgl/benchmark/main.cpp
)

target_link_libraries(
    mbgl-benchmark-runner
    PRIVATE mbgl-benchmark
)

add_executable(
    mbgl-render-test-runner
    ${MBGL_ROOT}/platform/default/src/mbgl/render-test/main.cpp
)

target_link_libraries(
    mbgl-render-test-runner
    PRIVATE mbgl-render-test
)

set_property(TARGET mbgl-benchmark-runner PROPERTY FOLDER Executables)
set_property(TARGET mbgl-test-runner PROPERTY FOLDER Executables)
set_property(TARGET mbgl-render-test-runner PROPERTY FOLDER Executables)

add_test(NAME mbgl-benchmark-runner COMMAND mbgl-benchmark-runner WORKING_DIRECTORY ${MBGL_ROOT})
add_test(NAME mbgl-test-runner COMMAND mbgl-test-runner WORKING_DIRECTORY ${MBGL_ROOT})

string(RANDOM LENGTH 5 ALPHABET 0123456789 MBGL_RENDER_TEST_SEED)

add_test(
    NAME mbgl-render-test
    COMMAND
        mbgl-render-test-runner
        render-tests
        --recycle-map
        --shuffle
        --manifestPath=${MBGL_ROOT}/render-test/mac-manifest.json
        --seed=${MBGL_RENDER_TEST_SEED}
)

add_test(NAME mbgl-render-test-probes COMMAND mbgl-render-test-runner tests --manifestPath=${MBGL_ROOT}/render-test/mac-probe-manifest.json)

add_test(NAME mbgl-query-test COMMAND mbgl-render-test-runner query-tests --manifestPath=${MBGL_ROOT}/render-test/mac-manifest.json)
