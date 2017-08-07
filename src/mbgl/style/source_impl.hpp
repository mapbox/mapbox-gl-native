#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <string>

namespace mbgl {

class RenderSource;

namespace style {

class SourceObserver;

class Source::Impl {
public:
    virtual ~Impl() = default;

    Impl& operator=(const Impl&) = delete;

    virtual optional<std::string> getAttribution() const = 0;

    const SourceType type;
    const std::string id;

protected:
    Impl(SourceType, std::string);
    Impl(const Impl&) = default;
};

} // namespace style
} // namespace mbgl
