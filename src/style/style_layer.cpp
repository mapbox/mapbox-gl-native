#include <llmr/style/style_layer.hpp>

namespace llmr {

StyleLayer::StyleLayer(const std::string &id, std::map<ClassID, ClassProperties> &&styles,
                       std::unique_ptr<const RasterizeProperties> &&rasterize)
    : id(id), styles(std::move(styles)), rasterize(std::move(rasterize)) {}

bool StyleLayer::isBackground() const {
    return id == "background";
}

}
