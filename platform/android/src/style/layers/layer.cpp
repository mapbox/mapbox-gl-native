#include "layer.hpp"
#include "../android_conversion.hpp"

#include <jni/jni.hpp>

#include <mbgl/platform/log.hpp>

//Java -> C++ conversion
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/layer.hpp>
#include <mbgl/style/conversion/source.hpp>

//C++ -> Java conversion
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

    Layer::Layer(mbgl::Map& coreMap, mbgl::style::Layer& coreLayer) : layer(coreLayer) , map(&coreMap) {
    }

    Layer::~Layer() {
    }

    jni::String Layer::getId(jni::JNIEnv& env) {
        return jni::Make<jni::String>(env, layer.getID());
    }

    std::unique_ptr<mbgl::style::Layer> Layer::releaseCoreLayer() {
        assert(ownedLayer != nullptr);
        return std::move(ownedLayer);
    }

    void Layer::setLayoutProperty(jni::JNIEnv& env, jni::String jname, jni::Object<> jvalue) {
        Value value(env, jvalue);

        //Convert and set property
        optional<mbgl::style::conversion::Error> error = mbgl::style::conversion::setLayoutProperty(layer, jni::Make<std::string>(env, jname), value);
        if (error) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting property: " + jni::Make<std::string>(env, jname) + " " + error->message);
            return;
        }
    }

    void Layer::setPaintProperty(jni::JNIEnv& env, jni::String jname, jni::Object<> jvalue) {
        Value value(env, jvalue);

        //Convert and set property
        optional<mbgl::style::conversion::Error> error = mbgl::style::conversion::setPaintProperty(layer, jni::Make<std::string>(env, jname), value, mbgl::optional<std::string>());
        if (error) {
            mbgl::Log::Error(mbgl::Event::JNI, "Error setting property: " + jni::Make<std::string>(env, jname) + " " + error->message);
            return;
        }
    }

    void Layer::updateStyle(jni::JNIEnv&, jni::jboolean updateClasses) {
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
            METHOD(&Layer::setSourceLayer, "nativeSetSourceLayer"),
            METHOD(&Layer::getMinZoom, "nativeGetMinZoom"),
            METHOD(&Layer::getMaxZoom, "nativeGetMaxZoom"),
            METHOD(&Layer::setMinZoom, "nativeSetMinZoom"),
            METHOD(&Layer::setMaxZoom, "nativeSetMaxZoom"),
            METHOD(&Layer::getVisibility, "nativeGetVisibility")
        );

    }

} //android
} //mbgl