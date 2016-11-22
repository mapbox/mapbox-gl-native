#include <mbgl/test/util.hpp>
#include <mbgl/util/io.hpp>

#include <QApplication>
#include <QGLWidget>
#include <QMapbox>
#include <QMapboxGL>

class QMapboxGLTest : public QObject, public ::testing::Test {
    Q_OBJECT

public:
    QMapboxGLTest() : app(argc, const_cast<char**>(&argv)), map(nullptr, settings) {
        connect(&map, SIGNAL(mapChanged(QMapbox::MapChange)),
                this, SLOT(onMapChanged(QMapbox::MapChange)));
        connect(&map, SIGNAL(needsRendering()),
                this, SLOT(onNeedsRendering()));

        widget.makeCurrent();
        QMapbox::initializeGLExtensions();

        map.resize(QSize(512, 512));
        map.setCoordinateZoom(QMapbox::Coordinate(60.170448, 24.942046), 14);
    }

    void runUntil(QMapbox::MapChange status) {
        changeCallback = [&](QMapbox::MapChange change) {
            if (change == status) {
                app.exit();
                changeCallback = nullptr;
            }
        };

        app.exec();
    }

private:
    int argc = 1;
    const char* argv = "mbgl-test";

    QApplication app;
    QGLWidget widget;

protected:
    QMapboxGLSettings settings;
    QMapboxGL map;

    std::function<void(QMapbox::MapChange)> changeCallback;

private slots:
    void onMapChanged(QMapbox::MapChange change) {
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
    runUntil(QMapbox::MapChangeDidFinishLoadingMap);

    map.setStyleJson("invalid json");
    runUntil(QMapbox::MapChangeDidFailLoadingMap);

    map.setStyleJson("\"\"");
    runUntil(QMapbox::MapChangeDidFailLoadingMap);

    map.setStyleJson(QString());
    runUntil(QMapbox::MapChangeDidFailLoadingMap);
}

TEST_F(QMapboxGLTest, TEST_DISABLED_ON_CI(styleUrl)) {
    QString url(QMapbox::defaultStyles()[0].first);

    map.setStyleUrl(url);
    ASSERT_EQ(map.styleUrl(), url);
    runUntil(QMapbox::MapChangeDidFinishLoadingMap);

    map.setStyleUrl("invalid://url");
    runUntil(QMapbox::MapChangeDidFailLoadingMap);

    map.setStyleUrl(QString());
    runUntil(QMapbox::MapChangeDidFailLoadingMap);
}

#include "qmapboxgl.moc"
