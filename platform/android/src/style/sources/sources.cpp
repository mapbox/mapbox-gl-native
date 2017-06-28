#include "sources.hpp"

#include <mbgl/style/source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/vector_source.hpp>

#include "source.hpp"
#include "geojson_source.hpp"
#include "image_source.hpp"
#include "raster_source.hpp"
#include "unknown_source.hpp"
#include "vector_source.hpp"

namespace {

    using namespace mbgl::android;

    Source* initializeSourcePeer(mbgl::style::Source& coreSource) {
        Source* source;
        if (coreSource.is<mbgl::style::VectorSource>()) {
            source = new VectorSource(*coreSource.as<mbgl::style::VectorSource>());
        } else if (coreSource.is<mbgl::style::RasterSource>()) {
            source = new RasterSource(*coreSource.as<mbgl::style::RasterSource>());
        } else if (coreSource.is<mbgl::style::GeoJSONSource>()) {
            source = new GeoJSONSource(*coreSource.as<mbgl::style::GeoJSONSource>());
        } else if (coreSource.is<mbgl::style::ImageSource>()) {
            source = new ImageSource(*coreSource.as<mbgl::style::ImageSource>());
        } else {
            source = new UnknownSource(coreSource);
        }

        return source;
    }
} // namespace

namespace mbgl {
namespace android {


jni::jobject* createJavaSourcePeer(jni::JNIEnv& env, AndroidRendererFrontend& frontend, mbgl::style::Source& coreSource) {
    std::unique_ptr<Source> peerSource = std::unique_ptr<Source>(initializeSourcePeer(coreSource));
    peerSource->setRendererFrontend(frontend);
    jni::jobject* result = peerSource->createJavaPeer(env);
    peerSource.release();
    return result;
}

void registerNativeSources(jni::JNIEnv& env) {
    Source::registerNative(env);
    GeoJSONSource::registerNative(env);
    ImageSource::registerNative(env);
    RasterSource::registerNative(env);
    UnknownSource::registerNative(env);
    VectorSource::registerNative(env);
}

}
}
