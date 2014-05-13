#include <llmr/util/clip_ids.hpp>
#include <llmr/map/tile.hpp>

#include <llmr/util/math.hpp>

#include <list>
#include <vector>
#include <bitset>
#include <cassert>
#include <iostream>
#include <algorithm>

namespace llmr {

struct TileHierarchy {
    TileHierarchy(Tile::ID id, std::list<TileHierarchy> &&children)
        : id(id), children(std::move(children)) {}

    const Tile::ID id;
    ClipID clip;
    std::list<TileHierarchy> children;
};

std::list<TileHierarchy> partition(std::forward_list<Tile::ID> &&array) {
    if (array.empty()) {
        // We don't have to update the clipping mask because there are no tiles
        // anyway.
        return {};
    }

    int8_t minZ = array.begin()->z;

    std::list<TileHierarchy> result;
    std::forward_list<Tile::ID> remainder;
    auto remainder_it = remainder.before_begin();

    while (!array.empty()) {
        const Tile::ID id = array.front();
        array.pop_front();
        if (id.z == minZ) {
            std::forward_list<Tile::ID> children;
            auto children_it = children.before_begin();

            array.remove_if([&id, &children, &children_it](const Tile::ID &child) {
                if (child.isChildOf(id)) {
                    children_it = children.insert_after(children_it, child);
                    return true;
                } else {
                    return false;
                }
            });

            result.emplace_back(id, partition(std::move(children)));
        } else {
            remainder_it = remainder.insert_after(remainder_it, id);
        }
    }

    // Concatenate the remainder.
    if (!array.empty()) {
        result.splice(result.begin(), partition(std::move(remainder)));
    }

    return result;
}

uint8_t prefix(std::list<TileHierarchy> &array, TileHierarchy *parent = nullptr) {
    if (array.empty()) {
        return 0;
    }

    bool all_children_are_immediate = true;
    uint8_t max_child_prefix_length = 0;

    struct Huffman {
        explicit Huffman(int prefix_length, TileHierarchy *item)
            : prefix_length(prefix_length), children(1, item) {}
        uint8_t prefix_length;
        std::vector<TileHierarchy *> children;
    };

    // Create a temporary structure that we use for sorting the prefix tree.
    std::list<Huffman> huffman;
    std::transform(array.begin(), array.end(), std::back_inserter(huffman), [parent, &all_children_are_immediate, &max_child_prefix_length](TileHierarchy &item) {
        uint8_t prefix_length = prefix(item.children, &item);

        if (prefix_length > max_child_prefix_length) {
            max_child_prefix_length = prefix_length;
        }

        if (!parent || item.id.z != parent->id.z + 1) {
            all_children_are_immediate = false;
        }

        return Huffman { prefix_length + 1, &item };
    });

    while (huffman.size() > 1) {
        huffman.sort([](const Huffman &a, const Huffman &b) {
            return a.prefix_length < b.prefix_length;
        });

        Huffman &first = *huffman.begin();
        Huffman &second = *(++huffman.begin());

        assert(&first != &second);

        // Prefix with 0
        std::for_each(first.children.begin(), first.children.end(), [](TileHierarchy *child) {
            child->clip.mask >>= 1;
            child->clip.mask.set(7, false); // noop
            child->clip.length++;
        });
        first.prefix_length++;

        // Prefix with 1
        std::for_each(second.children.begin(), second.children.end(), [](TileHierarchy *child) {
            child->clip.mask >>= 1;
            child->clip.mask.set(7, true);
            child->clip.length++;
        });
        second.prefix_length++;

        second.children.insert(second.children.end(), first.children.begin(), first.children.end());
        second.prefix_length = first.prefix_length + second.prefix_length;

        // Remove the first child as we've just merged it into the second version.
        huffman.erase(huffman.begin());
    }

    uint8_t prefix_length = 0;

    // Filter out all-zero bits
    bool filter_zero = !all_children_are_immediate || array.size() != 4;

    for (TileHierarchy &item : array) {
        if (filter_zero && !item.clip.mask.any()) {
            // Make sure we don't have a prefix that is all zeros.
            // item.clip.mask |= (0x80 >> item.length);
            item.clip.mask.set(7 - item.clip.length);
            item.clip.length++;
        }

        if (item.clip.length > prefix_length) {
            prefix_length = item.clip.length;
        }
    }

    return max_child_prefix_length + prefix_length;
}

void propagate(std::map<Tile::ID, ClipID> &mapping, std::list<TileHierarchy> &array, const ClipID &parent = ClipID{}) {
    for (auto &item : array) {
        item.clip.mask >>= parent.length;
        item.clip.mask |= parent.mask;
        item.clip.length += parent.length;
        auto result = mapping.emplace(item.id, item.clip);
        assert("Tried to insert a duplicate item" && result.second == true);
        propagate(mapping, item.children, const_cast<const ClipID &>(item.clip));
    };
}

void updateClipIDs(const std::list<Tile *> &array) {
    std::forward_list<Tile::ID> ids;
    std::transform(array.begin(), array.end(), std::front_inserter(ids), [](Tile *item) {
        return item->id;
    });

    const std::map<Tile::ID, ClipID> mapping = computeClipIDs(ids);

    std::for_each(array.begin(), array.end(), [&mapping](Tile *item) {
        auto it = mapping.find(item->id);
        if (it != mapping.end()) {
            item->clip = it->second;
        } else {
            item->clip = ClipID {};
        }
    });
}

std::map<Tile::ID, ClipID> computeClipIDs(std::forward_list<Tile::ID> array) {
    // Sort by zoom level and make sure that we don't have duplicate elements.
    array.sort();
    array.unique();

    std::list<TileHierarchy> hierarchy = partition(std::move(array));
    prefix(hierarchy);

    std::map<Tile::ID, ClipID> mapping;
    propagate(mapping, hierarchy);
    return mapping;
}

}
