#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {
namespace gl {
namespace value {

struct ClearDepth {
    using Type = float;
    static const constexpr Type Default = 1;
    static void Set(const Type&);
    static Type Get();
};

struct ClearColor {
    using Type = Color;
    static const constexpr Type Default = { 0, 0, 0, 0 };
    static void Set(const Type&);
    static Type Get();
};

struct ClearStencil {
    using Type = StencilValue;
    static const constexpr Type Default = 0;
    static void Set(const Type&);
    static Type Get();
};

struct StencilMask {
    using Type = StencilMaskValue;
    static const constexpr Type Default = ~0u;
    static void Set(const Type&);
    static Type Get();
};

struct DepthMask {
    using Type = bool;
    static const constexpr Type Default = true;
    static void Set(const Type&);
    static Type Get();
};

struct ColorMask {
    struct Type {
        bool r;
        bool g;
        bool b;
        bool a;
    };
    static const constexpr Type Default = { true, true, true, true };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const ColorMask::Type& a, const ColorMask::Type& b) {
    return a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a;
}

struct StencilFunc {
    struct Type {
        StencilTestFunction func;
        StencilValue ref;
        StencilMaskValue mask;
    };
    static const constexpr Type Default = { StencilTestFunction::Always, 0, ~0u };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const StencilFunc::Type& a, const StencilFunc::Type& b) {
    return a.func != b.func || a.ref != b.ref || a.mask != b.mask;
}

struct StencilTest {
    using Type = bool;
    static const constexpr Type Default = false;
    static void Set(const Type&);
    static Type Get();
};

struct StencilOp {
    struct Type {
        StencilTestOperation sfail;
        StencilTestOperation dpfail;
        StencilTestOperation dppass;
    };
    static const constexpr Type Default = { StencilTestOperation::Keep, StencilTestOperation::Keep,
                                            StencilTestOperation::Keep };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const StencilOp::Type& a, const StencilOp::Type& b) {
    return a.sfail != b.sfail || a.dpfail != b.dpfail || a.dppass != b.dppass;
}

struct DepthRange {
    struct Type {
        float near;
        float far;
    };
    static const constexpr Type Default = { 0, 1 };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const DepthRange::Type& a, const DepthRange::Type& b) {
    return a.near != b.near || a.far != b.far;
}

struct DepthTest {
    using Type = bool;
    static const constexpr Type Default = false;
    static void Set(const Type&);
    static Type Get();
};

struct DepthFunc {
    using Type = DepthTestFunction;
    static const constexpr Type Default = DepthTestFunction::Less;
    static void Set(const Type&);
    static Type Get();
};

struct Blend {
    using Type = bool;
    static const constexpr Type Default = true;
    static void Set(const Type&);
    static Type Get();
};

struct BlendFunc {
    struct Type {
        BlendSourceFactor sfactor;
        BlendDestinationFactor dfactor;
    };
    static const constexpr Type Default = { BlendSourceFactor::One, BlendDestinationFactor::Zero };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const BlendFunc::Type& a, const BlendFunc::Type& b) {
    return a.sfactor != b.sfactor || a.dfactor != b.dfactor;
}

struct BlendColor {
    using Type = Color;
    static const constexpr Type Default = { 0, 0, 0, 0 };
    static void Set(const Type&);
    static Type Get();
};

struct Program {
    using Type = gl::ProgramID;
    static const constexpr Type Default = 0;
    static void Set(const Type&);
    static Type Get();
};

struct LineWidth {
    using Type = float;
    static const constexpr Type Default = 1;
    static void Set(const Type&);
    static Type Get();
};

struct ActiveTexture {
    using Type = TextureUnit;
    static const constexpr Type Default = 0;
    static void Set(const Type&);
    static Type Get();
};

struct Viewport {
    struct Type {
        int32_t x;
        int32_t y;
        uint16_t width;
        uint16_t height;
    };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const Viewport::Type& a, const Viewport::Type& b) {
    return a.x != b.x || a.y != b.y || a.width != b.width || a.height != b.height;
}

struct BindFramebuffer {
    using Type = FramebufferID;
    static void Set(const Type&);
    static Type Get();
};

struct BindTexture {
    using Type = gl::TextureID;
    static const constexpr Type Default = 0;
    static void Set(const Type&);
    static Type Get();
};

struct BindVertexBuffer {
    using Type = gl::BufferID;
    static const constexpr Type Default = 0;
    static void Set(const Type&);
    static Type Get();
};

struct BindElementBuffer {
    using Type = gl::BufferID;
    static const constexpr Type Default = 0;
    static void Set(const Type&);
    static Type Get();
};

struct BindVertexArray {
    using Type = gl::VertexArrayID;
    static const constexpr Type Default = 0;
    static void Set(const Type&);
    static Type Get();
};

#if not MBGL_USE_GLES2

struct PixelZoom {
    struct Type {
        float xfactor;
        float yfactor;
    };
    static const constexpr Type Default = { 1, 1 };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const PixelZoom::Type& a, const PixelZoom::Type& b) {
    return a.xfactor != b.xfactor || a.yfactor != b.yfactor;
}

struct RasterPos {
    struct Type {
        double x;
        double y;
        double z;
        double w;
    };
    static const constexpr Type Default = { 0, 0, 0, 0 };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const RasterPos::Type& a, const RasterPos::Type& b) {
    return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}

#endif // MBGL_USE_GLES2

} // namespace value
} // namespace gl
} // namespace mbgl
