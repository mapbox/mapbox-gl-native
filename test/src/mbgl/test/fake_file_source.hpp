#pragma once

#include <mbgl/storage/file_source.hpp>

#include <algorithm>
#include <list>

namespace mbgl {

/*
   FakeFileSource is similar to StubFileSource, but it follows a post hoc, "push" model rather than
   a pre hoc, "pull" model. You pass it to code that makes requests, it records what requests are
   made, then you can examine them, make assertions about them, and respond as desired.

   This is particularly useful if you want to simulate multiple responses, e.g. as part of a resource
   revalidation flow. StubFileSource allows only a single response.
*/
class FakeFileSource : public FileSource {
public:
    class FakeFileRequest : public AsyncRequest {
    public:
        Resource resource;
        Callback callback;

        std::list<FakeFileRequest*>& list;
        std::list<FakeFileRequest*>::iterator link;

        FakeFileRequest(Resource resource_, Callback callback_, std::list<FakeFileRequest*>& list_)
            : resource(std::move(resource_)),
              callback(std::move(callback_)),
              list(list_),
              link((list.push_back(this), std::prev(list.end()))) {
        }

        ~FakeFileRequest() override {
            list.erase(link);
        }
    };

    std::unique_ptr<AsyncRequest> request(const Resource& resource, Callback callback) override {
        return std::make_unique<FakeFileRequest>(resource, callback, requests);
    }

    bool respond(Resource::Kind kind, const Response& response) {
        auto it = std::find_if(requests.begin(), requests.end(), [&] (FakeFileRequest* fakeRequest) {
            return fakeRequest->resource.kind == kind;
        });

        if (it != requests.end()) {
            // Copy the callback, in case calling it deallocates the AsyncRequest.
            Callback callback_ = (*it)->callback;
            callback_(response);
        }

        return it != requests.end();
    }

    std::list<FakeFileRequest*> requests;
};

} // namespace mbgl
