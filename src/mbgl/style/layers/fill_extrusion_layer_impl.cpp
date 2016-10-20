#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/renderer/bucket.hpp>

namespace mbgl {
namespace style {

void FillExtrusionLayer::Impl::cascade(const CascadeParameters&) {
}

bool FillExtrusionLayer::Impl::evaluate(const PropertyEvaluationParameters&) {
    return false;
}

std::unique_ptr<Bucket> FillExtrusionLayer::Impl::createBucket(BucketParameters&, const GeometryTileLayer&) const {
    return nullptr;
}

} // namespace style
} // namespace mbgl
