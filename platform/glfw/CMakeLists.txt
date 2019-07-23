find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW glfw3 REQUIRED)

add_executable(mbgl-glfw
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/glfw_view.cpp
    ${CMAKE_CURRENT_LIST_DIR}/glfw_gl_backend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/glfw_renderer_frontend.cpp
    ${CMAKE_CURRENT_LIST_DIR}/settings_json.cpp
)

target_include_directories(mbgl-glfw PRIVATE
    ${GLFW_INCLUDE_DIRS}
)

include(${PROJECT_SOURCE_DIR}/vendor/cheap-ruler-cpp.cmake)

# Use target_link_directories when
# we move away from CMake 3.10.
target_link_libraries(mbgl-glfw PRIVATE
    $<$<BOOL:${GLFW_LIBRARY_DIRS}>:-L${GLFW_LIBRARY_DIRS}>
)

target_link_libraries(mbgl-glfw PRIVATE
    ${GLFW_LIBRARIES}
    Mapbox::Base::Extras::args
    Mapbox::Map
    OpenGL::GL
    mbgl-vendor-cheap-ruler-cpp
)

set_property(TARGET mbgl-glfw PROPERTY FOLDER Executables)
