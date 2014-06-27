#include <llmr/style/style_properties.hpp>

namespace llmr {

template<> const FillProperties &defaultStyleProperties() { static const FillProperties p; return p; }
template<> const LineProperties &defaultStyleProperties() { static const LineProperties p; return p; }
template<> const IconProperties &defaultStyleProperties() { static const IconProperties p; return p; }
template<> const TextProperties &defaultStyleProperties() { static const TextProperties p; return p; }
template<> const CompositeProperties &defaultStyleProperties() { static const CompositeProperties p; return p; }
template<> const RasterProperties &defaultStyleProperties() { static const RasterProperties p; return p; }
template<> const BackgroundProperties &defaultStyleProperties() { static const BackgroundProperties p; return p; }

}
