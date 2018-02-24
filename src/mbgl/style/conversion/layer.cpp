#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion/make_property_setters.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<Error> setLayoutProperty(Layer& layer, const std::string& name, const Convertible& value) {
    static const auto setters = makeLayoutPropertySetters();
    auto it = setters.find(name);
    if (it == setters.end()) {
        return Error { "property not found" };
    }
    return it->second(layer, value);
}

optional<Error> setPaintProperty(Layer& layer, const std::string& name, const Convertible& value) {
    static const auto setters = makePaintPropertySetters();
    auto it = setters.find(name);
    if (it == setters.end()) {
        return Error { "property not found" };
    }
    return it->second(layer, value);
}

optional<Error> setPaintProperties(Layer& layer, const Convertible& value) {
    auto paintValue = objectMember(value, "paint");
    if (!paintValue) {
        return {};
    }
    if (!isObject(*paintValue)) {
        return { { "paint must be an object" } };
    }
    return eachMember(*paintValue, [&] (const std::string& k, const Convertible& v) {
        return setPaintProperty(layer, k, v);
    });
}

template <class LayerType>
optional<std::unique_ptr<Layer>> convertVectorLayer(const std::string& id, const Convertible& value, Error& error) {
    auto sourceValue = objectMember(value, "source");
    if (!sourceValue) {
        error = { "layer must have a source" };
        return {};
    }

    optional<std::string> source = toString(*sourceValue);
    if (!source) {
        error = { "layer source must be a string" };
        return {};
    }

    std::unique_ptr<LayerType> layer = std::make_unique<LayerType>(id, *source);

    auto sourceLayerValue = objectMember(value, "source-layer");
    if (sourceLayerValue) {
        optional<std::string> sourceLayer = toString(*sourceLayerValue);
        if (!sourceLayer) {
            error = { "layer source-layer must be a string" };
            return {};
        }
        layer->setSourceLayer(*sourceLayer);
    }

    auto filterValue = objectMember(value, "filter");
    if (filterValue) {
        optional<Filter> filter = convert<Filter>(*filterValue, error);
        if (!filter) {
            return {};
        }
        layer->setFilter(*filter);
    }

    return { std::move(layer) };
}

static optional<std::unique_ptr<Layer>> convertRasterLayer(const std::string& id, const Convertible& value, Error& error) {
    auto sourceValue = objectMember(value, "source");
    if (!sourceValue) {
        error = { "layer must have a source" };
        return {};
    }

    optional<std::string> source = toString(*sourceValue);
    if (!source) {
        error = { "layer source must be a string" };
        return {};
    }

    return { std::make_unique<RasterLayer>(id, *source) };
}

static optional<std::unique_ptr<Layer>> convertHillshadeLayer(const std::string& id, const Convertible& value, Error& error) {
    auto sourceValue = objectMember(value, "source");
    if (!sourceValue) {
        error = { "layer must have a source" };
        return {};
    }

    optional<std::string> source = toString(*sourceValue);
    if (!source) {
        error = { "layer source must be a string" };
        return {};
    }

    return { std::make_unique<HillshadeLayer>(id, *source) };
}


static optional<std::unique_ptr<Layer>> convertBackgroundLayer(const std::string& id, const Convertible&, Error&) {
    return { std::make_unique<BackgroundLayer>(id) };
}

optional<std::unique_ptr<Layer>> Converter<std::unique_ptr<Layer>>::operator()(const Convertible& value, Error& error) const {
    if (!isObject(value)) {
        error = { "layer must be an object" };
        return {};
    }

    auto idValue = objectMember(value, "id");
    if (!idValue) {
        error = { "layer must have an id" };
        return {};
    }

    optional<std::string> id = toString(*idValue);
    if (!id) {
        error = { "layer id must be a string" };
        return {};
    }

    auto typeValue = objectMember(value, "type");
    if (!typeValue) {
        error = { "layer must have a type" };
        return {};
    }

    optional<std::string> type = toString(*typeValue);
    if (!type) {
        error = { "layer type must be a string" };
        return {};
    }

    optional<std::unique_ptr<Layer>> converted;

    if (*type == "fill") {
        converted = convertVectorLayer<FillLayer>(*id, value, error);
    } else if (*type == "fill-extrusion") {
        converted = convertVectorLayer<FillExtrusionLayer>(*id, value, error);
    } else if (*type == "line") {
        converted = convertVectorLayer<LineLayer>(*id, value, error);
    } else if (*type == "circle") {
        converted = convertVectorLayer<CircleLayer>(*id, value, error);
    } else if (*type == "symbol") {
        converted = convertVectorLayer<SymbolLayer>(*id, value, error);
    } else if (*type == "raster") {
        converted = convertRasterLayer(*id, value, error);
    } else if (*type == "heatmap") {
        converted = convertVectorLayer<HeatmapLayer>(*id, value, error);
    } else if (*type == "hillshade") {
        converted = convertHillshadeLayer(*id, value, error);
    } else if (*type == "background") {
        converted = convertBackgroundLayer(*id, value, error);
    } else {
        error = { "invalid layer type" };
        return {};
    }

    if (!converted) {
        return converted;
    }

    std::unique_ptr<Layer> layer = std::move(*converted);

    auto minzoomValue = objectMember(value, "minzoom");
    if (minzoomValue) {
        optional<float> minzoom = toNumber(*minzoomValue);
        if (!minzoom) {
            error = { "minzoom must be numeric" };
            return {};
        }
        layer->setMinZoom(*minzoom);
    }

    auto maxzoomValue = objectMember(value, "maxzoom");
    if (maxzoomValue) {
        optional<float> maxzoom = toNumber(*maxzoomValue);
        if (!maxzoom) {
            error = { "maxzoom must be numeric" };
            return {};
        }
        layer->setMaxZoom(*maxzoom);
    }

    auto layoutValue = objectMember(value, "layout");
    if (layoutValue) {
        if (!isObject(*layoutValue)) {
            error = { "layout must be an object" };
            return {};
        }
        optional<Error> error_ = eachMember(*layoutValue, [&] (const std::string& k, const Convertible& v) {
            return setLayoutProperty(*layer, k, v);
        });
        if (error_) {
            error = *error_;
            return {};
        }
    }

    optional<Error> error_ = setPaintProperties(*layer, value);
    if (error_) {
        error = *error_;
        return {};
    }

    return std::move(layer);
}

} // namespace conversion
} // namespace style
} // namespace mbgl
