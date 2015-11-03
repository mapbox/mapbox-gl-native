#ifndef MBGL_STYLE_FUNCTION
#define MBGL_STYLE_FUNCTION

#include <vector>
#include <utility>

namespace mbgl {

template <typename T>
class Function {
public:
    using Stop = std::pair<float, T>;
    using Stops = std::vector<Stop>;

    // TODO: make explicit
    /* explicit */ Function(const T& constant)
        : stops({{ 0, constant }}) {}

    explicit Function(const Stops& stops_, float base_)
        : base(base_), stops(stops_) {}

    T evaluate(float z) const;

private:
    const float base = 1;
    const std::vector<std::pair<float, T>> stops;
};

}

#endif
