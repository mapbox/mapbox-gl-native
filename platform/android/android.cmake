if(NOT ANDROID_NDK_TOOLCHAIN_INCLUDED)
    message(FATAL_ERROR "-- Toolchain file not included, see https://developer.android.com/ndk/guides/cmake")
endif()

include(${PROJECT_SOURCE_DIR}/platform/gfx/gl/gl.cmake)

target_compile_definitions(mbgl-core PUBLIC
    MBGL_USE_GLES2
)

target_sources(mbgl-core PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/android_renderer_backend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/android_renderer_backend.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/android_renderer_frontend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/android_renderer_frontend.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/annotation/marker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/annotation/marker.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/annotation/multi_point.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/annotation/polygon.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/annotation/polygon.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/annotation/polyline.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/annotation/polyline.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/asset_manager.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/asset_manager_file_source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/asset_manager_file_source.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/async_task.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/attach_env.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/attach_env.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/bitmap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/bitmap.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/bitmap_factory.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/bitmap_factory.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/connectivity_listener.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/connectivity_listener.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/conversion/collection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/conversion/collection.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/conversion/color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/conversion/color.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/conversion/constant.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/conversion/constant.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/conversion/conversion.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/file_source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/file_source.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/feature.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/feature.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/feature_collection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/feature_collection.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/geometry.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/geometry.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/geometry_collection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/geometry_collection.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/line_string.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/line_string.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/multi_line_string.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/multi_line_string.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/multi_point.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/multi_point.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/multi_polygon.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/multi_polygon.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/point.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/point.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/polygon.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/polygon.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geojson/util.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geometry/lat_lng.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geometry/lat_lng.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geometry/lat_lng_bounds.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geometry/lat_lng_bounds.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geometry/lat_lng_quad.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geometry/lat_lng_quad.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geometry/projected_meters.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/geometry/projected_meters.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gl_functions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/graphics/pointf.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/graphics/pointf.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/graphics/rectf.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/graphics/rectf.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gson/json_array.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gson/json_array.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gson/json_element.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gson/json_element.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gson/json_object.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gson/json_object.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gson/json_primitive.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/gson/json_primitive.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/http_file_source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/java/util.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/java/util.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/java_types.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/java_types.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/jni.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/jni.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/jni_native.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/jni_native.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/logger.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/logger.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/logging_android.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/map/camera_position.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/map/camera_position.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/map/image.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/map/image.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/map_renderer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/map_renderer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/map_renderer_runnable.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/map_renderer_runnable.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/native_map_view.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/native_map_view.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/offline/offline_manager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/offline/offline_manager.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/offline/offline_region.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/offline/offline_region.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/offline/offline_region_definition.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/offline/offline_region_definition.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/offline/offline_region_error.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/offline/offline_region_error.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/offline/offline_region_status.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/offline/offline_region_status.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/run_loop.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/run_loop_impl.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/snapshotter/map_snapshot.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/snapshotter/map_snapshot.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/snapshotter/map_snapshotter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/snapshotter/map_snapshotter.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/string_util.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/android_conversion.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/conversion/filter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/conversion/filter.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/conversion/position.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/conversion/position.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/conversion/property_expression.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/conversion/property_value.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/conversion/transition_options.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/conversion/transition_options.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/conversion/url_or_tileset.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/conversion/url_or_tileset.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/formatted.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/formatted.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/formatted_section.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/formatted_section.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/background_layer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/background_layer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/circle_layer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/circle_layer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/custom_layer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/custom_layer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/fill_extrusion_layer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/fill_extrusion_layer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/fill_layer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/fill_layer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/heatmap_layer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/heatmap_layer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/hillshade_layer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/hillshade_layer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/layer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/layer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/layer_manager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/layer_manager.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/line_layer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/line_layer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/raster_layer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/raster_layer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/symbol_layer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/layers/symbol_layer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/light.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/light.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/position.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/position.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/custom_geometry_source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/custom_geometry_source.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/geojson_source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/geojson_source.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/image_source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/image_source.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/raster_dem_source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/raster_dem_source.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/raster_source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/raster_source.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/source.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/unknown_source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/unknown_source.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/vector_source.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/sources/vector_source.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/transition_options.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/transition_options.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/value.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/style/value.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/text/collator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/text/collator_jni.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/text/format_number.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/text/format_number_jni.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/text/local_glyph_rasterizer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/text/local_glyph_rasterizer_jni.hpp
    ${CMAKE_CURRENT_LIST_DIR}/src/thread.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/timer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/unaccent.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_backend.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_frontend.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gl/headless_backend.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/map/map_snapshotter.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/asset_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/default_file_source.cpp
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
    ${PROJECT_SOURCE_DIR}/platform/linux/src/headless_backend_egl.cpp
)

target_include_directories(mbgl-core PRIVATE
    ${PROJECT_SOURCE_DIR}/platform/default/include
)

include(${PROJECT_SOURCE_DIR}/vendor/icu.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/sqlite.cmake)

target_link_libraries(mbgl-core PRIVATE
    EGL
    GLESv2
    Mapbox::Base::jni.hpp
    android
    atomic
    jnigraphics
    log
    mbgl-vendor-icu
    mbgl-vendor-sqlite
    z
)

add_library(mapbox-gl SHARED
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
)

target_link_libraries(mapbox-gl PRIVATE
    Mapbox::Base::jni.hpp
    mbgl-core
)

add_library(example-custom-layer MODULE
    ${CMAKE_CURRENT_LIST_DIR}/src/example_custom_layer.cpp
)

target_include_directories(example-custom-layer PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(example-custom-layer PRIVATE
    GLESv2
    log
    mapbox-base-optional
)

add_executable(mbgl-test-runner
    ${CMAKE_CURRENT_LIST_DIR}/src/test/test_runner.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test/runtime.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test/runtime.hpp
)

target_link_libraries(mbgl-test-runner PRIVATE
    Mapbox::Base::jni.hpp
    mapbox-gl
    mbgl-test
)

add_executable(mbgl-benchmark-runner
    ${CMAKE_CURRENT_LIST_DIR}/src/test/benchmark_runner.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test/runtime.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/test/runtime.hpp
)

target_link_libraries(mbgl-benchmark-runner PRIVATE
    Mapbox::Base::jni.hpp
    mapbox-gl
    mbgl-benchmark
)

# Android has no concept of MinSizeRel on android.toolchain.cmake and
# provides configurations tuned for binary size. We can push it a bit
# more with code folding and LTO.
set_target_properties(example-custom-layer PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=safe")
set_target_properties(mapbox-gl PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=safe")
set_target_properties(mbgl-benchmark-runner PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=safe")
set_target_properties(mbgl-test-runner PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=safe")

target_compile_options(example-custom-layer PRIVATE $<$<CONFIG:Release>:-Qunused-arguments -flto>)
target_compile_options(mapbox-gl PRIVATE $<$<CONFIG:Release>:-Qunused-arguments -flto>)
target_compile_options(mbgl-core PRIVATE $<$<CONFIG:Release>:-Qunused-arguments -flto>)
target_compile_options(mbgl-vendor-icu PRIVATE $<$<CONFIG:Release>:-Qunused-arguments -flto>)
target_compile_options(mbgl-vendor-sqlite PRIVATE $<$<CONFIG:Release>:-Qunused-arguments -flto>)
