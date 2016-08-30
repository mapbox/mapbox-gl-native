#pragma once

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/util/font_stack.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <atomic>
#include <functional>
#include <string>
#include <memory>

namespace mbgl {

class AsyncRequest;
class FileSource;

class GlyphPBF : private util::noncopyable {
public:
    GlyphPBF(GlyphStore*,
             const FontStack&,
             const GlyphRange&,
             GlyphStoreObserver*,
             FileSource&);
    ~GlyphPBF();

    bool isParsed() const {
        return parsed;
    }

private:
    std::atomic<bool> parsed;
    std::unique_ptr<AsyncRequest> req;
    GlyphStoreObserver* observer = nullptr;
};

} // namespace mbgl
