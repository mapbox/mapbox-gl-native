#include <mbgl/storage/file_source_request.hpp>

#include <mbgl/actor/mailbox.hpp>
#include <mbgl/util/run_loop.hpp>

namespace mbgl {

FileSourceRequest::FileSourceRequest(FileSource::Callback&& callback)
    : responseCallback(callback)
    , mailbox(std::make_shared<Mailbox>(*util::RunLoop::Get())) {
}

FileSourceRequest::~FileSourceRequest() {
    if (cancelCallback) {
        cancelCallback();
    }

    mailbox->close();
}

void FileSourceRequest::onCancel(std::function<void()>&& callback) {
    cancelCallback = std::move(callback);
}

void FileSourceRequest::setResponse(const Response& response) {
    // Copy, because calling the callback will sometimes self
    // destroy this object. We cannot move because this method
    // can be called more than one.
    auto callback = responseCallback;
    callback(response);
}

ActorRef<FileSourceRequest> FileSourceRequest::actor() {
    return ActorRef<FileSourceRequest>(*this, mailbox);
}

} // namespace mbgl
