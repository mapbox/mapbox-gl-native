#pragma once

#include <mbgl/gfx/draw_mode.hpp>
#include <mbgl/util/ignore.hpp>

#include <vector>

namespace mbgl {
namespace gfx {

template <class DrawMode>
class IndexVector {
public:
    static constexpr std::size_t groupSize = BufferGroupSizeOf<DrawMode>::value;

    template <class... Args>
    void emplace_back(Args&&... args) {
        static_assert(sizeof...(args) == groupSize, "wrong buffer element count");
        util::ignore({ (v.emplace_back(std::forward<Args>(args)), 0)... });
    }

    std::size_t elements() const {
        return v.size();
    }

    std::size_t bytes() const {
        return v.size() * sizeof(uint16_t);
    }

    bool empty() const {
        return v.empty();
    }

    void clear() {
        v.clear();
    }

    const uint16_t* data() const {
        return v.data();
    }

    const std::vector<uint16_t>& vector() const {
        return v;
    }

private:
    std::vector<uint16_t> v;
};

} // namespace gfx
} // namespace mbgl
