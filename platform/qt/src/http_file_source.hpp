#pragma once

#include <mbgl/storage/http_file_source.hpp>
#include <mbgl/storage/resource.hpp>

#include <QMap>
#include <QNetworkAccessManager>
#include <QObject>
#include <QPair>
#include <QQueue>
#include <QUrl>
#include <QVector>

namespace mbgl {

class HTTPRequest;

class HTTPFileSource::Impl : public QObject
{
    Q_OBJECT

public:
    Impl();
    virtual ~Impl() = default;

    void request(HTTPRequest *);
    void cancel(HTTPRequest *);

public slots:
    void onReplyFinished();

private:
    QMap<QUrl, QPair<QNetworkReply *, QVector<HTTPRequest *>>> m_pending;
    QNetworkAccessManager *m_manager;
};

} // namespace mbgl
