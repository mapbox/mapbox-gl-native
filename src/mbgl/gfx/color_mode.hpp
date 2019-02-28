#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace gfx {

class ColorMode {
public:
    enum class BlendEquation {
        Add,
        Subtract,
        ReverseSubtract
    };

    enum class BlendFactor {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        DstColor,
        OneMinusDstColor,
        SrcAlphaSaturate,
        ConstantColor,
        OneMinusConstantColor,
        ConstantAlpha,
        OneMinusConstantAlpha
    };

    template <BlendEquation E>
    struct ConstantBlend {
        static constexpr BlendEquation equation = E;
        static constexpr BlendFactor srcFactor = BlendFactor::One;
        static constexpr BlendFactor dstFactor = BlendFactor::One;
    };

    template <BlendEquation E>
    struct LinearBlend {
        static constexpr BlendEquation equation = E;
        BlendFactor srcFactor;
        BlendFactor dstFactor;
    };

    struct Replace {
        static constexpr BlendEquation equation = BlendEquation::Add;
        static constexpr BlendFactor srcFactor = BlendFactor::One;
        static constexpr BlendFactor dstFactor = BlendFactor::Zero;
    };

    using Add              = LinearBlend<BlendEquation::Add>;
    using Subtract         = LinearBlend<BlendEquation::Subtract>;
    using ReverseSubtract  = LinearBlend<BlendEquation::ReverseSubtract>;

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
        return { Add{ BlendFactor::One, BlendFactor::OneMinusSrcAlpha }, {}, { true, true, true, true } };
    }

    static ColorMode additive() {
        return { Add{ BlendFactor::One, BlendFactor::One }, {}, { true, true, true, true } };
    }
};

constexpr bool operator!=(const ColorMode::Mask& a, const ColorMode::Mask& b) {
    return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
}

} // namespace gfx
} // namespace mbgl
