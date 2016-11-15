add_definitions(-DMBGL_USE_GLES2=1)

#Include to use build specific variables
include(${CMAKE_CURRENT_BINARY_DIR}/toolchain.cmake)

mason_use(jni.hpp VERSION 2.0.0 HEADER_ONLY)
mason_use(libjpeg-turbo VERSION 1.5.0)
mason_use(libpng VERSION 1.6.25)
mason_use(libzip VERSION 1.1.3)
mason_use(nunicode VERSION 1.7.1)
mason_use(sqlite VERSION 3.14.2)
mason_use(gtest VERSION 1.7.0)

macro(mbgl_platform_core)

    target_sources(mbgl-core
        # Loop
        PRIVATE platform/android/src/thread.cpp
        PRIVATE platform/android/src/async_task.cpp
        PRIVATE platform/android/src/run_loop.cpp
        PRIVATE platform/android/src/run_loop_impl.hpp
        PRIVATE platform/android/src/timer.cpp

        # File source
        PRIVATE platform/android/src/asset_file_source.cpp
        PRIVATE platform/android/src/http_file_source.cpp
        PRIVATE platform/default/default_file_source.cpp
        PRIVATE platform/default/local_file_source.cpp
        PRIVATE platform/default/online_file_source.cpp

        # Offline
        # PRIVATE include/mbgl/storage/offline.hpp
        PRIVATE platform/default/mbgl/storage/offline.cpp
        PRIVATE platform/default/mbgl/storage/offline_database.cpp
        PRIVATE platform/default/mbgl/storage/offline_database.hpp
        PRIVATE platform/default/mbgl/storage/offline_download.cpp
        PRIVATE platform/default/mbgl/storage/offline_download.hpp
        PRIVATE platform/default/sqlite3.cpp
        PRIVATE platform/default/sqlite3.hpp

        # Misc
        PRIVATE platform/android/src/log_android.cpp
        PRIVATE platform/default/string_stdlib.cpp

        # Image handling
        PRIVATE platform/default/image.cpp
        PRIVATE platform/default/png_reader.cpp
        PRIVATE platform/default/jpeg_reader.cpp

        # Thread pool
        PRIVATE platform/default/thread_pool.cpp

        # Native map
        platform/android/src/native_map_view.cpp
        platform/android/src/native_map_view.hpp

        # Main jni bindings
        platform/android/src/jni.cpp
        platform/android/src/jni.hpp
        platform/android/src/attach_env.cpp
        platform/android/src/attach_env.hpp
        platform/android/src/java_types.cpp
        platform/android/src/java_types.hpp
    )

    target_include_directories(mbgl-core
        PRIVATE platform/default
    )

    target_add_mason_package(mbgl-core PUBLIC sqlite)
    target_add_mason_package(mbgl-core PUBLIC nunicode)
    target_add_mason_package(mbgl-core PUBLIC libpng)
    target_add_mason_package(mbgl-core PUBLIC libjpeg-turbo)
    target_add_mason_package(mbgl-core PUBLIC libzip)
    target_add_mason_package(mbgl-core PUBLIC geojson)
    target_add_mason_package(mbgl-core PUBLIC jni.hpp)

    target_compile_options(mbgl-core
        PRIVATE -fvisibility=hidden
        PRIVATE -Os
    )

    target_link_libraries(mbgl-core
        PUBLIC -llog
        PUBLIC -landroid
        PUBLIC -lEGL
        PUBLIC -lGLESv2
        PUBLIC -lstdc++
        PUBLIC -latomic
        PUBLIC -lz
    )
endmacro()

add_library(mapbox-gl SHARED
    # Conversion C++ -> Java
    platform/android/src/conversion/constant.hpp
    platform/android/src/conversion/conversion.hpp
    platform/android/src/style/conversion/function.hpp
    platform/android/src/style/conversion/property_value.hpp
    platform/android/src/style/conversion/types.hpp
    platform/android/src/style/conversion/types_string_values.hpp

    # Style conversion Java -> C++
    platform/android/src/style/android_conversion.hpp
    platform/android/src/style/conversion/geojson.hpp
    platform/android/src/style/value.cpp
    platform/android/src/style/value.hpp
    platform/android/src/style/conversion/url_or_tileset.hpp

    # Style
    platform/android/src/style/layers/background_layer.cpp
    platform/android/src/style/layers/background_layer.hpp
    platform/android/src/style/layers/circle_layer.cpp
    platform/android/src/style/layers/circle_layer.hpp
    platform/android/src/style/layers/custom_layer.cpp
    platform/android/src/style/layers/custom_layer.hpp
    platform/android/src/style/layers/fill_layer.cpp
    platform/android/src/style/layers/fill_layer.hpp
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
    platform/android/src/style/sources/geojson_source.cpp
    platform/android/src/style/sources/geojson_source.hpp
    platform/android/src/style/sources/source.cpp
    platform/android/src/style/sources/source.hpp
    platform/android/src/style/sources/sources.cpp
    platform/android/src/style/sources/sources.hpp
    platform/android/src/style/sources/raster_source.cpp
    platform/android/src/style/sources/raster_source.hpp
    platform/android/src/style/sources/vector_source.cpp
    platform/android/src/style/sources/vector_source.hpp

    # Connectivity
    platform/android/src/connectivity_listener.cpp
    platform/android/src/connectivity_listener.hpp
)

target_add_mason_package(mapbox-gl PUBLIC rapidjson)

target_compile_options(mapbox-gl
    PRIVATE -fvisibility=hidden
    PRIVATE -Os
)

target_link_libraries(mapbox-gl
    PUBLIC mbgl-core
)

add_library(example-custom-layer SHARED
    platform/android/src/example_custom_layer.cpp
)

target_compile_options(example-custom-layer
    PRIVATE -fvisibility=hidden
    PRIVATE -Os
)

target_link_libraries(example-custom-layer
    PRIVATE mbgl-core
)

set(ANDROID_SDK_PROJECT_DIR ${CMAKE_SOURCE_DIR}/platform/android/MapboxGLAndroidSDK)
set(ANDROID_JNI_TARGET_DIR ${ANDROID_SDK_PROJECT_DIR}/src/main/jniLibs/${ANDROID_JNIDIR}/)
set(ANDROID_ASSETS_TARGET_DIR ${ANDROID_SDK_PROJECT_DIR}/src/main/assets/)
set(ANDROID_TEST_APP_JNI_TARGET_DIR ${CMAKE_SOURCE_DIR}/platform/android/MapboxGLAndroidSDKTestApp/src/main/jniLibs/${ANDROID_JNIDIR}/)

add_custom_target(copy-files
    DEPENDS mapbox-gl
    DEPENDS example-custom-layer
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ANDROID_JNI_TARGET_DIR}
    COMMAND ${STRIP_COMMAND} $<TARGET_FILE:mapbox-gl> -o ${ANDROID_JNI_TARGET_DIR}$<TARGET_FILE_NAME:mapbox-gl>
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ANDROID_ASSETS_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/common/ca-bundle.crt ${ANDROID_ASSETS_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/platform/default/resources/api_mapbox_com-digicert.der ${ANDROID_ASSETS_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/platform/default/resources/api_mapbox_com-geotrust.der ${ANDROID_ASSETS_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/platform/default/resources/star_tilestream_net.der ${ANDROID_ASSETS_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ANDROID_TEST_APP_JNI_TARGET_DIR}
    COMMAND ${STRIP_COMMAND} $<TARGET_FILE:example-custom-layer> -o ${ANDROID_TEST_APP_JNI_TARGET_DIR}$<TARGET_FILE_NAME:example-custom-layer>
)

add_custom_target(_all ALL
    DEPENDS mapbox-gl
    DEPENDS example-custom-layer
    DEPENDS copy-files
)

macro(mbgl_platform_test)

    # Get rid of pthread (from gtest)
    #get_target_property(TEST_LINK_FLAGS mbgl-test LINK_FLAGS)
    #STRING(REPLACE "-pthread" "" TEST_LINK_FLAGS ${TEST_LINK_FLAGS})
    #set_target_properties(mbgl-test PROPERTIES LINK_FLAGS ${TEST_LINK_FLAGS})
    #set_property(TARGET mbgl-test PROPERTY LINK_LIRARIES "")

    target_sources(mbgl-test
        # Main test files
        PRIVATE platform/android/src/test/main.jni.cpp

        # Headless view
        PRIVATE platform/default/headless_backend.cpp
        PRIVATE platform/default/headless_display.cpp
        PRIVATE platform/linux/src/headless_backend_egl.cpp
        PRIVATE platform/linux/src/headless_display_egl.cpp
    )

    target_link_libraries(mbgl-test
        PRIVATE mapbox-gl
    )
endmacro()

