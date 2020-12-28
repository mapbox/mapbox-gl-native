#include <mbgl/layermanager/layer_factory.hpp>

#include <mbgl/layout/layout.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>

#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/layer.hpp>

namespace mbgl {

optional<std::string> LayerFactory::getSource(const style::conversion::Convertible& value) const noexcept {
    auto sourceValue = objectMember(value, "source");
    if (!sourceValue) {
        return nullopt;
    }

    optional<std::string> source = toString(*sourceValue);
    if (!source) {
        return nullopt;
    }

    return source;
}

std::unique_ptr<Bucket> LayerFactory::createBucket(const BucketParameters&, const std::vector<Immutable<style::LayerProperties>>&) noexcept {
    assert(false);
    return nullptr;
}

std::unique_ptr<Layout> LayerFactory::createLayout(const LayoutParameters&,
                                                   std::unique_ptr<GeometryTileLayer>,
                                                   const std::vector<Immutable<style::LayerProperties>>&) noexcept {
    assert(false);
    return nullptr;
}

} // namespace mbgl
