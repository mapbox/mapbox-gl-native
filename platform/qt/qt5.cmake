find_package(Qt5Core     REQUIRED)
find_package(Qt5Gui      REQUIRED)
find_package(Qt5Network  REQUIRED)
find_package(Qt5OpenGL   REQUIRED)
find_package(Qt5Widgets  REQUIRED)
find_package(Qt5Sql      REQUIRED)

# Qt5 always build OpenGL ES2 which is the compatibility
# mode. Qt5 will take care of translating the desktop
# version of OpenGL to ES2.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMBGL_USE_GLES2")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -DMBGL_USE_GLES2")

set(MBGL_QT_CORE_LIBRARIES
    PUBLIC Qt5::Core
    PUBLIC Qt5::Gui
    PUBLIC Qt5::OpenGL
)

set(MBGL_QT_FILESOURCE_LIBRARIES
    PUBLIC Qt5::Network
    PUBLIC Qt5::Sql
)

target_link_libraries(mbgl-qt
    PRIVATE Qt5::Widgets
)
