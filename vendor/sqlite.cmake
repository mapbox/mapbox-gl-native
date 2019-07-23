if(TARGET mbgl-vendor-sqlite)
    return()
endif()

add_library(mbgl-vendor-sqlite STATIC
    ${CMAKE_CURRENT_LIST_DIR}/sqlite/src/sqlite3.c
)

include(CheckSymbolExists)
check_symbol_exists("strerror_r" "string.h" MBGL_SQLITE3_HAVE_STRERROR_R)

if(MBGL_SQLITE3_HAVE_STRERROR_R)
    target_compile_definitions(mbgl-vendor-sqlite PRIVATE
        HAVE_STRERROR_R
    )
endif()

# So we don't need to link with -ldl
target_compile_definitions(mbgl-vendor-sqlite PRIVATE
    SQLITE_OMIT_LOAD_EXTENSION
    SQLITE_THREADSAFE
)

target_include_directories(mbgl-vendor-sqlite SYSTEM INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/sqlite/include
)
