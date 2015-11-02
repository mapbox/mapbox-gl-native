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

    virtual HTTPRequestBase* createRequest(const Resource&,
                                       RequestBase::Callback,
                                       uv_loop_t*,
                                       std::shared_ptr<const Response>) = 0;
};

} // namespace mbgl

#endif // MBGL_STORAGE_HTTP_CONTEXT_BASE
