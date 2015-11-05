#include <mbgl/storage/frontline_file_source.hpp>
#include <mbgl/storage/request.hpp>
#include <mbgl/storage/response.hpp>

#include <mbgl/util/thread.hpp>
#include <mbgl/util/exception.hpp>
#include <mbgl/util/io.hpp>

#include <cassert>

namespace mbgl {

FrontlineFileSource::FrontlineFileSource()
    : thread(std::make_unique<util::Thread<Impl>>(
          util::ThreadContext{ "FrontlineFileSource", util::ThreadType::Unknown, util::ThreadPriority::Low })) {
}

FrontlineFileSource::~FrontlineFileSource() {
    MBGL_VERIFY_THREAD(tid);
}

class FrontlineFileSource::Impl {
public:
    void handleRequest(Request* req);
    void cancelRequest(Request* req);
};

void FrontlineFileSource::Impl::handleRequest(Request* req) {
    std::shared_ptr<Response> res = std::make_shared<Response>();

    try {
        res->data = std::make_shared<const std::string>(std::move(util::read_file("/tmp/foo.png")));
    } catch (const std::exception& err) {
        res->error = std::make_unique<Response::Error>(Response::Error::Reason::Other, err.what());
    }

    req->notify(res);
}

void FrontlineFileSource::Impl::cancelRequest(Request* req) {
    // no-op
    req->destruct();
}

bool FrontlineFileSource::handlesResource(const Resource& res) {
    if (res.kind == Resource::Kind::Tile) {
        return (res.url.find(".png") != std::string::npos);
    }
    return false;
}

Request* FrontlineFileSource::request(const Resource& resource, uv_loop_t* l, Callback callback) {
    assert(l);

    if (!callback) {
        throw util::MisuseException("FileSource callback can't be empty");
    }

    std::string url;

    switch (resource.kind) {
        case Resource::Kind::Tile:
            url = resource.url;
            break;

        default:
            url = "";
    }

    if (url.length()) {
        auto req = new Request({ resource.kind, url }, l, std::move(callback));
        thread->invoke(&Impl::handleRequest, req);
        return req;
    } else {
        return nullptr;
    }
}

void FrontlineFileSource::cancel(Request* req) {
    assert(req);
    req->cancel();
    thread->invoke(&Impl::cancelRequest, req);
}

} // namespace mbgl
