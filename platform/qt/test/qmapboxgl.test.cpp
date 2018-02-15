#include "qmapboxgl.test.hpp"

#include <mbgl/util/io.hpp>

#include <QMapbox>

// We're using QGLFramebufferObject, which is only available in Qt 5 and up.
#if QT_VERSION >= 0x050000

#include <QOpenGLContext>
#include <QOpenGLFunctions>

QMapboxGLTest::QMapboxGLTest() : size(512, 512), fbo((assert(widget.context()->isValid()), widget.makeCurrent(), size)), map(nullptr, settings, size) {
    connect(&map, SIGNAL(mapChanged(QMapboxGL::MapChange)),
            this, SLOT(onMapChanged(QMapboxGL::MapChange)));
    connect(&map, SIGNAL(needsRendering()),
            this, SLOT(onNeedsRendering()));
    map.resize(fbo.size());
    map.setFramebufferObject(fbo.handle(), fbo.size());
    map.setCoordinateZoom(QMapbox::Coordinate(60.170448, 24.942046), 14);
}

void QMapboxGLTest::runUntil(QMapboxGL::MapChange status) {
    changeCallback = [&](QMapboxGL::MapChange change) {
        if (change == status) {
            qApp->exit();
            changeCallback = nullptr;
        }
    };

    qApp->exec();
}

void QMapboxGLTest::onMapChanged(QMapboxGL::MapChange change) {
    if (changeCallback) {
        changeCallback(change);
    }
}

void QMapboxGLTest::onNeedsRendering() {
    widget.makeCurrent();
    fbo.bind();
    QOpenGLContext::currentContext()->functions()->glViewport(0, 0, fbo.width(), fbo.height());
    map.render();
}


TEST_F(QMapboxGLTest, TEST_DISABLED_ON_CI(styleJson)) {
    QString json = QString::fromStdString(
        mbgl::util::read_file("test/fixtures/resources/style_vector.json"));

    map.setStyleJson(json);
    ASSERT_EQ(map.styleJson(), json);
    runUntil(QMapboxGL::MapChangeDidFinishLoadingMap);

    map.setStyleJson("invalid json");
    runUntil(QMapboxGL::MapChangeDidFailLoadingMap);

    map.setStyleJson("\"\"");
    runUntil(QMapboxGL::MapChangeDidFailLoadingMap);

    map.setStyleJson(QString());
    runUntil(QMapboxGL::MapChangeDidFailLoadingMap);
}

TEST_F(QMapboxGLTest, TEST_DISABLED_ON_CI(styleUrl)) {
    QString url(QMapbox::defaultStyles()[0].first);

    map.setStyleUrl(url);
    ASSERT_EQ(map.styleUrl(), url);
    runUntil(QMapboxGL::MapChangeDidFinishLoadingMap);

    map.setStyleUrl("invalid://url");
    runUntil(QMapboxGL::MapChangeDidFailLoadingMap);

    map.setStyleUrl(QString());
    runUntil(QMapboxGL::MapChangeDidFailLoadingMap);
}

#endif
