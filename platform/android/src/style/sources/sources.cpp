#include "sources.hpp"

#include <mbgl/style/source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/sources/raster_source.hpp>
#include <mbgl/style/sources/vector_source.hpp>

#include "source.hpp"
#include "geojson_source.hpp"
#include "raster_source.hpp"
#include "vector_source.hpp"

namespace mbgl {
namespace android {

Source* initializeSourcePeer(mbgl::Map& map, mbgl::style::Source& coreSource) {
    Source* source;
    if (coreSource.is<mbgl::style::VectorSource>()) {
        source = new VectorSource(map, *coreSource.as<mbgl::style::VectorSource>());
    } else if (coreSource.is<mbgl::style::RasterSource>()) {
        source = new RasterSource(map, *coreSource.as<mbgl::style::RasterSource>());
    } else if (coreSource.is<mbgl::style::GeoJSONSource>()) {
        source = new GeoJSONSource(map, *coreSource.as<mbgl::style::GeoJSONSource>());
    } else {
        throw new std::runtime_error("Source type not implemented");
    }

    return source;
}

jni::jobject* createJavaSourcePeer(jni::JNIEnv& env, mbgl::Map& map, mbgl::style::Source& coreSource) {
    std::unique_ptr<Source> peerSource = std::unique_ptr<Source>(initializeSourcePeer(map, coreSource));
    jni::jobject* result = peerSource->createJavaPeer(env);
    peerSource.release();
    return result;
}

void registerNativeSources(jni::JNIEnv& env) {
    Source::registerNative(env);
    VectorSource::registerNative(env);
    RasterSource::registerNative(env);
    GeoJSONSource::registerNative(env);
}

}
}