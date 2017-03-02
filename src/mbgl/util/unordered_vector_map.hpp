#pragma once

#include <iterator>
#include <utility>
#include <vector>

namespace mbgl {
namespace util {

/*
   unordered_vector_map<Key, T> provides a replacement for std::unordered_map<Key, T>
   that is suitable for cases where the collection will remain small and it is desirable
   to minimize generated code size.

   unordered_vector_map is backed by a vector, and lookup operations are O(n) linear
   searches. Unlike unordered_map, modifications may invalidate references and iterators.

   The API is not a complete replacement for unordered_map; only the methods that were
   so far needed have been implemented.
*/
template <class Key, class T>
class unordered_vector_map {
private:
    using vector_type = std::vector<std::pair<const Key, T>>;

    vector_type v;

public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const Key, T>;

    using size_type      = typename vector_type::size_type;
    using iterator       = typename vector_type::iterator;
    using const_iterator = typename vector_type::const_iterator;

    bool empty() const {
        return v.empty();
    }

    size_type size() const {
        return v.size();
    }

    iterator begin() {
        return v.begin();
    }

    const_iterator begin() const {
        return v.begin();
    }

    const_iterator cbegin() const {
        return v.cbegin();
    }

    iterator end() {
        return v.end();
    }

    const_iterator end() const {
        return v.end();
    }

    const_iterator cend() const {
        return v.cend();
    }

    void clear() {
        v.clear();
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        value_type value(std::forward<Args>(args)...);
        iterator it = find(value.first);
        if (it != end()) {
            return std::make_pair(it, false);
        }
        v.push_back(std::move(value));
        return std::make_pair(std::prev(v.end()), true);
    }

    T& at(const Key& key) {
        iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        throw std::out_of_range("at");
    }

    const T& at(const Key& key) const {
        const_iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        throw std::out_of_range("at");
    }

    T& operator[](const Key& key) {
        iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        v.emplace_back(std::piecewise_construct,
                       std::forward_as_tuple(key),
                       std::tuple<>());
        return std::prev(v.end())->second;
    }

    T& operator[](Key&& key) {
        iterator it = find(key);
        if (it != end()) {
            return it->second;
        }
        v.emplace_back(std::piecewise_construct,
                       std::forward_as_tuple(std::move(key)),
                       std::tuple<>());
        return std::prev(v.end())->second;
    }

    iterator find(const Key& key) {
        return std::find_if(v.begin(), v.end(), [&] (const value_type& val) {
            return val.first == key;
        });
    }

    const_iterator find(const Key& key) const {
        return std::find_if(v.begin(), v.end(), [&] (const value_type& val) {
            return val.first == key;
        });
    }
};

} // namespace util
} // namespace mbgl
