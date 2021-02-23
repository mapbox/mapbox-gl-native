#include <mbgl/renderer/buckets/heatmap_bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/programs/heatmap_program.hpp>
#include <mbgl/style/layers/heatmap_layer_impl.hpp>
#include <mbgl/renderer/layers/render_heatmap_layer.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

using namespace style;

HeatmapBucket::HeatmapBucket(const BucketParameters& parameters, const std::vector<Immutable<style::LayerProperties>>& layers)
    : mode(parameters.mode) {
    for (const auto& layer : layers) {
        paintPropertyBinders.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(layer->baseImpl->id),
            std::forward_as_tuple(
                getEvaluated<HeatmapLayerProperties>(layer),
                parameters.tileID.overscaledZ));
    }
}

HeatmapBucket::~HeatmapBucket() = default;

void HeatmapBucket::upload(gfx::UploadPass& uploadPass) {
    vertexBuffer = uploadPass.createVertexBuffer(std::move(vertices));
    indexBuffer = uploadPass.createIndexBuffer(std::move(triangles));

    for (auto& pair : paintPropertyBinders) {
        pair.second.upload(uploadPass);
    }

    uploaded = true;
}

bool HeatmapBucket::hasData() const {
    return !segments.empty();
}

void HeatmapBucket::addFeature(const GeometryTileFeature& feature,
                               const GeometryCollection& geometry,
                               const ImagePositions&,
                               const PatternLayerMap&,
                               std::size_t featureIndex,
                               const CanonicalTileID& canonical) {
    constexpr const uint16_t vertexLength = 4;

    for (auto& points : geometry) {
        for(auto& point : points) {
            auto x = point.x;
            auto y = point.y;

            // Do not include points that are outside the tile boundaries.
            if (x < 0 || x >= util::EXTENT || y < 0 || y >= util::EXTENT) {
                continue;
            }

            if (segments.empty() || segments.back().vertexLength + vertexLength > std::numeric_limits<uint16_t>::max()) {
                // Move to a new segments because the old one can't hold the geometry.
                segments.emplace_back(vertices.elements(), triangles.elements());
            }

            // this geometry will be of the Point type, and we'll derive
            // two triangles from it.
            //
            // ┌─────────┐
            // │ 4     3 │
            // │         │
            // │ 1     2 │
            // └─────────┘
            //
            vertices.emplace_back(HeatmapProgram::vertex(point, -1, -1)); // 1
            vertices.emplace_back(HeatmapProgram::vertex(point,  1, -1)); // 2
            vertices.emplace_back(HeatmapProgram::vertex(point,  1,  1)); // 3
            vertices.emplace_back(HeatmapProgram::vertex(point, -1,  1)); // 4

            auto& segment = segments.back();
            assert(segment.vertexLength <= std::numeric_limits<uint16_t>::max());
            uint16_t index = segment.vertexLength;

            // 1, 2, 3
            // 1, 4, 3
            triangles.emplace_back(index, index + 1, index + 2);
            triangles.emplace_back(index, index + 3, index + 2);

            segment.vertexLength += vertexLength;
            segment.indexLength += 6;
        }
    }

    for (auto& pair : paintPropertyBinders) {
        pair.second.populateVertexVectors(feature, vertices.elements(), featureIndex, {}, {}, canonical);
    }
}

float HeatmapBucket::getQueryRadius(const RenderLayer& layer) const {
    (void)layer;
    return 0;
}

} // namespace mbgl
