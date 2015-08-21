#ifndef MBGL_STORAGE_ASSET_CONTEXT_BASE
#define MBGL_STORAGE_ASSET_CONTEXT_BASE

#include <mbgl/storage/request_base.hpp>

typedef struct uv_loop_s uv_loop_t;

namespace mbgl {

class AssetContextBase {
public:
    static std::unique_ptr<AssetContextBase> createContext(uv_loop_t*);

    virtual ~AssetContextBase() = default;
    virtual RequestBase* createRequest(const Resource&,
                                       RequestBase::Callback,
                                       uv_loop_t*,
                                       const std::string& assetRoot) = 0;
};

} // namespace mbgl

#endif // MBGL_STORAGE_ASSET_CONTEXT_BASE
