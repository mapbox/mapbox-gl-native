#include <mbgl/renderer/buckets/heatmap_bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/programs/heatmap_program.hpp>
#include <mbgl/style/layers/heatmap_layer_impl.hpp>
#include <mbgl/renderer/layers/render_heatmap_layer.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

using namespace style;

HeatmapBucket::HeatmapBucket(const BucketParameters& parameters, const std::vector<const RenderLayer*>& layers)
    : Bucket(LayerType::Heatmap),
      mode(parameters.mode) {
    for (const auto& layer : layers) {
        paintPropertyBinders.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(layer->getID()),
            std::forward_as_tuple(
                layer->as<RenderHeatmapLayer>()->evaluated,
                parameters.tileID.overscaledZ));
    }
}

void HeatmapBucket::upload(gl::Context& context) {
    vertexBuffer = context.createVertexBuffer(std::move(vertices));
    indexBuffer = context.createIndexBuffer(std::move(triangles));

    for (auto& pair : paintPropertyBinders) {
        pair.second.upload(context);
    }

    uploaded = true;
}

bool HeatmapBucket::hasData() const {
    return !segments.empty();
}

void HeatmapBucket::addFeature(const GeometryTileFeature& feature,
                              const GeometryCollection& geometry) {
    constexpr const uint16_t vertexLength = 4;

    for (auto& points : geometry) {
        for(auto& point : points) {
            auto x = point.x;
            auto y = point.y;

            // Do not include points that are outside the tile boundaries.
            // Include all points in Still mode. You need to include points from
            // neighbouring tiles so that they are not clipped at tile boundaries.
            if ((mode == MapMode::Continuous) &&
                (x < 0 || x >= util::EXTENT || y < 0 || y >= util::EXTENT)) continue;

            if (segments.empty() || segments.back().vertexLength + vertexLength > std::numeric_limits<uint16_t>::max()) {
                // Move to a new segments because the old one can't hold the geometry.
                segments.emplace_back(vertices.vertexSize(), triangles.indexSize());
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
        pair.second.populateVertexVectors(feature, vertices.vertexSize());
    }
}

float HeatmapBucket::getQueryRadius(const RenderLayer& layer) const {
    (void)layer;
    return 0;
}

} // namespace mbgl
