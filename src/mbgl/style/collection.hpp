#pragma once

#include <mbgl/util/immutable.hpp>
#include <mbgl/util/optional.hpp>

#include <memory>
#include <string>

namespace mbgl {
namespace style {

/*
    Manages an ordered collection of elements and their `Immutable<Impl>`s. The latter is
    itself stored in an Immutable container. Using immutability at the collection level
    allows us to short-circuit significant portions of the RenderStyle update logic via
    a simple pointer equality check, greatly improving performance.

    Element types are required to have:

      * An `Impl` inner class type
      * An `Immutable<Impl> baseImpl` member
      * A `std::string getID() const` method
*/
template <class T>
class Collection {
public:
    using Impl = typename T::Impl;
    using WrapperVector = std::vector<std::unique_ptr<T>>;
    using ImmutableVector = Immutable<std::vector<Immutable<Impl>>>;

    Collection();

    std::size_t size() const;
    T* get(const std::string&) const;

    std::vector<T*> getWrappers() const;
    ImmutableVector getImpls() const { return impls; }

    auto begin() const { return wrappers.begin(); }
    auto end() const { return wrappers.end(); }

    void clear();

    T* add(std::unique_ptr<T>, const optional<std::string>& = {});
    std::unique_ptr<T> remove(const std::string&);

    // Must be called whenever an element of the collection is internally mutated.
    // Typically, each element permits registration of an observer, and the observer
    // should call this method.
    void update(const T&);

private:
    std::size_t index(const std::string&) const;

    WrapperVector wrappers;
    ImmutableVector impls;
};

template <class T>
Collection<T>::Collection()
    : impls(makeMutable<std::vector<Immutable<Impl>>>()) {
}

template <class T>
std::size_t Collection<T>::size() const {
    return wrappers.size();
}

template <class T>
std::size_t Collection<T>::index(const std::string& id) const {
    return std::find_if(wrappers.begin(), wrappers.end(), [&](const auto& e) {
        return e->getID() == id;
    }) - wrappers.begin();
}

template <class T>
T* Collection<T>::get(const std::string& id) const {
    std::size_t i = index(id);
    return i < size() ? wrappers[i].get() : nullptr;
}

template <class T>
std::vector<T*> Collection<T>::getWrappers() const {
    std::vector<T*> result;
    result.reserve(wrappers.size());

    for (auto& wrapper : wrappers) {
        result.push_back(wrapper.get());
    }

    return result;
}

template <class T>
void Collection<T>::clear() {
    mutate(impls, [&] (auto& impls_) {
        impls_.clear();
    });

    wrappers.clear();
}

template <class T>
T* Collection<T>::add(std::unique_ptr<T> wrapper, const optional<std::string>& before) {
    std::size_t i = before ? index(*before) : size();

    mutate(impls, [&] (auto& impls_) {
        impls_.emplace(impls_.begin() + i, wrapper->baseImpl);
    });

    return wrappers.emplace(wrappers.begin() + i, std::move(wrapper))->get();
}

template <class T>
std::unique_ptr<T> Collection<T>::remove(const std::string& id) {
    std::size_t i = index(id);

    if (i >= size()) {
        return nullptr;
    }

    auto source = std::move(wrappers[i]);

    mutate(impls, [&] (auto& impls_) {
        impls_.erase(impls_.begin() + i);
    });

    wrappers.erase(wrappers.begin() + i);

    return source;
}

template <class T>
void Collection<T>::update(const T& wrapper) {
    mutate(impls, [&] (auto& impls_) {
        impls_.at(this->index(wrapper.getID())) = wrapper.baseImpl;
    });
}

} // namespace style
} // namespace mbgl
