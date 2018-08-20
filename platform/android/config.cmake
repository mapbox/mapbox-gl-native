add_definitions(-DMBGL_USE_GLES2=1)
include(cmake/test-files.cmake)
include(cmake/nunicode.cmake)

# Build thin archives.
set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> cruT <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> cruT <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_CXX_ARCHIVE_APPEND "<CMAKE_AR> ruT <TARGET> <LINK_FLAGS> <OBJECTS>")
set(CMAKE_C_ARCHIVE_APPEND "<CMAKE_AR> ruT <TARGET> <LINK_FLAGS> <OBJECTS>")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")

if ((ANDROID_ABI STREQUAL "armeabi-v7a") OR (ANDROID_ABI STREQUAL "arm64-v8a") OR
    (ANDROID_ABI STREQUAL "x86") OR (ANDROID_ABI STREQUAL "x86_64"))
    # Use Identical Code Folding on platforms that support the gold linker.
    set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=gold -Wl,--icf=safe ${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=gold -Wl,--icf=safe ${CMAKE_SHARED_LINKER_FLAGS}")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -Wl,--version-script=${CMAKE_SOURCE_DIR}/platform/android/version-script")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections -Wl,--version-script=${CMAKE_SOURCE_DIR}/platform/android/version-script")

# Use LTO in Release builds. Due to a toolchain issue, -O2 is also required for the link step (https://github.com/android-ndk/ndk/issues/721)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -flto")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -flto")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -flto")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto -O2")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -flto -O2")
set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -flto -O2")
set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} -flto -O2")

## mbgl core ##

macro(mbgl_platform_core)
    target_sources(mbgl-core
        # Loop
        PRIVATE platform/android/src/async_task.cpp
        PRIVATE platform/android/src/run_loop.cpp
        PRIVATE platform/android/src/run_loop_impl.hpp
        PRIVATE platform/android/src/timer.cpp

        # Misc
        PRIVATE platform/android/src/text/collator.cpp
        PRIVATE platform/android/src/text/collator_jni.hpp
        PRIVATE platform/android/src/text/local_glyph_rasterizer.cpp
        PRIVATE platform/android/src/text/local_glyph_rasterizer_jni.hpp
        PRIVATE platform/android/src/logging_android.cpp
        PRIVATE platform/android/src/thread.cpp
        PRIVATE platform/default/string_stdlib.cpp
        PRIVATE platform/default/bidi.cpp
        PRIVATE platform/default/thread_local.cpp
        PRIVATE platform/default/unaccent.cpp
        PRIVATE platform/default/unaccent.hpp
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

        # Conversion C++ -> Java
        PRIVATE platform/android/src/conversion/constant.hpp
        PRIVATE platform/android/src/conversion/conversion.hpp
        PRIVATE platform/android/src/style/conversion/property_expression.hpp
        PRIVATE platform/android/src/style/conversion/property_value.hpp
        PRIVATE platform/android/src/style/conversion/types.hpp
        PRIVATE platform/android/src/style/conversion/types_string_values.hpp
        PRIVATE platform/android/src/map/camera_position.cpp
        PRIVATE platform/android/src/map/camera_position.hpp
        PRIVATE platform/android/src/map/image.cpp
        PRIVATE platform/android/src/map/image.hpp

        # Style conversion Java -> C++
        PRIVATE platform/android/src/style/android_conversion.hpp
        PRIVATE platform/android/src/style/value.cpp
        PRIVATE platform/android/src/style/value.hpp
        PRIVATE platform/android/src/style/conversion/url_or_tileset.hpp

        # Style
        PRIVATE platform/android/src/style/transition_options.cpp
        PRIVATE platform/android/src/style/transition_options.hpp
        PRIVATE platform/android/src/style/layers/background_layer.cpp
        PRIVATE platform/android/src/style/layers/background_layer.hpp
        PRIVATE platform/android/src/style/layers/circle_layer.cpp
        PRIVATE platform/android/src/style/layers/circle_layer.hpp
        PRIVATE platform/android/src/style/layers/custom_layer.cpp
        PRIVATE platform/android/src/style/layers/custom_layer.hpp
        PRIVATE platform/android/src/style/layers/fill_extrusion_layer.cpp
        PRIVATE platform/android/src/style/layers/fill_extrusion_layer.hpp
        PRIVATE platform/android/src/style/layers/fill_layer.cpp
        PRIVATE platform/android/src/style/layers/fill_layer.hpp
        PRIVATE platform/android/src/style/layers/heatmap_layer.cpp
        PRIVATE platform/android/src/style/layers/heatmap_layer.hpp
        PRIVATE platform/android/src/style/layers/hillshade_layer.cpp
        PRIVATE platform/android/src/style/layers/hillshade_layer.hpp
        PRIVATE platform/android/src/style/layers/layer.cpp
        PRIVATE platform/android/src/style/layers/layer.hpp
        PRIVATE platform/android/src/style/layers/layers.cpp
        PRIVATE platform/android/src/style/layers/layers.hpp
        PRIVATE platform/android/src/style/layers/line_layer.cpp
        PRIVATE platform/android/src/style/layers/line_layer.hpp
        PRIVATE platform/android/src/style/layers/raster_layer.cpp
        PRIVATE platform/android/src/style/layers/raster_layer.hpp
        PRIVATE platform/android/src/style/layers/symbol_layer.cpp
        PRIVATE platform/android/src/style/layers/symbol_layer.hpp
        PRIVATE platform/android/src/style/layers/unknown_layer.cpp
        PRIVATE platform/android/src/style/layers/unknown_layer.hpp
        PRIVATE platform/android/src/style/sources/geojson_source.cpp
        PRIVATE platform/android/src/style/sources/geojson_source.hpp
        PRIVATE platform/android/src/style/sources/custom_geometry_source.cpp
        PRIVATE platform/android/src/style/sources/custom_geometry_source.hpp
        PRIVATE platform/android/src/style/sources/source.cpp
        PRIVATE platform/android/src/style/sources/source.hpp
        PRIVATE platform/android/src/style/sources/raster_source.cpp
        PRIVATE platform/android/src/style/sources/raster_source.hpp
        PRIVATE platform/android/src/style/sources/unknown_source.cpp
        PRIVATE platform/android/src/style/sources/unknown_source.hpp
        PRIVATE platform/android/src/style/sources/vector_source.cpp
        PRIVATE platform/android/src/style/sources/vector_source.hpp
        PRIVATE platform/android/src/style/sources/image_source.hpp
        PRIVATE platform/android/src/style/sources/image_source.cpp
        PRIVATE platform/android/src/style/sources/raster_dem_source.cpp
        PRIVATE platform/android/src/style/sources/raster_dem_source.hpp
        PRIVATE platform/android/src/style/position.cpp
        PRIVATE platform/android/src/style/position.hpp
        PRIVATE platform/android/src/style/light.cpp
        PRIVATE platform/android/src/style/light.hpp

        # Native map
        PRIVATE platform/android/src/native_map_view.cpp
        PRIVATE platform/android/src/native_map_view.hpp
        PRIVATE platform/android/src/map_renderer.cpp
        PRIVATE platform/android/src/map_renderer.hpp
        PRIVATE platform/android/src/map_renderer_runnable.cpp
        PRIVATE platform/android/src/map_renderer_runnable.hpp

        # Java core classes
        PRIVATE platform/android/src/java/lang.cpp
        PRIVATE platform/android/src/java/lang.hpp
        PRIVATE platform/android/src/java/util.cpp
        PRIVATE platform/android/src/java/util.hpp

        # Graphics
        PRIVATE platform/android/src/graphics/pointf.cpp
        PRIVATE platform/android/src/graphics/pointf.hpp
        PRIVATE platform/android/src/graphics/rectf.cpp
        PRIVATE platform/android/src/graphics/rectf.hpp

        # GeoJSON
        PRIVATE platform/android/src/geojson/feature.cpp
        PRIVATE platform/android/src/geojson/feature.hpp
        PRIVATE platform/android/src/geojson/feature_collection.cpp
        PRIVATE platform/android/src/geojson/feature_collection.hpp
        PRIVATE platform/android/src/geojson/geometry.cpp
        PRIVATE platform/android/src/geojson/geometry.hpp
        PRIVATE platform/android/src/geojson/geometry_collection.cpp
        PRIVATE platform/android/src/geojson/geometry_collection.hpp
        PRIVATE platform/android/src/geojson/line_string.cpp
        PRIVATE platform/android/src/geojson/line_string.hpp
        PRIVATE platform/android/src/geojson/multi_line_string.cpp
        PRIVATE platform/android/src/geojson/multi_line_string.hpp
        PRIVATE platform/android/src/geojson/multi_point.cpp
        PRIVATE platform/android/src/geojson/multi_point.hpp
        PRIVATE platform/android/src/geojson/multi_polygon.cpp
        PRIVATE platform/android/src/geojson/multi_polygon.hpp
        PRIVATE platform/android/src/geojson/point.cpp
        PRIVATE platform/android/src/geojson/point.hpp
        PRIVATE platform/android/src/geojson/polygon.cpp
        PRIVATE platform/android/src/geojson/polygon.hpp

        # Geometry
        PRIVATE platform/android/src/geometry/lat_lng.cpp
        PRIVATE platform/android/src/geometry/lat_lng.hpp
        PRIVATE platform/android/src/geometry/lat_lng_bounds.cpp
        PRIVATE platform/android/src/geometry/lat_lng_bounds.hpp
        PRIVATE platform/android/src/geometry/lat_lng_quad.cpp
        PRIVATE platform/android/src/geometry/lat_lng_quad.hpp
        PRIVATE platform/android/src/geometry/projected_meters.cpp
        PRIVATE platform/android/src/geometry/projected_meters.hpp

        # GSon
        PRIVATE platform/android/src/gson/json_array.cpp
        PRIVATE platform/android/src/gson/json_array.hpp
        PRIVATE platform/android/src/gson/json_element.cpp
        PRIVATE platform/android/src/gson/json_element.hpp
        PRIVATE platform/android/src/gson/json_object.cpp
        PRIVATE platform/android/src/gson/json_object.hpp
        PRIVATE platform/android/src/gson/json_primitive.cpp
        PRIVATE platform/android/src/gson/json_primitive.hpp

        # Annotation
        PRIVATE platform/android/src/annotation/marker.cpp
        PRIVATE platform/android/src/annotation/marker.hpp
        PRIVATE platform/android/src/annotation/polygon.cpp
        PRIVATE platform/android/src/annotation/polygon.hpp
        PRIVATE platform/android/src/annotation/polyline.cpp
        PRIVATE platform/android/src/annotation/polyline.hpp

        # Snapshots (SDK)
        PRIVATE platform/android/src/snapshotter/map_snapshotter.cpp
        PRIVATE platform/android/src/snapshotter/map_snapshotter.hpp
        PRIVATE platform/android/src/snapshotter/map_snapshot.cpp
        PRIVATE platform/android/src/snapshotter/map_snapshot.hpp

        # Main jni bindings
        PRIVATE platform/android/src/attach_env.cpp
        PRIVATE platform/android/src/attach_env.hpp
        PRIVATE platform/android/src/java_types.cpp
        PRIVATE platform/android/src/java_types.hpp

        # Main entry point
        PRIVATE platform/android/src/jni.hpp
        PRIVATE platform/android/src/jni.cpp
    )

    target_include_directories(mbgl-core
        PUBLIC platform/default
        PRIVATE platform/android
    )

    target_add_mason_package(mbgl-core PUBLIC geojson)
    target_add_mason_package(mbgl-core PUBLIC jni.hpp)
    target_add_mason_package(mbgl-core PUBLIC rapidjson)
    target_add_mason_package(mbgl-core PRIVATE icu)

    target_link_libraries(mbgl-core
        PRIVATE nunicode
        PUBLIC expected
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

        # FileSource holder
        PRIVATE platform/android/src/file_source.cpp
        PRIVATE platform/android/src/file_source.hpp

        # Connectivity
        PRIVATE platform/android/src/connectivity_listener.cpp
        PRIVATE platform/android/src/connectivity_listener.hpp

        # Offline
        PRIVATE platform/android/src/offline/offline_manager.cpp
        PRIVATE platform/android/src/offline/offline_manager.hpp
        PRIVATE platform/android/src/offline/offline_region.cpp
        PRIVATE platform/android/src/offline/offline_region.hpp
        PRIVATE platform/android/src/offline/offline_region_definition.cpp
        PRIVATE platform/android/src/offline/offline_region_definition.hpp
        PRIVATE platform/android/src/offline/offline_region_error.cpp
        PRIVATE platform/android/src/offline/offline_region_error.hpp
        PRIVATE platform/android/src/offline/offline_region_status.cpp
        PRIVATE platform/android/src/offline/offline_region_status.hpp

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

## Shared library

add_library(mapbox-gl SHARED
    platform/android/src/main.cpp
)

target_link_libraries(mapbox-gl
    PRIVATE mbgl-core
    PRIVATE mbgl-filesource
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
        PRIVATE mbgl-core
        PRIVATE mbgl-filesource
    )
endmacro()

## Custom layer example ##

add_library(example-custom-layer SHARED
    platform/android/src/example_custom_layer.cpp
)

target_include_directories(example-custom-layer
    PRIVATE include
)

target_link_libraries(example-custom-layer
    PRIVATE -llog
    PRIVATE -lGLESv2
)
