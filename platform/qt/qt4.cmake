find_package(Qt4 REQUIRED)

set(MBGL_QT_CORE_LIBRARIES
    PUBLIC Qt4::QtCore
    PUBLIC Qt4::QtGui
    PUBLIC Qt4::QtOpenGL
)

set(MBGL_QT_FILESOURCE_LIBRARIES
    PUBLIC Qt4::QtNetwork
    PUBLIC Qt4::QtSql
)

target_compile_options(qmapboxgl
    PRIVATE -Wno-inconsistent-missing-override
)

target_link_libraries(mbgl-qt
    PRIVATE Qt4::QtOpenGL
)
