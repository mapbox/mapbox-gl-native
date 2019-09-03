#pragma once

#include <mbgl/util/ignore.hpp>

#include <vector>

namespace mbgl {
namespace gfx {

template <class V>
class VertexVector {
public:
    using Vertex = V;
    template<typename Arg>
    void emplace_back(Arg&& vertex) {
        v.emplace_back(std::forward<Arg>(vertex));
    }

    void extend(std::size_t n, const Vertex& val) {
        v.resize(v.size() + n, val);
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
