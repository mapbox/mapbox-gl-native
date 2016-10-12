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

    virtual jni::jobject* createJavaPeer(jni::JNIEnv&) = 0;

    jni::String getId(jni::JNIEnv&);

    //Release the owned view and return it
    std::unique_ptr<mbgl::style::Source> releaseCoreSource();

protected:
    std::unique_ptr<mbgl::style::Source> ownedSource;
    mbgl::style::Source& source;
    mbgl::Map* map;

};

} //android
} //mbgl




