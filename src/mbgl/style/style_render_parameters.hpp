#ifndef STYLE_RENDER_PARAMETERS
#define STYLE_RENDER_PARAMETERS

namespace mbgl {

class TransformState;

class StyleRenderParameters {
public:
    StyleRenderParameters(const TransformState& state_)
        : state(state_) {}

    const TransformState& state;
};

}

#endif
