#ifndef MBGL_STORAGE_HTTP_CONTEXT_BASE
#define MBGL_STORAGE_HTTP_CONTEXT_BASE

#include <mbgl/storage/http_request_base.hpp>
#include <mbgl/storage/network_status.hpp>

#include <set>

namespace mbgl {

class HTTPContextBase {
public:
    static std::unique_ptr<HTTPContextBase> createContext();

    virtual ~HTTPContextBase() = default;
    virtual HTTPRequestBase* createRequest(const std::string& url,
                                       HTTPRequestBase::Callback,
                                       std::shared_ptr<const Response>) = 0;
};

} // namespace mbgl

#endif // MBGL_STORAGE_HTTP_CONTEXT_BASE
