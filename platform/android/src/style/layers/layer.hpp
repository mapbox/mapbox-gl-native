#pragma once

#include <mbgl/layermanager/layer_factory.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/layer.hpp>
#include "../../gson/json_array.hpp"
#include "../value.hpp"
#include "../../gson/json_element.hpp"

#include <jni/jni.hpp>

namespace mbgl {

namespace android {

class Layer {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/layers/Layer"; };

    static void registerNative(jni::JNIEnv&);

    virtual ~Layer();

    /**
     * Set core layer (ie return ownership after remove)
     */
    void setLayer(std::unique_ptr<mbgl::style::Layer>);

    void addToMap(mbgl::Map&, mbgl::optional<std::string>);

    jni::Local<jni::String> getId(jni::JNIEnv&);

    jni::Local<jni::String> getSourceId(jni::JNIEnv&);

    style::Layer& get();

    void setProperty(jni::JNIEnv&, const jni::String&, const jni::Object<>& value);

    // Zoom

    jni::jfloat getMinZoom(jni::JNIEnv&);

    jni::jfloat getMaxZoom(jni::JNIEnv&);

    void setMinZoom(jni::JNIEnv&, jni::jfloat zoom);

    void setMaxZoom(jni::JNIEnv&, jni::jfloat zoom);

    /* common properties, but not shared by all */

    void setFilter(jni::JNIEnv&, const jni::Array<jni::Object<>>&);

    jni::Local<jni::Object<gson::JsonElement>> getFilter(jni::JNIEnv&);

    void setSourceLayer(jni::JNIEnv&, const jni::String&);

    jni::Local<jni::String> getSourceLayer(jni::JNIEnv&);

    // Property getters

    jni::Local<jni::Object<jni::ObjectTag>> getVisibility(jni::JNIEnv&);

protected:
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
    Layer(std::unique_ptr<mbgl::style::Layer>);

    // Release the owned view and return it
    std::unique_ptr<mbgl::style::Layer> releaseCoreLayer();

    // Owned layer is set when creating a new layer, before adding it to the map
    std::unique_ptr<mbgl::style::Layer> ownedLayer;

    // Raw reference to the layer
    mbgl::style::Layer& layer;

    // Map is set when the layer is retrieved or after adding to the map
    mbgl::Map* map;
};

/**
 * @brief A factory class for a layer Java peer objects of a certain type.
 */
class JavaLayerPeerFactory {
public:
    virtual ~JavaLayerPeerFactory() = default;
    /**
     * @brief Create a non-owning peer.
     */
    virtual jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv&, mbgl::Map&, mbgl::style::Layer&) = 0;

    /**
     * @brief Create an owning peer.
     */
    virtual jni::Local<jni::Object<Layer>> createJavaLayerPeer(jni::JNIEnv& env, mbgl::Map& map, std::unique_ptr<mbgl::style::Layer>) = 0;

    /**
     * @brief Register peer methods.
     */
    virtual void registerNative(jni::JNIEnv&) = 0;

    /**
     * @brief Get the corresponding layer factory.
     * 
     * @return style::LayerFactory* must not be \c nullptr.
     */
    virtual LayerFactory* getLayerFactory() = 0;
};


} // namespace android
} // namespace mbgl
