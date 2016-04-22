#ifndef MBGL_TEXT_GLYPH_STORE
#define MBGL_TEXT_GLYPH_STORE

#include <mbgl/text/glyph.hpp>
#include <mbgl/text/glyph_set.hpp>
#include <mbgl/util/exclusive.hpp>
#include <mbgl/util/noncopyable.hpp>
#include <mbgl/util/work_queue.hpp>

#include <exception>
#include <set>
#include <string>
#include <unordered_map>

namespace mbgl {

class FileSource;
class GlyphPBF;

// The GlyphStore manages the loading and storage of Glyphs
// and creation of GlyphSet objects. The GlyphStore lives
// on the MapThread but can be queried from any thread.
class GlyphStore : private util::noncopyable {
public:
    class Observer {
    public:
        virtual ~Observer() = default;

        virtual void onGlyphsLoaded(const std::string& /* fontStack */, const GlyphRange&) {};
        virtual void onGlyphsError(const std::string& /* fontStack */, const GlyphRange&, std::exception_ptr) {};
    };

    GlyphStore(FileSource&);
    ~GlyphStore();

    util::exclusive<GlyphSet> getGlyphSet(const std::string& fontStack);

    // Returns true if the set of GlyphRanges are available and parsed or false
    // if they are not. For the missing ranges, a request on the FileSource is
    // made and when the glyph if finally parsed, it gets added to the respective
    // GlyphSet and a signal is emitted to notify the observers. This method
    // can be called from any thread.
    bool hasGlyphRanges(const std::string& fontStack, const std::set<GlyphRange>& glyphRanges);

    void setURL(const std::string &url) {
        glyphURL = url;
    }

    std::string getURL() const {
        return glyphURL;
    }

    void setObserver(Observer* observer);

private:
    void requestGlyphRange(const std::string& fontStackName, const GlyphRange& range);

    FileSource& fileSource;
    std::string glyphURL;

    std::unordered_map<std::string, std::map<GlyphRange, std::unique_ptr<GlyphPBF>>> ranges;
    std::mutex rangesMutex;

    std::unordered_map<std::string, std::unique_ptr<GlyphSet>> glyphSets;
    std::mutex glyphSetsMutex;

    util::WorkQueue workQueue;

    Observer nullObserver;
    Observer* observer = &nullObserver;
};

} // namespace mbgl

#endif
