#ifndef MBGL_UTIL_INTERACTIVE_FEATURES_IMPL
#define MBGL_UTIL_INTERACTIVE_FEATURES_IMPL

#include <mbgl/util/interactive_features.hpp>

#include <tuple>
#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wshadow"
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wdeprecated-register"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#pragma GCC diagnostic pop

namespace mbgl {

namespace FeatureBG = boost::geometry;
namespace FeatureBGM = FeatureBG::model;
namespace FeatureBGI = FeatureBG::index;

typedef FeatureBGM::point<int16_t, 2, FeatureBG::cs::cartesian> FeaturePoint;
typedef FeatureBGM::box<FeaturePoint> FeatureBox;
typedef std::tuple<FeatureBox, std::string, FeatureProperties> Feature;
typedef FeatureBGI::rtree<Feature, FeatureBGI::linear<16, 4>> FeatureTree;

}

#endif
