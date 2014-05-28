#include <llmr/text/glyph_store.hpp>

#include <llmr/util/std.hpp>
#include <llmr/util/string.hpp>
#include <llmr/platform/platform.hpp>
#include <uv.h>

namespace llmr {


GlyphSet::GlyphSet(const std::string &fontStack, GlyphRange glyphRange)
    : future(promise.get_future().share())
{
    // Load the glyph set URL
    std::string url = util::sprintf<255>("http://mapbox.s3.amazonaws.com/gl-glyphs/%s/%d-%d.pbf", fontStack.c_str(), glyphRange.first, glyphRange.second);

    // TODO: Find more reliable URL normalization function
    std::replace(url.begin(), url.end(), ' ', '+');

    platform::request_http(url, [&](platform::Response *res) {
        if (res->code != 200) {
            // Something went wrong with loading the glyph pbf. Pass on the error to the future listeners.
            const std::string msg = util::sprintf<255>("[ERROR] failed to load glyphs (%d): %s\n", res->code, res->error_message.c_str());
            promise.set_exception(std::make_exception_ptr(std::runtime_error(msg)));
        } else {
            // Transfer the data to the GlyphSet and signal its availability.
            // Once it is available, the caller will need to call parse() to actually
            // parse the data we received. We are not doing this here since this callback is being
            // called from another (unknown) thread.
            data.swap(res->body);
            promise.set_value(*this);
        }
    });
}

std::shared_future<GlyphSet &> GlyphSet::getFuture() {
    return future;
}

void GlyphSet::parse() {
    std::lock_guard<std::mutex> lock(mtx);

    if (!data.size()) {
        // If there is no data, this means we either haven't received any data, or
        // we have already parsed the data.
        return;
    }

    // TODO: parse the glyphs


    data.clear();
}


void GlyphStore::waitForGlyphRanges(const std::string &fontStack, const std::set<GlyphRange> &glyphRanges) {
    // We are implementing a blocking wait with futures: Every GlyphSet has a future that we are
    // waiting for until it is loaded.

    std::vector<std::shared_future<GlyphSet &>> futures;
    futures.reserve(glyphRanges.size());
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto &rangeSets = stacks[fontStack];

        // Attempt to load the glyph range. If the GlyphSet already exists, we are getting back
        // the same shared_future.
        for (GlyphRange range : glyphRanges) {
            futures.emplace_back(loadGlyphRange(fontStack, rangeSets, range));
        }
    }

    // Now that we potentially created all GlyphSets, we are waiting for the results, one by one.
    // When we get a result (or the GlyphSet is aready loaded), we are attempting to parse the
    // GlyphSet.
    for (std::shared_future<GlyphSet &> &future : futures) {
        future.get().parse();
    }
}

std::shared_future<GlyphSet &> GlyphStore::loadGlyphRange(const std::string &name, std::map<GlyphRange, std::unique_ptr<GlyphSet>> &rangeSets, const GlyphRange range) {
    auto range_it = rangeSets.find(range);
    if (range_it == rangeSets.end()) {
        // We don't have this glyph set yet for this font stack.
        range_it = rangeSets.emplace(range, std::make_unique<GlyphSet>(name, range)).first;
    }

    return range_it->second->getFuture();
}


}
