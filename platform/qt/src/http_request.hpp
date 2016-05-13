#pragma once

#include <mbgl/storage/http_request_handler.hpp>
#include <mbgl/util/async_request.hpp>

#include <QNetworkRequest>
#include <QUrl>

class QNetworkReply;

namespace mbgl {

class Response;

namespace storage {

class HTTPRequest : public AsyncRequest
{
public:
    HTTPRequest(HTTPRequestHandler::Impl*, const Resource&, HTTPRequestHandler::Callback);
    virtual ~HTTPRequest();

    QUrl requestUrl() const;
    QNetworkRequest networkRequest() const;

    void handleNetworkReply(QNetworkReply *reply);

private:
    HTTPRequestHandler::Impl* m_context;
    Resource m_resource;
    HTTPRequestHandler::Callback m_callback;

    bool m_handled = false;
};

} // namespace storage
} // namespace mbgl
