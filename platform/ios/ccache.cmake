find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    # Set up wrapper scripts
    set(C_LAUNCHER "${CCACHE_PROGRAM}")
    set(CXX_LAUNCHER "${CCACHE_PROGRAM}")
    configure_file(${PROJECT_SOURCE_DIR}/platform/ios/scripts/ccache-c.in ccache-c)
    configure_file(${PROJECT_SOURCE_DIR}/platform/ios/scripts/ccache-cxx.in ccache-cxx)
    execute_process(
        COMMAND
            chmod
            a+rx
            "${CMAKE_BINARY_DIR}/ccache-c"
            "${CMAKE_BINARY_DIR}/ccache-cxx"
    )

    if(CMAKE_GENERATOR STREQUAL "Xcode")
        # Set Xcode project attributes to route compilation and linking through our scripts
        set(CMAKE_XCODE_ATTRIBUTE_CC "${CMAKE_BINARY_DIR}/ccache-c")
        set(CMAKE_XCODE_ATTRIBUTE_CXX "${CMAKE_BINARY_DIR}/ccache-cxx")
        set(CMAKE_XCODE_ATTRIBUTE_LD "${CMAKE_BINARY_DIR}/ccache-c")
        set(CMAKE_XCODE_ATTRIBUTE_LDPLUSPLUS "${CMAKE_BINARY_DIR}/ccache-cxx")
    else()
        # Support Unix Makefiles and Ninja
        set(CMAKE_C_COMPILER_LAUNCHER "${CMAKE_BINARY_DIR}/ccache-c")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CMAKE_BINARY_DIR}/ccache-cxx")
    endif()
endif()
