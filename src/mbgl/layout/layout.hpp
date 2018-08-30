#pragma once

#include <mbgl/style/layer_type.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <memory>

namespace mbgl {

class Bucket;
class BucketParameters;
class RenderLayer;
class FeatureIndex;

class Layout {
public:
    Layout() = default;
    virtual ~Layout() = default;

    virtual void createBucket(const ImagePositions&,
                              std::unique_ptr<FeatureIndex>&,
                              std::unordered_map<std::string, std::shared_ptr<Bucket>>&,
                              const bool,
                              const bool) = 0;

    virtual void prepareSymbols(const GlyphMap&, const GlyphPositions&,
                                const ImageMap&, const ImagePositions&) {};

    virtual bool hasSymbolInstances() const {
        return true;
    };
    
    virtual bool hasDependencies() const = 0;
};

} // namespace mbgl
