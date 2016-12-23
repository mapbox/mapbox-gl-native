#pragma once

#include "qmapboxgl.hpp"

#include <mbgl/map/map.hpp>
#include <mbgl/map/backend.hpp>
#include <mbgl/map/view.hpp>
#include <mbgl/util/default_thread_pool.hpp>
#include <mbgl/storage/default_file_source.hpp>
#include <mbgl/util/geo.hpp>

#include <QObject>
#include <QSize>

class QMapboxGLPrivate : public QObject, public mbgl::View, public mbgl::Backend
{
    Q_OBJECT

public:
    explicit QMapboxGLPrivate(QMapboxGL *, const QMapboxGLSettings &, const QSize &size, qreal pixelRatio);
    virtual ~QMapboxGLPrivate();

    // mbgl::View implementation.
    float getPixelRatio() const;
    void bind() final;
    std::array<uint16_t, 2> getSize() const;
    std::array<uint16_t, 2> getFramebufferSize() const;

    void activate() final {}
    void deactivate() final {}
    void invalidate() final;
    void notifyMapChange(mbgl::MapChange) final;

    mbgl::EdgeInsets margins;
    QSize size { 0, 0 };
    QSize fbSize { 0, 0 };

    QMapboxGL *q_ptr { nullptr };

    std::unique_ptr<mbgl::DefaultFileSource> fileSourceObj;
    mbgl::ThreadPool threadPool;
    std::unique_ptr<mbgl::Map> mapObj;

    bool dirty { false };

    QOpenGLFramebufferObject *fbo { nullptr };

public slots:
    void connectionEstablished();

signals:
    void needsRendering();
    void mapChanged(QMapboxGL::MapChange);
    void copyrightsChanged(const QString &copyrightsHtml);
};
