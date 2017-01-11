#pragma once

#include <mbgl/storage/file_source.hpp>

namespace mbgl {

template <class> class Actor;
class Scheduler;
class Mailbox;

class AssetFileSource : public FileSource {
public:
    AssetFileSource(Scheduler&, const std::string& assetRoot);
    ~AssetFileSource() override;

    std::unique_ptr<AsyncRequest> request(const Resource&, Callback) override;

private:
    void respond(std::weak_ptr<Callback>, Response);

private:
    std::shared_ptr<Mailbox> mailbox;
    class Worker;
    const std::unique_ptr<Actor<Worker>> worker;
};

} // namespace mbgl
