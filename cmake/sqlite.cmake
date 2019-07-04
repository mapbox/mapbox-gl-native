add_vendor_target(sqlite STATIC)

target_compile_definitions(sqlite
    PRIVATE "-DVERSION=\"3.24.0\""
    PRIVATE "-DSTDC_HEADERS=int"
    PRIVATE "-DHAVE_SYS_TYPES_H=1"
    PRIVATE "-DHAVE_SYS_STAT_H=1"
    PRIVATE "-DHAVE_STDLIB_H=1"
    PRIVATE "-DHAVE_STRING_H=1"
    PRIVATE "-DHAVE_MEMORY_H=1"
    PRIVATE "-DHAVE_STRINGS_H=1"
    PRIVATE "-DHAVE_INTTYPES_H=1"
    PRIVATE "-DHAVE_STDINT_H=1"
    PRIVATE "-DHAVE_UNISTD_H=1"
    PRIVATE "-DHAVE_DLFCN_H=1"
    PRIVATE "-DHAVE_FDATASYNC=1"
    PRIVATE "-DHAVE_USLEEP=1"
    PRIVATE "-DHAVE_LOCALTIME_R=1"
    PRIVATE "-DHAVE_GMTIME_R=1"
    PRIVATE "-DHAVE_DECL_STRERROR_R=1"
    PRIVATE "-DHAVE_STRERROR_R=1"
    PRIVATE "-DHAVE_ZLIB_H=1"
    PRIVATE "-D_REENTRANT=1"
    PRIVATE "-DSQLITE_THREADSAFE=1"
    PRIVATE "-DSQLITE_HAVE_ZLIB"

    # These are disabled on purpose, since we're not using these features.
    # PRIVATE "-DSQLITE_ENABLE_FTS5"
    # PRIVATE "-DSQLITE_ENABLE_JSON1"
    # PRIVATE "-DSQLITE_ENABLE_FTS3"
    # PRIVATE "-DSQLITE_ENABLE_RTREE"
)

target_compile_options(sqlite
    PRIVATE "-Wno-int-conversion"
    PRIVATE "-Wno-implicit-fallthrough"
)

if(CMAKE_COMPILER_IS_GNUCXX)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8.0)
        target_compile_options(sqlite PRIVATE "-Wno-cast-function-type")
    else()
        target_compile_options(sqlite PRIVATE "-Wno-bad-function-cast")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    target_compile_options(sqlite PRIVATE "-Wno-cast-function-type")
endif()
