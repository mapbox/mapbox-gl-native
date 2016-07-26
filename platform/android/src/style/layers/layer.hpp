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
     * Called when a Java object is created on the c++ side
     */
    Layer(mbgl::Map&, mbgl::style::Layer&);

    /*
     * Called when a Java object was created from the jvm side
     */
    Layer(jni::JNIEnv&, std::unique_ptr<mbgl::style::Layer>);

    virtual ~Layer();

    virtual jni::jobject* createJavaPeer(jni::JNIEnv&) = 0;

    jni::String getId(jni::JNIEnv&);

    //Release the owned view and return it
    std::unique_ptr<mbgl::style::Layer> releaseCoreLayer();

    void setLayoutProperty(jni::JNIEnv&, jni::String, jni::Object<> value);

    void setPaintProperty(jni::JNIEnv&, jni::String, jni::Object<> value);

    void updateStyle(jni::JNIEnv&, jni::jboolean updateClasses);

    //Zoom

    jni::jfloat getMinZoom(jni::JNIEnv&);

    jni::jfloat getMaxZoom(jni::JNIEnv&);

    void setMinZoom(jni::JNIEnv&, jni::jfloat zoom);

    void setMaxZoom(jni::JNIEnv&, jni::jfloat zoom);

    /* common properties, but not shared by all */

    void setFilter(jni::JNIEnv& env, jni::Array<jni::Object<>> jfilter);

    void setSourceLayer(jni::JNIEnv& env, jni::String sourceLayer);

    //Property getters

    jni::Object<jni::ObjectTag> getVisibility(jni::JNIEnv&);

protected:
    std::unique_ptr<mbgl::style::Layer> ownedLayer;
    mbgl::style::Layer& layer;
    mbgl::Map* map;

};

} //android
} //mbgl




