#pragma once

#include <mbgl/gl/types.hpp>
#include <mbgl/gl/depth_mode.hpp>
#include <mbgl/gl/stencil_mode.hpp>
#include <mbgl/gl/color_mode.hpp>
#include <mbgl/gl/attribute.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/util/range.hpp>

namespace mbgl {
namespace gl {

class Context;

namespace value {

struct ClearDepth {
    using Type = float;
    static const constexpr Type Default = 1;
    static void Set(const Type&);
    static Type Get();
};

struct ClearColor {
    using Type = Color;
    static const Type Default;
    static void Set(const Type&);
    static Type Get();
};

struct ClearStencil {
    using Type = int32_t;
    static const constexpr Type Default = 0;
    static void Set(const Type&);
    static Type Get();
};

struct StencilMask {
    using Type = uint32_t;
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
    using Type = ColorMode::Mask;
    static const constexpr Type Default = { true, true, true, true };
    static void Set(const Type&);
    static Type Get();
};

struct StencilFunc {
    struct Type {
        uint32_t func;
        int32_t ref;
        uint32_t mask;
    };
    static const constexpr Type Default = { StencilMode::Always::func, 0, ~0u };
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
        StencilMode::Op sfail;
        StencilMode::Op dpfail;
        StencilMode::Op dppass;
    };
    static const constexpr Type Default = { StencilMode::Keep, StencilMode::Keep, StencilMode::Keep };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const StencilOp::Type& a, const StencilOp::Type& b) {
    return a.sfail != b.sfail || a.dpfail != b.dpfail || a.dppass != b.dppass;
}

struct DepthRange {
    using Type = Range<float>;
    static const constexpr Type Default = { 0, 1 };
    static void Set(const Type&);
    static Type Get();
};

struct DepthTest {
    using Type = bool;
    static const constexpr Type Default = false;
    static void Set(const Type&);
    static Type Get();
};

struct DepthFunc {
    using Type = DepthMode::Function;
    static const constexpr Type Default = DepthMode::Less;
    static void Set(const Type&);
    static Type Get();
};

struct Blend {
    using Type = bool;
    static const constexpr Type Default = true;
    static void Set(const Type&);
    static Type Get();
};

struct BlendEquation {
    using Type = ColorMode::BlendEquation;
    static const constexpr Type Default = ColorMode::BlendEquation::Add;
    static void Set(const Type&);
    static Type Get();
};

struct BlendFunc {
    struct Type {
        ColorMode::BlendFactor sfactor;
        ColorMode::BlendFactor dfactor;
    };
    static const constexpr Type Default = { ColorMode::One, ColorMode::Zero };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const BlendFunc::Type& a, const BlendFunc::Type& b) {
    return a.sfactor != b.sfactor || a.dfactor != b.dfactor;
}

struct BlendColor {
    using Type = Color;
    static const Type Default;
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

struct ActiveTextureUnit {
    using Type = TextureUnit;
    static const constexpr Type Default = 0;
    static void Set(const Type&);
    static Type Get();
};

struct Viewport {
    struct Type {
        int32_t x;
        int32_t y;
        Size size;
    };
    static const constexpr Type Default = { 0, 0, { 0, 0 } };
    static void Set(const Type&);
    static Type Get();
};

struct ScissorTest {
    using Type = bool;
    static const constexpr Type Default = false;
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const Viewport::Type& a, const Viewport::Type& b) {
    return a.x != b.x || a.y != b.y || a.size != b.size;
}

constexpr bool operator==(const Viewport::Type& a, const Viewport::Type& b) {
    return !(a != b);
}

struct BindFramebuffer {
    using Type = FramebufferID;
    static const constexpr Type Default = 0;
    static void Set(const Type&);
    static Type Get();
};

struct BindRenderbuffer {
    using Type = RenderbufferID;
    static const constexpr Type Default = 0;
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
    static void Set(const Type&, const Context&);
    static Type Get(const Context&);
};

struct VertexAttribute {
    using Type = optional<gl::AttributeBinding>;
    static const Type Default;
    static void Set(const Type&, Context&, AttributeLocation);
};

struct PixelStorePack {
    using Type = PixelStorageType;
    static const constexpr Type Default = { 4 };
    static void Set(const Type&);
    static Type Get();
};

struct PixelStoreUnpack {
    using Type = PixelStorageType;
    static const constexpr Type Default = { 4 };
    static void Set(const Type&);
    static Type Get();
};

#if not MBGL_USE_GLES2

struct PointSize {
    using Type = float;
    static const constexpr Type Default = 1;
    static void Set(const Type&);
    static Type Get();
};

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
    static const constexpr Type Default = { 0, 0, 0, 1 };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const RasterPos::Type& a, const RasterPos::Type& b) {
    return a.x != b.x || a.y != b.y || a.z != b.z || a.w != b.w;
}

struct PixelTransferDepth {
    struct Type {
        float scale;
        float bias;
    };
    static const constexpr Type Default = { 1, 0 };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const PixelTransferDepth::Type& a, const PixelTransferDepth::Type& b) {
    return a.scale != b.scale || a.bias != b.bias;
}

struct PixelTransferStencil {
    struct Type {
        int32_t shift;
        int32_t offset;
    };
    static const constexpr Type Default = { 0, 0 };
    static void Set(const Type&);
    static Type Get();
};

constexpr bool operator!=(const PixelTransferStencil::Type& a, const PixelTransferStencil::Type& b) {
    return a.shift != b.shift || a.offset != b.offset;
}

#endif // MBGL_USE_GLES2

} // namespace value
} // namespace gl
} // namespace mbgl
