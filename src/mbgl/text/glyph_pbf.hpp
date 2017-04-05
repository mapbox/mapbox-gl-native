#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <atomic>
#include <functional>
#include <string>
#include <memory>
#include <unordered_map>

namespace mbgl {

class GlyphAtlas;
class GlyphRequestor;
class GlyphAtlasObserver;
class AsyncRequest;
class FileSource;

std::vector<SDFGlyph> parseGlyphPBF(const GlyphRange&, const std::string& data);

class GlyphPBF : private util::noncopyable {
public:
    GlyphPBF(GlyphAtlas*,
             const FontStack&,
             const GlyphRange&,
             GlyphAtlasObserver*,
             FileSource&);
    ~GlyphPBF();

    bool isParsed() const {
        return parsed;
    }

    bool parsed;
    std::unique_ptr<AsyncRequest> req;
    GlyphAtlasObserver* observer = nullptr;
    std::unordered_map<GlyphRequestor*, std::shared_ptr<GlyphDependencies>> requestors;
};

} // namespace mbgl
