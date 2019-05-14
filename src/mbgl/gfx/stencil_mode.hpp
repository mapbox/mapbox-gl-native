#pragma once

#include <mbgl/gfx/types.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {
namespace gfx {

class StencilMode {
public:
    template <StencilFunctionType F>
    struct SimpleTest {
        static constexpr StencilFunctionType func = F;
        static constexpr uint32_t mask = 0;
    };

    template <StencilFunctionType F>
    struct MaskedTest {
        static constexpr StencilFunctionType func = F;
        uint32_t mask;
    };

    using Never        = SimpleTest<StencilFunctionType::Never>;
    using Less         = MaskedTest<StencilFunctionType::Less>;
    using Equal        = MaskedTest<StencilFunctionType::Equal>;
    using LessEqual    = MaskedTest<StencilFunctionType::LessEqual>;
    using Greater      = MaskedTest<StencilFunctionType::Greater>;
    using NotEqual     = MaskedTest<StencilFunctionType::NotEqual>;
    using GreaterEqual = MaskedTest<StencilFunctionType::GreaterEqual>;
    using Always       = SimpleTest<StencilFunctionType::Always>;

    using Test = variant<
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always>;

    Test test;
    int32_t ref;
    uint32_t mask;

    StencilOpType fail;
    StencilOpType depthFail;
    StencilOpType pass;

    static StencilMode disabled() {
       return StencilMode { Always(), 0, 0, StencilOpType::Keep, StencilOpType::Keep, StencilOpType::Keep };
    }
};

template <StencilFunctionType F>
constexpr StencilFunctionType StencilMode::SimpleTest<F>::func;

template <StencilFunctionType F>
constexpr uint32_t StencilMode::SimpleTest<F>::mask;

template <StencilFunctionType F>
constexpr StencilFunctionType StencilMode::MaskedTest<F>::func;

} // namespace gfx
} // namespace mbgl
