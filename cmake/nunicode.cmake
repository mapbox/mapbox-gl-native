add_vendor_target(nunicode STATIC)

target_compile_definitions(nunicode
    PRIVATE "-DNU_BUILD_STATIC"
)
