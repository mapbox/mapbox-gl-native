add_library(icu STATIC
    ${GIT_ROOT}/vendor/icu/src/cmemory.cpp
    ${GIT_ROOT}/vendor/icu/src/cstring.cpp
    ${GIT_ROOT}/vendor/icu/src/ubidi.cpp
    ${GIT_ROOT}/vendor/icu/src/ubidi_props.cpp
    ${GIT_ROOT}/vendor/icu/src/ubidiln.cpp
    ${GIT_ROOT}/vendor/icu/src/ubidiwrt.cpp
    ${GIT_ROOT}/vendor/icu/src/uchar.cpp
    ${GIT_ROOT}/vendor/icu/src/udataswp.cpp
    ${GIT_ROOT}/vendor/icu/src/uinvchar.cpp
    ${GIT_ROOT}/vendor/icu/src/umath.cpp
    ${GIT_ROOT}/vendor/icu/src/ushape.cpp
    ${GIT_ROOT}/vendor/icu/src/ustring.cpp
    ${GIT_ROOT}/vendor/icu/src/utf_impl.cpp
    ${GIT_ROOT}/vendor/icu/src/utrie2.cpp
    ${GIT_ROOT}/vendor/icu/src/utypes.cpp
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

target_include_directories(icu SYSTEM PUBLIC
    ${GIT_ROOT}/vendor/icu/include
)
