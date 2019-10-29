#include <mbgl/renderer/buckets/circle_bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/programs/circle_program.hpp>
#include <mbgl/style/layers/circle_layer_impl.hpp>
#include <mbgl/renderer/layers/render_circle_layer.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/math.hpp>

namespace mbgl {

using namespace style;

CircleBucket::CircleBucket(const std::map<std::string, Immutable<LayerProperties>>& layerPaintProperties,
                           const MapMode mode_,
                           const float zoom)
    : mode(mode_) {
    for (const auto& pair : layerPaintProperties) {
        paintPropertyBinders.emplace(std::piecewise_construct,
                                     std::forward_as_tuple(pair.first),
                                     std::forward_as_tuple(getEvaluated<CircleLayerProperties>(pair.second), zoom));
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
