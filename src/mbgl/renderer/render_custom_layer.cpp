#include <mbgl/renderer/render_custom_layer.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {

RenderCustomLayer::RenderCustomLayer(const style::CustomLayer::Impl& _impl)
        : RenderLayer(style::LayerType::Custom, _impl),
          impl(&_impl) {
}

std::unique_ptr<RenderLayer> RenderCustomLayer::clone() const {
    return std::make_unique<RenderCustomLayer>(*this);
}

bool RenderCustomLayer::evaluate(const style::PropertyEvaluationParameters&) {
    passes = RenderPass::Translucent;
    return false;
}

std::unique_ptr<Bucket> RenderCustomLayer::createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const {
    assert(false);
    return nullptr;
}
    
}
