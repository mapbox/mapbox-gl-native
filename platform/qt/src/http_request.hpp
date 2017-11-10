#pragma once

#include <mbgl/storage/http_file_source.hpp>
#include <mbgl/util/async_request.hpp>

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

namespace mbgl {

class Response;

class HTTPRequest : public AsyncRequest
{
public:
    HTTPRequest(HTTPFileSource::Impl *, const Resource&, FileSource::Callback);
    virtual ~HTTPRequest();

    QUrl requestUrl() const;
    QNetworkRequest networkRequest() const;

    void handleNetworkReply(QNetworkReply *, const QByteArray& data);

private:
    HTTPFileSource::Impl* m_context;
    Resource m_resource;
    FileSource::Callback m_callback;

    bool m_handled = false;
};

} // namespace mbgl
