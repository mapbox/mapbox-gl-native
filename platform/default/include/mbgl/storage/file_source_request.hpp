#pragma once

#include <mbgl/actor/actor_ref.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/util/async_request.hpp>

#include <memory>
#include <functional>

namespace mbgl {

class Mailbox;

class FileSourceRequest : public AsyncRequest {
public:
    FileSourceRequest(FileSource::Callback&& callback);
    ~FileSourceRequest() final;

    void onCancel(std::function<void()>&& callback);
    void setResponse(const Response& res);

    ActorRef<FileSourceRequest> actor();

private:
    FileSource::Callback responseCallback = nullptr;
    std::function<void()> cancelCallback = nullptr;

    std::shared_ptr<Mailbox> mailbox;
};

} // namespace mbgl
