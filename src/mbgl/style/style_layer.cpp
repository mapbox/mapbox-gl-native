#include <mbgl/style/style_layer.hpp>

namespace mbgl {

const util::ID<StyleLayer> StyleLayer::bucketID() const {
    return ref.valid() ? ref : id;
}

bool StyleLayer::hasRenderPass(RenderPass pass) const {
    return bool(passes & pass);
}

bool StyleLayer::needsRendering() const {
    return passes != RenderPass::None && visibility != VisibilityType::None;
}

} // namespace mbgl
