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
class CollectionBase {
public:
    using Impl = typename T::Impl;
    using WrapperVector = std::vector<std::unique_ptr<T>>;
    using ImmutableVector = Immutable<std::vector<Immutable<Impl>>>;

    CollectionBase();

    std::size_t size() const;
    T* get(const std::string&) const;

    std::vector<T*> getWrappers() const;
    ImmutableVector getImpls() const { return impls; }

    auto begin() const { return wrappers.begin(); }
    auto end() const { return wrappers.end(); }

    void clear();

protected:
    std::size_t index(const std::string&) const;
    T* add(std::size_t wrapperIndex, std::size_t implIndex, std::unique_ptr<T> wrapper);
    std::unique_ptr<T> remove(std::size_t wrapperIndex, std::size_t implIndex);
    // Must be called whenever an element of the collection is internally mutated.
    // Typically, each element permits registration of an observer, and the observer
    // should call this method.
    void update(std::size_t implIndex, const T&);

    WrapperVector wrappers;
    ImmutableVector impls;
};

template <class T, bool persistentImplsOrder = false>
class Collection;

template <class T>
class Collection<T, false /*persistentImplsOrder*/> : public CollectionBase<T> {
    using Base = CollectionBase<T>;

public:
    T* add(std::unique_ptr<T> wrapper, const optional<std::string>& before = nullopt) {
        std::size_t index = before ? Base::index(*before) : Base::size();
        return Base::add(index, index, std::move(wrapper));
    }
    std::unique_ptr<T> remove(const std::string& id) {
        std::size_t index = Base::index(id);
        return Base::remove(index, index);
    }
    void update(const T& wrapper) { Base::update(Base::index(wrapper.getID()), wrapper); }
};

template <class T>
class Collection<T, true /*persistentImplsOrder*/> : public CollectionBase<T> {
    using Base = CollectionBase<T>;

public:
    T* add(std::unique_ptr<T> wrapper) {
        std::size_t i = implsIndex(wrapper->getID());
        return Base::add(Base::size(), i, std::move(wrapper));
    }

    std::unique_ptr<T> remove(const std::string& id) { return Base::remove(Base::index(id), implsIndex(id)); }

    void update(const T& wrapper) { Base::update(implsIndex(wrapper.getID()), wrapper); }

private:
    std::size_t implsIndex(const std::string& id) const {
        const auto& impls_ = *Base::impls;
        auto it = std::lower_bound(
            impls_.begin(), impls_.end(), id, [](const auto& a, const std::string& b) { return a->id < b; });
        return it - impls_.begin();
    }
};
template <class T>
using CollectionWithPersistentOrder = Collection<T, true>;

template <class T>
CollectionBase<T>::CollectionBase() : impls(makeMutable<std::vector<Immutable<Impl>>>()) {}

template <class T>
std::size_t CollectionBase<T>::size() const {
    return wrappers.size();
}

template <class T>
std::size_t CollectionBase<T>::index(const std::string& id) const {
    return std::find_if(wrappers.begin(), wrappers.end(), [&](const auto& e) {
        return e->getID() == id;
    }) - wrappers.begin();
}

template <class T>
T* CollectionBase<T>::get(const std::string& id) const {
    std::size_t i = index(id);
    return i < size() ? wrappers[i].get() : nullptr;
}

template <class T>
std::vector<T*> CollectionBase<T>::getWrappers() const {
    std::vector<T*> result;
    result.reserve(wrappers.size());

    for (auto& wrapper : wrappers) {
        result.push_back(wrapper.get());
    }

    return result;
}

template <class T>
void CollectionBase<T>::clear() {
    mutate(impls, [&] (auto& impls_) {
        impls_.clear();
    });

    wrappers.clear();
}

template <class T>
T* CollectionBase<T>::add(std::size_t wrapperIndex, std::size_t implIndex, std::unique_ptr<T> wrapper) {
    assert(wrapperIndex <= size());
    assert(implIndex <= size());
    mutate(impls, [&](auto& impls_) { impls_.emplace(impls_.begin() + implIndex, wrapper->baseImpl); });

    return wrappers.emplace(wrappers.begin() + wrapperIndex, std::move(wrapper))->get();
}

template <class T>
std::unique_ptr<T> CollectionBase<T>::remove(std::size_t wrapperIndex, std::size_t implIndex) {
    if (wrapperIndex >= size()) {
        return nullptr;
    }
    assert(implIndex < size());

    auto source = std::move(wrappers[wrapperIndex]);

    mutate(impls, [&](auto& impls_) { impls_.erase(impls_.begin() + implIndex); });

    wrappers.erase(wrappers.begin() + wrapperIndex);

    return source;
}

template <class T>
void CollectionBase<T>::update(std::size_t implIndex, const T& wrapper) {
    if (implIndex >= size()) {
        assert(false);
        return;
    }
    mutate(impls, [&](auto& impls_) { impls_.at(implIndex) = wrapper.baseImpl; });
}

} // namespace style
} // namespace mbgl
