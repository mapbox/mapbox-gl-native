find_package(Qt4 REQUIRED)

set(MBGL_QT_LIBRARIES
    PRIVATE Qt4::QtCore
    PRIVATE Qt4::QtGui
    PRIVATE Qt4::QtNetwork
    PRIVATE Qt4::QtOpenGL
    PRIVATE Qt4::QtSql
)

set(MBGL_QT_TEST_LIBRARIES
    PRIVATE Qt4::QtCore
    PRIVATE Qt4::QtOpenGL
)

target_compile_options(qmapboxgl
    PRIVATE -Wno-inconsistent-missing-override
)

target_link_libraries(qmapboxgl
    PRIVATE mbgl-core
    PRIVATE Qt4::QtCore
    PRIVATE Qt4::QtGui
    PRIVATE Qt4::QtOpenGL
)

target_link_libraries(mbgl-qt
    PRIVATE qmapboxgl
    PRIVATE Qt4::QtGui
    PRIVATE Qt4::QtOpenGL
)
