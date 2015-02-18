#include <mbgl/style/style_layout.hpp>

namespace mbgl {

template<> const StyleLayoutFill &defaultStyleLayout() { static StyleLayoutFill p; return p; }
template<> const StyleLayoutLine &defaultStyleLayout() { static StyleLayoutLine p; return p; }
template<> const StyleLayoutSymbol &defaultStyleLayout() { static StyleLayoutSymbol p; return p; }
template<> const StyleLayoutRaster &defaultStyleLayout() { static StyleLayoutRaster p; return p; }
template<> const StyleLayoutBackground &defaultStyleLayout() { static StyleLayoutBackground p; return p; }

}