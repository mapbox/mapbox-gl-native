#ifndef MBGL_STORAGE_HTTP_CONTEXT_BASE
#define MBGL_STORAGE_HTTP_CONTEXT_BASE

#include <mbgl/storage/request_base.hpp>
#include <mbgl/storage/http_request_base.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/util/uv_detail.hpp>

#include <set>

namespace mbgl {

class HTTPContextBase {
public:
    static std::unique_ptr<HTTPContextBase> createContext(uv_loop_t*);

    HTTPContextBase(uv_loop_t*);
    virtual ~HTTPContextBase();

    virtual HTTPRequestBase* createRequest(const Resource&,
                                       RequestBase::Callback,
                                       uv_loop_t*,
                                       std::shared_ptr<const Response>) = 0;

    void addRequest(HTTPRequestBase*);
    void removeRequest(HTTPRequestBase*);

private:
    void retryRequests();

    // Will be fired when the network status becomes reachable.
    uv::async reachability;

    // A list of all pending HTTPRequestImpls that we need to notify when the network status
    // changes.
    std::set<HTTPRequestBase*> requests;
};

} // namespace mbgl

#endif // MBGL_STORAGE_HTTP_CONTEXT_BASE
