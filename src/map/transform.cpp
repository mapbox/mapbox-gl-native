#include <llmr/map/transform.hpp>

using namespace llmr;

transform::transform()
    : x(0.0f),
      y(0.0f),
      scale(1.0f) {

}

void transform::moveBy(double dx, double dy) {
    x += dx;
    y += dy;
}

void transform::scaleBy(double ds, double cx, double cy) {
    scale *= ds;
    x = x * ds + cx * (1 - ds);
    y = y * ds + cy * (1 - ds);
}
