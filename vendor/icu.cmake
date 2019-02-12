add_library(icu STATIC
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/cmemory.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/cstring.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/ubidi.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/ubidi_props.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/ubidiln.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/ubidiwrt.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/uchar.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/udataswp.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/uinvchar.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/umath.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/ushape.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/ustring.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/utf_impl.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/utrie2.cpp
    ${CMAKE_SOURCE_DIR}/vendor/icu/src/utypes.cpp
)

target_compile_definitions(icu PRIVATE
    UCONFIG_NO_BREAK_ITERATION=1
    UCONFIG_NO_LEGACY_CONVERSION=1
    U_CHARSET_IS_UTF8=1
    U_CHAR_TYPE=uint_least16_t
    U_HAVE_ATOMIC=1
    U_HAVE_STRTOD_L=0
    _REENTRANT
)

target_compile_options(icu PRIVATE
    -Wno-error
    -Wno-shorten-64-to-32
)

target_include_directories(icu SYSTEM PUBLIC
    ${CMAKE_SOURCE_DIR}/vendor/icu/include
)
