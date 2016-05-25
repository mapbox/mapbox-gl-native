#pragma once

namespace mbgl {

class TransformState;

class StyleRenderParameters {
public:
    StyleRenderParameters(const TransformState& state_)
        : state(state_) {}

    const TransformState& state;
};

}
