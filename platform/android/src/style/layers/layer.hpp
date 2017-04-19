#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/layer.hpp>

#include "../value.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class Layer : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/Layer"; };

    static jni::Class<Layer> javaClass;

    static void registerNative(jni::JNIEnv&);

    /*
     * Called when a non-owning peer object is created on the c++ side
     */
    Layer(mbgl::Map&, mbgl::style::Layer&);

    /*
     * Called when a owning peer object is created on the c++ side
     */
    Layer(mbgl::Map&, std::unique_ptr<mbgl::style::Layer>);

    /*
     * Called when a Java object was created from the jvm side
     */
    Layer(jni::JNIEnv&, std::unique_ptr<mbgl::style::Layer>);

    virtual ~Layer();

    virtual jni::jobject* createJavaPeer(jni::JNIEnv&) = 0;

    /**
     * Set core layer (ie return ownership after remove)
     */
    void setLayer(std::unique_ptr<mbgl::style::Layer>);

    void addToMap(mbgl::Map&, mbgl::optional<std::string>);

    jni::String getId(jni::JNIEnv&);

    style::Layer& get();

    void setLayoutProperty(jni::JNIEnv&, jni::String, jni::Object<> value);

    void setPaintProperty(jni::JNIEnv&, jni::String, jni::Object<> value);

    // Zoom

    jni::jfloat getMinZoom(jni::JNIEnv&);

    jni::jfloat getMaxZoom(jni::JNIEnv&);

    void setMinZoom(jni::JNIEnv&, jni::jfloat zoom);

    void setMaxZoom(jni::JNIEnv&, jni::jfloat zoom);

    /* common properties, but not shared by all */

    void setFilter(jni::JNIEnv&, jni::Array<jni::Object<>>);

    void setSourceLayer(jni::JNIEnv&, jni::String);

    jni::String getSourceLayer(jni::JNIEnv&);

    // Property getters

    jni::Object<jni::ObjectTag> getVisibility(jni::JNIEnv&);

protected:
    // Release the owned view and return it
    std::unique_ptr<mbgl::style::Layer> releaseCoreLayer();

    // Owned layer is set when creating a new layer, before adding it to the map
    std::unique_ptr<mbgl::style::Layer> ownedLayer;

    // Raw reference to the layer
    mbgl::style::Layer& layer;

    // Map is set when the layer is retrieved or after adding to the map
    mbgl::Map* map;

};

} // namespace android
} // namespace mbgl
