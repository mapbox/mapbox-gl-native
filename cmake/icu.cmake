add_vendor_target(icu STATIC)

target_compile_definitions(icu
    PRIVATE "-D_REENTRANT"
    PRIVATE "-DU_HAVE_ATOMIC=1"
    PRIVATE "-DU_HAVE_STRTOD_L=0"
    PRIVATE "-DU_CHARSET_IS_UTF8=1"
    PRIVATE "-DU_CHAR_TYPE=uint_least16_t"
    PRIVATE "-DUCONFIG_NO_LEGACY_CONVERSION=1"
    PRIVATE "-DUCONFIG_NO_BREAK_ITERATION=1"
)

target_compile_options(icu
    PRIVATE "-Wno-shadow"
)

if(MBGL_PLATFORM STREQUAL "android")
    target_compile_options(icu
        PRIVATE "-Wno-tautological-unsigned-enum-zero-compare"
    )
elseif(MBGL_PLATFORM STREQUAL "ios")
    target_compile_options(icu
        PRIVATE "-Wno-shorten-64-to-32"
    )
endif()

