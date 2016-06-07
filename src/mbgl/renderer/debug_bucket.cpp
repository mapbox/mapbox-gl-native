#include <mbgl/renderer/debug_bucket.hpp>
#include <mbgl/renderer/debug_renderable.hpp>
#include <mbgl/renderer/painter.hpp>
#include <mbgl/util/string.hpp>

#include <mbgl/gl/gl.hpp>

#include <cassert>
#include <string>

using namespace mbgl;

DebugBucket::DebugBucket(const OverscaledTileID& id,
                         const bool isRenderable_,
                         const bool isComplete_,
                         optional<Timestamp> modified_,
                         optional<Timestamp> expires_,
                         MapDebugOptions debugMode_)
    : isRenderable(isRenderable_),
      isComplete(isComplete_),
      modified(std::move(modified_)),
      expires(std::move(expires_)),
      debugMode(debugMode_),
      renderable(std::make_unique<DebugRenderable>()) {
    double baseline = 200;
    if (debugMode & MapDebugOptions::ParseStatus) {
        const std::string text =
            util::toString(id) + " - " +
            (isComplete ? "complete" : isRenderable ? "renderable" : "pending");
        renderable->fontBuffer.addText(text.c_str(), 50, baseline, 5);
        baseline += 200;
    }

    if (debugMode & MapDebugOptions::Timestamps && modified && expires) {
        const std::string modifiedText = "modified: " + util::iso8601(*modified);
        renderable->fontBuffer.addText(modifiedText.c_str(), 50, baseline, 5);

        const std::string expiresText = "expires: " + util::iso8601(*expires);
        renderable->fontBuffer.addText(expiresText.c_str(), 50, baseline + 200, 5);
    }
}

DebugBucket::~DebugBucket() = default;

DebugRenderable& DebugBucket::getRenderable() const {
    assert(renderable);
    return *renderable;
}
