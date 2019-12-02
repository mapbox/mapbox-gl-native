#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion_impl.hpp>

#include <mbgl/layermanager/layer_manager.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<Error> setPaintProperties(Layer& layer, const Convertible& value) {
    auto paintValue = objectMember(value, "paint");
    if (!paintValue) {
        return nullopt;
    }
    if (!isObject(*paintValue)) {
        return { { "paint must be an object" } };
    }
    return eachMember(*paintValue, [&](const std::string& k, const Convertible& v) { return layer.setProperty(k, v); });
}

optional<std::unique_ptr<Layer>> Converter<std::unique_ptr<Layer>>::operator()(const Convertible& value, Error& error) const {
    if (!isObject(value)) {
        error.message = "layer must be an object";
        return nullopt;
    }

    auto idValue = objectMember(value, "id");
    if (!idValue) {
        error.message = "layer must have an id";
        return nullopt;
    }

    optional<std::string> id = toString(*idValue);
    if (!id) {
        error.message = "layer id must be a string";
        return nullopt;
    }

    auto typeValue = objectMember(value, "type");
    if (!typeValue) {
        error.message = "layer must have a type";
        return nullopt;
    }

    optional<std::string> type = toString(*typeValue);
    if (!type) {
        error.message = "layer type must be a string";
        return nullopt;
    }

    std::unique_ptr<Layer> layer = LayerManager::get()->createLayer(*type, *id, value, error);
    if (!layer) {
        return nullopt;
    }

    auto minzoomValue = objectMember(value, "minzoom");
    if (minzoomValue) {
        optional<float> minzoom = toNumber(*minzoomValue);
        if (!minzoom) {
            error.message = "minzoom must be numeric";
            return nullopt;
        }
        layer->setMinZoom(*minzoom);
    }

    auto maxzoomValue = objectMember(value, "maxzoom");
    if (maxzoomValue) {
        optional<float> maxzoom = toNumber(*maxzoomValue);
        if (!maxzoom) {
            error.message = "maxzoom must be numeric";
            return nullopt;
        }
        layer->setMaxZoom(*maxzoom);
    }

    auto layoutValue = objectMember(value, "layout");
    if (layoutValue) {
        if (!isObject(*layoutValue)) {
            error.message = "layout must be an object";
            return nullopt;
        }
        optional<Error> error_ = eachMember(
            *layoutValue, [&](const std::string& k, const Convertible& v) { return layer->setProperty(k, v); });
        if (error_) {
            error = *error_;
            return nullopt;
        }
    }

    optional<Error> error_ = setPaintProperties(*layer, value);
    if (error_) {
        error = *error_;
        return nullopt;
    }

    return std::move(layer);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
