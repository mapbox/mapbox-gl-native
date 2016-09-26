#pragma once

#include <mbgl/util/variant.hpp>

namespace mbgl {
namespace gl {

class StencilMode {
public:
    template <uint32_t F>
    struct SimpleTest {
        static constexpr uint32_t func = F;
        static constexpr uint32_t mask = 0;
    };

    template <uint32_t F>
    struct MaskedTest {
        static constexpr uint32_t func = F;
        uint32_t mask;
    };

    using Never        = SimpleTest<0x0200>;
    using Less         = MaskedTest<0x0201>;
    using Equal        = MaskedTest<0x0202>;
    using LessEqual    = MaskedTest<0x0203>;
    using Greater      = MaskedTest<0x0204>;
    using NotEqual     = MaskedTest<0x0205>;
    using GreaterEqual = MaskedTest<0x0206>;
    using Always       = SimpleTest<0x0207>;

    using Test = variant<
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always>;

    enum Op {
        Zero          = 0x0000,
        Keep          = 0x1E00,
        Replace       = 0x1E01,
        Increment     = 0x1E02,
        Decrement     = 0x1E03,
        Invert        = 0x150A,
        IncrementWrap = 0x8507,
        DecrementWrap = 0x8508
    };

    Test test;
    int32_t ref;
    uint32_t mask;

    Op fail;
    Op depthFail;
    Op pass;

    static StencilMode disabled() {
       return StencilMode { Always(), 0, 0, Keep, Keep, Keep };
    }
};

} // namespace gl
} // namespace mbgl
