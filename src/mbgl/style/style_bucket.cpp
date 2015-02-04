#include <mbgl/style/style_bucket.hpp>

namespace mbgl {

template<> const StyleBucketFill &defaultLayoutProperties() { static StyleBucketFill p; return p; }
template<> const StyleBucketLine &defaultLayoutProperties() { static StyleBucketLine p; return p; }
template<> const StyleBucketSymbol &defaultLayoutProperties() { static StyleBucketSymbol p; return p; }
template<> const StyleBucketRaster &defaultLayoutProperties() { static StyleBucketRaster p; return p; }
template<> const StyleBucketBackground &defaultLayoutProperties() { static StyleBucketBackground p; return p; }

StyleBucket::StyleBucket(StyleLayerType type_) : type(type_) {
    switch (type) {
        case StyleLayerType::Fill: render.set<StyleBucketFill>(); break;
        case StyleLayerType::Line: render.set<StyleBucketLine>(); break;
        case StyleLayerType::Symbol: render.set<StyleBucketSymbol>(); break;
        case StyleLayerType::Raster: render.set<StyleBucketRaster>(); break;
        default: break;
    }
}

}