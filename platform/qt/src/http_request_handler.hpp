#pragma once

#include <mbgl/storage/http_request_handler.hpp>
#include <mbgl/storage/resource.hpp>

#include <QMap>
#include <QNetworkAccessManager>
#include <QObject>
#include <QPair>
#include <QQueue>
#include <QSslConfiguration>
#include <QUrl>
#include <QVector>

namespace mbgl {
namespace storage {

class HTTPRequest;

class HTTPRequestHandler::Impl : public QObject
{
    Q_OBJECT

public:
    Impl();
    virtual ~Impl() = default;

    void request(HTTPRequest*);
    void cancel(HTTPRequest*);

public slots:
    void replyFinish(QNetworkReply* reply);

private:
    QMap<QUrl, QPair<QNetworkReply*, QVector<HTTPRequest*>>> m_pending;
    QNetworkAccessManager *m_manager;
    QSslConfiguration m_ssl;
};

} // namespace storage
} // namespace mbgl
