#include <mbgl/test/util.hpp>
#include <mbgl/util/io.hpp>

#include <QApplication>
#include <QGLWidget>
#include <QMapbox>
#include <QMapboxGL>

class QMapboxGLTest : public QObject, public ::testing::Test {
    Q_OBJECT

public:
    QMapboxGLTest() : map(nullptr, settings) {
        connect(&map, SIGNAL(mapChanged(QMapboxGL::MapChange)),
                this, SLOT(onMapChanged(QMapboxGL::MapChange)));
        connect(&map, SIGNAL(needsRendering()),
                this, SLOT(onNeedsRendering()));

        widget.makeCurrent();
        QMapbox::initializeGLExtensions();

        map.resize(QSize(512, 512), QSize(512, 512));
        map.setCoordinateZoom(QMapbox::Coordinate(60.170448, 24.942046), 14);
    }

    void runUntil(QMapboxGL::MapChange status) {
        changeCallback = [&](QMapboxGL::MapChange change) {
            if (change == status) {
                qApp->exit();
                changeCallback = nullptr;
            }
        };

        qApp->exec();
    }

private:
    QGLWidget widget;

protected:
    QMapboxGLSettings settings;
    QMapboxGL map;

    std::function<void(QMapboxGL::MapChange)> changeCallback;

private slots:
    void onMapChanged(QMapboxGL::MapChange change) {
        if (changeCallback) {
            changeCallback(change);
        }
    };

    void onNeedsRendering() {
        map.render();
    };
};

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

#include "qmapboxgl.moc"
