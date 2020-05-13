option(MBGL_WITH_IOS_CCACHE "Enable ccache for iOS" OFF)

if(NOT DEFINED IOS_DEPLOYMENT_TARGET)
    set(IOS_DEPLOYMENT_TARGET "9.0")
endif()

macro(initialize_ios_target target)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${IOS_DEPLOYMENT_TARGET}")
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_ENABLE_BITCODE "YES")
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE bitcode)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)

    # Enable LTO & -Os for Release and RelWithDebInfo (which is currently still used by iOS release packages)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=MinSizeRel] $<$<CONFIG:MINSIZEREL>:s>)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=RelWithDebInfo] $<$<CONFIG:RELWITHDEBINFO>:s>)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_GCC_OPTIMIZATION_LEVEL[variant=Release] $<$<CONFIG:RELEASE>:s>)

    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_LLVM_LTO[variant=RelWithDebInfo] $<$<CONFIG:RELWITHDEBINFO>:YES>)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_LLVM_LTO[variant=Release] $<$<CONFIG:RELEASE>:YES>)
endmacro()

set_target_properties(mbgl-core PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)

if(MBGL_WITH_OPENGL)
    target_compile_definitions(
        mbgl-core
        PUBLIC MBGL_USE_GLES2 GLES_SILENCE_DEPRECATION
    )
    target_sources(
        mbgl-core
        PRIVATE
            ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gl/headless_backend.cpp
            ${PROJECT_SOURCE_DIR}/platform/darwin/src/gl_functions.cpp ${PROJECT_SOURCE_DIR}/platform/darwin/src/headless_backend_eagl.mm
    )
    target_link_libraries(
        mbgl-core
        PRIVATE "-framework GLKit" "-framework OpenGLES"
    )
endif()

target_sources(
    mbgl-core
    PRIVATE
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/async_task.cpp
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/collator.mm
        $<$<BOOL:${MBGL_PUBLIC_BUILD}>:${PROJECT_SOURCE_DIR}/platform/darwin/src/http_file_source.mm>
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/image.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/local_glyph_rasterizer.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/logging_nslog.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/native_apple_interface.m
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/nsthread.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/number_format.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/run_loop.cpp
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/string_nsstring.mm
        ${PROJECT_SOURCE_DIR}/platform/darwin/src/timer.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_backend.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/gfx/headless_frontend.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/layermanager/layer_manager.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/map/map_snapshotter.cpp
        ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/platform/time.cpp
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
)

target_include_directories(
    mbgl-core
    PRIVATE ${PROJECT_SOURCE_DIR}/platform/darwin/include ${PROJECT_SOURCE_DIR}/platform/default/include
)

include(${PROJECT_SOURCE_DIR}/vendor/icu.cmake)
if(MBGL_WITH_IOS_CCACHE)
    include(${PROJECT_SOURCE_DIR}/platform/ios/ccache.cmake)
endif()
if(MBGL_WITH_OPENGL)
    include(${PROJECT_SOURCE_DIR}/platform/ios/ios-test-runners.cmake)
endif()

initialize_ios_target(mbgl-core)
initialize_ios_target(mbgl-vendor-csscolorparser)
initialize_ios_target(mbgl-vendor-icu)
initialize_ios_target(mbgl-vendor-parsedate)

target_link_libraries(
    mbgl-core
    PRIVATE
        "-framework CoreGraphics"
        "-framework CoreImage"
        "-framework CoreLocation"
        "-framework CoreServices"
        "-framework CoreText"
        "-framework Foundation"
        "-framework ImageIO"
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
