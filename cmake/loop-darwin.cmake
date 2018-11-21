# Modify platform/darwin/loop-files.txt to change the source files for this target.
load_sources_list(MBGL_LOOP_DARWIN_FILES platform/darwin/loop-files.txt)
add_library(mbgl-loop-darwin STATIC ${MBGL_LOOP_DARWIN_FILES})

target_include_directories(mbgl-loop-darwin
    PRIVATE include
    PRIVATE src
)

create_source_groups(mbgl-loop-darwin)

set_target_properties(mbgl-loop-darwin PROPERTIES FOLDER "Core")

initialize_xcode_cxx_build_settings(mbgl-loop-darwin)

xcode_create_scheme(TARGET mbgl-loop-darwin)
