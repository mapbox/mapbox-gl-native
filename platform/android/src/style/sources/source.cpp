#include "source.hpp"
#include "../android_conversion.hpp"

#include <jni/jni.hpp>

#include <mbgl/style/style.hpp>
#include <mbgl/util/logging.hpp>

// Java -> C++ conversion
#include <mbgl/style/conversion.hpp>
#include <mbgl/style/conversion/source.hpp>

// C++ -> Java conversion
#include "../conversion/property_value.hpp"

#include <string>

// Core Sources
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/vector_source.hpp>

// Android Source peers
#include "geojson_source.hpp"
#include "image_source.hpp"
#include "raster_source.hpp"
#include "unknown_source.hpp"
#include "vector_source.hpp"
#include "custom_geometry_source.hpp"
#include "raster_dem_source.hpp"

namespace mbgl {
namespace android {

    static std::unique_ptr<Source> createSourcePeer(jni::JNIEnv& env, mbgl::style::Source& coreSource, AndroidRendererFrontend& frontend) {
        if (coreSource.is<mbgl::style::VectorSource>()) {
            return std::make_unique<VectorSource>(env, *coreSource.as<mbgl::style::VectorSource>(), frontend);
        } else if (coreSource.is<mbgl::style::RasterSource>()) {
            return std::make_unique<RasterSource>(env, *coreSource.as<mbgl::style::RasterSource>(), frontend);
        } else if (coreSource.is<mbgl::style::GeoJSONSource>()) {
            return std::make_unique<GeoJSONSource>(env, *coreSource.as<mbgl::style::GeoJSONSource>(), frontend);
        } else if (coreSource.is<mbgl::style::ImageSource>()) {
            return std::make_unique<ImageSource>(env, *coreSource.as<mbgl::style::ImageSource>(), frontend);
        } else {
            return std::make_unique<UnknownSource>(env, coreSource, frontend);
        }
    }

    jni::Object<Source> Source::peerForCoreSource(jni::JNIEnv& env, mbgl::style::Source& coreSource, AndroidRendererFrontend& frontend) {
        if (!coreSource.peer.has_value()) {
            coreSource.peer = createSourcePeer(env, coreSource, frontend);
        }
        return *mbgl::util::any_cast<std::unique_ptr<Source>>(&coreSource.peer)->get()->javaPeer;
    }

    Source::Source(jni::JNIEnv& env, mbgl::style::Source& coreSource, jni::Object<Source> obj, AndroidRendererFrontend& frontend)
        : source(coreSource)
        , javaPeer(obj.NewGlobalRef(env))
        , rendererFrontend(&frontend) {
    }

    Source::Source(jni::JNIEnv&, std::unique_ptr<mbgl::style::Source> coreSource)
        : ownedSource(std::move(coreSource))
        , source(*ownedSource) {
    }

    Source::~Source() {
        // Before being added to a map, the Java peer owns this C++ peer and cleans
        //  up after itself correctly through the jni native peer bindings.
        // After being added to the map, the ownership is flipped and the C++ peer has a strong reference
        //  to it's Java peer, preventing the Java peer from being GC'ed.
        //  In this case, the core source initiates the destruction, which requires releasing the Java peer,
        //  while also resetting it's nativePtr to 0 to prevent the subsequent GC of the Java peer from
        //  re-entering this dtor.
        if (ownedSource.get() == nullptr && javaPeer.get() != nullptr) {
            // Manually clear the java peer
            android::UniqueEnv env = android::AttachEnv();
            static auto nativePtrField = javaClass.GetField<jlong>(*env, "nativePtr");
            javaPeer->Set(*env, nativePtrField, (jlong) 0);
            javaPeer.reset();
        }
    }

    jni::String Source::getId(jni::JNIEnv& env) {
        return jni::Make<jni::String>(env, source.getID());
    }

    jni::String Source::getAttribution(jni::JNIEnv& env) {
        auto attribution = source.getAttribution();
        return attribution ? jni::Make<jni::String>(env, attribution.value()) : jni::Make<jni::String>(env,"");
    }

    void Source::addToMap(JNIEnv& env, jni::Object<Source> obj, mbgl::Map& map, AndroidRendererFrontend& frontend) {
        // Check to see if we own the source first
        if (!ownedSource) {
            throw std::runtime_error("Cannot add source twice");
        }

        // Add source to map and release ownership
        map.getStyle().addSource(std::move(ownedSource));

        // Add peer to core source
        source.peer = std::unique_ptr<Source>(this);

        // Add strong reference to java source
        javaPeer = obj.NewGlobalRef(env);

        rendererFrontend = &frontend;
    }

    void Source::removeFromMap(JNIEnv&, jni::Object<Source>, mbgl::Map& map) {
        // Cannot remove if not attached yet
        if (ownedSource) {
            throw std::runtime_error("Cannot remove detached source");
        }

        // Remove the source from the map and take ownership
        ownedSource = map.getStyle().removeSource(source.getID());

        // The source may not be removed if any layers still reference it
        if (!ownedSource) {
            return;
        }

        // Release the peer relationships. These will be re-established when the source is added to a map
        assert(ownedSource->peer.has_value());
        util::any_cast<std::unique_ptr<Source>>(&(ownedSource->peer))->release();
        ownedSource->peer.reset();

        // Release the strong reference to the java peer
        assert(javaPeer);
        javaPeer.release();

        rendererFrontend = nullptr;
    }

    jni::Class<Source> Source::javaClass;

    void Source::registerNative(jni::JNIEnv& env) {
        // Lookup the class
        Source::javaClass = *jni::Class<Source>::Find(env).NewGlobalRef(env).release();

        #define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

        // Register the peer
        jni::RegisterNativePeer<Source>(env, Source::javaClass, "nativePtr",
            METHOD(&Source::getId, "nativeGetId"),
            METHOD(&Source::getAttribution, "nativeGetAttribution")
        );

        // Register subclasses
        GeoJSONSource::registerNative(env);
        ImageSource::registerNative(env);
        RasterSource::registerNative(env);
        UnknownSource::registerNative(env);
        VectorSource::registerNative(env);
        CustomGeometrySource::registerNative(env);
        RasterDEMSource::registerNative(env);
    }

} // namespace android
} // namespace mbgl
