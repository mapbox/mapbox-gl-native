set(USE_GLES2 ON)

macro(initialize_ios_target target)
    set_xcode_property(${target} IPHONEOS_DEPLOYMENT_TARGET "9.0")
    set_xcode_property(${target} ENABLE_BITCODE "YES")
    set_xcode_property(${target} BITCODE_GENERATION_MODE bitcode)
    set_xcode_property(${target} ONLY_ACTIVE_ARCH $<$<CONFIG:Debug>:YES>)

    target_compile_options(${target}
        PRIVATE -fobjc-arc
    )
endmacro()


include(cmake/loop-darwin.cmake)
initialize_ios_target(icu)
initialize_ios_target(mbgl-loop-darwin)


macro(mbgl_platform_core)
    initialize_ios_target(mbgl-core)

    target_sources_from_file(mbgl-core PRIVATE platform/ios/core-files.json)

    target_include_directories(mbgl-core
        PRIVATE platform/ios/src
        PRIVATE platform/darwin/src
        PUBLIC platform/darwin/include
        PUBLIC platform/default/include
    )

    target_link_libraries(mbgl-core
        PUBLIC "-lz"
        PUBLIC "-framework Foundation"
        PUBLIC "-framework CoreText"
        PUBLIC "-framework CoreGraphics"
        PUBLIC "-framework OpenGLES"
        PUBLIC "-framework ImageIO"
        PUBLIC "-framework MobileCoreServices"
        PUBLIC "-framework SystemConfiguration"
    )
endmacro()


macro(mbgl_filesource)
    initialize_ios_target(mbgl-filesource)

    # Modify platform/darwin/filesource-files.json to change the source files for this target.
    target_sources_from_file(mbgl-filesource PRIVATE platform/darwin/filesource-files.json)

    target_link_libraries(mbgl-filesource
        PUBLIC "-lsqlite3"
        PUBLIC "-framework Foundation"
    )
endmacro()
