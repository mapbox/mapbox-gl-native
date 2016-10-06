#pragma once

namespace mbgl {

class Shaders;
class View;

class PaintParameters {
public:
    Shaders& shaders;
    View& view;
};

} // namespace mbgl
