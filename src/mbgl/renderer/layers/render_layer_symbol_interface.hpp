#pragma once

#include <memory>
#include <vector>
#include <string>

namespace mbgl {

class RenderTile;
class SymbolBucket;

class RenderLayerSymbolInterface {
public:
    virtual const std::string& layerID() const = 0;
    virtual const std::vector<std::reference_wrapper<RenderTile>>& getRenderTiles() const = 0;
    virtual SymbolBucket* getSymbolBucket(const RenderTile&) const = 0;

protected:
    virtual ~RenderLayerSymbolInterface() = default;
};

} // namespace mbgl
