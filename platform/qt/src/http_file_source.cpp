#include "http_file_source.hpp"
#include "http_request.hpp"

#include <mbgl/util/logging.hpp>

#include <QByteArray>
#include <QDir>
#include <QNetworkProxyFactory>
#include <QNetworkReply>
#include <QSslConfiguration>

namespace mbgl {

HTTPFileSource::Impl::Impl() : m_manager(new QNetworkAccessManager(this))
{
    QNetworkProxyFactory::setUseSystemConfiguration(true);
}

void HTTPFileSource::Impl::request(HTTPRequest* req)
{
    QUrl url = req->requestUrl();

    QPair<QNetworkReply*, QVector<HTTPRequest*>>& data = m_pending[url];
    QVector<HTTPRequest*>& requestsVector = data.second;
    requestsVector.append(req);

    if (requestsVector.size() > 1) {
        return;
    }

    QNetworkRequest networkRequest = req->networkRequest();
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

    data.first = m_manager->get(networkRequest);
    connect(data.first, SIGNAL(finished()), this, SLOT(onReplyFinished()));
    connect(data.first, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onReplyFinished()));
}

void HTTPFileSource::Impl::cancel(HTTPRequest* req)
{
    QUrl url = req->requestUrl();

    auto it = m_pending.find(url);
    if (it == m_pending.end()) {
        return;
    }

    QPair<QNetworkReply*, QVector<HTTPRequest*>>& data = it.value();
    QNetworkReply* reply = data.first;
    QVector<HTTPRequest*>& requestsVector = data.second;

    for (int i = 0; i < requestsVector.size(); ++i) {
        if (req == requestsVector.at(i)) {
            requestsVector.remove(i);
            break;
        }
    }

    if (requestsVector.empty()) {
        m_pending.erase(it);
        reply->abort();
    }
}

void HTTPFileSource::Impl::onReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
    const QUrl& url = reply->request().url();

    auto it = m_pending.find(url);
    if (it == m_pending.end()) {
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QVector<HTTPRequest*>& requestsVector = it.value().second;

    // Cannot use the iterator to walk the requestsVector
    // because calling handleNetworkReply() might get
    // requests added to the requestsVector.
    while (!requestsVector.isEmpty()) {
        requestsVector.takeFirst()->handleNetworkReply(reply, data);
    }

    m_pending.erase(it);
    reply->deleteLater();
}

HTTPFileSource::HTTPFileSource()
    : impl(std::make_unique<Impl>()) {
}

HTTPFileSource::~HTTPFileSource() = default;

std::unique_ptr<AsyncRequest> HTTPFileSource::request(const Resource& resource, Callback callback)
{
    return std::make_unique<HTTPRequest>(impl.get(), resource, callback);
}

} // namespace mbgl
