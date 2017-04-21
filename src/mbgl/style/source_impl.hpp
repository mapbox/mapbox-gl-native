#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/noncopyable.hpp>

namespace mbgl {

class FileSource;
class RenderSource;

namespace style {

class SourceObserver;

class Source::Impl : private util::noncopyable {
public:
    Impl(SourceType, std::string id, Source&);
    virtual ~Impl() = default;

    virtual void loadDescription(FileSource&) = 0;
    virtual std::unique_ptr<RenderSource> createRenderSource() const = 0;

    virtual optional<std::string> getAttribution() const { return {}; };

    const SourceType type;
    const std::string id;

    bool loaded = false;
    Source& base;

    void setObserver(SourceObserver*);
    SourceObserver* observer = nullptr;

    void dumpDebugLogs() const;
};

} // namespace style
} // namespace mbgl
