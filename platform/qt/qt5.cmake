find_package(Qt5Core     REQUIRED)
find_package(Qt5Gui      REQUIRED)
find_package(Qt5Location REQUIRED)
find_package(Qt5Network  REQUIRED)
find_package(Qt5OpenGL   REQUIRED)
find_package(Qt5Quick    REQUIRED)
find_package(Qt5Widgets  REQUIRED)
find_package(Qt5Sql      REQUIRED)

set(MBGL_QT_LIBRARIES
    PRIVATE Qt5::Core
    PRIVATE Qt5::Gui
    PRIVATE Qt5::Location
    PRIVATE Qt5::Network
    PRIVATE Qt5::OpenGL
    PRIVATE Qt5::Quick
    PRIVATE Qt5::Sql
)

target_sources(qmapboxgl
    PRIVATE platform/qt/src/qquickmapboxgl.cpp
    PRIVATE platform/qt/src/qquickmapboxgl.hpp
    PRIVATE platform/qt/src/qquickmapboxglmapparameter.cpp
    PRIVATE platform/qt/src/qquickmapboxglmapparameter.hpp
    PRIVATE platform/qt/src/qquickmapboxglrenderer.cpp
    PRIVATE platform/qt/src/qquickmapboxglrenderer.hpp
)

target_link_libraries(qmapboxgl
    PRIVATE mbgl-core
    PRIVATE Qt5::Core
    PRIVATE Qt5::Gui
    PRIVATE Qt5::Location
    PRIVATE Qt5::OpenGL
    PRIVATE Qt5::Quick
    PRIVATE Qt5::Sql
)

target_link_libraries(mbgl-qt
    PRIVATE qmapboxgl
    PRIVATE Qt5::OpenGL
    PRIVATE Qt5::Widgets
)

# QtQuick app
add_executable(mbgl-qt-qml
    platform/qt/qmlapp/main.cpp
    platform/qt/qmlapp/qml.qrc
    platform/qt/resources/common.qrc
)

target_link_libraries(mbgl-qt-qml
    PRIVATE qmapboxgl
    PRIVATE Qt5::Location
    PRIVATE Qt5::Quick
)
