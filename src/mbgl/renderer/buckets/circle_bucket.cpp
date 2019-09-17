#include <mbgl/renderer/buckets/circle_bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/programs/circle_program.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/renderer/layers/render_circle_layer.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

using namespace style;

CircleBucket::CircleBucket(const BucketParameters& parameters, const std::vector<Immutable<style::LayerProperties>>& layers)
    : mode(parameters.mode) {
    for (const auto& layer : layers) {
        paintPropertyBinders.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(layer->baseImpl->id),
            std::forward_as_tuple(
                getEvaluated<CircleLayerProperties>(layer),
                parameters.tileID.overscaledZ));
    }
}

CircleBucket::~CircleBucket() = default;

void CircleBucket::upload(gfx::UploadPass& uploadPass) {
    if (!uploaded) {
        vertexBuffer = uploadPass.createVertexBuffer(std::move(vertices));
        indexBuffer = uploadPass.createIndexBuffer(std::move(triangles));
    }

    for (auto& pair : paintPropertyBinders) {
        pair.second.upload(uploadPass);
    }

    uploaded = true;
}

bool CircleBucket::hasData() const {
    return !segments.empty();
}

void CircleBucket::addFeature(const GeometryTileFeature& feature, const GeometryCollection& geometry,
                              const ImagePositions&, const PatternLayerMap&, std::size_t featureIndex) {
    constexpr const uint16_t vertexLength = 4;

    for (auto& circle : geometry) {
        for(auto& point : circle) {
            auto x = point.x;
            auto y = point.y;

            // Do not include points that are outside the tile boundaries.
            // Include all points in Still mode. You need to include points from
            // neighbouring tiles so that they are not clipped at tile boundaries.
            if ((mode == MapMode::Continuous) &&
                (x < 0 || x >= util::EXTENT || y < 0 || y >= util::EXTENT)) continue;

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
            vertices.emplace_back(CircleProgram::vertex(point, -1, -1)); // 1
            vertices.emplace_back(CircleProgram::vertex(point,  1, -1)); // 2
            vertices.emplace_back(CircleProgram::vertex(point,  1,  1)); // 3
            vertices.emplace_back(CircleProgram::vertex(point, -1,  1)); // 4

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
        pair.second.populateVertexVectors(feature, vertices.elements(), featureIndex, {}, {});
    }
}

template <class Property>
static float get(const CirclePaintProperties::PossiblyEvaluated& evaluated, const std::string& id, const std::map<std::string, CircleProgram::Binders>& paintPropertyBinders) {
    auto it = paintPropertyBinders.find(id);
    if (it == paintPropertyBinders.end() || !it->second.statistics<Property>().max()) {
        return evaluated.get<Property>().constantOr(Property::defaultValue());
    } else {
        return *it->second.statistics<Property>().max();
    }
}

float CircleBucket::getQueryRadius(const RenderLayer& layer) const {
    const auto& evaluated = getEvaluated<CircleLayerProperties>(layer.evaluatedProperties);
    float radius = get<CircleRadius>(evaluated, layer.getID(), paintPropertyBinders);
    float stroke = get<CircleStrokeWidth>(evaluated, layer.getID(), paintPropertyBinders);
    auto translate = evaluated.get<CircleTranslate>();
    return radius + stroke + util::length(translate[0], translate[1]);
}

void CircleBucket::update(const FeatureStates& states, const GeometryTileLayer& layer, const std::string& layerID,
                          const ImagePositions& imagePositions) {
    auto it = paintPropertyBinders.find(layerID);
    if (it != paintPropertyBinders.end()) {
        it->second.updateVertexVectors(states, layer, imagePositions);
        uploaded = false;
    }
}

} // namespace mbgl
