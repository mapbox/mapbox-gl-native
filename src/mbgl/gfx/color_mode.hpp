#pragma once

#include <mbgl/gfx/types.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace gfx {

class ColorMode {
public:
    template <ColorBlendEquationType E>
    struct ConstantBlend {
        static constexpr ColorBlendEquationType equation = E;
        static constexpr ColorBlendFactorType srcFactor = ColorBlendFactorType::One;
        static constexpr ColorBlendFactorType dstFactor = ColorBlendFactorType::One;
    };

    template <ColorBlendEquationType E>
    struct LinearBlend {
        static constexpr ColorBlendEquationType equation = E;
        ColorBlendFactorType srcFactor;
        ColorBlendFactorType dstFactor;
    };

    struct Replace {
        static constexpr ColorBlendEquationType equation = ColorBlendEquationType::Add;
        static constexpr ColorBlendFactorType srcFactor = ColorBlendFactorType::One;
        static constexpr ColorBlendFactorType dstFactor = ColorBlendFactorType::Zero;
    };

    using Add              = LinearBlend<ColorBlendEquationType::Add>;
    using Subtract         = LinearBlend<ColorBlendEquationType::Subtract>;
    using ReverseSubtract  = LinearBlend<ColorBlendEquationType::ReverseSubtract>;

    using BlendFunction = variant<
        Replace,
        Add,
        Subtract,
        ReverseSubtract>;

    BlendFunction blendFunction;
    Color blendColor;

    struct Mask {
        bool r;
        bool g;
        bool b;
        bool a;
    };

    Mask mask;

    static ColorMode disabled() {
       return { Replace{}, {}, { false, false, false, false } };
    }

    static ColorMode unblended() {
       return { Replace{}, {}, { true, true, true, true } };
    }

    static ColorMode alphaBlended() {
        return { Add{ ColorBlendFactorType::One, ColorBlendFactorType::OneMinusSrcAlpha }, {}, { true, true, true, true } };
    }

    static ColorMode additive() {
        return { Add{ ColorBlendFactorType::One, ColorBlendFactorType::One }, {}, { true, true, true, true } };
    }
};

constexpr bool operator!=(const ColorMode::Mask& a, const ColorMode::Mask& b) {
    return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
}

} // namespace gfx
} // namespace mbgl
