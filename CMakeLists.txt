cmake_minimum_required(VERSION 3.9)
project(mbgl LANGUAGES CXX C)

set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo Sanitize)

# Compiler configuration
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Required to avoid warnings when setting visibility
cmake_policy(SET CMP0063 NEW)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11)

option(WITH_CXX11ABI "Use the C++11 ABI" ON)
option(WITH_COVERAGE "Enable coverage reports" OFF)
option(WITH_OSMESA   "Use OSMesa headless backend" OFF)
option(WITH_EGL      "Use EGL backend" OFF)
option(WITH_NODEJS   "Download test dependencies like NPM and Node.js" ON)
option(WITH_ERROR    "Add -Werror flag to build (turns warnings into errors)" ON)

if (WITH_ERROR)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Werror")
endif()

include(cmake/mbgl.cmake)
include(cmake/vendor.cmake)
include(cmake/mason.cmake)
include(cmake/xcode.cmake)
include(cmake/doxygen.cmake)

if(WITH_CXX11ABI)
    set(MASON_CXXABI_SUFFIX -cxx11abi)
    add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
else()
    add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
endif()

if(WITH_OSMESA AND WITH_EGL)
    message(FATAL_ERROR "WITH_OSMESA and WITH_EGL are mutually exclusive.")
endif()

if(WITH_EGL)
    set(USE_GLES2 ON)
endif()

if($ENV{CI})
    add_compile_options(-DCI_BUILD=1)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(EXISTS ${CMAKE_SOURCE_DIR}/.git/HEAD)
    exec_program(
        "git"
        ${CMAKE_SOURCE_DIR}
        ARGS "rev-parse --short=8 HEAD"
        OUTPUT_VARIABLE MBGL_VERSION_REV )
else()
    set(MBGL_VERSION_REV 00000000)
endif()

set_source_files_properties(src/mbgl/util/version.cpp PROPERTIES COMPILE_DEFINITIONS MBGL_VERSION_REV="${MBGL_VERSION_REV}")

include(cmake/mason-dependencies.cmake)

if(WITH_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
endif(WITH_COVERAGE)

if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=0 -w")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-depth=1024 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wno-variadic-macros -Wno-unknown-pragmas")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wshadow -Wno-variadic-macros -Wno-unknown-pragmas")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

if(APPLE AND CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    # -Wno-error=unused-command-line-argument is required due to https://llvm.org/bugs/show_bug.cgi?id=7798
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-command-line-argument")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os -DNDEBUG")
if(APPLE)
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Os -DNDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -Os -DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -O3 -DNDEBUG")
endif()
set(CMAKE_CXX_FLAGS_SANITIZE "${CMAKE_CXX_FLAGS_SANITIZE} -O1 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_C_FLAGS_SANITIZE "${CMAKE_C_FLAGS_SANITIZE} -O1 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unknown-warning-option")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=unknown-warning-option")
elseif(CMAKE_COMPILER_IS_GNUCXX)
    # https://svn.boost.org/trac/boost/ticket/9240
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals")
endif()

set(CMAKE_SHARED_LINKER_FLAGS_SANITIZE "${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_SANITIZE "${CMAKE_EXE_LINKER_FLAGS}")

# Technique from https://crascit.com/2016/04/09/using-ccache-with-cmake/
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(C_LAUNCHER   "${CCACHE_PROGRAM}")
    set(CXX_LAUNCHER "${CCACHE_PROGRAM}")

    if(CMAKE_GENERATOR STREQUAL "Xcode")
        # Set Xcode project attributes to route compilation and linking through our scripts
        # Xcode doesn't include the path to the compiler/linker by default, so we'll have to add it.
        configure_file(scripts/launch-c-xcode.in   launch-c   @ONLY)
        configure_file(scripts/launch-cxx-xcode.in launch-cxx @ONLY)

        set(CMAKE_XCODE_ATTRIBUTE_CC         "${CMAKE_BINARY_DIR}/launch-c")
        set(CMAKE_XCODE_ATTRIBUTE_CXX        "${CMAKE_BINARY_DIR}/launch-cxx")
        set(CMAKE_XCODE_ATTRIBUTE_LD         "${CMAKE_BINARY_DIR}/launch-c")
        set(CMAKE_XCODE_ATTRIBUTE_LDPLUSPLUS "${CMAKE_BINARY_DIR}/launch-cxx")
    else()
        # Support Unix Makefiles and Ninja
        configure_file(scripts/launch-c.in   launch-c   @ONLY)
        configure_file(scripts/launch-cxx.in launch-cxx @ONLY)

        set(CMAKE_C_COMPILER_LAUNCHER        "${CMAKE_BINARY_DIR}/launch-c")
        set(CMAKE_CXX_COMPILER_LAUNCHER      "${CMAKE_BINARY_DIR}/launch-cxx")
    endif()

    execute_process(COMMAND chmod a+rx "${CMAKE_BINARY_DIR}/launch-c" "${CMAKE_BINARY_DIR}/launch-cxx")

    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        # ccache splits up the compile steps, so we end up with unused arguments in some steps.
        # Clang also thinks that ccache isn't interactive, so we explicitly need to enable color.
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments -fcolor-diagnostics")
    endif()
elseif(WIN32)
    find_program(CLCACHE_PROGRAM clcache)
    if(CLCACHE_PROGRAM)
        # clcache doesn't work with external pdb files
        # https://github.com/frerich/clcache/issues/30
        string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
        string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    endif()
else()
    message(STATUS "Can't find ccache — consider installing ccache to improve recompilation performance")
endif()

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/platform/${MBGL_PLATFORM}/config.cmake)
    message(ERROR "Can't find config.cmake file for platform ${MBGL_PLATFORM}")
endif()

include(platform/${MBGL_PLATFORM}/config.cmake)

if(WITH_EGL)
    add_definitions(-DMBGL_WITH_EGL=1)
endif()

if(USE_GLES2)
    add_definitions(-DMBGL_USE_GLES2=1)
endif()

if (COMMAND mbgl_filesource)
    include(cmake/filesource.cmake)
endif()

include(cmake/core.cmake)

if(COMMAND mbgl_platform_test)
    include(cmake/test.cmake)
    include(cmake/render-test.cmake)
endif()

if(COMMAND mbgl_platform_benchmark)
    include(cmake/benchmark.cmake)
endif()

if(COMMAND mbgl_platform_glfw)
    include(cmake/glfw.cmake)
endif()

if(COMMAND mbgl_platform_render)
    include(cmake/render.cmake)
endif()

if(COMMAND mbgl_platform_offline)
    include(cmake/offline.cmake)
endif()

if(WITH_NODEJS AND COMMAND mbgl_platform_node)
    include(cmake/node.cmake)
endif()

if(CMAKE_GENERATOR STREQUAL "Xcode")
    set_xcconfig_target_properties(mbgl-core)
    set_xcconfig_target_properties(mbgl-filesource)
    file(GENERATE
        OUTPUT "${CMAKE_BINARY_DIR}/config.xcconfig"
        INPUT "${CMAKE_SOURCE_DIR}/scripts/config.xcconfig.in"
    )
endif()
