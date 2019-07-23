if (NOT CMAKE_GENERATOR STREQUAL Xcode)
    message(FATAL_ERROR "-- iOS build only supported with '-G Xcode'")
endif()

macro(initialize_ios_target target)
    set_target_properties(${target} PROPERTIES
        XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE bitcode
        XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES YES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
        XCODE_ATTRIBUTE_ENABLE_BITCODE YES
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 9.0
        XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>
    )
endmacro()

include(${PROJECT_SOURCE_DIR}/platform/gfx/gl/gl.cmake)

target_compile_definitions(mbgl-core PUBLIC
    MBGL_USE_GLES2
)

target_sources(mbgl-core PRIVATE
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/async_task.cpp
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/collator.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/gl_functions.cpp
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/headless_backend_eagl.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/http_file_source.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/image.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/local_glyph_rasterizer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/logging_nslog.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/nsthread.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/reachability.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/run_loop.cpp
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/string_nsstring.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/timer.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_backend.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_frontend.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gl/headless_backend.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/map/map_snapshotter.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/asset_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/default_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/file_source_request.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/local_file_request.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/local_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline_database.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/offline_download.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/online_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/sqlite3.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/bidi.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/png_writer.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/thread_local.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/util/utf.cpp
)

initialize_ios_target(mbgl-core)

target_include_directories(mbgl-core PRIVATE
    ${PROJECT_SOURCE_DIR}/platform/default/include
    ${PROJECT_SOURCE_DIR}/platform/darwin/include
)

include(${PROJECT_SOURCE_DIR}/vendor/icu.cmake)

target_link_libraries(mbgl-core PRIVATE
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

set(MBGL_MAPBOX_RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/framework/Info.plist
    ${CMAKE_CURRENT_LIST_DIR}/resources/Base.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/Images.xcassets
    ${CMAKE_CURRENT_LIST_DIR}/resources/ar.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/bg.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/ca.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/cs.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/da.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/de.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/en.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/es.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/fr.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/gl.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/he.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/hu.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/ja.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/ko.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/lt.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/pt-BR.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/pt-PT.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/ru.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/sv.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/uk.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/vi.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/zh-Hans.lproj
    ${CMAKE_CURRENT_LIST_DIR}/resources/zh-Hant.lproj
)

set(MBGL_MAPBOX_PUBLIC_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLAnnotationContainerView.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLAnnotationImage.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLAnnotationView.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLCalloutView.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLCameraChangeReason.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLCompactCalloutView.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLCompassButton.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLFaux3DUserLocationAnnotationView.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapAccessibilityElement.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapView+IBAdditions.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapView+Impl.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapView+OpenGL.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapView.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapViewDelegate.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapboxEvents.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLSDKUpdateChecker.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLScaleBar.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLTelemetryConfig.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLUserLocation.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLUserLocationAnnotationView.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLUserLocationHeadingArrowLayer.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLUserLocationHeadingBeamLayer.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLUserLocationHeadingIndicator.h
    ${CMAKE_CURRENT_LIST_DIR}/src/Mapbox.h
    ${CMAKE_CURRENT_LIST_DIR}/src/NSOrthography+MGLAdditions.h
    ${CMAKE_CURRENT_LIST_DIR}/src/UIColor+MGLAdditions.h
    ${CMAKE_CURRENT_LIST_DIR}/src/UIDevice+MGLAdditions.h
    ${CMAKE_CURRENT_LIST_DIR}/src/UIImage+MGLAdditions.h
    ${CMAKE_CURRENT_LIST_DIR}/src/UIView+MGLAdditions.h
    ${CMAKE_CURRENT_LIST_DIR}/src/UIViewController+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLAccountManager.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLAnnotation.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLAttributedExpression.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLAttributionInfo.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLBackgroundStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLCircleStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLClockDirectionFormatter.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLCluster.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLCompassDirectionFormatter.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLComputedShapeSource.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLConversion.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLCoordinateFormatter.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLDistanceFormatter.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFeature.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFillExtrusionStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFillStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLForegroundStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFoundation.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLGeometry.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLHeatmapStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLHillshadeStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLImageSource.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLight.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLineStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLocationManager.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLoggingConfiguration.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLMapCamera.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLMapSnapshotter.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLMultiPoint.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLNetworkConfiguration.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOfflinePack.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOfflineRegion.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOfflineStorage.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOpenGLStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOverlay.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPointAnnotation.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPointCollection.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPolygon.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPolyline.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRasterDEMSource.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRasterStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRasterTileSource.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRendererConfiguration.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRendererFrontend.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLSDKMetricsManager.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShape.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShapeCollection.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShapeOfflineRegion.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShapeSource.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLSource.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyle.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyleLayerManager.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyleValue.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLSymbolStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLTilePyramidOfflineRegion.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLTileSource.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLTypes.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLValueEvaluator.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLVectorStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLVectorTileSource.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSArray+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSBundle+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSCoder+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSComparisonPredicate+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSCompoundPredicate+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSDate+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSDictionary+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSException+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSExpression+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSExpression+MGLPrivateAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSPredicate+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSPredicate+MGLPrivateAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSString+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSURL+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSValue+MGLAdditions.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/NSValue+MGLStyleAttributeAdditions.h
)

add_library(Mapbox SHARED
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLAnnotationContainerView.m
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLAnnotationContainerView_Private.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLAnnotationImage.m
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLAnnotationImage_Private.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLAnnotationView.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLAnnotationView_Private.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLCompactCalloutView.m
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLCompassButton.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLCompassButton_Private.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLFaux3DUserLocationAnnotationView.m
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapAccessibilityElement.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapboxEvents.m
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapView+Impl.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapView.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapView+OpenGL.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLMapView_Private.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLScaleBar.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLSDKUpdateChecker.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLTelemetryConfig.m
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLUserLocationAnnotationView.m
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLUserLocationAnnotationView_Private.h
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLUserLocationHeadingArrowLayer.m
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLUserLocationHeadingBeamLayer.m
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLUserLocation.m
    ${CMAKE_CURRENT_LIST_DIR}/src/MGLUserLocation_Private.h
    ${CMAKE_CURRENT_LIST_DIR}/src/NSOrthography+MGLAdditions.m
    ${CMAKE_CURRENT_LIST_DIR}/src/UIColor+MGLAdditions.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/UIDevice+MGLAdditions.m
    ${CMAKE_CURRENT_LIST_DIR}/src/UIImage+MGLAdditions.mm
    ${CMAKE_CURRENT_LIST_DIR}/src/UIViewController+MGLAdditions.m
    ${CMAKE_CURRENT_LIST_DIR}/src/UIView+MGLAdditions.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-accounts-ios/MBXSKUToken.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Categories/CLLocationManager+MMEMobileEvents.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Categories/CLLocationManager+MMEMobileEvents.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Categories/CLLocation+MMEMobileEvents.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Categories/CLLocation+MMEMobileEvents.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Categories/MMECategoryLoader.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Categories/MMECategoryLoader.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Categories/NSData+MMEGZIP.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Categories/NSData+MMEGZIP.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Categories/UIKit+MMEMobileEvents.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Categories/UIKit+MMEMobileEvents.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MapboxMobileEvents.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEAPIClient.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEAPIClient.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMECertPin.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMECertPin.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMECommonEventData.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMECommonEventData.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEConfigurator.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEConfigurator.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEConstants.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEConstants.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEDate.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEDate.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEDependencyManager.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEDependencyManager.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEDispatchManager.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEDispatchManager.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEEvent.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEEventLogger.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEEventLogger.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEEventLogReportViewController.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEEventLogReportViewController.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEEvent.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEEventsConfiguration.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEEventsConfiguration.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEEventsManager.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEEventsManager.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMELocationManager.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMELocationManager.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEMetrics.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEMetrics.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEMetricsManager.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEMetricsManager.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMENamespacedDependencies.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMENSURLSessionWrapper.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMENSURLSessionWrapper.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEPinningConfigurationProvider.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEPinningConfigurationProvider.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMETimerManager.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMETimerManager.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMETypes.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEUIApplicationWrapper.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEUIApplicationWrapper.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEUINavigation.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEUINavigation.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEUniqueIdentifier.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/MMEUniqueIdentifier.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Reachability/MMEReachability.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Reachability/MMEReachability.m
    ${CMAKE_CURRENT_LIST_DIR}/vendor/SMCalloutView/SMCalloutView.h
    ${CMAKE_CURRENT_LIST_DIR}/vendor/SMCalloutView/SMCalloutView.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLAccountManager.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLAccountManager_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLAttributedExpression.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLAttributionInfo.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLAttributionInfo_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLBackgroundStyleLayer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLBackgroundStyleLayer_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLCircleStyleLayer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLCircleStyleLayer_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLClockDirectionFormatter.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLCompassDirectionFormatter.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLComputedShapeSource.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLComputedShapeSource_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLCoordinateFormatter.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLDistanceFormatter.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFeature.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFeature_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFillExtrusionStyleLayer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFillExtrusionStyleLayer_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFillStyleLayer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFillStyleLayer_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLForegroundStyleLayer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFoundation.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLFoundation_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLGeometry.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLGeometry_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLHeatmapStyleLayer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLHeatmapStyleLayer_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLHillshadeStyleLayer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLHillshadeStyleLayer_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLImageSource.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLight.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLight_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLineStyleLayer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLineStyleLayer_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLocationManager.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLocationManager_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLoggingConfiguration.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLLoggingConfiguration_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLMapCamera.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLMapSnapshotter.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLMultiPoint.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLMultiPoint_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLNetworkConfiguration.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLNetworkConfiguration_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOfflinePack.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOfflinePack_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOfflineRegion_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOfflineStorage.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOfflineStorage_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOpenGLStyleLayer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLOpenGLStyleLayer_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPointAnnotation.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPointCollection.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPointCollection_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPolygon.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPolygon_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPolyline.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLPolyline_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRasterDEMSource.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRasterStyleLayer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRasterStyleLayer_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRasterTileSource.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRasterTileSource_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLRendererConfiguration.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLSDKMetricsManager.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLSDKMetricsManager_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShapeCollection.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShape.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShapeOfflineRegion.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShapeOfflineRegion_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShape_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShapeSource.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLShapeSource_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLSource.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLSource_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyleLayerManager.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyleLayer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyleLayer_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyle.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyle_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyleValue.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLStyleValue_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLSymbolStyleLayer.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLSymbolStyleLayer_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLTilePyramidOfflineRegion.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLTilePyramidOfflineRegion_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLTileSource.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLTileSource_Private.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLTypes.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLVectorStyleLayer.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLVectorTileSource.mm
    ${PROJECT_SOURCE_DIR}/platform/darwin/src/MGLVectorTileSource_Private.h
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
    ${MBGL_MAPBOX_PUBLIC_HEADERS}
    ${MBGL_MAPBOX_RESOURCES}
)

initialize_ios_target(Mapbox)

set_target_properties(Mapbox PROPERTIES
    FRAMEWORK TRUE
    MACOSX_FRAMEWORK_IDENTIFIER com.mapbox.core
    MACOSX_FRAMEWORK_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/framework/Info.plist
    RESOURCE "${MBGL_MAPBOX_RESOURCES}"
)

# Setting as PUBLIC_HEADER target property will copy the
# headers to the framework but won't add the Mapbox/ prefix.
set_source_files_properties(${MBGL_MAPBOX_PUBLIC_HEADERS} PROPERTIES MACOSX_PACKAGE_LOCATION Headers/Mapbox)

target_include_directories(Mapbox PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/vendor/SMCalloutView
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-accounts-ios
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Categories
    ${CMAKE_CURRENT_LIST_DIR}/vendor/mapbox-events-ios/MapboxMobileEvents/Reachability
    ${PROJECT_SOURCE_DIR}/platform/darwin/include
    ${PROJECT_SOURCE_DIR}/platform/darwin/src
    ${PROJECT_SOURCE_DIR}/platform/default/include
)

target_include_directories(Mapbox INTERFACE
    $<TARGET_BUNDLE_DIR:Mapbox>/Headers
)

include(${PROJECT_SOURCE_DIR}/vendor/polylabel.cmake)

target_link_libraries(Mapbox PRIVATE
    mbgl-core
    mbgl-vendor-polylabel
)

set(MBGL_MAPBOXAPP_RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/app/Assets.xcassets
    ${CMAKE_CURRENT_LIST_DIR}/app/Base.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/Info.plist
    ${CMAKE_CURRENT_LIST_DIR}/app/LaunchScreen.storyboard
    ${CMAKE_CURRENT_LIST_DIR}/app/Main.storyboard
    ${CMAKE_CURRENT_LIST_DIR}/app/Settings.bundle
    ${CMAKE_CURRENT_LIST_DIR}/app/ar.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/bg.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/ca.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/cs.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/da.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/de.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/es.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/fi.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/fill_filter_style.json
    ${CMAKE_CURRENT_LIST_DIR}/app/fr.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/gl.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/he.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/hu.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/ja.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/ko.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/line_filter_style.json
    ${CMAKE_CURRENT_LIST_DIR}/app/lt.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/missing_icon.json
    ${CMAKE_CURRENT_LIST_DIR}/app/nl.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/numeric_filter_style.json
    ${CMAKE_CURRENT_LIST_DIR}/app/pl.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/points.geojson
    ${CMAKE_CURRENT_LIST_DIR}/app/polyline.geojson
    ${CMAKE_CURRENT_LIST_DIR}/app/pt-BR.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/pt-PT.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/ru.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/simple_route.json
    ${CMAKE_CURRENT_LIST_DIR}/app/sv.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/threestates.geojson
    ${CMAKE_CURRENT_LIST_DIR}/app/uk.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/vi.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/zh-Hans.lproj
    ${CMAKE_CURRENT_LIST_DIR}/app/zh-Hant.lproj
)

add_executable(MapboxApp
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXAnnotationView.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXAnnotationView.m
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXAppDelegate.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXAppDelegate.m
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXCustomCalloutView.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXCustomCalloutView.m
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXCustomLocationViewController.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXCustomLocationViewController.m
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXEmbeddedMapViewController.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXEmbeddedMapViewController.m
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXFrameTimeGraphView.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXFrameTimeGraphView.m
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXOfflinePacksTableViewController.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXOfflinePacksTableViewController.m
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXOrnamentsViewController.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXOrnamentsViewController.m
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXSnapshotsViewController.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXSnapshotsViewController.m
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXState.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXState.m
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXStateManager.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXStateManager.m
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXUserLocationAnnotationView.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXUserLocationAnnotationView.m
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXViewController.h
    ${CMAKE_CURRENT_LIST_DIR}/app/MBXViewController.m
    ${CMAKE_CURRENT_LIST_DIR}/app/main.m
    ${PROJECT_SOURCE_DIR}/platform/darwin/app/LimeGreenStyleLayer.h
    ${PROJECT_SOURCE_DIR}/platform/darwin/app/LimeGreenStyleLayer.m
    ${MBGL_MAPBOXAPP_RESOURCES}
)

initialize_ios_target(MapboxApp)

set_target_properties(MapboxApp PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_IDENTIFIER com.mapbox.MapboxGL
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_LIST_DIR}/app/Info.plist
    RESOURCE "${MBGL_MAPBOXAPP_RESOURCES}"
)

target_include_directories(MapboxApp PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${PROJECT_SOURCE_DIR}/platform/darwin/app
)

target_link_libraries(MapboxApp PRIVATE
    "-framework CoreGraphics"
    "-framework CoreLocation"
    "-framework Foundation"
    "-framework OpenGLES"
    "-framework QuartzCore"
    "-framework UIKit"
    Mapbox
)
