#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/tileset.hpp>
#include <mbgl/util/variant.hpp>

namespace mbgl {

class AsyncRequest;

namespace style {

class VectorSource final : public Source {
public:
    VectorSource(std::string id, variant<std::string, Tileset> urlOrTileset);
    ~VectorSource() final;

    const variant<std::string, Tileset>& getURLOrTileset() const;
    optional<std::string> getURL() const;

    class Impl;
    const Impl& impl() const;

    void loadDescription(FileSource&) final;

    mapbox::base::WeakPtr<Source> makeWeakPtr() override {
        return weakFactory.makeWeakPtr();
    }

private:
    const variant<std::string, Tileset> urlOrTileset;
    std::unique_ptr<AsyncRequest> req;
    mapbox::base::WeakPtrFactory<Source> weakFactory {this};
};

template <>
inline bool Source::is<VectorSource>() const {
    return getType() == SourceType::Vector;
}

} // namespace style
} // namespace mbgl
