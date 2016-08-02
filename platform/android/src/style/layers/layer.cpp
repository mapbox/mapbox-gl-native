#include "layer.hpp"
#include "../android_conversion.hpp"

#include <jni/jni.hpp>

#include <mbgl/platform/log.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/source.hpp>

#include <string>

namespace mbgl {
namespace android {

    /**
     * Invoked when the construction is initiated from the jvm through a subclass
     */
    Layer::Layer(jni::JNIEnv& env, std::unique_ptr<mbgl::style::Layer> coreLayer)
        : ownedLayer(std::move(coreLayer))
        , layer(*ownedLayer) {

        mbgl::Log::Debug(mbgl::Event::JNI, "Layer constructed, owning reference");
    }

    Layer::Layer(mbgl::Map& coreMap, mbgl::style::Layer& coreLayer) : layer(coreLayer) , map(&coreMap) {
        mbgl::Log::Debug(mbgl::Event::JNI, "Non-owning reference constructor");
    }

    Layer::~Layer() {
        mbgl::Log::Debug(mbgl::Event::JNI, "Layer destroyed");
    }

    jni::String Layer::getId(jni::JNIEnv& env) {
        return jni::Make<jni::String>(env, layer.getID());
    }

    std::unique_ptr<mbgl::style::Layer> Layer::releaseCoreLayer() {
        assert(ownedLayer != nullptr);
        return std::move(ownedLayer);
    }

    void Layer::setLayoutProperty(jni::JNIEnv& env, jni::String jname, jni::Object<> jvalue) {
        mbgl::Log::Debug(mbgl::Event::JNI, "Set layout property");

        Value value(env, jvalue);

        //Convert and set property
        optional<mbgl::style::conversion::Error> error = mbgl::style::conversion::setLayoutProperty(layer, jni::Make<std::string>(env, jname), value);
        if (error) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting property: " + jni::Make<std::string>(env, jname) + " " + error->message);
            return;
        }

        //Update the style if attached
        if (ownedLayer == nullptr) {
            map->update(mbgl::Update::RecalculateStyle);
        }
    }

    void Layer::setPaintProperty(jni::JNIEnv& env, jni::String jname, jni::Object<> jvalue) {
        mbgl::Log::Debug(mbgl::Event::JNI, "Set paint property");

        Value value(env, jvalue);

        //Convert and set property
        optional<mbgl::style::conversion::Error> error = mbgl::style::conversion::setPaintProperty(layer, jni::Make<std::string>(env, jname), value, mbgl::optional<std::string>());
        if (error) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting property: " + jni::Make<std::string>(env, jname) + " " + error->message);
            return;
        }
    }

    void Layer::updateStyle(jni::JNIEnv&, jni::jboolean updateClasses) {
        mbgl::Log::Debug(mbgl::Event::JNI, "Update style property. Update classes: " + std::to_string(updateClasses));

        //Update the style only if attached
        if (ownedLayer == nullptr) {
            Update flags = mbgl::Update::RecalculateStyle;
            if(updateClasses) {
                flags = flags | mbgl::Update::Classes;
            }
            map->update(flags);
        } else {
            mbgl::Log::Debug(mbgl::Event::JNI, "Not updating as layer is not attached to map (yet)");
        }
    }

    void Layer::setFilter(jni::JNIEnv& env, jni::Array<jni::Object<>> jfilter) {
        using namespace mbgl::style;
        using namespace mbgl::style::conversion;
        mbgl::Log::Debug(mbgl::Event::JNI, "Set filter");

        Value wrapped(env, jfilter);
        Filter filter;

        Result<Filter> converted = convert<Filter>(wrapped);
        if (!converted) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting filter: " + converted.error().message);
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
        } else {
            mbgl::Log::Warning(mbgl::Event::JNI, "Layer doesn't support filters");
        }
    }

    void Layer::setSourceLayer(jni::JNIEnv& env, jni::String sourceLayer) {
        using namespace mbgl::style;

        std::string layerId = jni::Make<std::string>(env, sourceLayer);
        mbgl::Log::Debug(mbgl::Event::JNI, "Set source layer: " + layerId);

        if (layer.is<FillLayer>()) {
            layer.as<FillLayer>()->setSourceLayer(layerId);
        } else if (layer.is<LineLayer>()) {
            layer.as<LineLayer>()->setSourceLayer(layerId);
        } else if (layer.is<SymbolLayer>()) {
            layer.as<SymbolLayer>()->setSourceLayer(layerId);
        } else if (layer.is<CircleLayer>()) {
            layer.as<CircleLayer>()->setSourceLayer(layerId);
        } else {
            mbgl::Log::Warning(mbgl::Event::JNI, "Layer doesn't support source layer");
        }
    }

    jni::Class<Layer> Layer::javaClass;

    void Layer::registerNative(jni::JNIEnv& env) {
        mbgl::Log::Debug(mbgl::Event::JNI, "Registering native base layer");

        //Lookup the class
        Layer::javaClass = *jni::Class<Layer>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        //Register the peer
        jni::RegisterNativePeer<Layer>(env, Layer::javaClass, "nativePtr",
            METHOD(&Layer::getId, "nativeGetId"),
            METHOD(&Layer::setLayoutProperty, "nativeSetLayoutProperty"),
            METHOD(&Layer::setPaintProperty, "nativeSetPaintProperty"),
            METHOD(&Layer::updateStyle, "nativeUpdateStyle"),
            METHOD(&Layer::setFilter, "nativeSetFilter"),
            METHOD(&Layer::setSourceLayer, "nativeSetSourceLayer")
        );

    }
}
}