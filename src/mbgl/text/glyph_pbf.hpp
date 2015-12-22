#ifndef MBGL_TEXT_GLYPH_PBF
#define MBGL_TEXT_GLYPH_PBF

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/glyph_store.hpp>
#include <mbgl/util/noncopyable.hpp>

#include <atomic>
#include <functional>
#include <string>
#include <memory>

namespace mbgl {

class FontStack;
class FileRequest;

class GlyphPBF : private util::noncopyable {
public:
    GlyphPBF(GlyphStore* store,
             const std::string& fontStack,
             const GlyphRange&,
             GlyphStore::Observer*);
    ~GlyphPBF();

    bool isParsed() const {
        return parsed;
    }

private:
    void parse(GlyphStore*, const std::string& fontStack, const GlyphRange&);

    std::shared_ptr<const std::string> data;
    std::atomic<bool> parsed;

    std::unique_ptr<FileRequest> req;

    GlyphStore::Observer* observer = nullptr;
};

} // namespace mbgl

#endif
