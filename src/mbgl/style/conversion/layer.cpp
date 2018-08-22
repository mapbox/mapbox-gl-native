#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion_impl.hpp>
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

optional<Error> setPaintProperties(Layer& layer, const Convertible& value) {
    auto paintValue = objectMember(value, "paint");
    if (!paintValue) {
        return nullopt;
    }
    if (!isObject(*paintValue)) {
        return { { "paint must be an object" } };
    }
    return eachMember(*paintValue, [&] (const std::string& k, const Convertible& v) {
        return layer.setPaintProperty(k, v);
    });
}

template <class LayerType>
optional<std::unique_ptr<Layer>> convertVectorLayer(const std::string& id, const Convertible& value, Error& error) {
    auto sourceValue = objectMember(value, "source");
    if (!sourceValue) {
        error.message = "layer must have a source";
        return nullopt;
    }

    optional<std::string> source = toString(*sourceValue);
    if (!source) {
        error.message = "layer source must be a string";
        return nullopt;
    }

    std::unique_ptr<LayerType> layer = std::make_unique<LayerType>(id, *source);

    auto sourceLayerValue = objectMember(value, "source-layer");
    if (sourceLayerValue) {
        optional<std::string> sourceLayer = toString(*sourceLayerValue);
        if (!sourceLayer) {
            error.message = "layer source-layer must be a string";
            return nullopt;
        }
        layer->setSourceLayer(*sourceLayer);
    }

    auto filterValue = objectMember(value, "filter");
    if (filterValue) {
        optional<Filter> filter = convert<Filter>(*filterValue, error);
        if (!filter) {
            return nullopt;
        }
        layer->setFilter(*filter);
    }

    return { std::move(layer) };
}

static optional<std::unique_ptr<Layer>> convertRasterLayer(const std::string& id, const Convertible& value, Error& error) {
    auto sourceValue = objectMember(value, "source");
    if (!sourceValue) {
        error.message = "layer must have a source";
        return nullopt;
    }

    optional<std::string> source = toString(*sourceValue);
    if (!source) {
        error.message = "layer source must be a string";
        return nullopt;
    }

    return { std::make_unique<RasterLayer>(id, *source) };
}

static optional<std::unique_ptr<Layer>> convertHillshadeLayer(const std::string& id, const Convertible& value, Error& error) {
    auto sourceValue = objectMember(value, "source");
    if (!sourceValue) {
        error.message = "layer must have a source";
        return nullopt;
    }

    optional<std::string> source = toString(*sourceValue);
    if (!source) {
        error.message = "layer source must be a string";
        return nullopt;
    }

    return { std::make_unique<HillshadeLayer>(id, *source) };
}


static optional<std::unique_ptr<Layer>> convertBackgroundLayer(const std::string& id, const Convertible&, Error&) {
    return { std::make_unique<BackgroundLayer>(id) };
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
        error.message = "invalid layer type";
        return nullopt;
    }

    if (!converted) {
        return converted;
    }

    std::unique_ptr<Layer> layer = std::move(*converted);

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
        optional<Error> error_ = eachMember(*layoutValue, [&] (const std::string& k, const Convertible& v) {
            return layer->setLayoutProperty(k, v);
        });
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
