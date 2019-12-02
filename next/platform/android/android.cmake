if(NOT ANDROID_NDK_TOOLCHAIN_INCLUDED)
    message(FATAL_ERROR "-- Toolchain file not included, see https://developer.android.com/ndk/guides/cmake")
endif()

target_compile_definitions(
    mbgl-core
    PUBLIC MBGL_USE_GLES2
)

target_sources(
    mbgl-core
    PRIVATE
        ${MBGL_ROOT}/platform/android/src/android_renderer_backend.cpp
        ${MBGL_ROOT}/platform/android/src/android_renderer_backend.hpp
        ${MBGL_ROOT}/platform/android/src/android_renderer_frontend.cpp
        ${MBGL_ROOT}/platform/android/src/android_renderer_frontend.hpp
        ${MBGL_ROOT}/platform/android/src/annotation/marker.cpp
        ${MBGL_ROOT}/platform/android/src/annotation/marker.hpp
        ${MBGL_ROOT}/platform/android/src/annotation/multi_point.hpp
        ${MBGL_ROOT}/platform/android/src/annotation/polygon.cpp
        ${MBGL_ROOT}/platform/android/src/annotation/polygon.hpp
        ${MBGL_ROOT}/platform/android/src/annotation/polyline.cpp
        ${MBGL_ROOT}/platform/android/src/annotation/polyline.hpp
        ${MBGL_ROOT}/platform/android/src/asset_manager.hpp
        ${MBGL_ROOT}/platform/android/src/asset_manager_file_source.cpp
        ${MBGL_ROOT}/platform/android/src/asset_manager_file_source.hpp
        ${MBGL_ROOT}/platform/android/src/async_task.cpp
        ${MBGL_ROOT}/platform/android/src/attach_env.cpp
        ${MBGL_ROOT}/platform/android/src/attach_env.hpp
        ${MBGL_ROOT}/platform/android/src/bitmap.cpp
        ${MBGL_ROOT}/platform/android/src/bitmap.hpp
        ${MBGL_ROOT}/platform/android/src/bitmap_factory.cpp
        ${MBGL_ROOT}/platform/android/src/bitmap_factory.hpp
        ${MBGL_ROOT}/platform/android/src/connectivity_listener.cpp
        ${MBGL_ROOT}/platform/android/src/connectivity_listener.hpp
        ${MBGL_ROOT}/platform/android/src/conversion/collection.cpp
        ${MBGL_ROOT}/platform/android/src/conversion/collection.hpp
        ${MBGL_ROOT}/platform/android/src/conversion/color.cpp
        ${MBGL_ROOT}/platform/android/src/conversion/color.hpp
        ${MBGL_ROOT}/platform/android/src/conversion/constant.cpp
        ${MBGL_ROOT}/platform/android/src/conversion/constant.hpp
        ${MBGL_ROOT}/platform/android/src/conversion/conversion.hpp
        ${MBGL_ROOT}/platform/android/src/geojson/feature.cpp
        ${MBGL_ROOT}/platform/android/src/geojson/feature.hpp
        ${MBGL_ROOT}/platform/android/src/geojson/feature_collection.cpp
        ${MBGL_ROOT}/platform/android/src/geojson/feature_collection.hpp
        ${MBGL_ROOT}/platform/android/src/geojson/geometry.cpp
        ${MBGL_ROOT}/platform/android/src/geojson/geometry.hpp
        ${MBGL_ROOT}/platform/android/src/geojson/geometry_collection.cpp
        ${MBGL_ROOT}/platform/android/src/geojson/geometry_collection.hpp
        ${MBGL_ROOT}/platform/android/src/geojson/line_string.cpp
        ${MBGL_ROOT}/platform/android/src/geojson/line_string.hpp
        ${MBGL_ROOT}/platform/android/src/geojson/multi_line_string.cpp
        ${MBGL_ROOT}/platform/android/src/geojson/multi_line_string.hpp
        ${MBGL_ROOT}/platform/android/src/geojson/multi_point.cpp
        ${MBGL_ROOT}/platform/android/src/geojson/multi_point.hpp
        ${MBGL_ROOT}/platform/android/src/geojson/multi_polygon.cpp
        ${MBGL_ROOT}/platform/android/src/geojson/multi_polygon.hpp
        ${MBGL_ROOT}/platform/android/src/geojson/point.cpp
        ${MBGL_ROOT}/platform/android/src/geojson/point.hpp
        ${MBGL_ROOT}/platform/android/src/geojson/polygon.cpp
        ${MBGL_ROOT}/platform/android/src/geojson/polygon.hpp
        ${MBGL_ROOT}/platform/android/src/geojson/util.hpp
        ${MBGL_ROOT}/platform/android/src/geometry/lat_lng.cpp
        ${MBGL_ROOT}/platform/android/src/geometry/lat_lng.hpp
        ${MBGL_ROOT}/platform/android/src/geometry/lat_lng_bounds.cpp
        ${MBGL_ROOT}/platform/android/src/geometry/lat_lng_bounds.hpp
        ${MBGL_ROOT}/platform/android/src/geometry/lat_lng_quad.cpp
        ${MBGL_ROOT}/platform/android/src/geometry/lat_lng_quad.hpp
        ${MBGL_ROOT}/platform/android/src/geometry/projected_meters.cpp
        ${MBGL_ROOT}/platform/android/src/geometry/projected_meters.hpp
        ${MBGL_ROOT}/platform/android/src/gl_functions.cpp
        ${MBGL_ROOT}/platform/android/src/graphics/pointf.cpp
        ${MBGL_ROOT}/platform/android/src/graphics/pointf.hpp
        ${MBGL_ROOT}/platform/android/src/graphics/rectf.cpp
        ${MBGL_ROOT}/platform/android/src/graphics/rectf.hpp
        ${MBGL_ROOT}/platform/android/src/gson/json_array.cpp
        ${MBGL_ROOT}/platform/android/src/gson/json_array.hpp
        ${MBGL_ROOT}/platform/android/src/gson/json_element.cpp
        ${MBGL_ROOT}/platform/android/src/gson/json_element.hpp
        ${MBGL_ROOT}/platform/android/src/gson/json_object.cpp
        ${MBGL_ROOT}/platform/android/src/gson/json_object.hpp
        ${MBGL_ROOT}/platform/android/src/gson/json_primitive.cpp
        ${MBGL_ROOT}/platform/android/src/gson/json_primitive.hpp
        ${MBGL_ROOT}/platform/android/src/http_file_source.cpp
        ${MBGL_ROOT}/platform/android/src/image.cpp
        ${MBGL_ROOT}/platform/android/src/java/util.cpp
        ${MBGL_ROOT}/platform/android/src/java/util.hpp
        ${MBGL_ROOT}/platform/android/src/java_types.cpp
        ${MBGL_ROOT}/platform/android/src/java_types.hpp
        ${MBGL_ROOT}/platform/android/src/jni.cpp
        ${MBGL_ROOT}/platform/android/src/jni.hpp
        ${MBGL_ROOT}/platform/android/src/jni_native.cpp
        ${MBGL_ROOT}/platform/android/src/jni_native.hpp
        ${MBGL_ROOT}/platform/android/src/mapbox.cpp
        ${MBGL_ROOT}/platform/android/src/mapbox.hpp
        ${MBGL_ROOT}/platform/android/src/map/camera_position.cpp
        ${MBGL_ROOT}/platform/android/src/map/camera_position.hpp
        ${MBGL_ROOT}/platform/android/src/map/image.cpp
        ${MBGL_ROOT}/platform/android/src/map/image.hpp
        ${MBGL_ROOT}/platform/android/src/map_renderer.cpp
        ${MBGL_ROOT}/platform/android/src/map_renderer.hpp
        ${MBGL_ROOT}/platform/android/src/map_renderer_runnable.cpp
        ${MBGL_ROOT}/platform/android/src/map_renderer_runnable.hpp
        ${MBGL_ROOT}/platform/android/src/native_map_view.cpp
        ${MBGL_ROOT}/platform/android/src/native_map_view.hpp
        ${MBGL_ROOT}/platform/android/src/offline/offline_manager.cpp
        ${MBGL_ROOT}/platform/android/src/offline/offline_manager.hpp
        ${MBGL_ROOT}/platform/android/src/offline/offline_region.cpp
        ${MBGL_ROOT}/platform/android/src/offline/offline_region.hpp
        ${MBGL_ROOT}/platform/android/src/offline/offline_region_definition.cpp
        ${MBGL_ROOT}/platform/android/src/offline/offline_region_definition.hpp
        ${MBGL_ROOT}/platform/android/src/offline/offline_region_error.cpp
        ${MBGL_ROOT}/platform/android/src/offline/offline_region_error.hpp
        ${MBGL_ROOT}/platform/android/src/offline/offline_region_status.cpp
        ${MBGL_ROOT}/platform/android/src/offline/offline_region_status.hpp
        ${MBGL_ROOT}/platform/android/src/run_loop.cpp
        ${MBGL_ROOT}/platform/android/src/run_loop_impl.hpp
        ${MBGL_ROOT}/platform/android/src/snapshotter/map_snapshot.cpp
        ${MBGL_ROOT}/platform/android/src/snapshotter/map_snapshot.hpp
        ${MBGL_ROOT}/platform/android/src/snapshotter/map_snapshotter.cpp
        ${MBGL_ROOT}/platform/android/src/snapshotter/map_snapshotter.hpp
        ${MBGL_ROOT}/platform/android/src/string_util.cpp
        ${MBGL_ROOT}/platform/android/src/style/android_conversion.hpp
        ${MBGL_ROOT}/platform/android/src/style/conversion/filter.cpp
        ${MBGL_ROOT}/platform/android/src/style/conversion/filter.hpp
        ${MBGL_ROOT}/platform/android/src/style/conversion/position.cpp
        ${MBGL_ROOT}/platform/android/src/style/conversion/position.hpp
        ${MBGL_ROOT}/platform/android/src/style/conversion/property_expression.hpp
        ${MBGL_ROOT}/platform/android/src/style/conversion/property_value.hpp
        ${MBGL_ROOT}/platform/android/src/style/conversion/transition_options.cpp
        ${MBGL_ROOT}/platform/android/src/style/conversion/transition_options.hpp
        ${MBGL_ROOT}/platform/android/src/style/conversion/url_or_tileset.cpp
        ${MBGL_ROOT}/platform/android/src/style/conversion/url_or_tileset.hpp
        ${MBGL_ROOT}/platform/android/src/style/formatted.cpp
        ${MBGL_ROOT}/platform/android/src/style/formatted.hpp
        ${MBGL_ROOT}/platform/android/src/style/formatted_section.cpp
        ${MBGL_ROOT}/platform/android/src/style/formatted_section.hpp
        ${MBGL_ROOT}/platform/android/src/style/layers/background_layer.cpp
        ${MBGL_ROOT}/platform/android/src/style/layers/background_layer.hpp
        ${MBGL_ROOT}/platform/android/src/style/layers/circle_layer.cpp
        ${MBGL_ROOT}/platform/android/src/style/layers/circle_layer.hpp
        ${MBGL_ROOT}/platform/android/src/style/layers/custom_layer.cpp
        ${MBGL_ROOT}/platform/android/src/style/layers/custom_layer.hpp
        ${MBGL_ROOT}/platform/android/src/style/layers/fill_extrusion_layer.cpp
        ${MBGL_ROOT}/platform/android/src/style/layers/fill_extrusion_layer.hpp
        ${MBGL_ROOT}/platform/android/src/style/layers/fill_layer.cpp
        ${MBGL_ROOT}/platform/android/src/style/layers/fill_layer.hpp
        ${MBGL_ROOT}/platform/android/src/style/layers/heatmap_layer.cpp
        ${MBGL_ROOT}/platform/android/src/style/layers/heatmap_layer.hpp
        ${MBGL_ROOT}/platform/android/src/style/layers/hillshade_layer.cpp
        ${MBGL_ROOT}/platform/android/src/style/layers/hillshade_layer.hpp
        ${MBGL_ROOT}/platform/android/src/style/layers/layer.cpp
        ${MBGL_ROOT}/platform/android/src/style/layers/layer.hpp
        ${MBGL_ROOT}/platform/android/src/style/layers/layer_manager.cpp
        ${MBGL_ROOT}/platform/android/src/style/layers/layer_manager.hpp
        ${MBGL_ROOT}/platform/android/src/style/layers/line_layer.cpp
        ${MBGL_ROOT}/platform/android/src/style/layers/line_layer.hpp
        ${MBGL_ROOT}/platform/android/src/style/layers/raster_layer.cpp
        ${MBGL_ROOT}/platform/android/src/style/layers/raster_layer.hpp
        ${MBGL_ROOT}/platform/android/src/style/layers/symbol_layer.cpp
        ${MBGL_ROOT}/platform/android/src/style/layers/symbol_layer.hpp
        ${MBGL_ROOT}/platform/android/src/style/light.cpp
        ${MBGL_ROOT}/platform/android/src/style/light.hpp
        ${MBGL_ROOT}/platform/android/src/style/position.cpp
        ${MBGL_ROOT}/platform/android/src/style/position.hpp
        ${MBGL_ROOT}/platform/android/src/style/sources/custom_geometry_source.cpp
        ${MBGL_ROOT}/platform/android/src/style/sources/custom_geometry_source.hpp
        ${MBGL_ROOT}/platform/android/src/style/sources/geojson_source.cpp
        ${MBGL_ROOT}/platform/android/src/style/sources/geojson_source.hpp
        ${MBGL_ROOT}/platform/android/src/style/sources/image_source.cpp
        ${MBGL_ROOT}/platform/android/src/style/sources/image_source.hpp
        ${MBGL_ROOT}/platform/android/src/style/sources/raster_dem_source.cpp
        ${MBGL_ROOT}/platform/android/src/style/sources/raster_dem_source.hpp
        ${MBGL_ROOT}/platform/android/src/style/sources/raster_source.cpp
        ${MBGL_ROOT}/platform/android/src/style/sources/raster_source.hpp
        ${MBGL_ROOT}/platform/android/src/style/sources/source.cpp
        ${MBGL_ROOT}/platform/android/src/style/sources/source.hpp
        ${MBGL_ROOT}/platform/android/src/style/sources/unknown_source.cpp
        ${MBGL_ROOT}/platform/android/src/style/sources/unknown_source.hpp
        ${MBGL_ROOT}/platform/android/src/style/sources/vector_source.cpp
        ${MBGL_ROOT}/platform/android/src/style/sources/vector_source.hpp
        ${MBGL_ROOT}/platform/android/src/style/transition_options.cpp
        ${MBGL_ROOT}/platform/android/src/style/transition_options.hpp
        ${MBGL_ROOT}/platform/android/src/style/value.cpp
        ${MBGL_ROOT}/platform/android/src/style/value.hpp
        ${MBGL_ROOT}/platform/android/src/thread.cpp
        ${MBGL_ROOT}/platform/android/src/timer.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/gfx/headless_backend.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/gfx/headless_frontend.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/gl/headless_backend.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/map/map_snapshotter.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/asset_file_source.cpp
        ${MBGL_ROOT}/platform/default/src/mbgl/storage/default_file_source.cpp
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
        ${MBGL_ROOT}/platform/linux/src/headless_backend_egl.cpp
)

target_include_directories(
    mbgl-core
    PRIVATE ${MBGL_ROOT}/platform/default/include
)

include(${PROJECT_SOURCE_DIR}/vendor/icu.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/sqlite.cmake)

target_link_libraries(
    mbgl-core
    PRIVATE
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

add_library(
    mbgl-core-android STATIC
    ${MBGL_ROOT}/platform/android/src/file_source.cpp
    ${MBGL_ROOT}/platform/android/src/file_source.hpp
    ${MBGL_ROOT}/platform/android/src/i18n/collator.cpp
    ${MBGL_ROOT}/platform/android/src/i18n/collator_jni.hpp
    ${MBGL_ROOT}/platform/android/src/i18n/number_format.cpp
    ${MBGL_ROOT}/platform/android/src/i18n/number_format_jni.hpp
    ${MBGL_ROOT}/platform/android/src/logger.cpp
    ${MBGL_ROOT}/platform/android/src/logger.hpp
    ${MBGL_ROOT}/platform/android/src/logging_android.cpp
    ${MBGL_ROOT}/platform/android/src/text/local_glyph_rasterizer.cpp
    ${MBGL_ROOT}/platform/android/src/text/local_glyph_rasterizer_jni.hpp
)

target_include_directories(
    mbgl-core-android
    PRIVATE ${MBGL_ROOT}/platform/default/include ${MBGL_ROOT}/src
)

target_link_libraries(
    mbgl-core-android
    PRIVATE Mapbox::Base::jni.hpp mbgl-core
)

add_library(
    mapbox-gl SHARED
    ${MBGL_ROOT}/platform/android/src/main.cpp
)

target_include_directories(
    mapbox-gl
    PRIVATE ${MBGL_ROOT}/platform/default/include ${MBGL_ROOT}/src
)

target_link_libraries(
    mapbox-gl
    PRIVATE Mapbox::Base::jni.hpp mbgl-core mbgl-core-android
)

add_library(
    example-custom-layer MODULE
    ${MBGL_ROOT}/platform/android/src/example_custom_layer.cpp
)

target_include_directories(
    example-custom-layer
    PRIVATE ${MBGL_ROOT}/include
)

target_link_libraries(
    example-custom-layer
    PRIVATE
        GLESv2
        Mapbox::Base::optional
        Mapbox::Base::typewrapper
        Mapbox::Base::value
        Mapbox::Base::weak
        log
)

add_executable(
    mbgl-test-runner
    ${MBGL_ROOT}/platform/android/src/test/runtime.cpp ${MBGL_ROOT}/platform/android/src/test/runtime.hpp
    ${MBGL_ROOT}/platform/android/src/test/test_runner.cpp
)

target_link_libraries(
    mbgl-test-runner
    PRIVATE Mapbox::Base::jni.hpp mapbox-gl mbgl-test
)

add_executable(
    mbgl-benchmark-runner
    ${MBGL_ROOT}/platform/android/src/test/benchmark_runner.cpp ${MBGL_ROOT}/platform/android/src/test/runtime.cpp
    ${MBGL_ROOT}/platform/android/src/test/runtime.hpp
)

target_link_libraries(
    mbgl-benchmark-runner
    PRIVATE Mapbox::Base::jni.hpp mapbox-gl mbgl-benchmark
)

add_library(
    mbgl-render-test-runner SHARED
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
    ${MBGL_ROOT}/platform/android/src/test/render_test_runner.cpp
    ${MBGL_ROOT}/platform/default/src/mbgl/text/local_glyph_rasterizer.cpp
    ${MBGL_ROOT}/platform/android/src/test/render_test_collator.cpp
    ${MBGL_ROOT}/platform/android/src/test/render_test_number_format.cpp
)

target_include_directories(
    mbgl-render-test-runner
    PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue ${MBGL_ROOT}/platform/android/src ${MBGL_ROOT}/src
)

target_link_libraries(
    mbgl-render-test-runner
    PRIVATE
        Mapbox::Base::jni.hpp
        android
        log
        mbgl-render-test
)

add_custom_command(
    TARGET mbgl-render-test-runner PRE_BUILD
    COMMAND
        ${CMAKE_COMMAND}
        -E
        copy
        ${MBGL_ROOT}/render-test/android-manifest-probe-memory.json
        ${MBGL_ROOT}/android-manifest-probe-memory.json
    COMMAND
        ${CMAKE_COMMAND}
        -E
        copy
        ${MBGL_ROOT}/render-test/android-manifest-probe-network-gfx.json
        ${MBGL_ROOT}/android-manifest-probe-network-gfx.json
    COMMAND
        ${CMAKE_COMMAND}
        -E
        copy
        ${MBGL_ROOT}/platform/node/test/ignores.json
        ${MBGL_ROOT}/render-test/ignores/ignores.json
    COMMAND
        ${CMAKE_COMMAND}
        -E
        tar
        "chf"
        "render-test/android/app/src/main/assets/data.zip"
        --format=zip
        --files-from=render-test/android/app/src/main/assets/to_zip.txt
    COMMAND
        ${CMAKE_COMMAND}
        -E
        remove
        ${MBGL_ROOT}/android-manifest*
    WORKING_DIRECTORY ${MBGL_ROOT}
)

# Android has no concept of MinSizeRel on android.toolchain.cmake and provides configurations tuned for binary size. We can push it a bit
# more with code folding and LTO.
set_target_properties(example-custom-layer PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=all")
set_target_properties(mapbox-gl PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=all")
set_target_properties(mbgl-benchmark-runner PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=all")
set_target_properties(mbgl-render-test-runner PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=all")
set_target_properties(mbgl-test-runner PROPERTIES LINK_FLAGS_RELEASE "-fuse-ld=gold -O2 -flto -Wl,--icf=all")

target_compile_options(example-custom-layer PRIVATE $<$<CONFIG:Release>:-Oz -Qunused-arguments -flto>)
target_compile_options(mapbox-gl PRIVATE $<$<CONFIG:Release>:-Oz -Qunused-arguments -flto>)
target_compile_options(mbgl-core PRIVATE $<$<CONFIG:Release>:-Oz -Qunused-arguments -flto>)
target_compile_options(mbgl-render-test-runner PRIVATE $<$<CONFIG:Release>:-Oz -Qunused-arguments -flto>)
target_compile_options(mbgl-vendor-icu PRIVATE $<$<CONFIG:Release>:-Oz -Qunused-arguments -flto>)
target_compile_options(mbgl-vendor-sqlite PRIVATE $<$<CONFIG:Release>:-Oz -Qunused-arguments -flto>)

install(TARGETS mapbox-gl LIBRARY DESTINATION lib)
