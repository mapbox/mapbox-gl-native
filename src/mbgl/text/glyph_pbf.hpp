#ifndef MBGL_TEXT_GLYPH_PBF
#define MBGL_TEXT_GLYPH_PBF

#include <mbgl/text/glyph.hpp>

#include <functional>
#include <atomic>
#include <string>

namespace mbgl {

class Environment;
class FontStack;
class Request;

class GlyphPBF {
public:
    using GlyphLoadedCallback = std::function<void(GlyphPBF*)>;
    using GlyphLoadingFailedCallback = std::function<void(const std::string&)>;

    GlyphPBF(const std::string &glyphURL,
             const std::string &fontStack,
             GlyphRange glyphRange,
             Environment &env,
             const GlyphLoadedCallback& successCallback,
             const GlyphLoadingFailedCallback& failureCallback);
    ~GlyphPBF();

    void parse(FontStack &stack);
    bool isParsed() const;

    std::string getURL() const {
        return url;
    }

private:
    GlyphPBF(const GlyphPBF &) = delete;
    GlyphPBF(GlyphPBF &&) = delete;
    GlyphPBF &operator=(const GlyphPBF &) = delete;
    GlyphPBF &operator=(GlyphPBF &&) = delete;

    std::string data;
    std::string url;
    std::atomic<bool> parsed;

    Environment& env;
    Request* req = nullptr;
};

} // end namespace mbgl

#endif
