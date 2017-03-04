find_package(Qt5Core     REQUIRED)
find_package(Qt5Gui      REQUIRED)
find_package(Qt5Location REQUIRED)
find_package(Qt5Network  REQUIRED)
find_package(Qt5OpenGL   REQUIRED)
find_package(Qt5Widgets  REQUIRED)
find_package(Qt5Sql      REQUIRED)

set(MBGL_QT_LIBRARIES
    PRIVATE Qt5::Core
    PRIVATE Qt5::Gui
    PRIVATE Qt5::Network
    PRIVATE Qt5::Sql
)

set(MBGL_QT_TEST_LIBRARIES
    PRIVATE Qt5::Core
    PRIVATE Qt5::Gui
    PRIVATE Qt5::Widgets
    PRIVATE Qt5::OpenGL
)

target_link_libraries(qmapboxgl
    PRIVATE mbgl-core
    PRIVATE Qt5::Core
    PRIVATE Qt5::Gui
    PRIVATE Qt5::Location
    PRIVATE Qt5::Sql
)

target_link_libraries(mbgl-qt
    PRIVATE qmapboxgl
    PRIVATE Qt5::OpenGL
    PRIVATE Qt5::Widgets
)
