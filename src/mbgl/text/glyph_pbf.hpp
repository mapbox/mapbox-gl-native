#ifndef MBGL_TEXT_GLYPH_PBF
#define MBGL_TEXT_GLYPH_PBF

#include <mbgl/text/glyph.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/storage/request_holder.hpp>

#include <atomic>
#include <functional>
#include <string>

namespace mbgl {

class GlyphStore;
class FontStack;
class Request;

class GlyphPBF : private util::noncopyable {
public:
    class Observer {
    public:
        virtual ~Observer() = default;

        virtual void onGlyphPBFLoaded() = 0;
        virtual void onGlyphPBFLoadingFailed(std::exception_ptr error) = 0;
    };

    GlyphPBF(GlyphStore* store,
             const std::string& fontStack,
             const GlyphRange& glyphRange);
    virtual ~GlyphPBF();

    bool isParsed() const {
        return parsed;
    };

    void setObserver(Observer* observer);

private:
    void emitGlyphPBFLoaded();
    void emitGlyphPBFLoadingFailed(const std::string& message);

    void parse(GlyphStore* store, const std::string& fontStack, const std::string& url);

    std::string data;
    std::atomic<bool> parsed;

    RequestHolder req;

    Observer* observer = nullptr;
};

}

#endif
