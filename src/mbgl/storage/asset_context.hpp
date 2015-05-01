#ifndef MBGL_STORAGE_DEFAULT_ASSET_CONTEXT
#define MBGL_STORAGE_DEFAULT_ASSET_CONTEXT

#include <mbgl/storage/request_base.hpp>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class AssetContext {
public:
    static std::unique_ptr<AssetContext> createContext(uv_loop_t*);

    virtual ~AssetContext() = default;
    virtual RequestBase* createRequest(const Resource&,
                                       RequestBase::Callback,
                                       uv_loop_t*,
                                       const std::string& assetRoot) = 0;
};

}

#endif
