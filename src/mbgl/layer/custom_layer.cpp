#include <mbgl/layer/custom_layer.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

CustomLayer::CustomLayer(const std::string& id_) {
    id = id_;
}

bool CustomLayer::recalculate(const StyleCalculationParameters&) {
    passes = RenderPass::Translucent;
    return false;
}

std::unique_ptr<Bucket> CustomLayer::createBucket(StyleBucketParameters&) const {
    return nullptr;
}

} // namespace mbgl
