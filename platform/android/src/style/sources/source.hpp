#pragma once

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/style/source.hpp>

#include "../value.hpp"

#include <jni/jni.hpp>

namespace mbgl {
namespace android {

class Source : private mbgl::util::noncopyable {
public:

    static constexpr auto Name() { return "com/mapbox/mapboxsdk/style/sources/Source"; };

    static jni::Class<Source> javaClass;

    static void registerNative(jni::JNIEnv&);

    /*
     * Called when a Java object is created on the c++ side
     */
    Source(mbgl::Map&, mbgl::style::Source&);

    /*
     * Called when a Java object was created from the jvm side
     */
    Source(jni::JNIEnv&, std::unique_ptr<mbgl::style::Source>);

    virtual ~Source();

    /**
     * Set core source (ie return ownership after remove)
     */
    void setSource(std::unique_ptr<style::Source>);

    style::Source& get();

    void addToMap(mbgl::Map&);

    virtual jni::jobject* createJavaPeer(jni::JNIEnv&) = 0;

    jni::String getId(jni::JNIEnv&);

protected:
    //Release the owned view and return it
    std::unique_ptr<mbgl::style::Source> releaseCoreSource();

    //Set on newly created sources until added to the map
    std::unique_ptr<mbgl::style::Source> ownedSource;

    //Raw pointer that is valid until the source is removed from the map
    mbgl::style::Source& source;

    //Map pointer is valid for newly created sources only after adding to the map
    mbgl::Map* map;
};

} //android
} //mbgl




