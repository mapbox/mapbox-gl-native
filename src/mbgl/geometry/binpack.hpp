#ifndef MBGL_GEOMETRY_BINPACK
#define MBGL_GEOMETRY_BINPACK

#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/rect.hpp>
#include <cstdint>
#include <list>

namespace mbgl {

template <typename T>
class BinPack : private util::noncopyable {
public:
    BinPack(T width, T height)
        : free(1, Rect<uint16_t>{ 0, 0, width, height }) {}
public:
    Rect<T> allocate(T width, T height) {
        // Find the smallest free rect angle
        auto smallest = free.end();
        for (auto it = free.begin(); it != free.end(); ++it) {
            const Rect<T>& ref = *it;
            const Rect<T>& rect = *smallest;
            if (width <= ref.w && height <= ref.h) {
                if (smallest == free.end() || (ref.y <= rect.y && ref.x <= rect.x)) {
                    smallest = it;
                } else {
                    // Our current "smallest" rect is already closer to 0/0.
                }
            } else {
                // The rect in the free list is not big enough.
            }
        }

        if (smallest == free.end()) {
            // There's no space left for this char.
            return Rect<uint16_t>{ 0, 0, 0, 0 };
        } else {
            Rect<T> rect = *smallest;
            free.erase(smallest);

            // Shorter/Longer Axis Split Rule (SAS)
            // http://clb.demon.fi/files/RectangleBinPack.pdf p. 15
            // Ignore the dimension of R and just split long the shorter dimension
            // See Also: http://www.cs.princeton.edu/~chazelle/pubs/blbinpacking.pdf
            if (rect.w < rect.h) {
                // split horizontally
                // +--+---+
                // |__|___|  <-- b1
                // +------+  <-- b2
                if (rect.w > width) free.emplace_back(rect.x + width, rect.y, rect.w - width, height);
                if (rect.h > height) free.emplace_back(rect.x, rect.y + height, rect.w, rect.h - height);
            } else {
                // split vertically
                // +--+---+
                // |__|   | <-- b1
                // +--|---+ <-- b2
                if (rect.w > width) free.emplace_back(rect.x + width, rect.y, rect.w - width, rect.h);
                if (rect.h > height) free.emplace_back(rect.x, rect.y + height, width, rect.h - height);
            }

            return Rect<uint16_t>{ rect.x, rect.y, width, height };
        }
    }


    void release(Rect<T> rect) {
        // Simple algorithm to recursively merge the newly released cell with its
        // neighbor. This doesn't merge more than two cells at a time, and fails
        // for complicated merges.
        for (auto it = free.begin(); it != free.end(); ++it) {
            Rect<T> ref = *it;
            if (ref.y == rect.y && ref.h == rect.h && ref.x + ref.w == rect.x) {
                ref.w += rect.w;
            }
            else if (ref.x == rect.x && ref.w == rect.w && ref.y + ref.h == rect.y) {
                ref.h += rect.h;
            }
            else if (rect.y == ref.y && rect.h == ref.h && rect.x + rect.w == ref.x) {
                ref.x = rect.x;
                ref.w += rect.w;
            }
            else if (rect.x == ref.x && rect.w == ref.w && rect.y + rect.h == ref.y) {
                ref.y = rect.y;
                ref.h += rect.h;
            } else {
                continue;
            }

            free.erase(it);
            release(ref);
            return;

        }

        free.emplace_back(rect);
    };

private:
    std::list<Rect<T>> free;
};

}

#endif
