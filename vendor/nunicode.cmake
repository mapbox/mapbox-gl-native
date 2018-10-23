add_library(nunicode STATIC
    ${GIT_ROOT}/vendor/nunicode/src/libnu/ducet.c
    ${GIT_ROOT}/vendor/nunicode/src/libnu/strcoll.c
    ${GIT_ROOT}/vendor/nunicode/src/libnu/strings.c
    ${GIT_ROOT}/vendor/nunicode/src/libnu/tolower.c
    ${GIT_ROOT}/vendor/nunicode/src/libnu/tounaccent.c
    ${GIT_ROOT}/vendor/nunicode/src/libnu/toupper.c
    ${GIT_ROOT}/vendor/nunicode/src/libnu/utf8.c
)

target_compile_definitions(nunicode PRIVATE
    NU_BUILD_STATIC
)

target_include_directories(nunicode SYSTEM PUBLIC
    ${GIT_ROOT}/vendor/nunicode/include
)
