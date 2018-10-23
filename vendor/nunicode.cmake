add_library(nunicode STATIC
    ${CMAKE_SOURCE_DIR}/vendor/nunicode/src/libnu/ducet.c
    ${CMAKE_SOURCE_DIR}/vendor/nunicode/src/libnu/strcoll.c
    ${CMAKE_SOURCE_DIR}/vendor/nunicode/src/libnu/strings.c
    ${CMAKE_SOURCE_DIR}/vendor/nunicode/src/libnu/tolower.c
    ${CMAKE_SOURCE_DIR}/vendor/nunicode/src/libnu/tounaccent.c
    ${CMAKE_SOURCE_DIR}/vendor/nunicode/src/libnu/toupper.c
    ${CMAKE_SOURCE_DIR}/vendor/nunicode/src/libnu/utf8.c
)

target_compile_definitions(nunicode PRIVATE
    NU_BUILD_STATIC
)

target_compile_options(nunicode PRIVATE
    -Wno-error
)

target_include_directories(nunicode SYSTEM PUBLIC
    ${CMAKE_SOURCE_DIR}/vendor/nunicode/include
)
