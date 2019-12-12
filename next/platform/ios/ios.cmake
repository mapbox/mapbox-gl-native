target_compile_definitions(
    mbgl-core
    PUBLIC MBGL_USE_GLES2 GLES_SILENCE_DEPRECATION
)

if(NOT DEFINED IOS_DEPLOYMENT_TARGET)
    set(IOS_DEPLOYMENT_TARGET "9.0")
endif()

macro(initialize_ios_target target)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "YES")
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE bitcode)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)
endmacro()

set_target_properties(mbgl-core PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)

target_sources(
    mbgl-core
    PRIVATE
        ${MBGL_ROOT}/platform/darwin/src/async_task.cpp
        ${MBGL_ROOT}/platform/darwin/src/collator.mm
        ${MBGL_ROOT}/platform/darwin/src/gl_functions.cpp
        ${MBGL_ROOT}/platform/darwin/src/headless_backend_eagl.mm
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
)

target_include_directories(
    mbgl-core
    PRIVATE ${MBGL_ROOT}/platform/darwin/include ${MBGL_ROOT}/platform/default/include
)

include(${PROJECT_SOURCE_DIR}/vendor/icu.cmake)

initialize_ios_target(mbgl-core)
initialize_ios_target(mbgl-vendor-icu)

target_link_libraries(
    mbgl-core
    PRIVATE
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

unset(IOS_DEPLOYMENT_TARGET CACHE)
