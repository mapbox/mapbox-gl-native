#ifndef MBGL_STORAGE_ASSET_CONTEXT_BASE
#define MBGL_STORAGE_ASSET_CONTEXT_BASE

#include <mbgl/storage/request_base.hpp>

namespace mbgl {

class AssetContextBase {
public:
    static std::unique_ptr<AssetContextBase> createContext();

    virtual ~AssetContextBase() = default;
    virtual RequestBase* createRequest(const std::string& url,
                                       RequestBase::Callback,
                                       const std::string& assetRoot) = 0;
};

} // namespace mbgl

#endif // MBGL_STORAGE_ASSET_CONTEXT_BASE
