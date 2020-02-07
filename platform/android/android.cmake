if(NOT ANDROID_NDK_TOOLCHAIN_INCLUDED)
    message(FATAL_ERROR "-- Toolchain file not included, see https://developer.android.com/ndk/guides/cmake")
endif()

target_compile_definitions(
    mbgl-core
    PUBLIC MBGL_USE_GLES2
)

include(${PROJECT_SOURCE_DIR}/vendor/icu.cmake)
include(${PROJECT_SOURCE_DIR}/vendor/sqlite.cmake)

target_compile_options(mbgl-vendor-icu PRIVATE $<$<CONFIG:Release>:-Oz> $<$<CONFIG:Release>:-Qunused-arguments> $<$<CONFIG:Release>:-flto>)

target_compile_options(
    mbgl-vendor-sqlite
    PRIVATE $<$<CONFIG:Release>:-Oz> $<$<CONFIG:Release>:-Qunused-arguments> $<$<CONFIG:Release>:-flto>
)

target_compile_options(
    mbgl-compiler-options
    INTERFACE $<$<CONFIG:Release>:-Oz> $<$<CONFIG:Release>:-Qunused-arguments> $<$<CONFIG:Release>:-flto>
)

target_link_libraries(
    mbgl-compiler-options
    INTERFACE
        $<$<CONFIG:Release>:-O2>
        $<$<CONFIG:Release>:-Wl,--icf=all>
        $<$<CONFIG:Release>:-flto>
        $<$<CONFIG:Release>:-fuse-ld=gold>
)

target_sources(
    mbgl-core
    PRIVATE
        ${PROJECT_SOURCE_DIR}/platform/android/src/android_renderer_backend.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/android_renderer_backend.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/android_renderer_frontend.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/android_renderer_frontend.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/annotation/marker.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/annotation/marker.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/annotation/multi_point.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/annotation/polygon.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/annotation/polygon.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/annotation/polyline.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/annotation/polyline.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/asset_manager.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/asset_manager_file_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/asset_manager_file_source.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/async_task.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/attach_env.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/attach_env.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/bitmap.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/bitmap.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/bitmap_factory.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/bitmap_factory.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/connectivity_listener.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/connectivity_listener.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/conversion/collection.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/conversion/collection.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/conversion/color.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/conversion/color.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/conversion/constant.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/conversion/constant.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/conversion/conversion.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/feature.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/feature.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/feature_collection.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/feature_collection.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/geometry.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/geometry.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/geometry_collection.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/geometry_collection.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/line_string.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/line_string.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/multi_line_string.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/multi_line_string.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/multi_point.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/multi_point.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/multi_polygon.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/multi_polygon.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/point.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/point.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/polygon.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/polygon.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geojson/util.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geometry/lat_lng.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geometry/lat_lng.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geometry/lat_lng_bounds.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geometry/lat_lng_bounds.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geometry/lat_lng_quad.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geometry/lat_lng_quad.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geometry/projected_meters.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/geometry/projected_meters.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/gl_functions.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/graphics/pointf.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/graphics/pointf.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/graphics/rectf.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/graphics/rectf.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/gson/json_array.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/gson/json_array.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/gson/json_element.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/gson/json_element.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/gson/json_object.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/gson/json_object.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/gson/json_primitive.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/gson/json_primitive.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/image.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/java/util.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/java/util.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/java_types.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/java_types.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/jni.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/jni.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/jni_native.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/jni_native.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/mapbox.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/mapbox.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/map/camera_position.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/map/camera_position.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/map/image.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/map/image.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/map_renderer.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/map_renderer.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/map_renderer_runnable.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/map_renderer_runnable.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/native_map_view.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/native_map_view.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/offline/offline_manager.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/offline/offline_manager.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/offline/offline_region.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/offline/offline_region.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/offline/offline_region_definition.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/offline/offline_region_definition.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/offline/offline_region_error.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/offline/offline_region_error.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/offline/offline_region_status.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/offline/offline_region_status.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/run_loop_impl.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/snapshotter/map_snapshot.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/snapshotter/map_snapshot.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/snapshotter/map_snapshotter.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/snapshotter/map_snapshotter.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/string_util.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/android_conversion.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/conversion/filter.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/conversion/filter.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/conversion/position.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/conversion/position.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/conversion/property_expression.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/conversion/property_value.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/conversion/transition_options.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/conversion/transition_options.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/conversion/url_or_tileset.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/conversion/url_or_tileset.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/formatted.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/formatted.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/formatted_section.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/formatted_section.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/background_layer.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/background_layer.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/circle_layer.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/circle_layer.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/custom_layer.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/custom_layer.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/fill_extrusion_layer.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/fill_extrusion_layer.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/fill_layer.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/fill_layer.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/heatmap_layer.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/heatmap_layer.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/hillshade_layer.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/hillshade_layer.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/layer.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/layer.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/layer_manager.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/layer_manager.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/line_layer.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/line_layer.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/raster_layer.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/raster_layer.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/symbol_layer.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/layers/symbol_layer.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/light.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/light.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/position.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/position.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/custom_geometry_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/custom_geometry_source.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/geojson_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/geojson_source.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/image_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/image_source.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/raster_dem_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/raster_dem_source.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/raster_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/raster_source.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/source.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/source.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/unknown_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/unknown_source.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/vector_source.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/sources/vector_source.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/transition_options.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/transition_options.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/value.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/style/value.hpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/thread.cpp
        ${PROJECT_SOURCE_DIR}/platform/android/src/timer.cpp
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
        ${PROJECT_SOURCE_DIR}/platform/linux/src/headless_backend_egl.cpp
)

target_include_directories(
    mbgl-core
    PRIVATE ${PROJECT_SOURCE_DIR}/platform/default/include
)

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
    ${PROJECT_SOURCE_DIR}/platform/android/src/file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/file_source.hpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/http_file_source.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/i18n/collator.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/i18n/collator_jni.hpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/i18n/number_format.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/i18n/number_format_jni.hpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/logger.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/logger.hpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/logging_android.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/run_loop.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/text/local_glyph_rasterizer.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/text/local_glyph_rasterizer_jni.hpp
)

target_include_directories(
    mbgl-core-android
    PRIVATE ${PROJECT_SOURCE_DIR}/platform/default/include ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    mbgl-core-android
    PRIVATE Mapbox::Base::jni.hpp mbgl-compiler-options mbgl-core
)

add_library(
    mapbox-gl SHARED
    ${PROJECT_SOURCE_DIR}/platform/android/src/main.cpp
)

target_include_directories(
    mapbox-gl
    PRIVATE ${PROJECT_SOURCE_DIR}/platform/default/include ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    mapbox-gl
    PRIVATE
        Mapbox::Base::jni.hpp
        mbgl-compiler-options
        mbgl-core
        mbgl-core-android
)

add_library(
    example-custom-layer MODULE
    ${PROJECT_SOURCE_DIR}/platform/android/src/example_custom_layer.cpp
)

target_include_directories(
    example-custom-layer
    PRIVATE ${PROJECT_SOURCE_DIR}/include
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
        mbgl-compiler-options
)

add_library(
    mbgl-test-runner SHARED
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/test_runner.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/test_runner_common.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/local_glyph_rasterizer.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/collator_test_stub.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/number_format_test_stub.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/test_run_loop.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/run_loop.cpp
)

target_compile_definitions(
    mbgl-test-runner
    PRIVATE MBGL_TEST
)

target_include_directories(
    mbgl-test-runner
    PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue ${PROJECT_SOURCE_DIR}/platform/android/src ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    mbgl-test-runner
    PRIVATE
        Mapbox::Base::jni.hpp
        mbgl-compiler-options
        -Wl,--whole-archive
        mbgl-test
        -Wl,--no-whole-archive
)

if(ANDROID_NATIVE_API_LEVEL VERSION_LESS 24)
    target_sources(
        mbgl-test-runner
        PRIVATE ${PROJECT_SOURCE_DIR}/platform/android/src/test/http_file_source_test_stub.cpp
    )
else()
    set(CURL_DIR ${PROJECT_SOURCE_DIR}/vendor/curl-android-ios/prebuilt-with-ssl/android)
    set(CURL_LIBRARY ${CURL_DIR}/${ANDROID_ABI}/libcurl.a)

    target_sources(
        mbgl-test-runner
        PRIVATE ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/storage/http_file_source.cpp
    )
    target_include_directories(
        mbgl-test-runner
        PRIVATE ${CURL_DIR}/include
    )
    target_link_libraries(
        mbgl-test-runner
        PRIVATE ${CURL_LIBRARY}
    )
endif()

add_custom_command(
    TARGET mbgl-test-runner PRE_BUILD
    COMMAND
        ${CMAKE_COMMAND}
        -E
        make_directory
        ${PROJECT_SOURCE_DIR}/test/results
    COMMAND
        ${CMAKE_COMMAND}
        -E
        tar
        "chf"
        "test/android/app/src/main/assets/data.zip"
        --format=zip
        --files-from=test/android/app/src/main/assets/to_zip.txt
    COMMAND
        ${CMAKE_COMMAND}
        -E
        remove_directory
        ${PROJECT_SOURCE_DIR}/test/results
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_library(
    mbgl-benchmark-runner SHARED
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/benchmark_runner.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/test_runner_common.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/local_glyph_rasterizer.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/collator_test_stub.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/number_format_test_stub.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/http_file_source_test_stub.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/run_loop.cpp
)

target_include_directories(
    mbgl-benchmark-runner
    PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue ${PROJECT_SOURCE_DIR}/platform/android/src ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    mbgl-benchmark-runner
    PRIVATE
        Mapbox::Base::jni.hpp
        mbgl-compiler-options
        -Wl,--whole-archive
        mbgl-benchmark
        -Wl,--no-whole-archive
)

add_custom_command(
    TARGET mbgl-benchmark-runner PRE_BUILD
    COMMAND
        ${CMAKE_COMMAND}
        -E
        make_directory
        ${PROJECT_SOURCE_DIR}/benchmark/results
    COMMAND
        ${CMAKE_COMMAND}
        -E
        tar
        "chf"
        "benchmark/android/app/src/main/assets/data.zip"
        --format=zip
        --files-from=benchmark/android/app/src/main/assets/to_zip.txt
    COMMAND
        ${CMAKE_COMMAND}
        -E
        remove_directory
        ${PROJECT_SOURCE_DIR}/benchmark/results
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_library(
    mbgl-render-test-runner SHARED
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/render_test_runner.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/test_runner_common.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/text/local_glyph_rasterizer.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/collator_test_stub.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/number_format_test_stub.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/test/http_file_source_test_stub.cpp
    ${PROJECT_SOURCE_DIR}/platform/android/src/run_loop.cpp
)

target_include_directories(
    mbgl-render-test-runner
    PRIVATE ${ANDROID_NDK}/sources/android/native_app_glue ${PROJECT_SOURCE_DIR}/platform/android/src ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(
    mbgl-render-test-runner
    PRIVATE
        Mapbox::Base::jni.hpp
        android
        log
        mbgl-compiler-options
        mbgl-render-test
)

add_custom_command(
    TARGET mbgl-render-test-runner PRE_BUILD
    COMMAND
        ${CMAKE_COMMAND}
        -E
        tar
        "chf"
        "render-test/android/app/src/main/assets/data.zip"
        --format=zip
        --files-from=render-test/android/app/src/main/assets/to_zip.txt
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

install(TARGETS mapbox-gl LIBRARY DESTINATION lib)
