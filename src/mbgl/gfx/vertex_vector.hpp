#pragma once

#include <mbgl/util/ignore.hpp>

#include <vector>

namespace mbgl {
namespace gfx {

template <class V>
class VertexVector {
public:
    using Vertex = V;

    template <class... Args>
    void emplace_back(Args&&... args) {
        static_assert(sizeof...(args) == 1, "wrong buffer element count");
        util::ignore({ (v.emplace_back(std::forward<Args>(args)), 0)... });
    }

    std::size_t elements() const {
        return v.size();
    }

    std::size_t bytes() const {
        return v.size() * sizeof(Vertex);
    }

    bool empty() const {
        return v.empty();
    }

    void clear() {
        v.clear();
    }

    const Vertex* data() const {
        return v.data();
    }

    const std::vector<Vertex>& vector() const {
        return v;
    }

private:
    std::vector<Vertex> v;
};

} // namespace gfx
} // namespace mbgl
