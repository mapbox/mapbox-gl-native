#include "layer.hpp"
#include "../android_conversion.hpp"

#include <jni/jni.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/circle_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/heatmap_layer.hpp>
#include <mbgl/style/layers/hillshade_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/raster_layer.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/util/logging.hpp>

// Java -> C++ conversion
#include <mbgl/style/conversion/filter.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/source.hpp>
#include <mbgl/style/conversion_impl.hpp>

// C++ -> Java conversion
#include "../conversion/property_value.hpp"
#include "custom_layer.hpp"
#include "background_layer.hpp"
#include <mbgl/style/filter.hpp>

#include <string>

namespace mbgl {
namespace android {

    /**
     * Invoked when the construction is initiated from the jvm through a subclass
     */
    Layer::Layer(jni::JNIEnv&, std::unique_ptr<mbgl::style::Layer> coreLayer)
        : ownedLayer(std::move(coreLayer))
        , layer(*ownedLayer) {
    }

    /**
     * Takes a non-owning reference. For lookup methods
     */
    Layer::Layer(mbgl::Map& coreMap, mbgl::style::Layer& coreLayer) : layer(coreLayer) , map(&coreMap) {
    }

    /**
     * Takes a owning reference. Ownership is transfered to this peer, eg after removing
     * from the map
     */
    Layer::Layer(mbgl::Map& coreMap, std::unique_ptr<mbgl::style::Layer> coreLayer)
        : ownedLayer(std::move(coreLayer))
        , layer(*ownedLayer)
        , map(&coreMap) {
    }

    Layer::~Layer() {
    }

    void Layer::addToMap(mbgl::Map& _map, mbgl::optional<std::string> before) {
        // Check to see if we own the layer first
        if (!ownedLayer) {
            throw std::runtime_error("Cannot add layer twice");
        }

        // Add layer to map
        _map.getStyle().addLayer(releaseCoreLayer(), before);

        // Save pointer to the map
        this->map = &_map;
    }

    void Layer::setLayer(std::unique_ptr<mbgl::style::Layer> sourceLayer) {
        this->ownedLayer = std::move(sourceLayer);
    }

    std::unique_ptr<mbgl::style::Layer> Layer::releaseCoreLayer() {
        assert(ownedLayer != nullptr);
        return std::move(ownedLayer);
    }

    jni::Local<jni::String> Layer::getId(jni::JNIEnv& env) {
        return jni::Make<jni::String>(env, layer.getID());
    }

    style::Layer& Layer::get() {
        return layer;
    }

    void Layer::setLayoutProperty(jni::JNIEnv& env, const jni::String& jname, const jni::Object<>& jvalue) {
        // Convert and set property
        optional<mbgl::style::conversion::Error> error = layer.setLayoutProperty(jni::Make<std::string>(env, jname), Value(env, jvalue));
        if (error) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting property: " + jni::Make<std::string>(env, jname) + " " + error->message);
            return;
        }
    }

    void Layer::setPaintProperty(jni::JNIEnv& env, const jni::String& jname, const jni::Object<>& jvalue) {
        // Convert and set property
        optional<mbgl::style::conversion::Error> error = layer.setPaintProperty(jni::Make<std::string>(env, jname), Value(env, jvalue));
        if (error) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting property: " + jni::Make<std::string>(env, jname) + " " + error->message);
            return;
        }
    }

    struct SetFilterEvaluator {
        style::Filter filter;

        void operator()(style::BackgroundLayer&) { Log::Warning(mbgl::Event::JNI, "BackgroundLayer doesn't support filters"); }
        void operator()(style::CustomLayer&) { Log::Warning(mbgl::Event::JNI, "CustomLayer doesn't support filters"); }
        void operator()(style::RasterLayer&) { Log::Warning(mbgl::Event::JNI, "RasterLayer doesn't support filters"); }
        void operator()(style::HillshadeLayer&) { Log::Warning(mbgl::Event::JNI, "HillshadeLayer doesn't support filters"); }

        template <class LayerType>
        void operator()(LayerType& layer) {
            layer.setFilter(filter);
        }
    };

    void Layer::setFilter(jni::JNIEnv& env, const jni::Array<jni::Object<>>& jfilter) {
        using namespace mbgl::style;
        using namespace mbgl::style::conversion;

        Error error;
        optional<Filter> converted = convert<Filter>(Value(env, jfilter), error);
        if (!converted) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting filter: " + error.message);
            return;
        }

        layer.accept(SetFilterEvaluator {std::move(*converted)});
    }

    struct GetFilterEvaluator {
        mbgl::style::Filter noop(std::string layerType) {
            Log::Warning(mbgl::Event::JNI, "%s doesn't support filter", layerType.c_str());
            return {};
        }

        mbgl::style::Filter operator()(style::BackgroundLayer&) { return noop("BackgroundLayer"); }
        mbgl::style::Filter operator()(style::CustomLayer&) { return noop("CustomLayer"); }
        mbgl::style::Filter operator()(style::RasterLayer&) { return noop("RasterLayer"); }
        mbgl::style::Filter operator()(style::HillshadeLayer&) { return noop("HillshadeLayer"); }

        template <class LayerType>
            mbgl::style::Filter operator()(LayerType& layer) {
            return layer.getFilter();
        }
    };

    jni::Local<jni::Object<gson::JsonElement>> Layer::getFilter(jni::JNIEnv& env) {
        using namespace mbgl::style;
        using namespace mbgl::style::conversion;

        Filter filter = layer.accept(GetFilterEvaluator());
        if (filter.expression) {
            mbgl::Value expressionValue = (*filter.expression)->serialize();
            return gson::JsonElement::New(env, expressionValue);
        } else {
            return jni::Local<jni::Object<gson::JsonElement>>(env, nullptr);
        }
    }

    struct SetSourceLayerEvaluator {
        std::string sourceLayer;

        void operator()(style::BackgroundLayer&) { Log::Warning(mbgl::Event::JNI, "BackgroundLayer doesn't support source layer"); }
        void operator()(style::CustomLayer&) { Log::Warning(mbgl::Event::JNI, "CustomLayer doesn't support source layer"); }
        void operator()(style::RasterLayer&) { Log::Warning(mbgl::Event::JNI, "RasterLayer doesn't support source layer"); }
        void operator()(style::HillshadeLayer&) { Log::Warning(mbgl::Event::JNI, "HillshadeLayer doesn't support source layer"); }

        template <class LayerType>
        void operator()(LayerType& layer) {
            layer.setSourceLayer(sourceLayer);
        }
    };

    void Layer::setSourceLayer(jni::JNIEnv& env, const jni::String& sourceLayer) {
        layer.accept(SetSourceLayerEvaluator {jni::Make<std::string>(env, sourceLayer)});
    }

    struct GetSourceLayerEvaluator {
        std::string noop(std::string layerType) {
            Log::Warning(mbgl::Event::JNI, "%s doesn't support source layer", layerType.c_str());
            return {};
        }

        std::string operator()(style::BackgroundLayer&) { return noop("BackgroundLayer"); }
        std::string operator()(style::CustomLayer&) { return noop("CustomLayer"); }
        std::string operator()(style::RasterLayer&) { return noop("RasterLayer"); }
        std::string operator()(style::HillshadeLayer&) { return noop("HillshadeLayer"); }

        template <class LayerType>
        std::string operator()(LayerType& layer) {
            return layer.getSourceLayer();
        }
    };

    jni::Local<jni::String> Layer::getSourceLayer(jni::JNIEnv& env) {
        return jni::Make<jni::String>(env, layer.accept(GetSourceLayerEvaluator()));
    }

    struct GetSourceIdEvaluator {
        std::string noop(std::string layerType) {
            Log::Warning(mbgl::Event::JNI, "%s doesn't support get source id", layerType.c_str());
            return {};
        }

        std::string operator()(style::BackgroundLayer&) { return noop("BackgroundLayer"); }
        std::string operator()(style::CustomLayer&) { return noop("CustomLayer"); }

        template <class LayerType>
        std::string operator()(LayerType& layer) {
            return layer.getSourceID();
        }
    };

    jni::Local<jni::String> Layer::getSourceId(jni::JNIEnv& env) {
        return jni::Make<jni::String>(env, layer.accept(GetSourceIdEvaluator()));
    }

    jni::jfloat Layer::getMinZoom(jni::JNIEnv&){
        return layer.getMinZoom();
    }

    jni::jfloat Layer::getMaxZoom(jni::JNIEnv&) {
        return layer.getMaxZoom();
    }

    void Layer::setMinZoom(jni::JNIEnv&, jni::jfloat zoom) {
        layer.setMinZoom(zoom);
    }

    void Layer::setMaxZoom(jni::JNIEnv&, jni::jfloat zoom) {
        layer.setMaxZoom(zoom);
    }

    jni::Local<jni::Object<>> Layer::getVisibility(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return std::move(*convert<jni::Local<jni::Object<>>>(env, layer.getVisibility()));
    }

    void Layer::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        static auto& javaClass = jni::Class<Layer>::Singleton(env);

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<Layer>(env, javaClass, "nativePtr",
            METHOD(&Layer::getId, "nativeGetId"),
            METHOD(&Layer::setLayoutProperty, "nativeSetLayoutProperty"),
            METHOD(&Layer::setPaintProperty, "nativeSetPaintProperty"),
            METHOD(&Layer::setFilter, "nativeSetFilter"),
            METHOD(&Layer::getFilter, "nativeGetFilter"),
            METHOD(&Layer::setSourceLayer, "nativeSetSourceLayer"),
            METHOD(&Layer::getSourceLayer, "nativeGetSourceLayer"),
            METHOD(&Layer::getSourceId, "nativeGetSourceId"),
            METHOD(&Layer::getMinZoom, "nativeGetMinZoom"),
            METHOD(&Layer::getMaxZoom, "nativeGetMaxZoom"),
            METHOD(&Layer::setMinZoom, "nativeSetMinZoom"),
            METHOD(&Layer::setMaxZoom, "nativeSetMaxZoom"),
            METHOD(&Layer::getVisibility, "nativeGetVisibility")
        );

    }

} // namespace android
} // namespace mbgl
