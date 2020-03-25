find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW glfw3 REQUIRED)

add_executable(
    mbgl-glfw
    ${PROJECT_SOURCE_DIR}/platform/glfw/main.cpp
    ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_view.cpp
    ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_renderer_frontend.cpp
    ${PROJECT_SOURCE_DIR}/platform/glfw/settings_json.cpp
    ${PROJECT_SOURCE_DIR}/platform/glfw/test_writer.cpp
    ${PROJECT_SOURCE_DIR}/platform/default/src/mbgl/map/map_snapshotter.cpp
)

set_property(
    SOURCE ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_view.cpp
    PROPERTY COMPILE_DEFINITIONS MAPBOX_PUCK_ASSETS_PATH=\"${PROJECT_SOURCE_DIR}/platform/glfw/assets/\"
)

if(MBGL_WITH_OPENGL)
    target_sources(
        mbgl-glfw
        PRIVATE ${PROJECT_SOURCE_DIR}/platform/glfw/glfw_gl_backend.cpp
    )
endif()

target_include_directories(
    mbgl-glfw
    PRIVATE
        ${GLFW_INCLUDE_DIRS}
        # For /platform/default/src/mbgl/map/map_snapshotter.hpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src
)

# Use target_link_directories when we move away from CMake 3.10.
target_link_libraries(
    mbgl-glfw
    PRIVATE $<$<BOOL:${GLFW_LIBRARY_DIRS}>:-L${GLFW_LIBRARY_DIRS}>
)

target_link_libraries(
    mbgl-glfw
    PRIVATE
        ${GLFW_LIBRARIES}
        Mapbox::Base::Extras::args
        Mapbox::Base::Extras::filesystem
        Mapbox::Base::Extras::rapidjson
        Mapbox::Map
        mbgl-compiler-options
        Mapbox::Base::cheap-ruler-cpp
)

if(MBGL_WITH_OPENGL)
    target_link_libraries(
        mbgl-glfw
        PRIVATE OpenGL::GL
    )
endif()

set_property(TARGET mbgl-glfw PROPERTY FOLDER Executables)

install(TARGETS mbgl-glfw RUNTIME DESTINATION bin)
