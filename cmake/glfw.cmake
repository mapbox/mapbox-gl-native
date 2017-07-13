add_executable(mbgl-glfw
    platform/glfw/main.cpp
)

target_sources(mbgl-glfw
    PRIVATE platform/glfw/glfw_view.hpp
    PRIVATE platform/glfw/glfw_view.cpp
    PRIVATE platform/glfw/settings_json.hpp
    PRIVATE platform/glfw/settings_json.cpp
    PRIVATE platform/default/mbgl/util/default_styles.hpp
)

# Our GL implementation is internal to mbgl-core, which causes the GL header to
# be included after GLFW's own header. They both attempt to define GLAPIENTRY,
# but unfortunately the GL header doesn't check if it was previously defined,
# causing a macro redefinition compiler error.
# There is no particular compiler warning flag to ignore this check on GCC
# neither it does accept ignoring '-Werror' via diagnostics pragmas. We can
# only suppress this by either replacing the header path inclusion from -I to
# -isystem, or completely suppressing errors. Until the former solution is not
# available, we'll suppress the errors from that definition file.
set_source_files_properties(platform/glfw/glfw_view.cpp PROPERTIES COMPILE_FLAGS -Wno-error)

target_compile_options(mbgl-glfw
    PRIVATE -fvisibility-inlines-hidden
)

target_include_directories(mbgl-glfw
    PRIVATE platform/default
)

target_link_libraries(mbgl-glfw
    PRIVATE mbgl-core
)

target_add_mason_package(mbgl-glfw PRIVATE cheap-ruler)
target_add_mason_package(mbgl-glfw PRIVATE geojson)
target_add_mason_package(mbgl-glfw PRIVATE geometry)
target_add_mason_package(mbgl-glfw PRIVATE glfw)
target_add_mason_package(mbgl-glfw PRIVATE variant)

mbgl_platform_glfw()

create_source_groups(mbgl-glfw)

xcode_create_scheme(
    TARGET mbgl-glfw
    OPTIONAL_ARGS
        "--style=file.json"
        "--lon=0"
        "--lat=0"
        "--zoom=1"
        "--bearing=0"
        "--pitch=0"
        "--fullscreen"
        "--benchmark"
)
