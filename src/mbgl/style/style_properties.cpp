#include <mbgl/style/style_properties.hpp>
#include <mbgl/style/piecewisefunction_properties.hpp>

namespace mbgl {

template<> const FillProperties &defaultStyleProperties() { static const FillProperties p; return p; }
template<> const LineProperties &defaultStyleProperties() { static const LineProperties p; return p; }
template<> const CircleProperties &defaultStyleProperties() { static const CircleProperties p; return p; }
template<> const SymbolProperties &defaultStyleProperties() { static const SymbolProperties p; return p; }
template<> const RasterProperties &defaultStyleProperties() { static const RasterProperties p; return p; }
template<> const BackgroundProperties &defaultStyleProperties() { static const BackgroundProperties p; return p; }

}
