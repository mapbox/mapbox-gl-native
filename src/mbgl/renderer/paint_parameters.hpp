#pragma once

namespace mbgl {

class Programs;
class View;

class PaintParameters {
public:
    Programs& programs;
    View& view;
};

} // namespace mbgl
