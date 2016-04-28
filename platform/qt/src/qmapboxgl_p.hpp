#ifndef QMAPBOXGL_P_H
#define QMAPBOXGL_P_H

#include <mbgl/map/map.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/run_loop.hpp>

#include <QMapboxGL>
#include <QObject>
#include <QSize>

class QMapboxGLPrivate : public QObject, public mbgl::View
{
    Q_OBJECT

public:
    explicit QMapboxGLPrivate(QMapboxGL *q, const QMapboxGLSettings &);
    virtual ~QMapboxGLPrivate();

    // mbgl::View implementation.
    float getPixelRatio() const final;
    std::array<uint16_t, 2> getSize() const final;
    std::array<uint16_t, 2> getFramebufferSize() const final;

    void activate() final {}
    void deactivate() final {}
    void invalidate() final;
    void notifyMapChange(mbgl::MapChange change) final;

    mbgl::EdgeInsets margins;
    QSize size;

    QMapboxGL *q_ptr = nullptr;

    mbgl::util::RunLoop loop;

    std::unique_ptr<mbgl::DefaultFileSource> fileSourceObj;
    std::unique_ptr<mbgl::Map> mapObj;

    bool dirty = false;

public slots:
    void connectionEstablished();

signals:
    void needsRendering();
    void mapChanged(QMapboxGL::MapChange);
};

#endif // QMAPBOXGL_P_H
