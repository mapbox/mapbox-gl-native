add_definitions(-DMBGL_USE_GLES2=1)
include(cmake/test-files.cmake)

# Build thin archives.
set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> cruT <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> cruT <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_APPEND "<CMAKE_AR> ruT <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_APPEND "<CMAKE_AR> ruT <TARGET> <LINK_FLAGS> <OBJECTS>")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")

if ((ANDROID_ABI STREQUAL "armeabi") OR (ANDROID_ABI STREQUAL "armeabi-v7a") OR (ANDROID_ABI STREQUAL "arm64-v8a") OR
    (ANDROID_ABI STREQUAL "x86") OR (ANDROID_ABI STREQUAL "x86_64"))
    # Use Identical Code Folding on platforms that support the gold linker.
    set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=gold -Wl,--icf=safe ${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=gold -Wl,--icf=safe ${CMAKE_SHARED_LINKER_FLAGS}")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -Wl,--version-script=${CMAKE_SOURCE_DIR}/platform/android/version-script")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl,--version-script=${CMAKE_SOURCE_DIR}/platform/android/version-script")

mason_use(jni.hpp VERSION 3.0.0 HEADER_ONLY)
mason_use(nunicode VERSION 1.7.1)
mason_use(sqlite VERSION 3.14.2)
mason_use(gtest VERSION 1.8.0)
mason_use(icu VERSION 58.1-min-size)

## mbgl core ##

macro(mbgl_platform_core)
    target_sources(mbgl-core
        # Loop
        PRIVATE platform/android/src/async_task.cpp
        PRIVATE platform/android/src/run_loop.cpp
        PRIVATE platform/android/src/run_loop_impl.hpp
        PRIVATE platform/android/src/timer.cpp

        # Misc
        PRIVATE platform/android/src/text/local_glyph_rasterizer_jni.hpp
        PRIVATE platform/android/src/text/local_glyph_rasterizer.cpp
        PRIVATE platform/android/src/logging_android.cpp
        PRIVATE platform/android/src/thread.cpp
        PRIVATE platform/default/string_stdlib.cpp
        PRIVATE platform/default/bidi.cpp
        PRIVATE platform/default/thread_local.cpp
        PRIVATE platform/default/utf.cpp

        # Image handling
        PRIVATE platform/default/png_writer.cpp
        PRIVATE platform/android/src/bitmap.cpp
        PRIVATE platform/android/src/bitmap.hpp
        PRIVATE platform/android/src/bitmap_factory.cpp
        PRIVATE platform/android/src/bitmap_factory.hpp
        PRIVATE platform/android/src/image.cpp

        # Thread pool
        PRIVATE platform/default/mbgl/util/shared_thread_pool.cpp
        PRIVATE platform/default/mbgl/util/shared_thread_pool.hpp
        PRIVATE platform/default/mbgl/util/default_thread_pool.cpp
        PRIVATE platform/default/mbgl/util/default_thread_pool.hpp

        # Rendering
        PRIVATE platform/android/src/android_renderer_backend.cpp
        PRIVATE platform/android/src/android_renderer_backend.hpp
        PRIVATE platform/android/src/android_renderer_frontend.cpp
        PRIVATE platform/android/src/android_renderer_frontend.hpp

        # Snapshots (core)
        PRIVATE platform/default/mbgl/gl/headless_backend.cpp
        PRIVATE platform/default/mbgl/gl/headless_backend.hpp
        PRIVATE platform/default/mbgl/gl/headless_frontend.cpp
        PRIVATE platform/default/mbgl/gl/headless_frontend.hpp
        PRIVATE platform/default/mbgl/map/map_snapshotter.cpp
        PRIVATE platform/default/mbgl/map/map_snapshotter.hpp
        PRIVATE platform/linux/src/headless_backend_egl.cpp
    )

    target_include_directories(mbgl-core
        PUBLIC platform/default
        PRIVATE platform/android
    )

    target_add_mason_package(mbgl-core PUBLIC nunicode)
    target_add_mason_package(mbgl-core PUBLIC geojson)
    target_add_mason_package(mbgl-core PUBLIC jni.hpp)
    target_add_mason_package(mbgl-core PUBLIC rapidjson)
    target_add_mason_package(mbgl-core PRIVATE icu)

    target_link_libraries(mbgl-core
        PUBLIC -llog
        PUBLIC -landroid
        PUBLIC -ljnigraphics
        PUBLIC -lEGL
        PUBLIC -lGLESv2
        PUBLIC -lstdc++
        PUBLIC -latomic
        PUBLIC -lz
    )
endmacro()


macro(mbgl_filesource)
    target_sources(mbgl-filesource
        # File source
        PRIVATE platform/android/src/http_file_source.cpp
        PRIVATE platform/android/src/asset_manager.hpp
        PRIVATE platform/android/src/asset_manager_file_source.cpp
        PRIVATE platform/android/src/asset_manager_file_source.hpp

        # Database
        PRIVATE platform/default/sqlite3.cpp
    )

    target_add_mason_package(mbgl-filesource PUBLIC sqlite)
    target_add_mason_package(mbgl-filesource PUBLIC jni.hpp)

    target_link_libraries(mbgl-filesource
        PUBLIC -llog
        PUBLIC -landroid
        PUBLIC -lstdc++
        PUBLIC -latomic
    )
endmacro()


## Main library ##

add_library(mbgl-android STATIC
    # Conversion C++ -> Java
    platform/android/src/conversion/constant.hpp
    platform/android/src/conversion/conversion.hpp
    platform/android/src/style/conversion/function.hpp
    platform/android/src/style/conversion/property_value.hpp
    platform/android/src/style/conversion/types.hpp
    platform/android/src/style/conversion/types_string_values.hpp
    platform/android/src/map/camera_position.cpp
    platform/android/src/map/camera_position.hpp
    platform/android/src/map/image.cpp
    platform/android/src/map/image.hpp

    # Style conversion Java -> C++
    platform/android/src/style/android_conversion.hpp
    platform/android/src/style/value.cpp
    platform/android/src/style/value.hpp
    platform/android/src/style/conversion/url_or_tileset.hpp

    # Style
    platform/android/src/style/transition_options.cpp
    platform/android/src/style/transition_options.hpp
    platform/android/src/style/layers/background_layer.cpp
    platform/android/src/style/layers/background_layer.hpp
    platform/android/src/style/layers/circle_layer.cpp
    platform/android/src/style/layers/circle_layer.hpp
    platform/android/src/style/layers/custom_layer.cpp
    platform/android/src/style/layers/custom_layer.hpp
    platform/android/src/style/layers/fill_extrusion_layer.cpp
    platform/android/src/style/layers/fill_extrusion_layer.hpp
    platform/android/src/style/layers/fill_layer.cpp
    platform/android/src/style/layers/fill_layer.hpp
    platform/android/src/style/layers/heatmap_layer.cpp
    platform/android/src/style/layers/heatmap_layer.hpp
    platform/android/src/style/layers/hillshade_layer.cpp
    platform/android/src/style/layers/hillshade_layer.hpp
    platform/android/src/style/layers/layer.cpp
    platform/android/src/style/layers/layer.hpp
    platform/android/src/style/layers/layers.cpp
    platform/android/src/style/layers/layers.hpp
    platform/android/src/style/layers/line_layer.cpp
    platform/android/src/style/layers/line_layer.hpp
    platform/android/src/style/layers/raster_layer.cpp
    platform/android/src/style/layers/raster_layer.hpp
    platform/android/src/style/layers/symbol_layer.cpp
    platform/android/src/style/layers/symbol_layer.hpp
    platform/android/src/style/layers/unknown_layer.cpp
    platform/android/src/style/layers/unknown_layer.hpp
    platform/android/src/style/sources/geojson_source.cpp
    platform/android/src/style/sources/geojson_source.hpp
    platform/android/src/style/sources/custom_geometry_source.cpp
    platform/android/src/style/sources/custom_geometry_source.hpp
    platform/android/src/style/sources/source.cpp
    platform/android/src/style/sources/source.hpp
    platform/android/src/style/sources/raster_source.cpp
    platform/android/src/style/sources/raster_source.hpp
    platform/android/src/style/sources/unknown_source.cpp
    platform/android/src/style/sources/unknown_source.hpp
    platform/android/src/style/sources/vector_source.cpp
    platform/android/src/style/sources/vector_source.hpp
    platform/android/src/style/sources/image_source.hpp
    platform/android/src/style/sources/image_source.cpp
    platform/android/src/style/sources/raster_dem_source.cpp
    platform/android/src/style/sources/raster_dem_source.hpp
    platform/android/src/style/functions/stop.cpp
    platform/android/src/style/functions/stop.hpp
    platform/android/src/style/functions/categorical_stops.cpp
    platform/android/src/style/functions/categorical_stops.hpp
    platform/android/src/style/functions/exponential_stops.cpp
    platform/android/src/style/functions/exponential_stops.hpp
    platform/android/src/style/functions/identity_stops.cpp
    platform/android/src/style/functions/identity_stops.hpp
    platform/android/src/style/functions/interval_stops.cpp
    platform/android/src/style/functions/interval_stops.hpp
    platform/android/src/style/position.cpp
    platform/android/src/style/position.hpp
    platform/android/src/style/light.cpp
    platform/android/src/style/light.hpp

    # FileSource holder
    platform/android/src/file_source.cpp
    platform/android/src/file_source.hpp

    # Connectivity
    platform/android/src/connectivity_listener.cpp
    platform/android/src/connectivity_listener.hpp

    # Native map
    platform/android/src/native_map_view.cpp
    platform/android/src/native_map_view.hpp
    platform/android/src/map_renderer.cpp
    platform/android/src/map_renderer.hpp
    platform/android/src/map_renderer_runnable.cpp
    platform/android/src/map_renderer_runnable.hpp

    # Java core classes
    platform/android/src/java/util.cpp
    platform/android/src/java/util.hpp

    # Graphics
    platform/android/src/graphics/pointf.cpp
    platform/android/src/graphics/pointf.hpp
    platform/android/src/graphics/rectf.cpp
    platform/android/src/graphics/rectf.hpp

    # GeoJSON
    platform/android/src/geojson/feature.cpp
    platform/android/src/geojson/feature.hpp
    platform/android/src/geojson/feature_collection.cpp
    platform/android/src/geojson/feature_collection.hpp
    platform/android/src/geojson/geometry.cpp
    platform/android/src/geojson/geometry.hpp
    platform/android/src/geojson/line_string.cpp
    platform/android/src/geojson/line_string.hpp
    platform/android/src/geojson/multi_line_string.cpp
    platform/android/src/geojson/multi_line_string.hpp
    platform/android/src/geojson/multi_point.cpp
    platform/android/src/geojson/multi_point.hpp
    platform/android/src/geojson/multi_polygon.cpp
    platform/android/src/geojson/multi_polygon.hpp
    platform/android/src/geojson/point.cpp
    platform/android/src/geojson/point.hpp
    platform/android/src/geojson/polygon.cpp
    platform/android/src/geojson/polygon.hpp

    # Geometry
    platform/android/src/geometry/lat_lng.cpp
    platform/android/src/geometry/lat_lng.hpp
    platform/android/src/geometry/lat_lng_bounds.cpp
    platform/android/src/geometry/lat_lng_bounds.hpp
    platform/android/src/geometry/lat_lng_quad.cpp
    platform/android/src/geometry/lat_lng_quad.hpp
    platform/android/src/geometry/projected_meters.cpp
    platform/android/src/geometry/projected_meters.hpp

    # GSon
    platform/android/src/gson/json_array.cpp
    platform/android/src/gson/json_array.hpp
    platform/android/src/gson/json_element.cpp
    platform/android/src/gson/json_element.hpp
    platform/android/src/gson/json_object.cpp
    platform/android/src/gson/json_object.hpp
    platform/android/src/gson/json_primitive.cpp
    platform/android/src/gson/json_primitive.hpp

    # Annotation
    platform/android/src/annotation/marker.cpp
    platform/android/src/annotation/marker.hpp
    platform/android/src/annotation/polygon.cpp
    platform/android/src/annotation/polygon.hpp
    platform/android/src/annotation/polyline.cpp
    platform/android/src/annotation/polyline.hpp

    # Offline
    platform/android/src/offline/offline_manager.cpp
    platform/android/src/offline/offline_manager.hpp
    platform/android/src/offline/offline_region.cpp
    platform/android/src/offline/offline_region.hpp
    platform/android/src/offline/offline_region_definition.cpp
    platform/android/src/offline/offline_region_definition.hpp
    platform/android/src/offline/offline_region_error.cpp
    platform/android/src/offline/offline_region_error.hpp
    platform/android/src/offline/offline_region_status.cpp
    platform/android/src/offline/offline_region_status.hpp

    # Snapshots (SDK)
    platform/android/src/snapshotter/map_snapshotter.cpp
    platform/android/src/snapshotter/map_snapshotter.hpp
    platform/android/src/snapshotter/map_snapshot.cpp
    platform/android/src/snapshotter/map_snapshot.hpp

    # Main jni bindings
    platform/android/src/attach_env.cpp
    platform/android/src/attach_env.hpp
    platform/android/src/java_types.cpp
    platform/android/src/java_types.hpp

    # Main entry point
    platform/android/src/jni.hpp
    platform/android/src/jni.cpp
)

target_link_libraries(mbgl-android
    PUBLIC mbgl-filesource
    PUBLIC mbgl-core
)

## Shared library

add_library(mapbox-gl SHARED
    platform/android/src/main.cpp
)

target_link_libraries(mapbox-gl
    PRIVATE mbgl-android
)

## Test library ##

set(MBGL_TEST_TARGET_TYPE "library")
macro(mbgl_platform_test)
    target_sources(mbgl-test
        PRIVATE platform/default/mbgl/test/main.cpp

        # Main test entry point
        platform/android/src/test/main.jni.cpp
    )

    target_include_directories(mbgl-test
        PRIVATE platform/android
    )

    target_link_libraries(mbgl-test
        PRIVATE mbgl-android
    )
endmacro()

## Custom layer example ##

add_library(example-custom-layer SHARED
    platform/android/src/example_custom_layer.cpp
)

target_link_libraries(example-custom-layer
    PRIVATE mbgl-core
)
