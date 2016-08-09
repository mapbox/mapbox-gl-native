find_package(Qt4 REQUIRED)

QT4_ADD_RESOURCES(MBGL_QT_FILES platform/qt/qmapbox.qrc)

set(MBGL_QT_LIBRARIES
    PRIVATE Qt4::QtCore
    PRIVATE Qt4::QtGui
    PRIVATE Qt4::QtNetwork
    PRIVATE Qt4::QtOpenGL
)

add_library(qmapboxgl SHARED)

target_link_libraries(qmapboxgl
    PUBLIC mbgl-core
    PRIVATE Qt4::QtCore
)

target_link_libraries(mbgl-qt
    PRIVATE qmapboxgl
    PRIVATE Qt4::QtGui
    PRIVATE Qt4::QtOpenGL
)
