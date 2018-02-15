#pragma once

#include <mbgl/util/variant.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace gl {

class ColorMode {
public:
    enum class BlendEquation {
        Add                   = 0x8006,
        Subtract              = 0x800A,
        ReverseSubtract       = 0x800B
    };

    enum BlendFactor {
        Zero                  = 0x0000,
        One                   = 0x0001,
        SrcColor              = 0x0300,
        OneMinusSrcColor      = 0x0301,
        SrcAlpha              = 0x0302,
        OneMinusSrcAlpha      = 0x0303,
        DstAlpha              = 0x0304,
        OneMinusDstAlpha      = 0x0305,
        DstColor              = 0x0306,
        OneMinusDstColor      = 0x0307,
        SrcAlphaSaturate      = 0x0308,
        ConstantColor         = 0x8001,
        OneMinusConstantColor = 0x8002,
        ConstantAlpha         = 0x8003,
        OneMinusConstantAlpha = 0x8004
    };

    template <BlendEquation E>
    struct ConstantBlend {
        static constexpr BlendEquation equation = E;
        static constexpr BlendFactor srcFactor = One;
        static constexpr BlendFactor dstFactor = One;
    };

    template <BlendEquation E>
    struct LinearBlend {
        static constexpr BlendEquation equation = E;
        BlendFactor srcFactor;
        BlendFactor dstFactor;
    };

    struct Replace {
        static constexpr BlendEquation equation = BlendEquation::Add;
        static constexpr BlendFactor srcFactor = One;
        static constexpr BlendFactor dstFactor = Zero;
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
       return ColorMode { Replace(), {}, { false, false, false, false } };
    }

    static ColorMode unblended() {
       return ColorMode { Replace(), {}, { true, true, true, true } };
    }

    static ColorMode alphaBlended() {
        return ColorMode { Add { One, OneMinusSrcAlpha }, {}, { true, true, true, true } };
    }

    static ColorMode additive() {
        return ColorMode { Add { One, One }, {}, { true, true, true, true } };
    }
};

constexpr bool operator!=(const ColorMode::Mask& a, const ColorMode::Mask& b) {
    return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
}

} // namespace gl
} // namespace mbgl
