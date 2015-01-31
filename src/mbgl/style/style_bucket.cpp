#include <mbgl/style/style_bucket.hpp>

namespace mbgl {

StyleBucket::StyleBucket(StyleLayerType type_) :type(type_) {}

template<> const StyleBucketFill &defaultLayoutProperties() { static StyleBucketFill p; return p; }
template<> const StyleBucketLine &defaultLayoutProperties() { static StyleBucketLine p; return p; }
template<> const StyleBucketSymbol &defaultLayoutProperties() { static StyleBucketSymbol p; return p; }
template<> const StyleBucketRaster &defaultLayoutProperties() { static StyleBucketRaster p; return p; }
template<> const StyleBucketBackground &defaultLayoutProperties() { static StyleBucketBackground p; return p; }

}