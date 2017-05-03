#include "layer.hpp"
#include "../android_conversion.hpp"

#include <jni/jni.hpp>

#include <mbgl/style/transition_options.hpp>
#include <mbgl/util/logging.hpp>

// Java -> C++ conversion
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/source.hpp>

// C++ -> Java conversion
#include "../conversion/property_value.hpp"

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
        _map.addLayer(releaseCoreLayer(), before);

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

    jni::String Layer::getId(jni::JNIEnv& env) {
        return jni::Make<jni::String>(env, layer.getID());
    }

    style::Layer& Layer::get() {
        return layer;
    }

    void Layer::setLayoutProperty(jni::JNIEnv& env, jni::String jname, jni::Object<> jvalue) {
        Value value(env, jvalue);

        // Convert and set property
        optional<mbgl::style::conversion::Error> error = mbgl::style::conversion::setLayoutProperty(layer, jni::Make<std::string>(env, jname), value);
        if (error) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting property: " + jni::Make<std::string>(env, jname) + " " + error->message);
            return;
        }
    }

    void Layer::setPaintProperty(jni::JNIEnv& env, jni::String jname, jni::Object<> jvalue) {
        Value value(env, jvalue);

        // Convert and set property
        optional<mbgl::style::conversion::Error> error = mbgl::style::conversion::setPaintProperty(layer, jni::Make<std::string>(env, jname), value, mbgl::optional<std::string>());
        if (error) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting property: " + jni::Make<std::string>(env, jname) + " " + error->message);
            return;
        }
    }

    void Layer::setFilter(jni::JNIEnv& env, jni::Array<jni::Object<>> jfilter) {
        using namespace mbgl::style;
        using namespace mbgl::style::conversion;

        Value wrapped(env, jfilter);
        Filter filter;

        Error error;
        optional<Filter> converted = convert<Filter>(wrapped, error);
        if (!converted) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting filter: " + error.message);
            return;
        }
        filter = std::move(*converted);

        if (layer.is<FillLayer>()) {
            layer.as<FillLayer>()->setFilter(filter);
        } else if (layer.is<LineLayer>()) {
            layer.as<LineLayer>()->setFilter(filter);
        } else if (layer.is<SymbolLayer>()) {
            layer.as<SymbolLayer>()->setFilter(filter);
        } else if (layer.is<CircleLayer>()) {
            layer.as<CircleLayer>()->setFilter(filter);
        } else if (layer.is<FillExtrusionLayer>()){
            layer.as<FillExtrusionLayer>()->setFilter(filter);
        } else {
            mbgl::Log::Warning(mbgl::Event::JNI, "Layer doesn't support filters");
        }
    }

    void Layer::setSourceLayer(jni::JNIEnv& env, jni::String sourceLayer) {
        using namespace mbgl::style;

        std::string layerId = jni::Make<std::string>(env, sourceLayer);

        if (layer.is<FillLayer>()) {
            layer.as<FillLayer>()->setSourceLayer(layerId);
        } else if (layer.is<LineLayer>()) {
            layer.as<LineLayer>()->setSourceLayer(layerId);
        } else if (layer.is<SymbolLayer>()) {
            layer.as<SymbolLayer>()->setSourceLayer(layerId);
        } else if (layer.is<CircleLayer>()) {
            layer.as<CircleLayer>()->setSourceLayer(layerId);
        } else if(layer.is<FillExtrusionLayer>()) {
            layer.as<FillExtrusionLayer>()->setSourceLayer(layerId);
        } else {
            mbgl::Log::Warning(mbgl::Event::JNI, "Layer doesn't support source layer");
        }
    }

    jni::String Layer::getSourceLayer(jni::JNIEnv& env) {
        using namespace mbgl::style;

        std::string sourceLayerId;
        if (layer.is<FillLayer>()) {
            sourceLayerId = layer.as<FillLayer>()->getSourceLayer();
        } else if (layer.is<LineLayer>()) {
            sourceLayerId = layer.as<LineLayer>()->getSourceLayer();
        } else if (layer.is<SymbolLayer>()) {
            sourceLayerId = layer.as<SymbolLayer>()->getSourceLayer();
        } else if (layer.is<CircleLayer>()) {
            sourceLayerId = layer.as<CircleLayer>()->getSourceLayer();
        } else if (layer.is<FillExtrusionLayer>()) {
            sourceLayerId = layer.as<FillExtrusionLayer>()->getSourceLayer();
        } else {
            mbgl::Log::Warning(mbgl::Event::JNI, "Layer doesn't support source layer");
        }

        return jni::Make<jni::String>(env, sourceLayerId);
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

    jni::Object<jni::ObjectTag> Layer::getVisibility(jni::JNIEnv& env) {
        using namespace mbgl::android::conversion;
        return jni::Object<jni::ObjectTag>(*convert<jni::jobject*>(env, layer.getVisibility()));
    }

    jni::Class<Layer> Layer::javaClass;

    void Layer::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        Layer::javaClass = *jni::Class<Layer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<Layer>(env, Layer::javaClass, "nativePtr",
            METHOD(&Layer::getId, "nativeGetId"),
            METHOD(&Layer::setLayoutProperty, "nativeSetLayoutProperty"),
            METHOD(&Layer::setPaintProperty, "nativeSetPaintProperty"),
            METHOD(&Layer::setFilter, "nativeSetFilter"),
            METHOD(&Layer::setSourceLayer, "nativeSetSourceLayer"),
            METHOD(&Layer::getSourceLayer, "nativeGetSourceLayer"),
            METHOD(&Layer::getMinZoom, "nativeGetMinZoom"),
            METHOD(&Layer::getMaxZoom, "nativeGetMaxZoom"),
            METHOD(&Layer::setMinZoom, "nativeSetMinZoom"),
            METHOD(&Layer::setMaxZoom, "nativeSetMaxZoom"),
            METHOD(&Layer::getVisibility, "nativeGetVisibility")
        );

    }

} // namespace android
} // namespace mbgl
